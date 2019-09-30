This is solution for producer-consumer problem using pthreads.

# Dependencies

make, gcc, pthreads

# BUILD

$ make <br />
gcc -g -Wall -Wextra -Werror -Wstrict-prototypes -DDEBUG -I. -c -o main.o main.c <br />
gcc -g -Wall -Wextra -Werror -Wstrict-prototypes -DDEBUG -I. -c -o thread_lib.o thread_lib.c <br />
gcc -g -Wall -Wextra -Werror -Wstrict-prototypes -DDEBUG -I. -c -o producer_consumer.o producer_consumer.c <br />
gcc -o a.out main.o thread_lib.o producer_consumer.o -L. -lpthread <br />

$ make clean <br />
rm -f a.out *.o cscope.out tags <br />

# How to run

Usage <br />

$ ./a.out -h <br />
Usage: <br />
./a.out [-p no_of_producer_threads] [-c no_of_consumer_threads] [-t producer_delay_in_sec] [-T consumer_delay_in_sec] [-s buf_size] <br />

NOTE: Press Ctlr-c (SIGINT) to stop process. <br />

Example, <br />

$ ./a.out -p 5 -c 1  <br />
INFO: No of producer threads = 5, producer delay = 1 sec <br />
INFO: No of consumer threads = 1, consumer delay = 1 sec <br />
INFO: buf_size = 4096 <br />
INFO: Press Ctlr-c OR 'kill -SIGINT 14624' to stop process. <br />
INFO: Producer thread running, thread id - 331106048 <br />
INFO: Producer thread id - 331106048, Adding item - 'N' to buffer <br />
INFO: Producer thread running, thread id - 314320640 <br />
INFO: Producer thread id - 314320640, Adding item - 'W' to buffer <br />
INFO: Producer thread running, thread id - 305927936 <br />
INFO: Producer thread id - 305927936, Adding item - 'L' to buffer <br />
INFO: Producer thread running, thread id - 297535232 <br />
INFO: Producer thread id - 297535232, Adding item - 'R' to buffer <br />
INFO: Producer thread running, thread id - 322713344 <br />
INFO: Producer thread id - 322713344, Adding item - 'B' to buffer <br />
INFO: Consumer thread running, thread id - 289142528 <br />
INFO: Consumer thread id - 289142528, Removing item - 'N' from buffer <br />
INFO: Producer thread id - 331106048, Adding item - 'B' to buffer <br />
INFO: Producer thread id - 314320640, Adding item - 'M' to buffer <br />
INFO: Producer thread id - 297535232, Adding item - 'Q' to buffer <br />
INFO: Producer thread id - 305927936, Adding item - 'B' to buffer <br />
INFO: Producer thread id - 322713344, Adding item - 'H' to buffer <br />
INFO: Consumer thread id - 289142528, Removing item - 'W' from buffer <br />
INFO: Producer thread id - 331106048, Adding item - 'C' to buffer <br />
INFO: Producer thread id - 314320640, Adding item - 'D' to buffer <br />
INFO: Producer thread id - 305927936, Adding item - 'A' to buffer <br />
INFO: Producer thread id - 322713344, Adding item - 'R' to buffer <br />
INFO: Producer thread id - 297535232, Adding item - 'Z' to buffer <br />
INFO: Consumer thread id - 289142528, Removing item - 'L' from buffer <br />
^CINFO: Received termination signal - 2(Interrupt) <br />
DEBUG: Producer thread exiting, thread id - 314320640 <br />
DEBUG: Producer thread exiting, thread id - 331106048 <br />
DEBUG: Producer thread exiting, thread id - 305927936 <br />
DEBUG: Producer thread exiting, thread id - 322713344 <br />
DEBUG: Producer thread exiting, thread id - 297535232 <br />
DEBUG: Consumer thread exiting, thread id - 289142528 <br />


# How to stop 

Press Ctlr-c OR 'kill -SIGINT <pid>' to stop process. <br />
