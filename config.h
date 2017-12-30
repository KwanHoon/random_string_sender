#define DEFAULT_CONFIG_PATH


struct cfg_info
{
	char *timestamp_fmt;
	size_t len;
	size_t interval;
	char http_server_ip[16];
	short http_port;
}

int load_config(const char *path, struct cfg_info *cfg);
int parse_config(char *buf, size_t len, struct cfg_info *cfg);
