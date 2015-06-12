#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include <jansson.h>
#include <curl/curl.h>

#define BUFFER_SIZE (350 * 1024) //350 KB

//temporary variant structure
typedef struct {
    char** alternateBases;
    //wrong type
	int calls;
    int created;
    long long end;
    char * id;
	//wrong type
    int info;
    char** names;
    char* referenceBases;
    char* referenceName;
    long long start;
    int updated;
    char * variantSetId;
} Variant;

struct write_result
{
	char *data;
	int pos;
};


char* post_search_variants(char* baseURL, char*postFields);

void write_vcf_file(char* data, char* name_vcf);


