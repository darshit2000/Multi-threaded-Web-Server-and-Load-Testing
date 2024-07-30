#include <stdio.h>

void ctrlC_Handler(int sig);

void ctrlC_Handler(int sig){
     printf("\nServer Exiting...\n");
     close(server_socket_fd);
     for(int i=0 ;i<THREAD_POOL_SIZE; i++){
         pthread_kill(threadPool[i], SIGKILL);
     }
     for(int i=0 ;i<THREAD_POOL_SIZE; i++){
         pthread_join(threadPool[i], NULL);
     }
    printf("\nServer Exited...\n");
    exit(0);
}