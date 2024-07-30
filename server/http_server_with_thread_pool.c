#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include "serverHelperFunctions.h"
#include "interuptHandler.h"

int main(int argc, char const* argv[]){
    struct sockaddr_in addr;
    struct sockaddr_in new_client_addr;
    socklen_t new_client_addr_length;
    long new_socket_fd;
    long port = atol(argv[1]);

    signal(SIGINT, ctrlC_Handler);
    if(argc != 2){
        fprintf(stderr, "Error : Please provide the port number to proceed.\n");
        exit(1);
    }

    signal(SIGINT, ctrlC_Handler);

    for(int i = 0 ; i < MAX_FD_ARRAY_SIZE ; i++) {
        FileDescripterQueue[i] = -1;
    }

    // Create new socket
	isSuccess((server_socket_fd = socket(AF_INET, SOCK_STREAM, 0)), (char *)"Server Socket Creation Failed ");

    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
	// htons is used to convert host bytes to network byte so that it is correctly converted 
	// according to what the underlying architecture wants (if it is little endian or big endian)
    addr.sin_port = htons(port);

    // Socket Binding
	isSuccess((bind(server_socket_fd, (struct sockaddr *)&addr, sizeof(addr))), (char *)"Server Socket Binding Failed ");

    int id;
    //Thread Pool
    for(int i = 0 ; i < THREAD_POOL_SIZE ; i++) {
        pthread_t th;
        id = i;
        if(pthread_create(&th, NULL, threadPoolFunction, &id) != 0) {
                perror("pthread_create");
        }
        sleep(1);
        threadPool[i] = th;
        printf("Creating thread %d\n", id);
    }

 	// Socket Listen
	isSuccess((listen(server_socket_fd, MAX_FD_ARRAY_SIZE)), (char *)"Server Socket Listen Failed ");

	printf("\nServer Listening...\nPlease press Ctrl+C to exit the server.\n");

    new_client_addr_length = sizeof(new_client_addr);
    
    while(1){
        // If FD array is full
        while(FileDescripterQueueSize == MAX_FD_ARRAY_SIZE){
            pthread_cond_wait(&empty, &mutex);
        }
        
        // Socket Accept
		isSuccess((new_socket_fd = accept(server_socket_fd, (struct sockaddr *)&new_client_addr, &new_client_addr_length)), (char *)"Server Socket Accept Failed ");	

        // With Thread Pool
        pthread_mutex_lock(&mutex);

        for(int i = 0 ; i < MAX_FD_ARRAY_SIZE; i++){
            if(FileDescripterQueue[i] == -1){
                FileDescripterQueue[i] = new_socket_fd;
                FileDescripterQueueSize++;
                break;
            }
        }
        
        pthread_cond_signal(&fill);
        pthread_mutex_unlock(&mutex);

        signal(SIGINT, ctrlC_Handler);

        // Without thread pool
		/* 
		pthread_t tid;
		pthread_create(&tid, NULL, serviceClient, &new_socket_fd);
		*/
    }
    return 0;
}
