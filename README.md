## Project 2: System Calls and Processes ##

**Prof. Willian Mongan**

**Team** - 
* Ayush Sobti  
* Lakshit Dhanda

[**Wiki for this project**](https://github.com/xbonez/CS-370/wiki/Project-2)

#### Description ####

For this assignment, we were required to add several system calls to the
Linux kernel and test them.

The work was done on the linux kernel (ver. 2.6.22.19). 
Compilation was done on Tux, using gcc (ver. 4.6.3).
Versioning was done using Git, hosted on Github.

All implemented syscalls were tested using test programs we created, located
in [test_programs](https://github.com/xbonez/CS-370/tree/P2/test_programs). As far as we can tell, all syscalls work as expected.

This was an interesting, and moderately challenging assignment. Since this
was our first exposure to the linux kernel, we did occassionally have to
google some issues we had, but we were able to figure everything out either
by reading articles on the web, or by reading the source code.


#### Assignment Details ####

#####sys_steal()#####

**Syscall number:** 286

**Location:**
[`sched.c`](https://github.com/xbonez/CS-370/blob/P2/linux-2.6.22.19-cs543/kernel/sched.c) (Line number 7218)

This syscall takes a `pid_t` as an argument and sets the `UID` and `EUID` for the
task to `0`, effectively giving it root privilege. The method returns a `0` on
success, and a `-1` on failure.

We had no issues implementing this syscall. It was fairly simple, and didn't
require anything too fancy. We were simply required to loop over all
processes untill we found the target task and then change it's `UID` and
`EUID` to `0`.

We tested it using
[`test_steal`](https://github.com/xbonez/CS-370/blob/P2/test_programs/test_steal.c).
We ran the program passing it the `PID` of `bash`. After the program had
executed, running `whoami` on bash showed us `root`, despite being logged in
as `user`.


#####sys_quad()#####

**Syscall number:** 287

**Location:**
[`sched.c`](https://github.com/xbonez/CS-370/blob/P2/linux-2.6.22.19-cs543/kernel/sched.c)
(Line number 7248)

This syscall takes a `pid_t` as an argument and increases the time slice of
the task by a factor of 4. The syscall returns the new timeslice on success
and a `-1` on failure.

We had no issues implementing this syscall but had some trouble testing it
as described below.

We attempted to test it using
[`test_quad`](https://github.com/xbonez/CS-370/blob/P2/test_programs/test_quad.c).
The program would run fine and return the new timeslice of the target task,
however, we weren't sure if it had indeed quadrupled the timeslice. We
figured we could run it on the same process multiple times consecutively and
see the timeslice increasing by a factor of 4 everytime, but soon realized
that wouldn't work because the scheduler is constantly updating timeslices.



#####sys_swipe()#####

**Syscall number:** 288

**Location:**
[`sched.c`](https://github.com/xbonez/CS-370/blob/P2/linux-2.6.22.19-cs543/kernel/sched.c)
(Line number 7282)

This syscall takes a target `pid_t` and a victim `pid_t` as arguments. It
increases the target's timeslice by the sum of the timeslices of the victim
and all it's children, and sets their timeslices to 0 effectively swiping
their time slice. The method returns the total swiped timeslice.

Before swiping from the victim or any of it's children, the syscall ensures
the task it is about to swipe from isn't the target task. Swiping the
victim's timeslice was very easy having done `sys_quad`. We had to delve
into some other functions within `sched.c` and view `task_struct` to
understand how to iterate all children of victim. Once we figured that out,
it was trivial from there on out.

We attempted to test it using
[`test_swipe`](https://github.com/xbonez/CS-370/blob/P2/test_programs/test_swipe.c).
Once again, there was no deterministic way of ensuring that the timeslice
was swiped from victim and all of it's chidren. We were satisfied with
receiving a return value > 0, proving that some time had definitely been
stolen.
