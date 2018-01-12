#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "sender.h"

int init_regex(regex_t *regex)
{
	int reti = 0;

	if(regex == NULL) {
		fprintf(stderr, "regex is null\n");
		return -1;
	}

	reti = regcomp(regex, IP_MATCH, REG_EXTENDED);
	if(reti) {
		fprintf(stderr, "Could not compile regex\n");
		return -1;
	}

	return 0;
}

int match_ip(regex_t *regex, const char *ip)
{
	int reti = 0;
	int result = 0;
	char msg[100];

	if(regex == NULL) {
		fprintf(stderr, "regex is null\n");
		return -1;
	}	
	if(ip == NULL) {
		fprintf(stderr, "ip is null\n");
		return -1;
	}	

	reti = regexec(regex, ip, 0, NULL, 0);
	if(!reti)
		result = 0;
	else if(reti == REG_NOMATCH)
		result = -1;
	else {
		regerror(reti, regex, msg, sizeof(msg));
		fprintf(stderr, "regex failed: %s\n", msg);
		result = -1;
	}	

	regfree(regex);
	
	return result;	
}

int init_sender(struct sender_t *sender, struct cfg_info *cfg)
{
	if(sender == NULL || cfg == NULL) {
		fprintf(stderr, "Faeild to init sender.null pointer\n");
		return -1; }

	memset(sender->hosturl, '\0', sizeof(sender->hosturl));
	memset(sender->hostparam, '\0', sizeof(sender->hostparam));

	strcpy(sender->host, cfg->host);
	sender->host[strlen(cfg->host) - 1] = '\0';
	strncpy(sender->hosturl, cfg->hosturl, sizeof(cfg->hosturl));
	sender->hosturl[strlen(cfg->hosturl) - 1] = '\0';
	strncpy(sender->hostparam, cfg->hostparam, sizeof(cfg->hostparam));
	sender->hosturl[strlen(cfg->hostparam) - 1] = '\0';
	sender->port = cfg->port;
	sender->is_connected = 0;

	sender->queue = create_queue(MAX_QUEUE_SIZE);

	pthread_mutex_init(&sender->send_sync_mtx, NULL);
	pthread_cond_init(&sender->send_sync_cond, NULL);

	if(init_regex(&sender->ip_regex) != 0) {
		fprintf(stderr, "Failed to init regex\n");
		return -1;
	}

	return 0;
}


void *send_func(void *arg)
{
	struct sender_t *sender = (struct sender_t *)arg;
	struct str_with_tm_t send_str;

	// http
	//const char *ip_match = "([0-9])\.([0-9])\.([0-9])\.([0-9])";	
	char http_header[HTTP_HEADER_SIZE];
	char http_url[HTTP_URL_SIZE];
	char http_header_fmt[] = 
	"POST %s HTTP/1.1\r\n"
	"Host: %s:%d\r\n"
	"User-Agent: sender\r\n"
	"Accept: text/plain\r\n"
	"Accept-Language: en-us,en;q=0.5\r\n"
	"Accept-Charset: utf-8\r\n"
	"Connection: keep-alive\r\n"
	"Cache-Control: no-cache\r\n"
	"Content-Length: %lu\r\n\r\n";
	char recv_buf[MAX_RECVBUF_SIZE];
	//char *recv_buf = NULL;
	size_t recv_len = 0;
	size_t total_cnt = 0;

	int sock = 0;
	int flags = 0;
	char *hostname;
	struct sockaddr_in srv;
	struct in_addr addr;
	struct hostent *ent;
	size_t h = 0;
	long *add = NULL;
	char host_ip[16];

	fprintf(stderr, "[debug] config host: %s\n", sender->host);

	// host is not ip address
	if(match_ip(&sender->ip_regex, sender->host) != 0) {
		ent = gethostbyname(sender->host);
		if(ent == NULL) {
			perror("Failed to get host name");
			return NULL;
		}

		fprintf(stderr, "[debug] host name: %s\n", ent->h_name);
		for(h = 0; ent->h_addr_list[h]; h++) {
			memcpy(&addr.s_addr, ent->h_addr_list[h], sizeof(addr.s_addr));
			inet_ntop(AF_INET, &addr, host_ip, sizeof(host_ip));
		}
	}
	else {
		strncpy(host_ip, sender->host, sizeof(sender->host));	
	}
	fprintf(stderr, "IP: %s\n", host_ip);

	// set host url and parameter
	memset(http_url, '\0', sizeof(http_url));
	if(sender->hosturl[0] != '\0')
		strncpy(http_url, sender->hosturl, strlen(sender->hosturl));
	else
		strcpy(http_url, "/");	

	if(sender->hostparam[0] != '\0')
		strncat(http_url, sender->hostparam, strlen(sender->hostparam));

	fprintf(stderr, "[debug] request url: %s\n", http_url);

	// create socket and connection
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0) {
		perror("Failed to create socket");
		return NULL;
	}
	memset(&srv, '0', sizeof(srv));

	srv.sin_addr.s_addr = inet_addr(host_ip);
	srv.sin_family = AF_INET;
	srv.sin_port = htons(sender->port);
	
	if(connect(sock, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
		perror("Failed to connect server");
		return NULL;
	}

	fprintf(stderr, "Connection Success [%s:%d]\n", sender->host, sender->port);
	sender->is_connected = 1;	

	// set non-block socket
	flags = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);

	// dequeue and send. that's all
	while(1) {
		pthread_mutex_lock(&sender->send_sync_mtx);
		pthread_cond_wait(&sender->send_sync_cond, &sender->send_sync_mtx);
		memset(&send_str, 0x00, sizeof(send_str));
		if(sender->queue->size) {
			send_str = dequeue(sender->queue);
			//fprintf(stderr, "recv from queue: \n%s\n", send_str.fullstr);
			//pthread_mutex_unlock(&sender->send_sync_mtx);

			memset(recv_buf, 0x00, MAX_RECVBUF_SIZE);

			// send http header
			sprintf(http_header, http_header_fmt, http_url, ent->h_name, sender->port, send_str.fulllen);
			if(write(sock, http_header, strlen(http_header)) < 0) {
				perror("Failed to send header");
			}
			else {
				if(write(sock, send_str.fullstr, send_str.fulllen) < 0) {
					perror("Failed to send body");
				}
				else {
					fprintf(stderr, "[debug] send: \n%s%s\n", http_header, send_str.fullstr);
					pthread_mutex_unlock(&sender->send_sync_mtx);
									
					total_cnt = 0;
					while(total_cnt < MAX_RECVBUF_SIZE) {
						recv_len = read(sock, &recv_buf[total_cnt], MAX_RECVBUF_SIZE - total_cnt);
						if(recv_len < 0 && errno == EAGAIN) {
							break;
						}
						else if(recv_len >= 0) {
							total_cnt += recv_len;	
						}	
					}
					fprintf(stderr, "[debug] recv: \n%s\n", recv_buf);	
				}
			}
		}
		else {
			pthread_mutex_unlock(&sender->send_sync_mtx);
		}
	}

	//free(recv_buf);
}
