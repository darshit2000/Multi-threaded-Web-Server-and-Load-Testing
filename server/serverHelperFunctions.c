#include <stdio.h>
#include "serverVariables.c"

int isSuccess(int fd, char *msg);
char *readFileContent(char url[]);
struct HTTP_Request getRequest(char buffer[BUFFER_SIZE]);
struct HTTP_Response getResponse(struct HTTP_Request request);
char* convertToString(struct HTTP_Response response);
void serviceClient(long fd);
void *threadPoolFunction(void *arg);

struct HTTP_Request {
  char HTTP_version[100];
  char method[100];
  char url[100];
};

struct HTTP_Response {
  char HTTP_version[100]; 
  char status_code[100]; 
  char status_text[100]; 
  char content_type[100];
  char content_length[100];
  char body[BUFFER_SIZE];
};

int isSuccess(int fd, char *msg) {
	if(fd < 0) {
		perror(msg);
		exit(1);
	} else {
		return fd;
	}
}

char *readFileContent(char url[]){
    struct stat sb;
    char *data = (char *)malloc(BUFFER_SIZE*sizeof(char));
    FILE* file = fopen(url, "r");

    if (file == NULL) {
        perror("Please provide valid file URL.\n");
    }

    stat(url,&sb);
    int remaining = sb.st_size;
    char str[remaining];

    while (fgets(str, remaining, file) != NULL) {
        strncat(data, str, remaining);
    }

    fclose(file);
    return data;
}

struct HTTP_Request getRequest(char buffer[BUFFER_SIZE]){
    long tokenNumber = 0;
    struct HTTP_Request request;

	char *ptr = strtok(buffer, "\n");
	while(ptr != NULL && tokenNumber < 3)
	{
        char *token = strtok(ptr, " ");
        while(token != NULL){
            if (tokenNumber == 0) {
                strcpy(request.method, token);
            }
            if (tokenNumber == 1) {
                strcpy(request.url, token);
            }
            tokenNumber = tokenNumber + 1;
            token = strtok(NULL, " ");
        }
		ptr = strtok(NULL, " ");
	}
    return request;
}

struct HTTP_Response getResponse(struct HTTP_Request request){
    struct HTTP_Response response;
    char data[BUFFER_SIZE];
    char httpVersion[100] = "HTTP/1.1";

    if(strcmp(request.method, "GET") == 0){
        struct stat stats;
        char url[BUFFER_SIZE];

        strcpy(url, "./htmlFiles");

        strncat(url, request.url, strnlen(request.url, BUFFER_SIZE));
        
        if(stat(url, &stats) == 0) {
            if (S_ISDIR(stats.st_mode)) {
                strcat(url, "/index.html");
            }

            char *content = readFileContent(url);
            strcpy(data, content);
            free(content);

            strcpy(response.HTTP_version, httpVersion);
            strcpy(response.status_code, "200");
            strcpy(response.status_text, "OK");
            strcpy(response.content_type,"text/html");
            sprintf(response.content_length, "%d", (int)strnlen(data, BUFFER_SIZE));
            strcpy(response.body, data);
            

        } else { 
            char *content = readFileContent("./htmlFiles/404.html");
            strcpy(data, content);
            free(content);

            strcpy(response.HTTP_version, httpVersion);
            strcpy(response.status_code, "404");
            strcpy(response.status_text, "Not Found");
            strcpy(response.content_type,"text/html");
            sprintf(response.content_length, "%d", (int)strnlen(data, BUFFER_SIZE));
            strcpy(response.body, data);
            
        }   

    } else {
        char *content = readFileContent("./htmlFiles/500.html");
        strcpy(data, content);
        free(content); 

        strcpy(response.HTTP_version, httpVersion);
        strcpy(response.status_code, "500");
        strcpy(response.status_text, "Unsupported Format");
        strcpy(response.content_type,"text/html");
        sprintf(response.content_length, "%d", (int)strnlen(data, BUFFER_SIZE));
        strcpy(response.body, data);
        
    }
    return response;
}

char* convertToString(struct HTTP_Response response){
    char *finalResponseString = (char *)malloc(BUFFER_SIZE*sizeof(char));
    bzero(finalResponseString, BUFFER_SIZE);

    strcpy(finalResponseString, response.HTTP_version);
    strcat(finalResponseString, " ");
    strcat(finalResponseString, response.status_code);
    strcat(finalResponseString, " ");
    strcat(finalResponseString, response.status_text);
    strcat(finalResponseString, " ");
    strcat(finalResponseString, "\r\n");

    strcat(finalResponseString, "Content-Type: ");
    strcat(finalResponseString, response.content_type);
    strcat(finalResponseString, "\r\n");
    
    strcat(finalResponseString, "Content-Length: ");
    strcat(finalResponseString, response.content_length);
    strcat(finalResponseString, "\r\n\n");

    strcat(finalResponseString, response.body);

    return finalResponseString;
}

void serviceClient(long fd) {
    long n;
    char buffer[BUFFER_SIZE];
    long client_socket_fd = fd;
    struct HTTP_Request http_request;
    struct HTTP_Response http_response;
    char *finalResponse;

    bzero(buffer, BUFFER_SIZE);

    if((n = read(client_socket_fd, (void *)buffer, BUFFER_SIZE)) < 0) {
        perror("Error in reading from client socket.\n");
    }
    
    http_request = getRequest(buffer);
    
    http_response = getResponse(http_request);

    finalResponse = convertToString(http_response);

    write(client_socket_fd, finalResponse, strnlen(finalResponse, BUFFER_SIZE));
    
    free(finalResponse);
    close(client_socket_fd);
}

void *threadPoolFunction(void *arg){
    long k = *((long *)arg);
    long client_socket_fd = -1;

    while(1){
        pthread_mutex_lock(&mutex);
        while(FileDescripterQueueSize == 0){
            pthread_cond_wait(&fill, &mutex);
        }
        for(int i = 0; i < MAX_FD_ARRAY_SIZE; i++) {
            if(FileDescripterQueue[i] != -1){
                client_socket_fd = FileDescripterQueue[i];
                FileDescripterQueue[i] = -1;
                FileDescripterQueueSize--;
                break;
            }
        }
        counter++;

        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);

        serviceClient(client_socket_fd);
    }
}