#include "converter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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

/*
char *make_json_obj(char *kv)
{
	char *json_obj = NULL;

	if(kv == NULL) {
		fprintf(stderr, "kv is null\n");
		return NULL;
	}
	
	json_obj = (char *)malloc(strlen(kv) + 3);
	json_obj[0] = '{';
	memcpy(json_obj, kv, strlen(kv));
	json_obj[strlen(kv)] = '}';
	json_obj[strlen(kv) + 1] = '\0';

	fprintf(stderr, "[debug] json obj: {%s}\n", json_obj);
	
	return json_obj;
}
*/

void *convert_json(void *arg)
{
	struct convert_t *json_str = (struct convert_t *)arg;
	char json_str_key[] = "string";
	char json_tm_key[] ="timestamp";

	char *str_kv = NULL;
	char *tm_kv = NULL;
	char total_json[1024];
	char total_json_fmt[] = "{%s,%s}";
	struct queue_node node;
	struct str_with_ts *rand_str = NULL;

	// http
	char http_header[1024];
	char http_body[1024];
	char http_header_fmt[] = "POST / HTTP/1.1\r\n\
							  Host: pigh\r\n\
							  User-Agent: pigh\r\n\
							  Accept: application/json\r\n\
							  Accept-Language: en-us,en;q=0.5\r\n\
							  Accept-Charset: utf-8\r\n\
							  Connection: keep-alive\r\n\
							  Cache-Control: no-cache\r\n\
							  Content-Length: %lu\r\n\r\n";

	char http_server_ip[] = "127.0.0.1";		//temp
	short http_server_port = 8000;				//temp
	int sock = 0;
	struct sockaddr_in srv;

	if(json_str == NULL) {
		fprintf(stderr, "json str is null\n");
		return NULL;
	}
	//sprintf(http_header, http_header_fmt, http_server_ip); 

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0) {
		perror("Failed to create socket");
		return NULL;
	}

	srv.sin_addr.s_addr = inet_addr(http_server_ip);
	srv.sin_family = AF_INET;
	srv.sin_port = htons(http_server_port);

	if(connect(sock, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
		perror("Failed to connect server");
		return NULL;
	}

	fprintf(stderr, "Connection Success [%s:%d]\n", http_server_ip, http_server_port);

	while(1) {
		if(is_empty(json_str->queue) != 0) {
			memset(total_json, '\0', sizeof(total_json));	

			node = dequeue(json_str->queue);
			rand_str = (struct str_with_ts *)node.data;

			//fprintf(stderr, "[debug] str: %s, tm: %s\n", rand_str->str, rand_str->tm_str);
			str_kv = make_kv(
					json_str_key, sizeof(json_str_key) - 1,
					rand_str->str, rand_str->len);
			
			tm_kv = make_kv(
					json_tm_key, sizeof(json_tm_key) - 1,
					rand_str->tm_str, rand_str->tm_len);

			sprintf(total_json, total_json_fmt, str_kv, tm_kv);
			fprintf(stderr, "[debug] json str: %s\n", total_json);	

			memcpy(http_body, total_json, strlen(total_json));

			// send http message
			sprintf(http_header, http_header_fmt, strlen(total_json));
			if(write(sock, http_header, strlen(http_header), 0) < 0) {
				perror("Failed to send header");
			}
			else {
				if(write(sock, total_json, strlen(total_json)) < 0) {
					perror("Failed to send body");
				}
			}

			free(str_kv);
			free(tm_kv);
		}
	}
}
