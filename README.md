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
[sched.c](https://github.com/xbonez/CS-370/blob/P2/linux-2.6.22.19-cs543/kernel/sched.c) (Line number 7218)

This syscall takes a pid_t as an argument and sets the UID and EUID for the
task to 0, effectively giving it root privilege.

We had no issues implementing this syscall. It was fairly simple, and didn't
require anything too fancy. We were simply required to loop over all
processes untill we found the target task and then change it's UID and EUID
to 0.

We tested it using
[test_steal](https://github.com/xbonez/CS-370/blob/P2/test_programs/test_steal.c).
We ran the program passing it the PID of bash. After the program had
executed, running `whoami` on bash showed us `root`, despite being logged in
as `user`.
