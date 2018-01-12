#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

int load_config(const char *path, struct cfg_info *cfg)
{
	char *cfg_path = NULL;
	int cfg_fd = 0;
	char cfg_buf[1024];	

	FILE *cfg_file = NULL;
	size_t len = 0;
	ssize_t read_len = 0;
	char *line = NULL;

	char *vcfg = NULL;

	// check config file path
	if(path == NULL)
		cfg_path = DEFAULT_CFG_PATH;	
	else
		cfg_path = path;

	if(cfg == NULL) {
		fprintf(stderr, "exception. config info is null\n");
		return -1;
	}

	cfg_file = fopen(cfg_path, "r");
	if(cfg_file == NULL) {
		perror("Failed to open config file");
		return -1;
	}

	memset(cfg->host, '\0', sizeof(cfg->host));
	memset(cfg->hosturl, '\0', sizeof(cfg->hosturl));
	memset(cfg->hostparam, '\0', sizeof(cfg->hostparam));

	cfg->port = 0;

	while((read_len = getline(&line, &len, cfg_file)) != -1) {
		if((vcfg =strchr(line, '#') == NULL)) {
			//fprintf(stderr, "[debug] cfg_line: %s\n", line);	
			if((vcfg = strstr(line, "host=")) != NULL) {
				strcpy(cfg->host, vcfg + strlen("host="));
			}
			else if((vcfg = strstr(line, "url=")) != NULL) {
				strcpy(cfg->hosturl, vcfg + strlen("url="));
			}
			else if((vcfg = strstr(line, "param=")) != NULL) {
				strcpy(cfg->hostparam, vcfg + strlen("param="));
			}
			else if((vcfg = strstr(line, "port=")) != NULL) {
				cfg->port = atoi(vcfg + strlen("port="));
			}
			else if((vcfg = strstr(line, "length=")) != NULL) {
				cfg->len = atoi(vcfg + strlen("length="));
			}
			else if((vcfg = strstr(line, "timestamp_format=")) != NULL) {
				strcpy(cfg->timestamp_fmt, vcfg + strlen("timestamp_format="));
			}
			else if((vcfg = strstr(line, "interval=")) != NULL) {
				cfg->str_int = atoi(vcfg + strlen("interval="));
			}
			else if((vcfg = strstr(line, "send_time=")) != NULL) {
				cfg->send_int_time = atoi(vcfg + strlen("send_time="));
			}
			else if((vcfg = strstr(line, "send_count=")) != NULL) {
				cfg->send_int_count = atoi(vcfg + strlen("send_count="));
			}
		}
	}
	//fprintf(stderr, "[debug] http info %s:%d\n", cfg->host, cfg->port);

	fclose(cfg_file);
	if(line)
		free(line);

	return 0;
}
