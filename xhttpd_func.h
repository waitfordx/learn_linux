#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define SERVER_URL "www.baidu.com"
#define SERVER_NAME "xhttpd"
#define N 4096

 void send_headers(int status, char *title, char *extra_header,  char *mime_type, off_t length, time_t mod);
 void send_error(int status, char *title, char *extra_header, char *text);
 void strencode(char *to, size_t tosize, const char*from);
 void file_infos(char *dir, char *name);
 void strdecode(char *to, char *from);
 char *get_mime_type(char *name);
 int hexit(char c);
