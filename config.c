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
	cfg->port = 0;

	while((read_len = getline(&line, &len, cfg_file)) != -1) {
		fprintf(stderr, "[debug] cfg_line: %s\n", line);
		if(strstr(line, "host") != NULL) {
			if((vcfg = strchr(line, '=')) != NULL) {
				//memcpy(cfg->host, vcfg + 1, strlen(vcfg + 1));
				strcpy(cfg->host, vcfg + 1);
				//cfg->host[strlen(cfg->host)] = '\0';
			}
		}
		else if(strstr(line, "port") != NULL) {
			if((vcfg = strchr(line, '=')) != NULL)
				cfg->port = atoi(vcfg + 1);
		}
		else if(strstr(line, "length") != NULL) {
			if((vcfg = strchr(line, '=')) != NULL)
				cfg->len = atoi(vcfg + 1);
		}
		else if(strstr(line, "timestamp_format") != NULL) {
			if((vcfg = strchr(line, '=')) != NULL)
				memcpy(cfg->timestamp_fmt, vcfg + 1, sizeof(cfg->timestamp_fmt));
		}
		else if(strstr(line, "interval") != NULL) {
			if((vcfg = strchr(line, '=')) != NULL)
				cfg->interval = atoi(vcfg + 1);
		}
	}

	//fprintf(stderr, "[debug] http info %s:%d\n", cfg->host, cfg->port);
	//fprintf(stderr, "[debug] tm_fmt: %s\n", cfg->timestamp_fmt);

	fclose(cfg_file);
	if(line)
		free(line);
	return 0;
}
