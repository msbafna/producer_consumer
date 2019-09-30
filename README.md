This is solution for producer-consumer problem using pthreads.

# Dependencies

make, gcc, pthreads

# BUILD

# make
gcc -g -Wall -Wextra -Werror -Wstrict-prototypes -DDEBUG -I. -c -o main.o main.c
gcc -g -Wall -Wextra -Werror -Wstrict-prototypes -DDEBUG -I. -c -o thread_lib.o thread_lib.c
gcc -g -Wall -Wextra -Werror -Wstrict-prototypes -DDEBUG -I. -c -o producer_consumer.o producer_consumer.c
gcc -o a.out main.o thread_lib.o producer_consumer.o -L. -lpthread

# make clean
rm -f a.out *.o cscope.out tags

# How to run

# Usage 

# ./a.out -h
Usage:
./a.out [-p no_of_producer_threads] [-c no_of_consumer_threads] [-t producer_delay_in_sec] [-T consumer_delay_in_sec] [-s buf_size]

NOTE: Press Ctlr-c (SIGINT) to stop process.

Example,

# ./a.out -p 5 -c 1 
INFO: No of producer threads = 5, producer delay = 1 sec
INFO: No of consumer threads = 1, consumer delay = 1 sec
INFO: buf_size = 4096
INFO: Press Ctlr-c OR 'kill -SIGINT 14624' to stop process.
INFO: Producer thread running, thread id - 331106048
INFO: Producer thread id - 331106048, Adding item - 'N' to buffer
INFO: Producer thread running, thread id - 314320640
INFO: Producer thread id - 314320640, Adding item - 'W' to buffer
INFO: Producer thread running, thread id - 305927936
INFO: Producer thread id - 305927936, Adding item - 'L' to buffer
INFO: Producer thread running, thread id - 297535232
INFO: Producer thread id - 297535232, Adding item - 'R' to buffer
INFO: Producer thread running, thread id - 322713344
INFO: Producer thread id - 322713344, Adding item - 'B' to buffer
INFO: Consumer thread running, thread id - 289142528
INFO: Consumer thread id - 289142528, Removing item - 'N' from buffer
INFO: Producer thread id - 331106048, Adding item - 'B' to buffer
INFO: Producer thread id - 314320640, Adding item - 'M' to buffer
INFO: Producer thread id - 297535232, Adding item - 'Q' to buffer
INFO: Producer thread id - 305927936, Adding item - 'B' to buffer
INFO: Producer thread id - 322713344, Adding item - 'H' to buffer
INFO: Consumer thread id - 289142528, Removing item - 'W' from buffer
INFO: Producer thread id - 331106048, Adding item - 'C' to buffer
INFO: Producer thread id - 314320640, Adding item - 'D' to buffer
INFO: Producer thread id - 305927936, Adding item - 'A' to buffer
INFO: Producer thread id - 322713344, Adding item - 'R' to buffer
INFO: Producer thread id - 297535232, Adding item - 'Z' to buffer
INFO: Consumer thread id - 289142528, Removing item - 'L' from buffer
^CINFO: Received termination signal - 2(Interrupt)
DEBUG: Producer thread exiting, thread id - 314320640
DEBUG: Producer thread exiting, thread id - 331106048
DEBUG: Producer thread exiting, thread id - 305927936
DEBUG: Producer thread exiting, thread id - 322713344
DEBUG: Producer thread exiting, thread id - 297535232
DEBUG: Consumer thread exiting, thread id - 289142528


# How to stop

Press Ctlr-c OR 'kill -SIGINT <pid>' to stop process.
