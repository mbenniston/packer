#include <stdio.h>
#include <argp.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/*
	Packer
	
	Usage:
		packer [-i input_filename] [-o output_filename] [-d header_filename] [-v variable_name] [-t variable_type] [-p] [-c]
		
		Input File: From file or stdin
			-i "filename" (for input file)
			
		Output File: To file or stdout
	        	-o "filename" (for output file)

		Variable Name: ""
			-v "name"
		
		Variable Type: unsigned char, char
			-t "type"

		Output Header File: 
			-d "filename" (for output header file)
		
		Pragma Use In Header File:
			-p (to enable use of pragma once for header guarding)

		Extern C Usage in Header file:
			-c (to enable use of extern "C" in the header file)
*/

const char* const VAR_LENGTH_POSTFIX = "_length";

const char* var_name = "var_name";
const char* var_type = "const unsigned char";
const char* var_fmt = "0x%X";
const char* input_filename = NULL;
const char* output_filename = NULL;
const char* output_header_filename = NULL;
int header_use_pragma = 0;
int header_externc = 0;

size_t pack(FILE* in_file, FILE* out_file);
void header_definition(FILE* out_file, size_t bytesRead);

//argp callback function
static int parse_opt(int key, char* arg, struct argp_state* state);

int main(int argc, char** argv)
{
	struct argp_option options[] = {
		{0, 'i', "input filename", 0, "Specifies the input filename, default is stdin"},
		{0, 'o', "output filename", 0, "Specifies the output filename, default is stdout"},
		{0, 'v', "variable name", 0, "The name of the variable in the source file"},
		{0, 't', "variable type", 0, "The type of the variable in the source file"},
		{0, 'd', "output header filename", 0, "Specifies the output filename for the header, does not create by default"},
		{0, 'p', 0, 0, "Add for use of #pragma once in header file"},
		{0, 'c', 0, 0, "Add for use of extern \"C\" in header file"},
		{ 0 }
	};

	struct argp argp = {options, parse_opt};
	
	int ret = argp_parse(&argp, argc, argv, 0, 0, 0);
	//exit the program if argp did not succeed  
	if(ret != 0) {
		return ret;
	}

	FILE* in_file = stdin;
	FILE* out_file = stdout;
	FILE* out_header_file = NULL;
	
	//open the input file if specified	
	if(input_filename != NULL) 
	{
		in_file = fopen(input_filename, "rb");
		if(!in_file) {
			fprintf(stderr, "Cannot open input file!\n");
			return 1;
		}
	}
	
	//open the output file if specified	
	if(output_filename != NULL)
	{
		out_file = fopen(output_filename, "w");
		if(!out_file) {
			if(input_filename != NULL) fclose(in_file);
			fprintf(stderr, "Cannot open output file!\n");
			return 1;
		}
	}

	//pack the bytes in an array	
	size_t bytesRead = pack(in_file, out_file);

	//close input/output files if they have been opened
	if(input_filename != NULL) 
		fclose(in_file);
	
	if(output_filename != NULL) 
		fclose(out_file);
	
	//open and write the header file if specified 
	if(output_header_filename != NULL)
	{
		out_header_file = fopen(output_header_filename, "w");
		if(!out_header_file) {
			fprintf(stderr, "Cannot open header file!\n");
			return 1;
		}
	
		header_definition(out_header_file, bytesRead);
	}

	if(output_header_filename != NULL) 
		fclose(out_header_file);

	return 0;
}

size_t pack(FILE* in_file, FILE* out_file)
{
	//preamble
	fprintf(out_file, "");
	fprintf(out_file, "%s %s[] = { ", var_type, var_name);

	//read from input file and print to output file, byte by byte.
	size_t totalBytes = 0;
	while(!feof(in_file))
	{
		//get byte
		int b = fgetc(in_file);
		if(b != EOF) {
			fprintf(out_file, "0x%X, ", b);
			totalBytes++;
		}
	}

	//closing symbols
	fprintf(out_file, "};\n");

	fprintf(out_file, "\nconst size_t %s%s = %zu;\n\n", var_name, VAR_LENGTH_POSTFIX, totalBytes);

	return totalBytes;
}

void header_definition(FILE* out_file, size_t bytesRead)
{
	if(header_use_pragma)
	{
		fprintf(out_file, "#pragma once\n");
	}
	else
	{
		//get uppercase version of variable name
		size_t len = strlen(var_name);
		char* upperName = malloc((len+1) * sizeof(char));
	
		if(!upperName) return;


		for(int i = 0; i < len; i++) {
			upperName[i] = toupper(var_name[i]);
		}
		
		upperName[len] = 0;

		fprintf(out_file, "#ifndef %s_HEADER\n", upperName);
		fprintf(out_file, "#define %s_HEADER\n", upperName);
		
		free(upperName);
	}
	
	if(header_externc) 
		fprintf(out_file, "\n#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n");

	fprintf(out_file, "#include <stddef.h>\n");

	fprintf(out_file, "\nextern const size_t %s%s;\n", var_name, VAR_LENGTH_POSTFIX);
	fprintf(out_file, "extern %s %s[%zu];\n", var_type, var_name, bytesRead);

	if(header_externc) 
		fprintf(out_file, "\n#ifdef __cplusplus\n}\n#endif\n\n");

	if(!header_use_pragma) 
		fprintf(out_file, "\n#endif\n");	
}

static int parse_opt(int key, char* arg, struct argp_state* state)
{
	switch(key)
	{
		case 'i':	
			input_filename = arg;
			break;
		case 'o':
			output_filename = arg;
			break;
		case 'v':
			var_name = arg;
			break;
		case 't':
			var_type = arg;
			break;
		case 'f':
			var_fmt = arg;
			break;
		case 'd':
			output_header_filename = arg;
			break;
		case 'p':
			header_use_pragma = 1;
			break;
		case 'c':
			header_externc = 1;
		default:
			break;
	}

	return 0;
}