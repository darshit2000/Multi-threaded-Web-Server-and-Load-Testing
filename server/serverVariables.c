#define THREAD_POOL_SIZE 5
#define MAX_FD_ARRAY_SIZE 30000
#define BUFFER_SIZE 4096

long server_socket_fd;
pthread_t threadPool[THREAD_POOL_SIZE];

long FileDescripterQueue[MAX_FD_ARRAY_SIZE];
long FileDescripterQueueSize = 0;
long counter = 0;

pthread_mutex_t mutex;
pthread_cond_t empty, fill;