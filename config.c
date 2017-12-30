#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int load_config(const char *path, struct cfg_info *cfg)
{
	int cfg_fd = 0;
	char cfg_buf[1024];	

	// check config file path
	if(path == NULL) {
		fprintf(stderr, "exception. config file path is null\n");
		return -1;
	}

	if(cfg == NULL) {
		fprintf(stderr, "exception. config info is null\n");
		return -1;
	}

	// config file open & read
	cfg_fd = open(path, O_RDONLY);
	if(cfg_fd != 0) {
		ssize_t read_len = 0;

		read_len = read(cfg_fd, cfg_buf, 1024);
		if(read_len < 0) {
			perror("Error on read config file");
		}
	}

	// parse config file
	if(parse_config(cfg_buf, read_len, cfg) != 0) {
		fprintf(stderr, "Failed to parse config file\n");
		return -1;
	}

}

int parse_config(char *buf, size_t len, struct cfg_info *cfg)
{
	
}
