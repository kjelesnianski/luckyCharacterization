#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define  BUF_LEN	200
#define  KRIS_TASKS_NUM	10

/* Struct Definition */
struct kris_tasks_config {
	int pid;
	unsigned long long tickets;
	double min_exec;
	int max_exec;
	double min_inter_arrival;
	double max_inter_arrival;
	double min_offset;
	double max_offset;
};

//GLOBALS
pid_t kris_tasks_pid [KRIS_TASKS_NUM];
int kris_tasks_num=0;

/*****************************************************************************/
/*  Helper Functions for "Kris_tasks_config" Array                           */
/*****************************************************************************/

/* Prints current values of entire task config Array */
void print_kris_tasks_config(struct kris_tasks_config *tasks, int num)
{
	int i;
	printf("\nKRIS TASKS CONFIG\n");
	printf("pid\ttickets\n");
	printf("pid\tmin_c\tmax_c\tmin_t\tmax_t\tticket\tmin_o\tmax_o\n");
	for(i=0;i<num;i++){
		printf("%d\t%f\t%f\t%f\t%f\t%llu\t%f\t%f\n",
		tasks[i].pid,
		tasks[i].tickets,
		tasks[i].min_exec,
		tasks[i].max_exec,
		tasks[i].min_inter_arrival,
		tasks[i].max_inter_arrival,
		tasks[i].min_offset,
		tasks[i].max_offset
		);
	}
}

/* Resets All tasks config values to ZERO */
void clear_kris_tasks_config_info(struct kris_tasks_config *tasks, int num)
{
	int i;	
	for(i=0;i<num;i++){
		tasks[i].pid=0;
		tasks[i].min_exec=0;
		tasks[i].max_exec=0;
		tasks[i].min_inter_arrival=0;
		tasks[i].max_inter_arrival=0;
		tasks[i].tickets=0;
		tasks[i].min_offset=0;
		tasks[i].max_offset=0;
	}
}

void get_kris_task_config_info(char * str, struct kris_tasks_config *tasks,int *n)
{
	char *s ,*s1;
	int i=0;
	s = s1=str;
	while(i<3){
		if(*s=='\t'){
			*s='\0';
			switch(i){
				case 0:
					tasks[*n].pid = atoi(s1);
					s1=s+1;
					i++;
				break;
				case 1:
					tasks[*n].tickets = atoll(s1);
					s1=s+1;
					i++;
				break;
				case 2:
					tasks[*n].max_exec = atoi(s1);
					s1=s+1;
					i++;
				break;
			}
		}
		s++;
	}
	(*n)++;
}

void get_kris_tasks_config_info(char *file, int *duration, struct kris_tasks_config *tasks,int *n)
{
	char buffer[BUF_LEN];
	int count=0;
	FILE* fd  = fopen(file, "r");
	*n=0;
	buffer[0]='\0';
	while( (fgets(buffer, BUF_LEN, fd))!=NULL) {
		if(strlen(buffer)>1){
			printf("%s\n", buffer);
			switch(count){
				case 0:
					*duration=get_int_val(buffer);
					count++;
				break;
				default:
					get_kris_task_config_info(buffer, tasks,n);
			}
		}
		buffer[0]='\0';
	}
	fclose(fd);
}

void start_simulation()
{
	int i;
	printf("I will send a SIGUSR1 signal to start all tasks\n");
	for(i=0;i<kris_tasks_num;i++){
		kill(kris_tasks_pid[i],SIGUSR1);
	}
}

void end_simulation(int signal)
{
	int i;
	printf("I will send a SIGUSR2 signal to finish all tasks\n");
	for(i=0;i<kris_tasks_num;i++){
		kill(kris_tasks_pid[i],SIGUSR2);
	}
}

void help(char* name)
{
	fprintf(stderr, "Usage: %s file_name (system configuration)\n",	name);
	exit(0);
}

int get_int_val (char* str) 
{
	char* s = str;
	int val;
	for (s = str;*s!='\t';s++);
	*s='\0';
	val=atoi(str);
	return val;
}

///////////////////////////////////////////////////////////////////////////////
//    MAIN
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	int duration,i,j,k,num_options;
	struct itimerval sim_time;
	struct sched_param param;

	//Define Task Config Array
	struct kris_tasks_config kris_tasks_config[KRIS_TASKS_NUM];

	char tmp[KRIS_TASKS_NUM][BUF_LEN],*parg[KRIS_TASKS_NUM];

	if(argc!=2){
		help(argv[0]);
	}

	//Reset task config array
	clear_kris_tasks_config_info(kris_tasks_config, KRIS_TASKS_NUM);
	//Read task Config options into "Kris_tasks_config" array
 	get_kris_tasks_config_info(argv[1],&duration,kris_tasks_config,&kris_tasks_num);

	param.sched_priority=99;
/*This is the Host Guardian (Ruling) Thread intiating all other sub-tasks.
  For this reason this thread should have the highest priority over all the 
  other tasks on the machine.
  1. So it is not interrupted by the new "custom" scheduling policy
  2. So that it can inject more tasks into the runqueue while earlier sub-
	tasks are still running.
*/
	sched_setscheduler( 0, SCHED_FIFO, (struct sched_param *)&param );

	//Initialize Simulation Values
	sim_time.it_interval.tv_sec = 0;
	sim_time.it_interval.tv_usec = 0;
	sim_time.it_value.tv_sec = duration;
	sim_time.it_value.tv_usec = 0;

	//Initialize Signal to kill all sub-tasks once simulation is finished
	signal(SIGALRM, end_simulation);
	//??
	setitimer(ITIMER_REAL, &sim_time, NULL);

	//Import "Kris_tasks_config" lines to tmp before copying to parg
	for(i=0;i<kris_tasks_num;i++){
		strcpy(tmp[0],"kris_task");
		sprintf(tmp[1],"%d",kris_tasks_config[i].pid);
		sprintf(tmp[2],"%llu",kris_tasks_config[i].tickets);
		sprintf(tmp[3],"%d",kris_tasks_config[i].max_exec);
		sprintf(tmp[4],"%f",kris_tasks_config[i].min_inter_arrival);
		sprintf(tmp[5],"%f",kris_tasks_config[i].max_inter_arrival);
		sprintf(tmp[6],"%f",kris_tasks_config[i].min_offset);
		sprintf(tmp[7],"%f",kris_tasks_config[i].max_offset);
		sprintf(tmp[8],"%ld", 0);
		num_options=9;
		printf("transforming\n");
		//Transform 2D char array to 1D char*
		for( k = 0 ; k < num_options ; k++ ){
			parg[k]=tmp[k];
		}
		//Last task is set to NULL to signal the end
		parg[k]=NULL;
		
		print_kris_tasks_config(kris_tasks_config, num_options);
		//Fork off for the new sub-task
		kris_tasks_pid[i]=fork();
		//Only let parent execute continue, while children 
		// start the sub-task "kris_task" with pargs and then exit.
		if(kris_tasks_pid[i]==0){
			execv("./kris_task",parg);
			perror("Error: execv\n");
			printf("hello\n");
			exit(0);
		}
		printf("about to sleep 1\n");
		sleep(1);
		printf("slept, reruning next sub-task\n");
	}//end for all tasks
	
	start_simulation();  //time zero of the execution

	printf("Waiting for %d tasks done", kris_tasks_num);
	for(i=0;i<kris_tasks_num;i++){
		wait(NULL);
	}
	printf("All tasks have finished properly!!!\n");

	return 0;
}//END MAIN
