#include "converter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int init_converter(struct convert_t *converter, struct cfg_info *cfg)
{
	if(converter == NULL || cfg == NULL) {
		fprintf(stderr, "init value for converter is null\n");
		return -1;
	}
	
	memcpy(converter->http_server_ip, cfg->http_server_ip, 16);
	converter->http_server_port = cfg->http_server_port;

	converter->queue = createQueue(1000);

	return 0;
}


/* { 
 *    "string": "{random string}",
 *    "timestamp": "{timestamp}"
 * }
 */

char *make_kv(char *key, size_t klen, char *value, size_t vlen)
{
	char *kv_buf = NULL;
	char *kv_fmt = "\"%s\": \"%s\"";

	if(key == NULL || value == NULL) {
		fprintf(stderr, "key or value is null\n");
		return NULL;	
	}

	kv_buf = (char *)malloc(50);
	if(kv_buf == NULL) {
		perror("Failed alloc key value buffer");
		return NULL;

	}
	sprintf(kv_buf, kv_fmt, key, value);

	//fprintf(stderr, "[debug] key: value {%s}\n", kv_buf);

	return kv_buf;
}

void *convert_json(void *arg)
{
	struct convert_t *json_str = (struct convert_t *)arg;
	char json_str_key[] = "string";
	char json_tm_key[] ="timestamp";

	char *str_kv = NULL;
	char *tm_kv = NULL;
	char total_json[1024];
	char total_json_fmt[] = "{%s,%s}";
	//struct queue_node node;
	struct Element elem;
	struct str_with_ts *rand_str = NULL;

	// http
	char http_header[1024];
	char http_body[1024];
	char http_header_fmt[] = 
	"GET / HTTP/1.1\r\n\
	Host: pigh\r\n\
	User-Agent: pigh\r\n\
	Accept: application/json\r\n\
	Accept-Language: en-us,en;q=0.5\r\n\
	Accept-Charset: utf-8\r\n\
	Connection: keep-alive\r\n\
	Cache-Control: no-cache\r\n\
	Content-Length: %lu\r\n\r\n";

	int sock = 0;
	struct sockaddr_in srv;

	if(json_str == NULL) {
		fprintf(stderr, "json str is null\n");
		return NULL;
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0) {
		perror("Failed to create socket");
		return NULL;
	}
	memset(&srv, '0', sizeof(srv));

	srv.sin_addr.s_addr = inet_addr(json_str->http_server_ip);
	//srv.sin_addr.s_addr = inet_addr("127.0.0.1");
	srv.sin_family = AF_INET;
	srv.sin_port = htons(json_str->http_server_port);
	//srv.sin_port = htons(17250);

	if(connect(sock, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
		perror("Failed to connect server");
		fprintf(stderr, "ip: %s, port: %d\n", json_str->http_server_ip, json_str->http_server_port);
		return NULL;
	}

	fprintf(stderr, "  Connection Success [%s:%d]\n", 
	  json_str->http_server_ip, json_str->http_server_port);

	while(1) {
		if(isEmpty(json_str->queue) != 0) {
			memset(total_json, '\0', sizeof(total_json));	

			if((rand_str = (struct str_with_ts *)dequeue(json_str->queue)) == NULL)
			 continue;
			//rand_str = (struct str_with_ts *)elem.data;

			//fprintf(stderr, "[debug] str: %s, tm: %s\n", rand_str->str, rand_str->tm_str);
			str_kv = make_kv(
					json_str_key, sizeof(json_str_key) - 1,
					rand_str->str, rand_str->len);
			
			tm_kv = make_kv(
					json_tm_key, sizeof(json_tm_key) - 1,
					rand_str->tm_str, rand_str->tm_len);

			sprintf(total_json, total_json_fmt, str_kv, tm_kv);
			fprintf(stderr, "[debug] json str: %s\n", total_json);	

			//memcpy(http_body, total_json, strlen(total_json));

			// send http message
			sprintf(http_header, http_header_fmt, strlen(total_json));
			if(write(sock, http_header, strlen(http_header), 0) < 0) {
				perror("Failed to send header");
			}
			else {
				if(write(sock, total_json, strlen(total_json)) < 0) {
					perror("Failed to send body");
				}
				fprintf(stderr, "send msg: %s%s\n", http_header, total_json); 
			}

			free(str_kv);
			free(tm_kv);
		}
	}
}
