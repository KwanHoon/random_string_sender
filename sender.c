#include "sender.h"

int init_sender(struct sender_t *sender, struct cfg_info *cfg)
{
	if(sender == NULL || cfg == NULL) {
		fprintf(stderr, "Faeild to init sender.null pointer\n");
		return -1;
	}

	strcpy(sender->host, cfg->host);
	sender->host[strlen(cfg->host) - 1] = '\0';
	sender->port = cfg->port;

	sender->queue = create_queue(MAX_QUEUE_SIZE);

	pthread_mutex_init(&sender->send_sync_mtx, NULL);
	pthread_cond_init(&sender->send_sync_mtx, NULL);

	return 0;
}


void *send_func(void *arg)
{
	struct sender_t *sender = (struct sender_t *)arg;

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
	char *json_str;

	int sock = 0;
	char *hostname;
	struct sockaddr_in srv;
	struct in_addr addr;
	struct hostent *ent;
	size_t h = 0;
	long *add = NULL;
	char host_ip[16];

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0) {
		perror("Failed to create socket");
		return NULL;
	}
	memset(&srv, '0', sizeof(srv));

	fprintf(stderr, "[debug] config host: %s\n", sender->host);
	ent = gethostbyname(sender->host);
	if(ent == NULL) {
		perror("Failed to get host name");
		return NULL;
	}

	fprintf(stderr, "[debug] host name: %s\n", ent->h_name);
	for(h = 0; ent->h_addr_list[h]; h++) {
		memcpy(&addr.s_addr, ent->h_addr_list[h], sizeof(addr.s_addr));
		inet_ntop(AF_INET, &addr, host_ip, sizeof(host_ip));
		fprintf(stderr, "IP: %s\n", host_ip);
	}
	
	srv.sin_addr.s_addr = inet_addr(host_ip);
	srv.sin_family = AF_INET;
	srv.sin_port = htons(converter->port);

	if(connect(sock, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
		perror("Failed to connect server");
		return NULL;
	}

	fprintf(stderr, "Connection Success [%s:%d]\n", sender->host, sender->port);
	
	// dequeue and send. that's all
	while(1) {
		pthread_mutex_lock(&sender->send_sync_mtx);
		pthread_cond_wait(&sender->send_sync_cond, &sender->send_sync_mtx);
		if((json_str = (char *)dequeue(sender->queue)) != NULL) {
			pthread_mutex_unlock(&sender->send_sync_mtx);

			// send http header
			sprintf(http_header, http_header_fmt, ent->h_name, converter->port, strlen(total_json));
			if(write(sock, http_header, strlen(http_header), 0) < 0) {
				perror("Failed to send header");
			}
			else {

			}
		}
		else {
			pthread_mutex_unlock(&sender->send_sync_mtx);
		}
	}
}
