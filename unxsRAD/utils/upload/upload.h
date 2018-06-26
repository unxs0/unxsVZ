/*
FILE
	upload.h
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
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>


typedef struct multipart_parser multipart_parser;
typedef struct multipart_parser_settings multipart_parser_settings;
typedef struct multipart_parser_state multipart_parser_state;

typedef int (*multipart_data_cb) (multipart_parser*, const char *at, size_t length);
typedef int (*multipart_notify_cb) (multipart_parser*);

struct multipart_parser_settings {
  multipart_data_cb on_header_field;
  multipart_data_cb on_header_value;
  multipart_data_cb on_part_data;

  multipart_notify_cb on_part_data_begin;
  multipart_notify_cb on_headers_complete;
  multipart_notify_cb on_part_data_end;
  multipart_notify_cb on_body_end;
};

multipart_parser* multipart_parser_init
    (const char *boundary, const multipart_parser_settings* settings);

void multipart_parser_free(multipart_parser* p);

size_t multipart_parser_execute(multipart_parser* p, const char *buf, size_t len);

void multipart_parser_set_data(multipart_parser* p, void* data);
void * multipart_parser_get_data(multipart_parser* p);



struct multipart_chunk_data {
    char *data;
    size_t len;

    int mime;
};

struct form_data {
    struct multipart_chunk_data chunk_data[3];
    int form_element;

    int status;
};

enum {
    TITLE,
    DESCRIPTION,
    PICTURE_DATA
};

enum {
    OK,
    NOT_OK
};

enum {
    IMG_JPG,
    IMG_PNG,
    TEXT_PLAIN
};

// Naive implementation of a fixed length strstr
char *sstrstr(const char *haystack, const char *needle, size_t length);

// Make sure the request method and content type match
int check_request_method(const char *method);
int check_content_type(const char *mime);

// Returns 0 if content length is missing
long get_content_length();

// returns the multipart boundary string, caller must free when finished
char *get_boundary();

// Multipart parser callback functions
int read_header_value(multipart_parser* p, const char *at, size_t length);
int read_data_part(multipart_parser* p, const char *at, size_t length);
int read_multipart_end(multipart_parser* p);

// Init and free form data
void init_form_data(struct form_data * form_data);
void free_form_data(struct form_data * form_data);

// Main
int iUpload(int argc, const char * argv[],char *cFilename,char *cTitle,char *cDescription);

