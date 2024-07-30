#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include "loadGenHelperFunctions.c"

int main(int argc, char *argv[]) {
	long user_count; 
	long portno;
	long test_duration;
	double think_time;
	char *hostname;

	if (argc != 6) {
		fprintf(stderr,
				"Usage: %s <hostname> <server port> <number of concurrent users> "
				"<think time (in s)> <test duration (in s)>\n",
				argv[0]);
		exit(0);
	}

	hostname = argv[1];
	portno = atol(argv[2]);
	user_count = atol(argv[3]);
	think_time = atof(argv[4]);
	test_duration = atol(argv[5]);

	printf("Hostname: %s\n", hostname);
	printf("Port: %ld\n", portno);
	printf("User Count: %ld\n", user_count);
	printf("Think Time: %lf seconds\n", think_time);
	printf("Test Duration: %ld seconds\n", test_duration);

	log_file = fopen("load_gen.log", "w");

	pthread_t threads[user_count];
	struct user_info userInfo[user_count];
	struct timeval startTimer, endTimer;

	gettimeofday(&startTimer, NULL);

	time_up = 0;
	int iden;
	for (int i = 0; i < user_count; i++) {
		iden = i;
		userInfo[i].id = iden;
		userInfo[i].portno = portno;
		userInfo[i].think_time = think_time;
		userInfo[i].hostname = hostname;
		userInfo[i].total_count = 0;
		userInfo[i].total_rtt = 0.0;

		pthread_create(&threads[i], NULL, threadFunction, &userInfo[i]);

	}

	sleep(test_duration);

	pthread_mutex_lock(&mutex);
	time_up = 1;
	pthread_mutex_unlock(&mutex);

	gettimeofday(&endTimer, NULL);

	for(int i = 0; i < user_count; ++i) {
		pthread_join(threads[i], NULL);
	}

	long totalRequests = 0;
	double totalRtt = 0.0;
	
	for(int i = 0; i < user_count; ++i) {
		totalRequests = totalRequests + userInfo[i].total_count;
        totalRtt = totalRtt + userInfo[i].total_rtt;
	}

	float averageThroughput = ((totalRequests*1.0)/(test_duration*1.0));
	float averageRtt = ((totalRtt*1.0)/(totalRequests*1.0));

	fprintf(log_file,"Test Duration : %ld", test_duration);
	fprintf(log_file,"\nTotal Requests : %ld", totalRequests);
	fprintf(log_file,"\nTotal RTT : %lf", totalRtt);
    fprintf(log_file, "\nTotal number of users : %ld", user_count);
    fprintf(log_file, "\nAverage Throughput of Server : %.2f", averageThroughput);
    fprintf(log_file, "\nAverage Response Time of Server : %.5f", averageRtt);

	printf("\nTest Duration : %ld", test_duration);
	printf("\nTotal Requests : %ld", totalRequests);
	printf("\nTotal RTT : %lf", totalRtt);
	printf("\nTotal number of users : %ld", user_count);
    printf("\nAverage Throughput of Server : %.2f", averageThroughput);
    printf("\nAverage Response Time of Server : %.6f", averageRtt);

	fclose(log_file);

	return 0;
}
