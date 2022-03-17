#include <sys/times.h>
#include <time.h>
#include <stdio.h> 
#include <pthread.h>

#define MAX_LOOP 100000000

unsigned long data[100];

void *access_data_array(void *param)
{
	int index = *((int *)param);
	printf("%s, %u\n", __func__, index);

	unsigned long i;

	for (i = 0; i < MAX_LOOP; i++)
		data[index] += 1;
}

int main(void)
{
	pthread_t thread_1;
	pthread_t thread_2;
	unsigned long total_time;

	struct timespec time_start, time_end;

	int start = 0, end = 1;

	clock_gettime(CLOCK_REALTIME,&time_start);
	pthread_create(&thread_1, NULL, &access_data_array, (void*)&start);
	pthread_create(&thread_2, NULL, &access_data_array, (void*)&end);
	pthread_join(thread_1, NULL);
	pthread_join(thread_2, NULL);
	clock_gettime(CLOCK_REALTIME,&time_end);

	total_time = (time_end.tv_sec - time_start.tv_sec)*1000 +
		(time_end.tv_nsec - time_start.tv_nsec)/1000000;

	printf(" cache false sharing: %lu ms \n", total_time);

	end = 90;

	clock_gettime(CLOCK_REALTIME,&time_start);
	pthread_create(&thread_1, NULL, &access_data_array, (void*)&start);
	pthread_create(&thread_2, NULL, &access_data_array, (void*)&end);
	pthread_join(thread_1, NULL);
	pthread_join(thread_2, NULL);
	clock_gettime(CLOCK_REALTIME,&time_end);

	total_time = (time_end.tv_sec - time_start.tv_sec)*1000 +
		(time_end.tv_nsec - time_start.tv_nsec)/1000000;

	printf(" cache without false sharing: %lu ms \n", total_time);

	return 0;

}


