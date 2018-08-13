#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define N 4096

#define log(info, str) \
	do{\
	    fprintf(tmp, "%s%s", info, str);\
		fflush(tmp);\
	  }while(0)

void send_headers(char *type)
{
	printf("%s, %d, %s\r\n", "HTTP/1.1", 200,  "OK");
	printf("Content-Type: %s\r\n", type);
	printf("Connection: close\r\n");
	printf("\r\n");

	return ;
}

void send_err(int status, char *title, char *text)
{
	send_headers("text/html");
	printf("<html><head><title>%d %s</title></head>\n", status, title);
	printf("<body bgcolor=\"#cc99cc\"><h4>%d %s</h4>\n", status, title);
	printf("%s\n", text);
	printf("<hr>\n</body>\n</html>\n");

	fflush(stdout);

	exit(1);
}

int main(int argc, char *argv[])
{
	char line[N];
	char method[N], path[N], protocol[N];
	char *file;
		
	struct stat sb;
	FILE * fp;
	int ich;
	char *type;

	if(argc != 2)
		send_err(500, "Internal Error", "config error - no dir specified.");
	if(chdir(argv[1]) < 0)
		send_err(501, "Internal Error", "config error - couldnt chdir...");
	if(fgets(line, sizeof(line), stdin) == NULL )	
		send_err(400, "Bad request", "No request found");
	if(sscanf(line, "%[^ ] %[^ ] %[^ ]", method, path, protocol) != 3)
		send_err(400, "Bad request", "cant parse request");
	while(fgets(line, sizeof(line), stdin) != NULL)
		if(strcmp(line, "\n") == 0 || strcmp (line, "\r\n") == 0)
			break;
	
	if(strcasecmp("GET", method) != 0)
		send_err(503, "Not Implemented", "That method is not implemented");

	file = path+1;
		
	#if 1
		FILE *tmp = fopen("/home/lm/program/xhttpd/tmp.txt", "a");
		if(tmp == NULL)
		{
			perror("fopen tmp.txt error!");
			exit(1);
		}
		log("path+1", path+1);
		fclose(tmp);
	#endif
	
	if (stat(file, &sb) < 0)
		send_err(404, "NOT found", "file not found.");

		fp = fopen(file, "r");
		if(fp == NULL)
			send_err(403, "forbidden", "file is protected.");

		char *dot = strrchr(file, '.');
		
		if(dot == NULL)
			type = "text/plain; charset=utf-8";
		else if(strcmp(dot,".html") == 0)
			type = "text/html; charset=utf-8";
		else if(strcmp(dot, ".jpg") == 0)
			type = "image/jpeg";
		else if(strcmp(dot, ".gif") == 0)
			type = "image/gif";
		else if(strcmp(dot, ".png") == 0)
			type = "image/png";
		else if(strcmp(dot, ".mp3") == 0)
			type = "audio/mpeg";
			else
				type = "text/plain; charset=iso-8859-1";
	
	send_headers(type);
	
	while(ich != EOF)
	{
		ich = getc(fp);
		putchar(ich);
	}
	fflush(stdout);

	fclose(fp);

	return 0;
}




