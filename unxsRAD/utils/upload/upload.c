#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./multipart-parser-c/multipart_parser.h"

char boundary[256] = "--"; // boundary starts with double dash

struct parsedata
{
    int inContentDisposition;  // flag for the right header to look for fields
    char *partname;            // field name
    char *filename;            // file name for an upload field
    FILE *saveto;              // file to save contents to
};

void showForm(void)
{
    puts("<form action=\"upload.cgi\" method=\"post\" "
            "enctype=\"multipart/form-data\">"
        "<p>Photo to Upload: <input type=\"file\" name=\"photo\" /></p>"
        "<p>Your Email Address: <input type=\"text\" "
            "name=\"email_address\" /></p>"
        "<p><input type=\"submit\" name=\"Submit\" value=\"Submit Form\""
        "/></p></form>");
}

int handle_headername(multipart_parser *parser, const char *at, size_t length)
{
    struct parsedata *data = multipart_parser_get_data(parser);
    data->inContentDisposition = !strncmp(at, "Content-Disposition", length);
    return 0;
}

int handle_headervalue(multipart_parser *parser, const char *at, size_t length)
{
    char localfilename[1024];

    struct parsedata *data = multipart_parser_get_data(parser);
    if (data->inContentDisposition)
    {
        char *hdrval = calloc(1, length + 1);
        strncpy(hdrval, at, length);
        if (strtok(hdrval, "; "))
        {
            char *tok;
            while ((tok = strtok(0, "; ")))
            {
                char *rquot;
                if (!strncmp(tok, "name=\"", 6) && 
                        ((rquot = strrchr(tok, '"')) > tok + 6))
                {
                    *rquot = 0;
                    free(data->partname);
                    data->partname = malloc(strlen(tok + 6) + 1);
                    strcpy(data->partname, tok + 6);
                }
                else if (!strncmp(tok, "filename=\"", 10) &&
                        ((rquot = strrchr(tok, '"')) > tok + 10))
                {
                    *rquot = 0;
                    free(data->filename);
                    data->filename = malloc(strlen(tok + 10) + 1);
                    strcpy(data->filename, tok + 10);
                    if (data->saveto) fclose(data->saveto);

                    // determine local location, adapt to your needs:
                    // for production code, ADD SANITY CHECKS, the following code
                    // allows an attacker to write any location of your server
                    // with a filename containing relative paths!
                    snprintf(localfilename, 1024, "/var/www/unxs/html/images/%s", data->filename);
			FILE *fp=NULL;
                    if((fp=fopen(localfilename, "w"))!=NULL)
                    	data->saveto = fp;
		    else
			{
				puts("Content-Type: text/html\n");
				printf("<html><head><title>Test</title></head><body>Error fopen() %s %lu</body></html>",
					localfilename,length);
				exit(0);
			}
                }
            }
        }
        free(hdrval);
    }
    return 0;

}//handle_headervalue()


int handle_contentdata(multipart_parser *parser, const char *at, size_t length)
{
    struct parsedata *data = multipart_parser_get_data(parser);

    // only handle file upload of field "photo"
    // you have to extend this to get the values of other form fields
    if (data->partname && data->filename && !strcmp(data->partname, "photo"))
    {
        if(!fwrite(at, length, 1, data->saveto))
	{
    		puts("Content-Type: text/html\n");
    		printf("<html><head><title>Test</title></head><body>Error handle_contentdata() %s %lu %s</body></html>",
			data->partname,length,boundary);
    		exit(0);
	}
    }

    return 0;
}

char *upload(void)
{
    // can only upload with POST
    const char *method = getenv("REQUEST_METHOD");
    if (!method || strcmp(method, "POST")) return 0;

    // check for multipart/form-data and extract boundary if present
    const char *conttype = getenv("CONTENT_TYPE");
    if (!conttype || sscanf(conttype,
                "multipart/form-data; boundary=%254s", boundary+2)
            < 1) return 0;

if(0)
{
    // see https://github.com/iafonov/multipart-parser-c
    multipart_parser_settings callbacks = {0};
    callbacks.on_header_field = handle_headername;
    callbacks.on_header_value = handle_headervalue;
    callbacks.on_part_data = handle_contentdata;

    struct parsedata data = {0};

    multipart_parser *parser = multipart_parser_init(boundary, &callbacks);
    multipart_parser_set_data(parser, &data);
}

//debug
char localfilename[1024]={""};
snprintf(localfilename, 1023, "/var/www/unxs/html/images/debug.contents");
FILE *fp=NULL;
if((fp=fopen(localfilename, "w"))==NULL)
	return 0;
//debug

    // read body from stdin:
    char reqdata[64 * 1024];
    size_t length;
    while ((length = fread(reqdata, 1, 64 * 1024, stdin)) > 0)
    {
        // and feed it to the parser:
        //multipart_parser_execute(parser, reqdata, length);
//debug
fprintf(fp,"%s",reqdata);
//debug path
    }
//debug
return(0);
//debug

//    multipart_parser_free(parser);

 //   free(data.partname);
  //  if (data.filename && data.saveto)
   // {
    //    fclose(data.saveto);
     //   return data.filename;
   // }
    //free(data.filename);

    return 0;
}

int main(void)
{
    char *uploaded = upload();

    puts("Content-Type: text/html\n");
    puts("<html><head><title>Test</title></head><body>");

    if (uploaded)
    {
        printf("<b>%s</b> uploaded.<p><img src=/images/%s>",uploaded,uploaded);
        free(uploaded);
    }
    else
    {
        showForm();
    }

    puts("</body></html>");

    return(0);
}
