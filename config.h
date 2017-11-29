#ifndef CONFIG_H_
#define CONFIG_H_

#define DEFAULT_CFG_PATH "./config"

struct cfg_info
{
	char timestamp_fmt[20];
	size_t len;
	char http_server_ip[16];
	short http_server_port;
};

int load_config(const char *path, struct cfg_info *cfg);
int parse_config(char *buf, size_t len, struct cfg_info *cfg);

#endif
