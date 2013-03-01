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

For this assignment, we modified the task_timeslice() function in
[`kernel/sched.c`](https://github.com/xbonez/CS-370/blob/P4/linux-2.6.22.19-cs543/kernel/sched.c)
file (line 229). We completely ignored the priority aspect of scheduling,
i.e. while the scheduler continued assigning priorities to tasks to reward/
penalize them for IO access etc., we paid no attention to this assigned
priority. 

In our implementation, allocation of timeslice entirely depends on the numer
of users running tasks on the system, and the number of processes per user.

The timeslice is calculated using 
`((TIMESLICE_USER_FACTOR * DEF_TIMESLICE) / USER_PROCESSES)`

`DEF_TIMESLICE` was a pre-existing default value that signifies a default
timeslice.

`TIMESLICE_USER_FACTOR` is a new constant we introduced with an arbitrary
value of 5. The reason we did this was that `DEF_TIMESLICE` is meant to be a
default timeslice for a single task. If we use the same value for a user,
each process the user is running will end up getting just a small portion of
that, making context switches too frequent.  

`USER_PROCESSES` is the total number of processes being run by the user of the
current task. We were able to access this value using
`atomic_read(&(p->user->processes))`.

Although we initially thought that we did not need to worry about division
by  (because we're getting current_user from the current task, thus it is
guaranteed that the current_user always has atleast one process), we
realized that it might be possible for the current task to end while
`task_timeslice()` was working. We weren't entirely sure if this was possible,
but decided to play it safe by ensuring the denominator is never 0.

We tested this implementation by creating two users (u1 and u2). u1 ran two
instances of `spin.py`, and u2 ran one instance of `spin.py`. (`spin.py` is a
never-ending while loop). As expected, the two instances of the process run
by u1 receive 25% CPU time, and u2's instance receives 50% CPU time (as
reported by top). Every few seconds (once per 5-10 seconds), the CPU usage
of these processes sharply drops for an instant, although they continue to
maintain the 0.25-0.25-0.50 ratio (an example, as can be  seen in the
screencast is when usage drops to 17%-17%-34%). We suspect these spikes
are due to the several other processes being run by root (including `top`).

We also implemented `read_myproc()` function to  list each of these users and
processes and their associated CPU timeslice allocated from the scheduler.
We placed this function in [`fs/proc/proc_misc.c`](https://github.com/xbonez/CS-370/blob/P4/linux-2.6.22.19-cs543/fs/proc/proc_misc.c) file. In this function we basically loop over each process in the task_struct and print out the user's UID, task's PID and the timeslice allocated to that task. 

The definition of the new method begins at line 82, and matches the
signature of all other proc methods. Our new method is registered on line
802 of the same file.
