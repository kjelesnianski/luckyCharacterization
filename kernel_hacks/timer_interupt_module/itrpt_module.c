/*
 * itrpt Module
 * Interrupt Handler to update task_struct's perf counters per-interval
 *
 * Author:	K Jelesnianski
 * Contact:	kjski@vt.edu
 * Date:	04/13/17
 */

#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/init.h>
#include <linux/random.h>
#include <linux/kfifo.h>

#include <linux/interrupt.h>
#include <linux/hrtimer.h>
#include <linux/sched.h>

/* GLOBALS */
static struct hrtimer htimer;
static ktime_t kt_period;

/* FUNCTION DECLARATION */
static void timer_init(void);
static void timer_cleanup(void);
static enum hrtimer_restart timer_function(struct hrtimer * timer);

static int __init itrpt_init(void){
	printk("Num online cpus: %d\n",num_online_cpus());
	printk("Num possible cpus: %d\n",num_possible_cpus());
	printk("Num present cpus: %d\n",num_present_cpus());
	printk("Num active cpus: %d\n",num_active_cpus());

//	timer_init();
	return 0;
}

static void __exit itrpt_exit(void){
//	timer_cleanup();
}

/*****************************************************************************/
/* Helper Funcs                                                              */
/*****************************************************************************/
static void timer_init(void){
/*	kt_period = ktime_set(0, 104167); //seconds, nanosec
	hrtimer_init( &htimer, CLOCK_REALTIME, HRTIMER_MODE_REL);
	htimer.function = timer_function;
	htimer.start( &htimer, kt_period, HRTIMER_MODE_REL);
*/	
}

static void timer_cleanup(void){
	hrtimer_cancel( &htimer);
}

static enum hrtimer_restart timer_function(struct hrtimer * timer){

	// @Do your work here.
	

	hrtimer_forward_now(timer, kt_period);
	return HRTIMER_RESTART;
}

/*****************************************************************************/
/* MODULE LABELING                                                           */
/*****************************************************************************/
module_init(itrpt_init);
module_exit(itrpt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("K Jelesnianski <kjski@vt.edu>");
MODULE_DESCRIPTION("Perf Counter Update for Scheduler Module");
