### Project 1: Booting Your Custom Kernel ###

Prof. Willian Mongan

**Team** - Ayush Sobti, Lakshit Dhanda

[Wiki for this project](https://github.com/xbonez/CS-370/wiki/Project-1)

For this project we were required to set up the environment, build the kernel and then modify it to accept a new command line argument called ‘printme’. In the presence of this command line arugment, the kernel would print a debug message ‘Hello World From Me!’. 

We modified the "main.c" file. We added a new variable called 'printme'. We then added EXPORT_SYMBOL(printme). We then created a function called 'set_printme' and called __setup("printme", set_printme). Finally in start_kernel, we checked if printme was checked and printed the message using printk.

We did not really face any major difficulties in any of the steps. We were slightly stumped as to how the command line argument was to be supplied. Initially we tried to add it to the end of the command that launches the VM, but we soon realized the argument was being caught by the VM and not our kernel. Adding this new argument within the ‘append’ argument achieved what we were intending.

Replace 'main.c' in your kernel's init folder and run your VM with this modified kernel.
