#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "common_defs.h"
#include "thread_lib.h"

/*
 * Deallocate thread data resources
 */
void
dealloc_thread_data(thread_data_t *tdata)
{
	pthread_mutex_destroy(&tdata->mutex);
	pthread_cond_destroy(&tdata->producer_cond_wait);
	pthread_cond_destroy(&tdata->consumer_cond_wait);

	free(tdata->buffer);
	free(tdata);
}

/*
 * Allocate and initialize thread shared data
 */
int
alloc_thread_data(thread_data_t **tdata, program_options_t *options)
{
	thread_data_t	*local_tdata;
	char		*buffer;
	int		ret;

	/*
	 * Memory allocations to hold thread data and buffer
	 */
	local_tdata = calloc(sizeof (thread_data_t), 1);
	if (local_tdata == NULL) {
		ERR("memory allocation failure - %s", strerror(errno));
		return ERR_ENOMEM;
	}

	buffer = calloc(options->buf_size * sizeof (char), 1);
	if (buffer == NULL) {
		ERR("memory allocation failure - %s", strerror(errno));
		free(local_tdata);
		return ERR_ENOMEM;
	}

	local_tdata->buffer = buffer;
	local_tdata->buf_size = options->buf_size;

	local_tdata->producer_delay = options->producer_delay;
	local_tdata->consumer_delay = options->consumer_delay;

	/*
	 * Initialize thread mutex and conditional vars
	 */
	ret = pthread_mutex_init(&local_tdata->mutex, NULL);
	if (ret != 0) {
		ERR("pthread_mutex_init failed - %s", strerror(errno));
		free(buffer);
		free(local_tdata);
		return ERR_MUTEX_INIT;
	}

	ret = pthread_cond_init(&local_tdata->producer_cond_wait, NULL);
	if (ret != 0) {
		ERR("pthread_cond_init failed - %s", strerror(errno));
		free(buffer);
		free(local_tdata);
		pthread_mutex_destroy(&local_tdata->mutex);
		return ERR_COND_INIT;
	}

	ret = pthread_cond_init(&local_tdata->consumer_cond_wait, NULL);
	if (ret != 0) {
		ERR("pthread_cond_init failed - %s", strerror(errno));
		free(buffer);
		free(local_tdata);
		pthread_mutex_destroy(&local_tdata->mutex);
		pthread_cond_destroy(&local_tdata->producer_cond_wait);
		return ERR_COND_INIT;
	}

	*tdata = local_tdata;
	
	return RET_SUCCESS;
}

/*
 * Release thread allocations
 */
void
delete_threads(pthread_t **tids, int nthreads)
{
	int	i;

	for (i = 0; i < nthreads; i++) {
		free(tids[i]);
	} 

	free(tids);
	return;
}

/*
 * Create and start threads with thread_fn
 */
int
create_threads(pthread_t ***tids, void * (*thread_fn)(void *), int nthreads, thread_data_t *tdata)
{
	pthread_attr_t	attr;
	pthread_t	**local_tids;
	int		i, ret = RET_SUCCESS;

	/*
	 * Create bound threads (system scope) for better concurrency
	 */
	ret = pthread_attr_init(&attr);
	if (ret != 0) {
		ERR("pthread_attr_init failed - %s", strerror(errno));
		return ERR_THREAD_ATTR;
	}

	ret = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	if (ret != 0) {
		ERR("pthread_attr_setscope failed - %s", strerror(errno));
		pthread_attr_destroy(&attr);
		return ERR_THREAD_ATTR;
	}

	/*
	 * Allocate thread structs
	 */
	local_tids = calloc(sizeof (pthread_t *), nthreads);
	if (local_tids == NULL) {
		ERR("Memory allocation failed - %s", strerror(errno));
		return ERR_ENOMEM;
	}

	/*
	 * Start threads
	 */
	for (i = 0; i < nthreads; i++) {
		local_tids[i] = calloc(sizeof (pthread_t), 1);
		if (local_tids[i] == NULL) {
			ERR("Memory allocation failed - %s", strerror(errno));	
			pthread_attr_destroy(&attr);
			delete_threads(local_tids, i);
			return ERR_ENOMEM;
		}

		ret = pthread_create(local_tids[i], &attr, thread_fn, (void *) tdata);
		if (ret != RET_SUCCESS) {
			ERR("pthread_create failed - %s", strerror(ret));
			pthread_attr_destroy(&attr);
			delete_threads(local_tids, i + 1);
			return ERR_THREAD_CREATE;	
		}
	}

	/*
	 * Release thread attributes object as it is no longer required. This has
	 * no effect on threads that were created using this object
	 */
	pthread_attr_destroy(&attr);

	*tids = local_tids;

	return ret;
}


/*
 * Wait for threads to finish
 */
int
wait_threads(pthread_t **tids, int nthreads)
{
	int	i, ret;

	for (i = 0; i < nthreads; i++) {
		/*
		 * Wait for thread to finish
		 */
		ret = pthread_join(*tids[i], NULL);
		if (ret != 0) {
			ERR("pthread_join failed for thread id - %u\n", (unsigned int) *tids[i]);
		}
	}

	return ret;
}
