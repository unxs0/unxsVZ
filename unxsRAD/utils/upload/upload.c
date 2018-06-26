/*
FILE
	upload.c
PURPOSE
	Provide upload of png/jpg image file functionality for C cgi's
BASED ON AND INCLUDES CODE BY
	Based on node-formidable by Felix Geisendörfer
	Igor Afonov - afonov@gmail.com - 2012
	Colin Luoma 2017
ADDITIONAL AUTHORSHIP
	Gary Wallis 2018-2019 (C) for Unixservice, LLC.
LICENSE
	MIT License - http://www.opensource.org/licenses/mit-license.php
USAGE
	As static C library: libupload.a /usr/include/openisp/upload.h
	form with enc type multipart/form-data
	cTitle text input
	cDescription text input
	file_source upload file input
	
	File must be png or jpg. Must be max uMAXIMAGESIZE.

	CGI calls at authorized section: Where argc and argv are the CGI main args.
	int iUpload(int argc, const char * argv[],char *cFilename,char *cTitle,char *cDescription)

	iUpload returns not 0 on error:
	uRetVal=1;//not post
        uRetVal=2;//not multipart/form-data
        uRetVal=3;//no boundary
        uRetVal=4;//Too large or no length
        uRetVal=5;//malloc error
        uRetVal=6;//read error
        uRetVal=7;//not a valid image format
        uRetVal=8;//fopen error
        uRetVal=9;//fwrite error

*/


#define uMAXIMAGESIZE 8000000
#define PICTURE_FILE_PATH "/var/www/unxs/html/images/"
#include "upload.h"


#define NOTIFY_CB(FOR)                                                 \
do {                                                                   \
  if (p->settings->on_##FOR) {                                         \
    if (p->settings->on_##FOR(p) != 0) {                               \
      return i;                                                        \
    }                                                                  \
  }                                                                    \
} while (0)

#define EMIT_DATA_CB(FOR, ptr, len)                                    \
do {                                                                   \
  if (p->settings->on_##FOR) {                                         \
    if (p->settings->on_##FOR(p, ptr, len) != 0) {                     \
      return i;                                                        \
    }                                                                  \
  }                                                                    \
} while (0)


#define LF 10
#define CR 13

struct multipart_parser {
  void * data;

  size_t index;
  size_t boundary_length;

  unsigned char state;

  const multipart_parser_settings* settings;

  char* lookbehind;
  char multipart_boundary[1];
};

enum state {
  s_uninitialized = 1,
  s_start,
  s_start_boundary,
  s_header_field_start,
  s_header_field,
  s_headers_almost_done,
  s_header_value_start,
  s_header_value,
  s_header_value_almost_done,
  s_part_data_start,
  s_part_data,
  s_part_data_almost_boundary,
  s_part_data_boundary,
  s_part_data_almost_end,
  s_part_data_end,
  s_part_data_final_hyphen,
  s_end
};

multipart_parser* multipart_parser_init
    (const char *boundary, const multipart_parser_settings* settings) {

  multipart_parser* p = malloc(sizeof(multipart_parser) +
                               strlen(boundary) +
                               strlen(boundary) + 9);

  strcpy(p->multipart_boundary, boundary);
  p->boundary_length = strlen(boundary);

  p->lookbehind = (p->multipart_boundary + p->boundary_length + 1);

  p->index = 0;
  p->state = s_start;
  p->settings = settings;

  return p;
}

void multipart_parser_free(multipart_parser* p) {
  free(p);
}

void multipart_parser_set_data(multipart_parser *p, void *data) {
    p->data = data;
}

void *multipart_parser_get_data(multipart_parser *p) {
    return p->data;
}

size_t multipart_parser_execute(multipart_parser* p, const char *buf, size_t len) {
  size_t i = 0;
  size_t mark = 0;
  char c, cl;
  int is_last = 0;

  while(i < len) {
    c = buf[i];
    is_last = (i == (len - 1));
    switch (p->state) {
      case s_start:
        p->index = 0;
        p->state = s_start_boundary;

      /* fallthrough */
      case s_start_boundary:
        if (p->index == p->boundary_length) {
          if (c != CR) {
            return i;
          }
          p->index++;
          break;
        } else if (p->index == (p->boundary_length + 1)) {
          if (c != LF) {
            return i;
          }
          p->index = 0;
          NOTIFY_CB(part_data_begin);
          p->state = s_header_field_start;
          break;
        }
        if (c != p->multipart_boundary[p->index]) {
          return i;
        }
        p->index++;
        break;

      case s_header_field_start:
        mark = i;
        p->state = s_header_field;

      /* fallthrough */
      case s_header_field:
        if (c == CR) {
          p->state = s_headers_almost_done;
          break;
        }

        if (c == ':') {
          EMIT_DATA_CB(header_field, buf + mark, i - mark);
          p->state = s_header_value_start;
          break;
        }

        cl = tolower(c);
        if ((c != '-') && (cl < 'a' || cl > 'z')) {
          return i;
        }
        if (is_last)
            EMIT_DATA_CB(header_field, buf + mark, (i - mark) + 1);
        break;

      case s_headers_almost_done:
        if (c != LF) {
          return i;
        }

        p->state = s_part_data_start;
        break;

      case s_header_value_start:
        if (c == ' ') {
          break;
        }

        mark = i;
        p->state = s_header_value;

      /* fallthrough */
      case s_header_value:
        if (c == CR) {
          EMIT_DATA_CB(header_value, buf + mark, i - mark);
          p->state = s_header_value_almost_done;
          break;
        }
        if (is_last)
            EMIT_DATA_CB(header_value, buf + mark, (i - mark) + 1);
        break;

      case s_header_value_almost_done:
        if (c != LF) {
          return i;
        }
        p->state = s_header_field_start;
        break;

      case s_part_data_start:
        NOTIFY_CB(headers_complete);
        mark = i;
        p->state = s_part_data;

      /* fallthrough */
      case s_part_data:
        if (c == CR) {
            EMIT_DATA_CB(part_data, buf + mark, i - mark);
            mark = i;
            p->state = s_part_data_almost_boundary;
            p->lookbehind[0] = CR;
            break;
        }
        if (is_last)
            EMIT_DATA_CB(part_data, buf + mark, (i - mark) + 1);
        break;

      case s_part_data_almost_boundary:
        if (c == LF) {
            p->state = s_part_data_boundary;
            p->lookbehind[1] = LF;
            p->index = 0;
            break;
        }
        EMIT_DATA_CB(part_data, p->lookbehind, 1);
        p->state = s_part_data;
        mark = i --;
        break;

      case s_part_data_boundary:
        if (p->multipart_boundary[p->index] != c) {
          EMIT_DATA_CB(part_data, p->lookbehind, 2 + p->index);
          p->state = s_part_data;
          mark = i --;
          break;
        }
        p->lookbehind[2 + p->index] = c;
        if ((++ p->index) == p->boundary_length) {
            NOTIFY_CB(part_data_end);
            p->state = s_part_data_almost_end;
        }
        break;

      case s_part_data_almost_end:
        if (c == '-') {
            p->state = s_part_data_final_hyphen;
            break;
        }
        if (c == CR) {
            p->state = s_part_data_end;
            break;
        }
        return i;

      case s_part_data_final_hyphen:
        if (c == '-') {
            NOTIFY_CB(body_end);
            p->state = s_end;
            break;
        }
        return i;

      case s_part_data_end:
        if (c == LF) {
            p->state = s_header_field_start;
            NOTIFY_CB(part_data_begin);
            break;
        }
        return i;

      case s_end:
        break;

      default:
        return 0;
    }
    ++ i;
  }

  return len;
}//size_t multipart_parser_execute(multipart_parser* p, const char *buf, size_t len) {


char *sstrstr(const char *haystack, const char *needle, size_t length)
{
    size_t needle_length = strlen(needle);
    size_t i;

    for (i = 0; i < length; i++)
    {
        if (i + needle_length > length)
        {
            return NULL;
        }

        if (strncmp(&haystack[i], needle, needle_length) == 0)
        {
            return (char *)&haystack[i];
        }
    }
    return NULL;
}


int check_request_method(const char *method)
{
    char *r_method = getenv("REQUEST_METHOD");
    if (r_method == NULL || strcmp(r_method, method) != 0)
    {
        return 0;
    }
    return 1;
}


int check_content_type(const char *mime)
{
    char *c_type = getenv("CONTENT_TYPE");
    if (c_type == NULL)
    {
        return 0;
    }
    char *t = strstr(c_type, mime);
    if (t == NULL || c_type != t)
    {
        return 0;
    }
    return 1;
}


long get_content_length()
{
    char *env_length = getenv("CONTENT_LENGTH");
    if (env_length == NULL)
    {
        return 0;
    }
    errno = 0;
    long length = strtol(env_length, NULL, 10);
    if (errno != 0)
    {
        return 0;
    }
    return length;
}


char *get_boundary()
{
    char *boundary = strstr(getenv("CONTENT_TYPE"), "boundary=");
    if (boundary != NULL)
    {
        boundary += strlen("boundary=");
        char *ret_str = calloc(1, strlen(boundary) + 3);

        if (ret_str == NULL) return NULL;

        strcat(ret_str, "--");
        strcat(ret_str, boundary);
        return ret_str;
    }
    return NULL;
}


int read_header_value(multipart_parser* p, const char *at, size_t length)
{
   struct form_data *form_data = (struct form_data *)multipart_parser_get_data(p);

   if (sstrstr(at, "name=\"cTitle\"", length) != NULL)
   {
       form_data->form_element = TITLE;
       form_data->chunk_data[TITLE].mime = TEXT_PLAIN;
   }
   else if (sstrstr(at, "name=\"uImageNumber\"", length) != NULL)
   {
       form_data->form_element = IMAGENUMBER;
       form_data->chunk_data[IMAGENUMBER].mime = TEXT_PLAIN;
   }
   else if (sstrstr(at, "name=\"cDescription\"", length) != NULL)
   {
       form_data->form_element = DESCRIPTION;
       form_data->chunk_data[DESCRIPTION].mime = TEXT_PLAIN;
   }
   else if (sstrstr(at, "name=\"file_source\"", length) != NULL)
   {
       form_data->form_element = PICTURE_DATA;
       form_data->chunk_data[PICTURE_DATA].mime = TEXT_PLAIN;
   }

   if (form_data->form_element == PICTURE_DATA && sstrstr(at, "image/png", length) != NULL)
   {
       form_data->chunk_data[PICTURE_DATA].mime = IMG_PNG;
   }
   else if (form_data->form_element == PICTURE_DATA && sstrstr(at, "image/jpeg", length) != NULL)
   {
       form_data->chunk_data[PICTURE_DATA].mime = IMG_JPG;
   }

   return 0;
}//int read_header_value(multipart_parser* p, const char *at, size_t length)


int read_data_part(multipart_parser* p, const char *at, size_t length)
{
    if (length == 0) return 0;

    struct form_data *form_data = (struct form_data *)multipart_parser_get_data(p);

    char * cur_data = form_data->chunk_data[form_data->form_element].data;
    size_t cur_len = form_data->chunk_data[form_data->form_element].len;

    cur_data = realloc(cur_data, cur_len + length);

    if (cur_data == NULL)
    {
        form_data->status = NOT_OK;
        return 1;
    }

    memcpy(cur_data + cur_len, at, length);
    form_data->chunk_data[form_data->form_element].data = cur_data;
    form_data->chunk_data[form_data->form_element].len += length;

    return 0;
}


int read_multipart_end(multipart_parser* p)
{
    struct form_data *form_data = (struct form_data *)multipart_parser_get_data(p);

    // Data from picture was not acceptable
    if (form_data->chunk_data[PICTURE_DATA].data == NULL ||
        form_data->chunk_data[PICTURE_DATA].mime == TEXT_PLAIN)
    {
        form_data->status = NOT_OK;
        return 1;
    }

    // No message or name data, replace with empty string
    if (form_data->chunk_data[TITLE].data == NULL)
    {
        form_data->chunk_data[TITLE].data = calloc(1, 1);
    }
    if (form_data->chunk_data[DESCRIPTION].data == NULL)
    {
        form_data->chunk_data[DESCRIPTION].data = calloc(1, 1);
    }
    if (form_data->chunk_data[IMAGENUMBER].data == NULL)
    {
        form_data->chunk_data[IMAGENUMBER].data = calloc(1, 1);
    }

    return 0;
}//int read_multipart_end(multipart_parser* p)


void init_form_data(struct form_data * form_data)
{
    form_data->chunk_data[TITLE].data = NULL;
    form_data->chunk_data[TITLE].len = 0;
    form_data->chunk_data[TITLE].mime = TEXT_PLAIN;
    form_data->chunk_data[DESCRIPTION].data = NULL;
    form_data->chunk_data[DESCRIPTION].len = 0;
    form_data->chunk_data[DESCRIPTION].mime = TEXT_PLAIN;
    form_data->chunk_data[PICTURE_DATA].data = NULL;
    form_data->chunk_data[PICTURE_DATA].len = 0;
    form_data->chunk_data[PICTURE_DATA].mime = TEXT_PLAIN;
    form_data->chunk_data[IMAGENUMBER].data = NULL;
    form_data->chunk_data[IMAGENUMBER].len = 0;
    form_data->chunk_data[IMAGENUMBER].mime = TEXT_PLAIN;
    form_data->status = OK;
}


void free_form_data(struct form_data * form_data)
{
    register int i=0;
    for (i = 0; i < 4; i++)
    {
        char *data = form_data->chunk_data[i].data;
        if (data != NULL)
            free(data);
    }
}


int iUpload(int argc, const char * argv[],char *cFilename,
		char *cTitle,char *cDescription,unsigned *uptrImageNumber)
{

    unsigned uRetVal=0;

    if (!check_request_method("POST"))
    {
        uRetVal=1;//not post
        goto fail_request;
    }

    if (!check_content_type("multipart/form-data"))
    {
        uRetVal=2;//not multipart/form-data
        goto fail_content;
    }

    char *boundary = get_boundary();
    if (boundary == NULL)
    {
        uRetVal=3;//no boundary
        goto fail_boundary;
    }

    long length = get_content_length();
    if (length == 0 || length > uMAXIMAGESIZE)
    {
        uRetVal=4;//Too large or no length
        goto fail_length;
    }

    char *input = malloc(length);
    if (input == NULL)
    {
        uRetVal=5;//malloc error
        goto fail_input;
    }

    size_t read_bytes = fread(input, 1, length, stdin);
    if (read_bytes < length)
    {
        uRetVal=6;//read error
        goto fail_read;
    }

    // Init struct for form data
    struct form_data form_data;
    init_form_data(&form_data);

    // Setup multipart parser callbacks
    multipart_parser_settings callbacks;
    memset(&callbacks, 0, sizeof(multipart_parser_settings));
    callbacks.on_header_value = read_header_value;
    callbacks.on_part_data = read_data_part;
    callbacks.on_body_end = read_multipart_end;

    multipart_parser* parser = multipart_parser_init(boundary, &callbacks);
    multipart_parser_set_data(parser, &form_data);
    multipart_parser_execute(parser, input, length);
    multipart_parser_free(parser);

    //Ok?
    if (form_data.status == NOT_OK ||
        (form_data.chunk_data[PICTURE_DATA].mime != IMG_PNG && form_data.chunk_data[PICTURE_DATA].mime != IMG_JPG) )
    {
        uRetVal=7;//not a valid image format
        goto fail_parse;
    }

    free(input);
    free(boundary);

    int epoch = (int)time(NULL);
    char filename[20] = { 0 };
    if (form_data.chunk_data[PICTURE_DATA].mime == IMG_JPG)
    {
        sprintf(filename, "%d.%s", epoch, "jpg");
    }
    else if (form_data.chunk_data[PICTURE_DATA].mime == IMG_PNG)
    {
        sprintf(filename, "%d.%s", epoch, "png");
    }

    char *file_path = malloc(strlen(PICTURE_FILE_PATH) + strlen(filename) + 1);
    sprintf(file_path, "%s%s", PICTURE_FILE_PATH, filename);
    FILE *out = fopen(file_path, "wb+");
    if (out == NULL)
    {
        uRetVal=8;//fopen error
        goto fail_open_file;
    }

    size_t out_bytes = fwrite(form_data.chunk_data[PICTURE_DATA].data, 1,
                              form_data.chunk_data[PICTURE_DATA].len, out);
    if (out_bytes != form_data.chunk_data[PICTURE_DATA].len)
    {
        uRetVal=9;//fwrite error
        goto fail_write_out;
    }
    fclose(out);


    char cFormat[128];
    sprintf(cFilename,"%.99s",filename);

    sprintf(cFormat,"%%.%lus",form_data.chunk_data[TITLE].len);
    sprintf(cTitle,cFormat,form_data.chunk_data[TITLE].data);
    sscanf(form_data.chunk_data[IMAGENUMBER].data,"%u ",uptrImageNumber);

    if(form_data.chunk_data[DESCRIPTION].len<511)
    	sprintf(cFormat,"%%.%lus",form_data.chunk_data[DESCRIPTION].len);
    else
    	sprintf(cFormat,"%%.511s");
    sprintf(cDescription,cFormat,form_data.chunk_data[DESCRIPTION].data);

    free(file_path);
    free_form_data(&form_data);
    return 0; //Done ok

    //Exit points for non nesting mess
fail_write_out:
fail_open_file:
    if (file_path != NULL) free(file_path);
fail_parse:
    free_form_data(&form_data);
fail_read:
fail_input:
    if (input != NULL) free(input);
fail_length:
fail_boundary:
    if (boundary != NULL) free(boundary);
fail_content:
fail_request:
    return uRetVal;

}//iUpload()
