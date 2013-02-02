## Project 2: System Calls and Processes ##

**Prof. Willian Mongan**

**Team** - 
* Ayush Sobti  
* Lakshit Dhanda

[**Wiki for this project**](https://github.com/xbonez/CS-370/wiki/Project-2)

-----

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

-----

#### Assignment Details ####

####sys_steal()####

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


####sys_quad()####

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


####sys_swipe()####

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


####sys_zombify()####

**Syscall number:** 290

**Location:**
[`sched.c`](https://github.com/xbonez/CS-370/blob/P2/linux-2.6.22.19-cs543/kernel/sched.c)
(Line number 7343)

This syscall takes a `pid_t` as argument, and sets the target task's state
to `EXIT_ZOMBIE`. The syscall returns a `0` on success and a `-1` on
failure.

This sycall did not pose too much of a trouble either. We iterated over the
list of all processes looking for the target task. Once we found it, we set
it's state to `EXIT_ZOMBIE`, and return.

Unlike our `sys_zombify()` syscall, `do_exit()` does a lot more work. It
makes sure the task being killed isn't `init` or an interrupt handler. It
confirms the process being killed doesn't hold any locks sends a SIGCHLD
signal so the parent of the task can now that the child is now dying.

We tested this syscall using
[`test_zombify`](https://github.com/xbonez/CS-370/blob/P2/test_programs/test_zombify.c).
We ran the program passing it the PID of our bash. The program exits
successfully, but zombifies our bash, which means we lose control of the
terminal.


####sys_myjoin()####

**Syscall number:** 291

**Location:**
[`exit.c`](https://github.com/xbonez/CS-370/blob/P2/linux-2.6.22.19-cs543/kernel/exit.c)
(Line number 1072)

This syscall takes in a target task PID, and blocks the current process
until the target task has finished running.

This syscall requires some sharing of information between `sys_myjoin()` and
`do_exit()`. So, we began by creating a `struct myjoin_shared` in
[`sched.h`](https://github.com/xbonez/CS-370/blob/P2/linux-2.6.22.19-cs543/include/linux/sched.h).
This struct has three fields: 
`isWaiting` which indicates whether there is a process waiting to be awoken
up.  
`*currentTask` : A pointer to the current task that has been put to sleep
and needs to be awoken.  
`*targetTask` : The task we are waiting for.

We create an instance of this shared object gloabl to `exit.c` so it may be
shared between `do_exit()` and `sys_myjoin()` without having to be passed
around. The declaration can be found at line 56 of `exit.c`. 

`sys_myjoin()` (beginning at line 1072) begins by iterating over all
processes to find the target task. It confirms that a target task was indeed
found, and uses double-checked locking to confirm that the task is not a
zombie or dead. It then sets the `isWaiting` flag in `sharedObj` to 1, and
assigned `sharedObj.currentTask` and `sharedObj.targetTask` with pointers
to the current and target task respectively.

We then set the current task to `TASK_UNINTERRUPTIBLE` using
`set_current_state()`. `set_current_state()` sets the state of the current
task to the state passed in as an argument. The subsequent call to the
scheduler makes the scheduler remove this task from the run queue and
schedule another task to run. The task put to sleep can later be woken up
using `wake_up_process()`. We found this strategy
[here](http://www.linuxjournal.com/article/8144) when searching for the
best way to put the current task to sleep such that it can be woken up
later.

Once this has been done, `do_exit()` is responsible for awakening the task
put to sleep when the targetTask finishes. The target task will call
`do_exit()` upon completion. At the end of `do_exit()` (line 1010), we
appended coded that checks if the `isWaiting` flag is set to `1`, and if the
task that was just exited was our target task. If found to be our target
task, the earlier process that was put to sleep is woken up via a call to
`wake_up_process()`, and the `isWaiting` flag is reset to 0.

The assignemnt asked us to not worry about multiple tasks joining, however,
our solution can be easily extended to multiple tasks by changing
`targetTask` to an array, and incrementing `isWaiting` by 1 for every
process that is being waited on.


To test this program, we wrote
[`test_myjoin`](https://github.com/xbonez/CS-370/blob/P2/test_programs/test_myjoin.c)
as well as a small python program called
[`spin_process`](https://github.com/xbonez/CS-370/blob/P2/test_programs/spin_process.py)
that runs an empty while loop for as long as specified via a command line
argument. We ran `spin_process` for 60 seconds, pushed it to the background
while keeping it running, grabbed it's PID and passed it to `test_myjoin`.
Our bash shell locked up until `spin_process` completed at which point we
were returned control.


####sys_forcewrite()####

**Syscall number:** 290

**Location:**
[`read_write.c`](https://github.com/xbonez/CS-370/blob/P2/linux-2.6.22.19-cs543/fs/read_write.c)
(Line number 836)

This syscall functions much like `write()` syscall, except that it does not
check for file permissions and allows the user to write regardless of
whether they have writable permissions or not. 

We found there were several ways to approach this. When using `write()` to
write to a file, it needs to first be opened using the `open()` syscall,
which fails if you attempt to open a read-only file in `O_WRONLY` or
`O_APPEND` modes. We could have written our own version of `open()` that
allows you to open a read-only file in either of those modes, however, we
decided to take the approach where if you want to write to a read-only file,
you open it using the `O_RONLY` flag, but then when you make the call to
`forcewrite()` you will be allowed to write to it despite the file being
opened in read mode.

Upon inspecting `sys_write()` we found that the permissions check wasn't
being done in there, but instead in the call to `vfs_write()`. Thus,
`forcewrite()` is an exact copy of `sys_write()` with the only change being
that instead of calling `vfs_write()`, it calls `my_vfs_write()`. We then
wrote our own version of `vfs_write()` called `my_vfs_write()` where we
commented out the code that checks permissions. Without these checks in
place, the user cna now write successfully to a read only file.

To test this program, we wrote
[`test_forcewrite`](https://github.com/xbonez/CS-370/blob/P2/test_programs/test_forcewrite.c).
We created a file `unwritable_file` and ran `chmod 500 unwritable_file` to
make sure no one had write permissions on the file. After that, we ran
`test_forcewrite` passing it `unwritable_file` as the destination and
confirmed that were, in fact, able to write to this file.


-----


###Additional Notes###
