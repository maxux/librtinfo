#ifndef __MISC_H
	#define __MISC_H
	
	#define rtinfo_debug(...) if(LIBRTINFO_DEBUG) { printf(__VA_ARGS__); }
	
	void diep(char *str);
	
	long long sum_line(char *line);
	long long indexll(char *line, int index);
	char * file_get(char *filename, char *data, size_t size);
	char * getinterfacename(char *line);
	char * skip_until_colon(char *str);
#endif
