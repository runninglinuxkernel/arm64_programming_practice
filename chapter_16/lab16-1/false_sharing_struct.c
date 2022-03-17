#include <sys/times.h>
#include <time.h>
#include <stdio.h> 
#include <pthread.h>

struct data_with_false_sharing {
	unsigned long x;
	unsigned long y;
} cacheline_aligned;

#define cacheline_aligned __attribute__((__aligned__(64)))

struct padding {
	char x[0];
} cacheline_aligned;

struct data_wo_false_sharing {
	unsigned long x;
	struct padding _pad;
	unsigned long y;
} cacheline_aligned;

#define MAX_LOOP 1000000000

void *access_data(void *param)
{
	unsigned long *data = (unsigned long *)param;
	unsigned long i;
	unsigned long var;

	for (i = 0; i < MAX_LOOP; i++) {
		var = *data;
		*data += i;
	}
}

int main(void)
{
	struct data_with_false_sharing data0;
	struct data_wo_false_sharing data1;
	pthread_t thread_1;
	pthread_t thread_2;
	unsigned long total_time;

	struct timespec time_start, time_end;

	clock_gettime(CLOCK_REALTIME,&time_start);
	pthread_create(&thread_1, NULL, &access_data, (void*)&data0.x);
	pthread_create(&thread_2, NULL, &access_data, (void*)&data0.y);
	pthread_join(thread_1, NULL);
	pthread_join(thread_2, NULL);
	clock_gettime(CLOCK_REALTIME,&time_end);

	total_time = (time_end.tv_sec - time_start.tv_sec)*1000 +
		(time_end.tv_nsec - time_start.tv_nsec)/1000000;

	printf(" cache false sharing: %lu ms \n", total_time);

	clock_gettime(CLOCK_REALTIME,&time_start);
	pthread_create(&thread_1, NULL, &access_data, (void*)&data1.x);
	pthread_create(&thread_2, NULL, &access_data, (void*)&data1.y);
	pthread_join(thread_1, NULL);
	pthread_join(thread_2, NULL);
	clock_gettime(CLOCK_REALTIME,&time_end);

	total_time = (time_end.tv_sec - time_start.tv_sec)*1000 +
		(time_end.tv_nsec - time_start.tv_nsec)/1000000;

	printf(" cache without false sharing: %lu ms \n", total_time);

	return 0;
}


