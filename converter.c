#include "converter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

int init_converter(struct convert_t *converter, struct cfg_info *cfg)
{
	if(converter == NULL || cfg == NULL) {
		fprintf(stderr, "init value for converter is null\n");
		return -1;
	}
	
	memcpy(converter->host, cfg->host, strlen(cfg->host));
	converter->host[strlen(cfg->host) - 1] = '\0';
	converter->port = cfg->port;

	converter->queue = createQueue(5000);

	converter->is_connected = 0;
	//pthread_mutex_init(&converter->sync_mutex, NULL);
	//pthread_cond_init(&converter->sync_cond, NULL);

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
	"POST / HTTP/1.1\r\n"
	"Host: %s:%d\r\n"
	"User-Agent: sender\r\n"
	"Accept: text/plain\r\n"
	"Accept-Language: en-us,en;q=0.5\r\n"
	"Accept-Charset: utf-8\r\n"
	"Connection: keep-alive\r\n"
	"Cache-Control: no-cache\r\n"
	"Content-Length: %lu\r\n\r\n";
	char recv_buf[2048];
	size_t recv_len = 0;

	int sock = 0;
	char *hostname;
	struct sockaddr_in srv;
	struct in_addr addr;
	struct hostent *ent;
	size_t h = 0;
	long *add = NULL;
	char host_ip[16];

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

	fprintf(stderr, "[debug] config host: %s\n", json_str->host);
	ent = gethostbyname(json_str->host);
	if(ent == NULL) {
		perror("Failed to get host name");
		return NULL;
	}

	//pthread_mutex_lock(&json_str->sync_mutex);

	fprintf(stderr, "[debug] host name: %s\n", ent->h_name);
	for(h = 0; ent->h_addr_list[h]; h++) {
		memcpy(&addr.s_addr, ent->h_addr_list[h], sizeof(addr.s_addr));
		inet_ntop(AF_INET, &addr, host_ip, sizeof(host_ip));
		fprintf(stderr, "IP: %s\n", host_ip);
	}
	
	srv.sin_addr.s_addr = inet_addr(host_ip);
	srv.sin_family = AF_INET;
	srv.sin_port = htons(json_str->port);

	if(connect(sock, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
		perror("Failed to connect server");
		return NULL;
	}

	fprintf(stderr, "  Connection Success [%s:%d]\n", json_str->host, json_str->port);
	json_str->is_connected = 1;
	//pthread_cond_signal(&json_str->sync_cond);
	//fprintf(stderr, " singal to start make string\n");
	//pthread_mutex_unlock(&json_str->sync_mutex);
	//sleep(1);
	while(1) {
		//pthread_mutex_lock(&json_str->queue->mtx);
		//fprintf(stderr, "a\n");
		//if(isEmpty(json_str->queue) != 0) {
		if((rand_str = (struct str_with_ts *)dequeue(json_str->queue)) != NULL ) {
			memset(total_json, '\0', sizeof(total_json));	
			memset(recv_buf, 0x00, sizeof(recv_buf));

			//fprintf(stderr, "get string from queue\n");
			//rand_str = (struct str_with_ts *)dequeue(json_str->queue);
			//pthread_mutex_unlock(&json_str->queue->mtx);
			//if(rand_str == NULL) {
			 //fprintf(stderr, "bb\n");
			 //getchar();
			 //continue;
			//}

			/*
			if((rand_str = (struct str_with_ts *)dequeue(json_str->queue)) == NULL) {
				//fprintf(stderr, "dequque is null\n");	
			 continue;
			}
			*/
			//rand_str = (struct str_with_ts *)elem.data;

			//fprintf(stderr, "[debug] str: %s, tm: %s\n", rand_str->str, rand_str->tm_str);
			str_kv = make_kv(
					json_str_key, sizeof(json_str_key) - 1,
					rand_str->str, rand_str->len);
			
			tm_kv = make_kv(
					json_tm_key, sizeof(json_tm_key) - 1,
					rand_str->tm_str, rand_str->tm_len);

			sprintf(total_json, total_json_fmt, str_kv, tm_kv);
			//fprintf(stderr, "[debug] json str: %s\n", total_json);	

			//memcpy(http_body, total_json, strlen(total_json));

			// send http message
			sprintf(http_header, http_header_fmt, ent->h_name, json_str->port, strlen(total_json));
			if(write(sock, http_header, strlen(http_header), 0) < 0) {
				perror("Failed to send header");
			}
			else {
				if(write(sock, total_json, strlen(total_json)) < 0) {
					perror("Failed to send body");
				}
				fprintf(stderr, "[%lu]send: \n%s%s\n",strlen(http_header) + strlen(total_json), http_header, total_json); 
			}

			if((recv_len = read(sock, recv_buf, sizeof(recv_buf))) < 0 ) {
				perror("Failed to recv");
			}

			fprintf(stderr, "[%lu]recv: \n%s\n",recv_len,  recv_buf);

			free(str_kv);
			free(tm_kv);
		}
		else {
			//fprintf(stderr, "size: %d\n", json_str->queue->size);
			//fprintf(stderr, "      empty\n");
		}
	}

	fprintf(stderr, "[debug] thread is end\n");
}
