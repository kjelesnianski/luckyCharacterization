/*
 *
 * Example from 
 * http://man7.org/linux/man-pages/man2/perf_event_open.2.html
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MAX_SIZE 40


static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
		int cpu, int group_fd, unsigned long flags){
	int ret;
	ret = syscall(__NR_perf_event_open, hw_event,pid, cpu,group_fd, flags);
	return ret;
}

struct sbuf{
	long mtype;
	unsigned long mtext;
};

int main(int argc, char **argv){
	struct perf_event_attr pe;
	struct perf_event_attr pe_llc_lm;
	struct perf_event_attr pe_llc_sm;
	long long count, c_llc_lm, c_llc_sm;
	int fd, fd_llc_lm, fd_llc_sm;

	pid_t test;

	memset(&pe, 0, sizeof(struct perf_event_attr));
	pe.type = PERF_TYPE_HARDWARE;
	pe.size = sizeof(struct perf_event_attr);
	pe.config = PERF_COUNT_HW_INSTRUCTIONS;
	pe.disabled = 1;
	pe.exclude_kernel = 1;
	pe.exclude_hv = 1;

	memset(&pe_llc_lm, 0, sizeof(struct perf_event_attr));
	pe_llc_lm.type = PERF_TYPE_HW_CACHE;
	pe_llc_lm.size = sizeof(struct perf_event_attr);
/*EQ  To calculate the appropriate config value use the following equation:
  (perf_hw_cache_id) | (perf_hw_cache_op_id << 8) | (perf_hw_cache_op_result_id << 16)

  From LINUX/include/uapi/linux/perf_event.h
  PERF_COUNT_HW_CACHE_LL                  = 2,

  PERF_COUNT_HW_CACHE_OP_READ             = 0,
  PERF_COUNT_HW_CACHE_OP_WRITE            = 1,
  PERF_COUNT_HW_CACHE_OP_PREFETCH         = 2,

  PERF_COUNT_HW_CACHE_RESULT_ACCESS       = 0,
  PERF_COUNT_HW_CACHE_RESULT_MISS         = 1,

SO:
LLC-load-miss = (PERF_COUNT_HW_CACHE_LL) | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16)
LLC-store-miss = (PERF_COUNT_HW_CACHE_LL) | (PERF_COUNT_HW_CACHE_OP_WRITE << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16)
*/
	pe_llc_lm.config = (PERF_COUNT_HW_CACHE_LL) | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16);
	pe_llc_lm.disabled = 1;
	pe_llc_lm.exclude_kernel = 0;
	pe_llc_lm.exclude_hv = 1;

	memset(&pe_llc_sm, 0, sizeof(struct perf_event_attr));
	pe_llc_sm.type = PERF_TYPE_HW_CACHE;
	pe_llc_sm.size = sizeof(struct perf_event_attr);
	pe_llc_sm.config = (PERF_COUNT_HW_CACHE_LL) | (PERF_COUNT_HW_CACHE_OP_WRITE << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16);
	pe_llc_sm.disabled = 1;
	pe_llc_sm.exclude_kernel = 0;
	pe_llc_sm.exclude_hv = 1;
	printf("Parent PID:%d\n",getpid());

////// MSG stuff
	int msgid;
	int msgflg = IPC_CREAT | 0666;
	key_t kek = 1234;
	struct sbuf buff;


	test = fork();
if( test == 0 ){
	printf("Child PID:%d\n",getpid());
	//I am child

	//Wait for PID
	if((msgid = msgget(kek, msgflg)) < 0 ){
		printf("Child Msg GET error\n");
		exit(0);
	}

	buff.mtype = 1;

	//Find out my PID
	buff.mtext = getpid();
	printf(">>find:%lu\n", buff.mtext);


	//Send PID
	if( msgsnd(msgid, &buff, sizeof(buff),0 ) < 0){
		printf("Msg SEND error: %d, %ld, %lu\n",msgid,buff.mtype,buff.mtext);
		exit(0);
	}
	printf("C: Msg SENT!!\n");

	//printf("Measuring instruction count for this printf\n");
	execv("../benchmarks/SNU_NPB-1.0.3/NPB3.3-OMP-C/bin/cg.A.x",NULL);

}else{
	//I am parent
	
	//Wait for PID
	if((msgid = msgget(kek, 0666)) < 0 ){
		printf("Parent Msg GET error\n");
		exit(0);
	}
	printf("P: Msg ID!!\n");
	if(msgrcv(msgid, &buff, sizeof(buff),1,0) < 0 ){
		printf("Msg RECV error\n");
		exit(0);
	}
	printf("P: Msg Recved!!\n");
	//Got PID
	pid_t target_pid = (pid_t) buff.mtext;
	printf("PID got: %d!\n",target_pid);
		
	//Open event lines 
	fd = perf_event_open(&pe, target_pid, -1, -1, 0);
	fd_llc_lm = perf_event_open(&pe_llc_lm, target_pid, -1, -1, 0);
	fd_llc_sm = perf_event_open(&pe_llc_sm, target_pid, -1, -1, 0);
	if (fd == -1) {
		fprintf(stderr, "Error opening leader %llx\n", pe.config);
		exit(EXIT_FAILURE);
	}
	if (fd_llc_lm == -1) {
		fprintf(stderr, "Error opening leader %llx\n", pe.config);
		exit(EXIT_FAILURE);
	}
	if (fd_llc_sm == -1) {
		fprintf(stderr, "Error opening leader %llx\n", pe.config);
		exit(EXIT_FAILURE);
	}

	ioctl(fd, PERF_EVENT_IOC_RESET, 0);
	ioctl(fd_llc_lm, PERF_EVENT_IOC_RESET, 0);
	ioctl(fd_llc_sm, PERF_EVENT_IOC_RESET, 0);

	ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
	ioctl(fd_llc_lm, PERF_EVENT_IOC_ENABLE, 0);
	ioctl(fd_llc_sm, PERF_EVENT_IOC_ENABLE, 0);
	
	wait(test);
}
	//Wrap Up and Print
	ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
        ioctl(fd_llc_lm, PERF_EVENT_IOC_DISABLE, 0);
        ioctl(fd_llc_sm, PERF_EVENT_IOC_DISABLE, 0);

        read(fd, &count, sizeof(long long));
        read(fd_llc_lm, &c_llc_lm, sizeof(long long));
        read(fd_llc_sm, &c_llc_sm, sizeof(long long));

        printf("Used %lld instructions\n", count);
        printf("Had %lld LLC-load-misses\n", c_llc_lm);
        printf("Had %lld LLC-store-misses\n", c_llc_sm);

        close(fd);
        close(fd_llc_lm);
        close(fd_llc_sm);
}//END MAIN
