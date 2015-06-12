#include <stdlib.h>
#include <stdio.h>
#include "ga4gh.h"

#define URL_STRING "http://wwwdev.ebi.ac.uk/eva/webservices/rest/v1/ga4gh"
#define URL_DATA "{\"referenceName\":22, \"start\":25662282, \"end\":25662302}"

int main(int argc, char *argv[])
{
	int i;
	char*data = NULL;
	
	
	data = post_search_variants(URL_STRING,URL_DATA);
	write_vcf_file(data, "test.vcf");
	
	return 0;
}