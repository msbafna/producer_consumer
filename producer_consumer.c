#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "common_defs.h"
#include "thread_lib.h"

/*
 * Producer threads fn
 */
void
producer_thread_fn(void *arg)
{
	struct timespec	to;
	thread_data_t	*tdata = arg;
	unsigned int	tid = (unsigned int) pthread_self();
	char		item;

	INFO("Producer thread running, thread id - %u", tid);
	while (termination_signal != TRUE) {
		pthread_mutex_lock(&tdata->mutex);

		/*
		 * If buffer is full, then producer threads would wait
		 * for consumer threads signal. Consumer threads would
		 * consume items and thus making space available in
		 * buffer
		 */
		while (tdata->current_total_items == tdata->buf_size) {
			/*
			 * Set pthread_cond_wait timeout
			 */
			to.tv_sec = time(NULL) + COND_WAIT_TIMEOUT;
			to.tv_nsec = 0;
			pthread_cond_timedwait(&tdata->producer_cond_wait, &tdata->mutex, &to);

			/*
			 * Check if thread has been signalled to terminate
			 */
			if (termination_signal == TRUE) {
				pthread_mutex_unlock(&tdata->mutex);
				DBG("Producer thread exiting, thread id - %u", tid);
				pthread_exit((void *) NULL);
			}
		}

		/*
		 * Now producer thread has space to add item to buffer
		 */
		item = 'A' + (random() % 26);
		INFO("Producer thread id - %u, Adding item - '%c' to buffer", tid, item);

		tdata->buffer[tdata->producer_index] = item;
		tdata->producer_index = (tdata->producer_index + 1) % tdata->buf_size;

		/*
		 * Update current_total_items filled in buffer and signal waiting consumer
		 * threads for item availability
		 */
		tdata->current_total_items++;
		pthread_cond_signal(&tdata->consumer_cond_wait);

		pthread_mutex_unlock(&tdata->mutex);

		/*
		 * Producer delay with check if thread has been signalled to terminate
		 */
		COND_SLEEP(tdata->producer_delay, termination_signal != TRUE);
	}

	DBG("Producer thread exiting, thread id - %u", tid);
	pthread_exit((void *) NULL);
}

/*
 * Consumer threads fn
 */
void
consumer_thread_fn(void *arg)
{
	struct timespec	to;
	thread_data_t	*tdata = arg;
	unsigned int	tid = (unsigned int) pthread_self();
	char		item;


	INFO("Consumer thread running, thread id - %u", tid);
	while (termination_signal != TRUE) {
		pthread_mutex_lock(&tdata->mutex);

		/*
		 * If buffer is empty, then consumer threads would wait
		 * for producer threads signal. Producer threads would
		 * add items to buffer thus making items available for
		 * consumer threads to consume
		 */
		while (tdata->current_total_items == 0) {
			/*
			 * Set pthread_cond_wait timeout
			 */
			to.tv_sec = time(NULL) + COND_WAIT_TIMEOUT;
			to.tv_nsec = 0;
			pthread_cond_timedwait(&tdata->consumer_cond_wait, &tdata->mutex, &to);

			/*
			 * Check if thread has been signalled to terminate
			 */
			if (termination_signal == TRUE) {
				pthread_mutex_unlock(&tdata->mutex);
				DBG("Consumer thread exiting, thread id - %u", tid);
				pthread_exit((void *) NULL);
			}
		}

		/*
		 * Now consumer thread can consume available item from buffer
		 */
		item = tdata->buffer[tdata->consumer_index];
		INFO("Consumer thread id - %u, Removing item - '%c' from buffer", tid, item);

		tdata->consumer_index = (tdata->consumer_index + 1) % tdata->buf_size;

		/*
		 * Update current_total_items in buffer and signal waiting producer threads
		 * for space availability
		 */
		tdata->current_total_items--;
		pthread_cond_signal(&tdata->producer_cond_wait);

		pthread_mutex_unlock(&tdata->mutex);

		/*
		 * Producer delay with check if thread has been signalled to terminate
		 */
		COND_SLEEP(tdata->consumer_delay, termination_signal != TRUE);
	}

	DBG("Consumer thread exiting, thread id - %u", tid);
	pthread_exit((void *) NULL);
}
