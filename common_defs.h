#ifndef __COMMON_DEFS_H
#define __COMMON_DEFS_H

#define TRUE			1
#define FALSE			0

#define RET_SUCCESS		0

#define ERR_USAGE		1	/* invalid usage */
#define ERR_EINVAL		2	/* invalid input */
#define ERR_ENOMEM		3	/* memory allocation failed */
#define ERR_THREAD_CREATE	4	/* thread create failed */
#define ERR_SIGNAL_HANDLER	5	/* signal handler setup failed */
#define ERR_MUTEX_INIT		6	/* thread mutex init failed */
#define ERR_COND_INIT		7	/* thread condvar init failed */
#define ERR_THREAD_ATTR		8	/* thread attr set failed */

#define INFO(fmt, ...)		do {				\
	fprintf(stdout, "INFO: " fmt "\n", __VA_ARGS__);	\
} while (0)

#ifdef DEBUG 
#define DBG(fmt, ...)		do {				\
	fprintf(stdout, "DEBUG: " fmt "\n", __VA_ARGS__);	\
} while (0)
#else
#define DBG(fmt, ...)
#endif

#define ERR(fmt, ...)		do {				\
	fprintf(stderr, "ERR:" fmt "\n", __VA_ARGS__);		\
} while (0)


#define COND_SLEEP(sec, cond)	do {		\
	unsigned int	i;			\
	for (i = 0; i < sec && (cond); i++) {	\
		sleep(1);			\
	}					\
} while (0)

#define DEFAULT_PRODUCER_THREADS	1
#define DEFAULT_CONSUMER_THREADS	1
#define DEFAULT_DELAY_IN_SEC		1
#define DEFAULT_BUFFER_SIZE		4096

#define MAX_CONSUMER_THREADS		1024
#define MAX_PRODUCER_THREADS		1024
#define MAX_DELAY_IN_SEC		(60 * 60)
#define MAX_BUFFER_SIZE			(4096 * 1024)

#define COND_WAIT_TIMEOUT		1	/* cond wait timeout - 1 sec */

/*
 * Program command line arguments
 */
typedef struct program_options {
	unsigned int	no_of_producer_threads;
	unsigned int	no_of_consumer_threads;

	unsigned int	producer_delay;
	unsigned int	consumer_delay;

	unsigned int	buf_size;
} program_options_t;

/*
 * Program thread shared data
 */
typedef struct thread_data {
	char		*buffer;
	unsigned int	buf_size;

	unsigned int	producer_index;
	unsigned int	consumer_index;
	unsigned int	current_total_items;

	pthread_mutex_t mutex;
	pthread_cond_t	producer_cond_wait;
	pthread_cond_t	consumer_cond_wait;

	unsigned int	producer_delay;
	unsigned int	consumer_delay;
} thread_data_t;

extern volatile int	termination_signal;

#endif
