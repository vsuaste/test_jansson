#include "ga4gh.h"

static size_t write_response(void *ptr, size_t size, size_t nmemb, void *stream)
{
	struct write_result *result = (struct write_result*)stream;
	
	if(result->pos + size * nmemb >= BUFFER_SIZE)
	{
		fprintf(stderr,"error: too small buffer\n");
		return 0;	
	}	
	
	memcpy(result->data+result->pos,ptr,size*nmemb);
	result->pos+= size*nmemb;
	
	return  size * nmemb;
}

static char* append_protocol_string(const char* baseURL, const char* protocol_name)
{
	int size = strlen(baseURL) + strlen(protocol_name);
	char *completeURL = malloc(size + 1);
	
	strcpy(completeURL, baseURL);
	strcat(completeURL,protocol_name);
	
	return completeURL;
}

char* post_search_variants(char* baseURL, char*postFields)
{
	curl_global_init( CURL_GLOBAL_ALL );
	CURL * myHandle;
	CURLcode result; 
	struct curl_slist *headers = NULL;
	char *data = NULL;	
	char *completeURL = append_protocol_string(baseURL,"/variants/search");
	data = malloc(BUFFER_SIZE);
	struct write_result writeResult = {
		.data = data,
		.pos = 0
	};
		
	myHandle = curl_easy_init();
	headers = curl_slist_append(headers, "Content-Type: application/json");	
	// Notice the lack of major error checking so far....
	curl_easy_setopt(myHandle, CURLOPT_URL,completeURL);
	curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, postFields);
	curl_easy_setopt(myHandle,CURLOPT_HTTPHEADER,headers);			
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, write_response);
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &writeResult);
		
	result = curl_easy_perform( myHandle );
	data[writeResult.pos]='\0';

	curl_easy_cleanup(myHandle);
    curl_slist_free_all(headers);
    curl_global_cleanup();
	
	return data;
}

void write_vcf_file(char* data, char* name_vcf)
{
	FILE *output_file;
	json_t *root = NULL;
	json_error_t error;
	json_t *variants,*item;
	//chrom, pos, id, ref, alt (missing fields...)
	json_t *vcf_data[5];
	const char* variants_fields[5] = {"referenceName","start","id","referenceBases","alternateBases"};
	int i,j;
	
	root = json_loads(data,0,&error);
	
	if(!json_is_object(root))
	{
		printf("error in json object \n");
		json_decref(root);
		return;
	}
			
	variants = json_object_get(root,"variants");
	if(!json_is_array(variants))
	{
		printf("error in json array variants \n");
		json_decref(root);
		return;
	}
	
	output_file = fopen(name_vcf,"w");
	//write header
	fprintf(output_file,"##fileformat=VCFv4.1\n");
 	fprintf(output_file,"#CHROM \t POS \t ID \t REF \t ALT \t QUAL \t FILTER \t INFO \t FORMAT \n");
	
	for(i=0; i<json_array_size(variants); i++ )
	{
		//read each variant
		item = json_array_get(variants,i);
		if(!json_is_object(item))
		{
			printf("error in json object item \n");
			json_decref(item);
			return;
		}
		//extract variant data
		for(j=0; j<5; j++)
		{
			vcf_data[j] = json_object_get(item,variants_fields[j]);
		}
		//write row in vcf file
		fprintf(output_file,"%s \t",json_string_value(vcf_data[0]));
		fprintf(output_file,"%d \t",(int)json_integer_value(vcf_data[1]));
		fprintf(output_file,"%s \t",json_string_value(vcf_data[2]));
		fprintf(output_file,"%s \t",json_string_value(vcf_data[3]));
		fprintf(output_file,"\n");
	}
	fclose(output_file);
}





