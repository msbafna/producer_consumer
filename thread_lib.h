#ifndef __THREADLIB_H
#define __THREADLIB_H

extern int	alloc_thread_data(thread_data_t **, program_options_t *);
extern void	dealloc_thread_data(thread_data_t *);

extern int	create_threads(pthread_t ***, void * (*)(void *), int, thread_data_t *);
extern void	delete_threads(pthread_t **, int);

extern int	wait_threads(pthread_t **, int);

#endif
