#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>

#include <sched.h>

// !!!!!! This value is CPU-dependent !!!!!!

//#define LOOP_ITERATIONS_PER_MILLISEC 155000

#define LOOP_ITERATIONS_PER_MILLISEC 1782500
//#define LOOP_ITERATIONS_PER_MILLISEC 178250
//#define LOOP_ITERATIONS_PER_MILLISEC 193750

#define MILLISEC 	1000
#define MICROSEC	1000000
#define NANOSEC 	1000000000

/*   This is how KRIS tasks are specified 
In the file linux-2.6.24-kris/include/linux/sched.h
also in the file /usr/include/bits/sched.h (the user space scheduler file)
*/

double min_offset,max_offset; //seconds
double min_exec_time; //seconds
int max_exec_time; //seconds
double min_inter_arrival_time,max_inter_arrival_time; //seconds

unsigned int kris_id=1;
struct itimerval inter_arrival_time;

void burn_1millisecs() {
	unsigned long long i;
	for(i=0; i<LOOP_ITERATIONS_PER_MILLISEC; i++)
        ;
}

void burn_cpu(long milliseconds){

	long i;
	for(i=0; i<milliseconds; i++)
		burn_1millisecs();

}

void clear_sched_param_t(struct sched_param *param)
{
//	param->ticket = 0;
	param->sched_priority = 1;
}

void print_task_param(struct sched_param *param)
{
	printf("child pid %d\n", getpid());
	printf("param->sched_priority %u, max_exec_time %d\n", 
		param->sched_priority, max_exec_time);
}

void clear_signal_timer(struct itimerval *t)
{
/*	t->it_interval.tv_sec = 0;
	t->it_interval.tv_usec = 0;
	t->it_value.tv_sec = 0;
	t->it_value.tv_usec = 0;
*/
}

void set_signal_timer(struct itimerval *t,double secs)
{
/*	t->it_interval.tv_sec = 0;
	t->it_interval.tv_usec =0 ;
	t->it_value.tv_sec = (int)secs;
	t->it_value.tv_usec = (secs-t->it_value.tv_sec)*MICROSEC;
*/
}

void print_signal_timer(struct itimerval *t)
{
/*	printf("Interval: secs [%ld] usecs [%ld] Value: secs [%ld] usecs [%ld]\n",
                        t->it_interval.tv_sec,
                        t->it_interval.tv_usec,
                        t->it_value.tv_sec,
                        t->it_value.tv_usec);
*/
}

double get_time_value(double min, double max)
{
	if(min==max)
		return min;
	return (min + (((double)rand()/RAND_MAX)*(max-min)));
}

void start_task(int s)
{
	//printf("Task(%d) has just started\n", kris_id);
}

void do_work(int s)
{
	printf(">> [%s] Started in PID:%d\n",__func__,getpid());
	//clock_t start, end;
	//int start_time, end_time;
	//printf("Task(%d) do_work()\n", kris_id);
    
    //start = clock();
    //start_time = (int)time(NULL);
	burn_cpu(max_exec_time * MILLISEC);
    //end_time = (int)time(NULL);
	//end = clock();
	printf(">>EXIT Jack: lot_id %d execution time\n",getpid());
	// %d\n", kris_id, end_time - start_time); 
	exit(0);
}

#if 0
void end_task(int s)
{
	printf("\nTask(%d) has finished on CORE:%d\n",kris_id,sched_getcpu());
	
	exit(0);
}
#endif

int main(int argc, char** argv) 
{
	printf("hello\n");
	//sleep(1);
	
//	int i;
	struct sched_param param;

	//PID of current forked child	
	kris_id = getpid();
	clear_sched_param_t(&param);

	        printf("\nKRIS TASKS CONFIG %d, pid:%d\n",kris_id,getpid());
        printf("name\t\tpid\tmax_ex\tLLCMS\tMIPS\n");
	printf("%s\t%s\t%s\t%s\t%s\n",
		argv[0], 
		argv[1],
		argv[2],
		argv[3],
		argv[4]
		);


	//param.sched_priority=atoi(argv[2]);
	param.sched_priority = 99;
/*
	param.llcms = argv[3];
	param.mips = argv[4];
*/

	max_exec_time=atoi(argv[2]);
	printf("sched_priority:%d ", param.sched_priority);
	//printf("sched_priority:%d llc:%d M:%d\n", param.sched_priority, (long)param.llcms, (long)param.mips);

	signal(SIGUSR1, do_work);
	//signal(SIGUSR2, end_task);

//	print_task_param(&param);

	if( sched_setscheduler( 0, SCHED_B2BIAS, &param ) != 0 ){
		perror("ERROR");
		exit(0);
	}

	//kill(atoi(argv[4]),SIGUSR2);
	while(1){
		pause();
	}

	return 0;
}//END MAIN
