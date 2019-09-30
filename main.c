#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "common_defs.h"
#include "thread_lib.h"
#include "producer_consumer.h"

/*
 * variable to signal threads to exit from processing loop
 */
volatile int	termination_signal = FALSE;

/*
 * signal handler fn
 */
void
termination_handler(int signum)
{
	INFO("Received termination signal - %d(%s)", signum, strsignal(signum));

	termination_signal = TRUE;
	return;
}

/*
 * Setup SIGINT signal handler
 */
int
setup_signal_handler(void)
{
	struct sigaction	new_action;
	int			ret;

	/*
	 * Setup handler fn to be invoked on SIGINT
	 */
	new_action.sa_handler = termination_handler;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = SA_RESTART;

	ret = sigaction(SIGINT, &new_action, NULL);
	if (ret != 0) {
		ERR("Setting up SIGINT handler failed - %s", strerror(errno));
		return ERR_SIGNAL_HANDLER;
	}

	return RET_SUCCESS;
}

/*
 * exit fn
 */
void
die(int ecode)
{	
	exit(ecode);
}

/*
 * Print usage fn
 */
void
print_usage(char *prgname, FILE *fp)
{
	fprintf(fp, "Usage:\n");
	fprintf(fp, "%s [-p no_of_producer_threads] "
		"[-c no_of_consumer_threads] "
		"[-t producer_delay_in_sec] "
		"[-T consumer_delay_in_sec] "
		"[-s buf_size]\n", prgname);
	fprintf(fp, "\nNOTE: Press Ctlr-c (SIGINT) to stop process.\n");
	return;
}

/*
 * Utility fn to convert string to unsigned int
 */
int
convert_to_uint(char *str, unsigned int *value, unsigned int max_limit)
{
	char	*endptr;

	errno = 0;
	*value = strtoul(str, &endptr, 10);
	if (errno != 0) {
		ERR("strtoul failed - %s", strerror(errno));
		return ERR_USAGE;
	}

	/*
	 * Check if str doesn't have any digits
	 */
	if (endptr == str) {
		ERR("invalid argument - %s", str);
		return ERR_EINVAL;
	}

	/*
	 * Check if str has any non-digit
	 */ 
	if (*endptr != '\0') {
		ERR("invalid argument - %s", str);
		return ERR_EINVAL;
	}

	/*
	 * Check if value is more than max_limit set for option
	 */
	if (*value > max_limit) {
		ERR("value can't go beyond max_limit(%u)", max_limit);
		return ERR_EINVAL;
	}

	return RET_SUCCESS;
}


/*
 * Parse program command line options
 */

int
parse_args(int argc, char *argv[], program_options_t *options)
{
	unsigned int	value;
	char		*prgname = argv[0];
	int 		ret, opt;
	
	/*
	 * Setup default values
	 */
	options->no_of_producer_threads = DEFAULT_PRODUCER_THREADS;
	options->no_of_consumer_threads = DEFAULT_CONSUMER_THREADS;

	options->producer_delay = DEFAULT_DELAY_IN_SEC;
	options->consumer_delay = DEFAULT_DELAY_IN_SEC;

	options->buf_size = DEFAULT_BUFFER_SIZE;

	/*
	 * Extract command line options
	 */
	while ((opt = getopt(argc, argv, "p:c:s:t:T:h")) != -1) {
		switch (opt) {
		case 'p':
			ret = convert_to_uint(optarg, &value, MAX_PRODUCER_THREADS);
			if (ret != 0) {
				print_usage(prgname, stdout);
				die(ret);	
			}

			options->no_of_producer_threads = value;
			break;


		case 'c':
			ret = convert_to_uint(optarg, &value, MAX_CONSUMER_THREADS);
			if (ret != 0) {
				print_usage(prgname, stdout);
				die(ret);	
			}

			options->no_of_consumer_threads = value;
			break;

		case 't':
			ret = convert_to_uint(optarg, &value, MAX_DELAY_IN_SEC);
			if (ret != 0) {
				print_usage(prgname, stdout);
				die(ret);	
			}

			options->producer_delay = value;
			break;

		case 'T':
			ret = convert_to_uint(optarg, &value, MAX_DELAY_IN_SEC);
			if (ret != 0) {
				print_usage(prgname, stdout);
				die(ret);	
			}

			options->consumer_delay = value;
			break;

		case 's':
			ret = convert_to_uint(optarg, &value, MAX_BUFFER_SIZE);
			if (ret != 0) {
				print_usage(prgname, stdout);
				die(ret);
			}

			options->buf_size = value;
			break;

		case 'h':
			print_usage(prgname, stdout);
			die(RET_SUCCESS);

		default:
			print_usage(prgname, stderr);
			die(ERR_USAGE);
		}
	}

	INFO("No of producer threads = %u, producer delay = %u sec",
		options->no_of_producer_threads, options->producer_delay);
	INFO("No of consumer threads = %u, consumer delay = %u sec",
		options->no_of_consumer_threads, options->consumer_delay);
	INFO("buf_size = %u", options->buf_size);

	return RET_SUCCESS;
}

/*
 * main fn
 */
int
main(int argc, char *argv[])
{
	program_options_t	options;
	thread_data_t		*tdata = NULL;
	pthread_t		**producer_tids = NULL, **consumer_tids = NULL;
	int 			ret;

	/*
	 * Parse command line arguments
	 */
	memset(&options, 0, sizeof (program_options_t));
	ret = parse_args(argc, argv, &options);
	if (ret != RET_SUCCESS) {
		die(ret);
	}

	/*
	 * Setup SIGINT handler
	 */
	ret = setup_signal_handler();
	if (ret != RET_SUCCESS) {
		return ret;
	}

	INFO("Press Ctlr-c OR 'kill -SIGINT %d' to stop process.", getpid());

	/*
	 * Setup thread shared data
	 */
	ret = alloc_thread_data(&tdata, &options);
	if (ret != RET_SUCCESS) {
		return ret;
	}

	/*
	 * Create producer threads
	 */
	ret = create_threads(&producer_tids, producer_thread_fn, options.no_of_producer_threads, tdata);
	if (ret != RET_SUCCESS) {
		dealloc_thread_data(tdata);
		die(ret);
	}

	/*
	 * Create Consumer threads
	 */
	ret = create_threads(&consumer_tids, consumer_thread_fn, options.no_of_consumer_threads, tdata);
	if (ret != RET_SUCCESS) {
		delete_threads(producer_tids, options.no_of_producer_threads);
		dealloc_thread_data(tdata);
		die(ret);
	}

	/*
	 * Wait for producer threads to finish
	 */
	ret = wait_threads(producer_tids, options.no_of_producer_threads);
	if (ret != RET_SUCCESS) {
		delete_threads(producer_tids, options.no_of_producer_threads);
		delete_threads(consumer_tids, options.no_of_consumer_threads);
		dealloc_thread_data(tdata);
		die(ret);
	}

	/*
	 * Wait for consumer threads to finish
	 */
	ret = wait_threads(consumer_tids, options.no_of_consumer_threads);
	if (ret != RET_SUCCESS) {
		delete_threads(producer_tids, options.no_of_producer_threads);
		delete_threads(consumer_tids, options.no_of_consumer_threads);
		dealloc_thread_data(tdata);
		die(ret);
	}

	/*
	 * Cleanup
	 */
	delete_threads(producer_tids, options.no_of_producer_threads);
	delete_threads(consumer_tids, options.no_of_consumer_threads);

	dealloc_thread_data(tdata);

	return RET_SUCCESS;
}
