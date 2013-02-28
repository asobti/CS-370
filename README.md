## Project 4: Fair Share Scheduling ##

**Prof. Willian Mongan**

**Team** - 
* Ayush Sobti  
* Lakshit Dhanda

[**Wiki for this project**](https://github.com/xbonez/CS-370/wiki/Project-4)

-----
#### Description ####

For this assignment, we were required  to implement a new scheduling policy that allocates equal CPU time to each user account.

The work was done on the linux kernel (ver. 2.6.22.19). 
Compilation was done on Tux, using gcc (ver. 4.6.3).
Versioning was done using Git, hosted on Github.

This was an interesting, and moderately challenging assignment. It really put our thinking to test.

#### Assignment Details ####

For this assignment, we modified the task_timeslice() function in kernel/sched.c file. We completely ignored the priority aspect of scheduling. In our implementation, allocation of timeslice entirely depends on the numer of users and number of processes per user.

The timeslice is calculated using ((TIMESLICE_USER_FACTOR * DEF_TIMESLICE) / USER_PROCESSES) where TIMESLICE_USER_FACTOR is 5 miliseconds.  We were not worried about division by 0, because we're getting current_user from the current task, thus it is guaranteed that the current_user always has atleast one process. We got the number of processes per user by using atomic_read().
