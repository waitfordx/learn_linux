#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>
#include "xhttpd_func.h" 



#ifdef DEBUG
#define log(info, str)\
do{\
	fprintf(fp_tmp,"%s%s", info, str);\
	fflush(fp_tmp); \
}while(0)
#endif

int main(int argc, char *argv[])
{
	// define variable
	char line[N], method[N], path[N], protocol[N], location[N];
	char *file;
	struct stat sb;
	size_t len;
	int  n;
	int ich = 0;
	struct dirent **dl;	
	FILE *fp;

	if(argc != 2)
		send_error(500, "Internal Error", NULL, "config error - no dir specified.");
	
	if(chdir(argv[1]) < 0)
		send_error(500, "Internal Error", NULL, "config error - couldn't chdir()");
	
	// read the head of http protocol
	if(fgets(line, sizeof(line), stdin) == NULL)
		send_error(401, "Bad request", NULL, "NO request found");

	// parse the head of http protocol
	if(sscanf(line, "%[^ ] %[^ ] %[^ ]", method, path, protocol) != 3)
		send_error(402, "Bad request", NULL, "couldn't parse request");
	// reading data...
	while(fgets(line, sizeof(line), stdin) != NULL)
		if(strcmp(line, "\n") == 0 || strcmp(line, "\r\n") == 0)
			break;
	
	if(strcasecmp(method, "GET") != 0)
		send_error(501, "Not Implented", NULL, "that method is not implemented...");
	
	if(path[0] != '/')
		send_error(403, "Bad request", NULL, "Bad filename...");
	
	file = &(path[1]);
	
//	strdecode(file,file);

	if(file[0] == '\0')
		file="./";

	// judge if the filename is legal or illegal
	len = strlen(file);
	printf("the length of file is %d\n", len);
	if(file[0] == '/' || strcmp(file,"..") ==0 || strncmp(file, "../", 3) == 0 || strstr(file, "/../") != NULL || strcmp(&(file[len-3]),"/..") == 0)
		send_error(405, "Bad request", (char*)0 , "iiliagal dirctory!");

	if (stat(file, &sb) < 0)
		send_error(404, "Not found", (char*)0 , "File not found");

	// file is a directory	
	if(S_ISDIR(sb.st_mode))
	{
		if(file[len-1] != '/')
		{
			snprintf(location, sizeof(location), "Location: %s/", path);
			send_error(302, "Found", location, "Directories must end with a slash.");
		}
	
/*
	snprintf(idx, sizeof(idx), "%sindex.html", file);
	if(stat(idx, &sb) >= 0)
	{
		file = idx;
		goto do_file
	}
*/	
	send_headers(200, "OK", NULL, "text/html", -1, sb.st_mtime);
	printf("<html><head><title> Index of %s</title></head>\n<body bgcolor=\"#99cc99\"><h4>Index of %s</h4>\n<pre>\n", file, file);

	
 	 n = scandir(file, &dl, NULL, alphasort);
	if(n < 0)
		perror("scandir");
	else
		for( int i = 0; i < n; ++i)
			file_infos(file, dl[i]->d_name);

		printf("</pre>\n<hr>\n<address><a href=\"%s\">%s</a></address>\n</body></html>\n", SERVER_URL, SERVER_NAME);
}		
else
{




	fp = fopen("/home/lm/program/xhttpd1/test/te.c", "r");
	printf("%s\n", file);
	if (fp == NULL)
		send_error(403, "forbidden", (char*)0, "file is protected");
		
		send_headers(200, "OK", (char*)0, get_mime_type(file), sb.st_size, sb.st_mtime);


		while((ich=getc(fp)) != EOF)
			putchar(ich);


	//	fflush(stdout);
	
	fclose(fp);
	

}
	fflush(stdout);


	
	return 0;
		
	
 }












