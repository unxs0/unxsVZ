#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#define PICTURE_FILE_PATH "/var/www/unxs/html/images/"

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
    NAME,
    MESSAGE,
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
int upload(int argc, const char * argv[],char *cFilename,char *cName,char *cMessage);

