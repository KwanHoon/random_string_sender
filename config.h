#ifndef CONFIG_H_
#define CONFIG_H_

#define DEFAULT_CFG_PATH "./config"

struct cfg_info
{
	char timestamp_fmt[20];
	size_t len;
	char host[1024];
	short port;
	size_t str_int;
	size_t send_int_time;
	size_t send_int_count;
};

int load_config(const char *path, struct cfg_info *cfg);

#endif
