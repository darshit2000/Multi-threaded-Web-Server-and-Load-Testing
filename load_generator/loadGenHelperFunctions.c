#include <stdio.h>
#include "loadGenVariables.c"

int isSuccess(int fd, char *msg);
void *threadFunction(void *arg);

struct user_info {
  long id;
  long portno;
  char *hostname;
  double think_time;
  long total_count;   // total requests send by each user in test duration
  double total_rtt;   // total time take
};

int isSuccess(int fd, char *msg) {
	if(fd < 0) {
		perror(msg);
		exit(1);
	} else {
		return fd;
	}
}

void *threadFunction(void *arg) {
	struct user_info *userInfo = (struct user_info *)arg;

	int n;
	char buffer[256] = "GET /apart3 HTTP/1.0\r\nHOST: localhost\r\nContent-Type: text/plain\r\n\r\n";
	char response[4096];
	struct timeval startTimer, endTimer;

	struct sockaddr_in server_addr;
	struct hostent *server;

	int client_socket_fd;
	userInfo->total_count = 0;
	userInfo->total_rtt = 0.0;

	float totalRtt = 0;
	
	while (1) {
		pthread_mutex_lock(&mutex);
		if (time_up){
			pthread_mutex_unlock(&mutex);
			break;
		}
		pthread_mutex_unlock(&mutex);

		gettimeofday(&startTimer, NULL);

		// create socket
		isSuccess((client_socket_fd = socket(AF_INET, SOCK_STREAM, 0)),"Client Socket Creation Failed ");

		server_addr.sin_family = AF_INET;
    	server_addr.sin_port = htons(userInfo->portno);
    	isSuccess((inet_pton(AF_INET, userInfo->hostname, &server_addr.sin_addr)),"Invalid Server Address ");

		// connect to server
		isSuccess((connect(client_socket_fd,(struct sockaddr *)&server_addr, sizeof(server_addr))),"Connection to server failed ");

		// write to server
		isSuccess((n = write(client_socket_fd, buffer, strlen(buffer))),"Error in writing to the server ");

		// read from server
        isSuccess((n = read(client_socket_fd, response, sizeof(buffer))),"Reading from server failed ");

		// close socket
		close(client_socket_fd);

		gettimeofday(&endTimer, NULL);

		userInfo->total_count++;
	
	    long seconds = endTimer.tv_sec - startTimer.tv_sec;
        long microseconds = endTimer.tv_usec - startTimer.tv_usec;
        double rtt = seconds + microseconds*1e-6;
        
        userInfo->total_rtt = userInfo->total_rtt + rtt;

		usleep(((userInfo->think_time) * 1000000));
	}

	fflush(log_file);
	pthread_exit(NULL);
}
