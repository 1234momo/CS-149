# CS-149
C assignments from Operating Systems using Kubuntu


# Assignment 2 - Reading files
The focus of this assignment is to read floating numbers from multiple files and average them. 

<br />**fork()** is used to compute the sums and counts in parallel. 
<br />**pipe()** is used to bring the sums and counts from children processes to the parent process. 


# Assignment 3 - Children processes
The focus of this assignment is to execute command line commands from a file using the exec call and rerun the commands that run more than 5 seconds.
The program stores the commands from the file into a linked list and then spawns children processes that would execute the commands from the linked list. 
If a command runs for 2 seconds or more, then the parent process will respawn a child process and rerun the command. If a command runs for less than 2 seconds, then the command doesn't need to be rerun.

<br />**fork()** is used to create children processes that would run exec().
<br />**exec()** is used to run command line commands.
<br />**dup2()** is used to reroute the file handlers to files X.out (i.e. 1.out for the first command, 2.out for the second command, 3.out for the third command, etc.) and X.err (i.e. 1.err for the first command, 2.err for the second command, 3.err for the third command, etc).
<br />**struct** is used to create linked list nodes.

If the file with commands contain the following:
```
prompt> cat cmdfile
sleep 5
ls -latr
sleep 3
pwd
sleep 1
wc /etc/passwd
```

then the output should be:
```
$ ./proc_manager cmdfile
$ cat 1.out
Starting command 1: child 2353 pid of parent 2234
Finished at 90, runtime duration 1

$ cat 1.err
Exited with exitcode = 0
spawning too fast

$ cat 2.out
Starting command 2: child 2363 pid of parent 2234
Finished at 100, runtime duration 10
Starting command 2: child 2387 pid of parent 2234
Finished at 110, runtime duration 10

$ cat 2.err
Exited with exitcode = 0
Killed with signal 15
spawning too fast

$ cat 3.out
Starting command 3: child 2377 pid of parent 2234
Finished at 95, runtime duration 1

$ cat 3.err
Exited with exitcode = 0
spawning too fast
...
...
```


# Assignment 4 - Memory leaks
The focus of this assignment is to clear memory leaks from allocated memory. 
Commands from a file are stored into the rows of an array of type char** and the commands in each row of char** is stored into a linked list node.
The program is able to take up any number of lines of commands from a file.
In the end of the program, no memory leaks should occur, all allocated memory should be freed, and the commands in each linked list node should be printed recursively.

<br />**realloc()** is used to resize the char** array by resizing the array to include more rows. 
<br />**malloc()** is used to allocate memory for a linked list node.
<br />**free()** is used to free allocated memory. 
<br />**struct** is used to create linked list nodes.
<br />**valgrind** is used to check for memory leaks.
<br />**dup2()** is used to reroute the file handlers to files X.out (i.e. 1.out for the first command, 2.out for the second command, 3.out for the third command, etc.) and X.err (i.e. 1.err for the first command, 2.err for the second command, 3.err for the third command, etc).


# Assignment 5 - Concurrency
The focus of this assignment is to concurrently read and store input from the terminal.
The program uses two threads: one thread for reading input and storing the input as the head of the linked list, and another thread that prints out the head of the linked list.
The second thread only prints when the head of the linked list is changed, causing the second thread to be sleeping for the majority of the program.
On the other hand, the first thread waits for input and stores the input when entered. 
If the input is just a new line, then the two threads stop and deallocate any memory that was allocated. 
If the input isn't a new line, then the first thread reads the input from the terminal, allocate a linked list node, stores the input into the node, and inserts the node as the head of the linked list.
The second thread wakes up and prints the content of the linked list along with the date and time of the execution.  

<br />**malloc()** is used to allocate memory for a linked list node.
<br />**free()** is used to free allocated memory. 
<br />**struct** is used to create linked list nodes.
<br />**valgrind** is used to check for memory leaks.
<br />**pthread_join()** is used to wait for the threads to finish.
<br />**pthread_mutex_lock()** is used to lock the critical sections.
<br />**pthread_mutex_unlock** is used to unlock the critical sections.
<br />**pthread_join()** is used to wait for the threads to finish.

Example execution:
```
create first thread
create second thread
wait for first thread to exit
This is thread 140478111237888 (p=0x7fc394000b20)
This is thread 140478102845184 (p=0x7fc394000b20)
example
Logindex 0, thread 140478111237888, PID 1409, 21/05/2020 00:41:52 am:  Head of linked list contains example
Logindex 1, thread 140478102845184, PID 1409, 21/05/2020 00:41:52 am: Head of linked list contains example
command
Logindex 2, thread 140478111237888, PID 1409, 21/05/2020 00:41:54 am:  Head of linked list contains command
Logindex 3, thread 140478102845184, PID 1409, 21/05/2020 00:41:54 am: Head of linked list contains command
running
Logindex 4, thread 140478111237888, PID 1409, 21/05/2020 00:41:59 am:  Head of linked list contains running
Logindex 5, thread 140478102845184, PID 1409, 21/05/2020 00:41:59 am: Head of linked list contains running

Logindex 6, thread 140478111237888, PID 1409, 21/05/2020 00:41:59 am: Freed linked list
Logindex 7, thread 140478111237888, PID 1409, 21/05/2020 00:41:59 am: Didn't touch ThreadData
Logindex 8, thread 140478102845184, PID 1409, 21/05/2020 00:41:59 am: I deleted the ThreadData object
first thread exited
wait for second thread to exit
second thread exited
```

