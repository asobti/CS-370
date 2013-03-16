## Project 5: TAR FS ##

**Prof. Willian Mongan**

**Team** - 
* Ayush Sobti  
* Lakshit Dhanda

[**Wiki for this project**](https://github.com/xbonez/CS-370/wiki/Project-5)

-----
**Description**

For this assignment, we were required read the tar file format specifications and implement the tar fs module capable of mounting a tar file at a mount point.

The work was done on the linux kernel (ver. 2.6.22.19). Compilation was done on Tux, using gcc (ver. 4.6.3). Versioning was done using Git, hosted on Github.

This was an interesting, and challenging assignment. It really put our thinking to test.

**Assignment Details**

For this assignment, we first wrote a program called **tarreader.c** that can read a tar file, list each filename, file size, and other attributes in user space. We have provided a makefile to compile and run this program. The input tarfile is readme.tar and it has two files bar and foo. To compile and run this program, type make and make run. 
We defined a struct called tarfile in tarreader.h file. This program basically opens up the input tarfile and then decodes the contents of the tarfile and stores them in an array tarfile structs. After that, it lists the contents of the tarfile and then extracts the files inside the tarfile. 
The offset for reading different attributes and contents of the files gets calculated using `offset = (offset + ((tarfile_size/512) + 1) * 512) + 512`

We used to some utility functions to accomplish this task.

Utility function to convert an octal string to int - `static int octalStringToInt(char *string, unsigned int size);` 
Source: http://stackoverflow.com/questions/2505042/how-to-parse-a-tar-file-in-c

Utility functions to open / read files - `struct file* file_open(const char* path, int flags, int rights);`, 
`int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size);` 
Source: http://stackoverflow.com/questions/1184274/how-to-read-write-files-within-a-linux-kernel-module

We wrote a program called tar.c [`/ linux-2.6.22.19-cs543 / fs / tar.c`] for mounting a tar file at a mount point. Our program successfully mounts the tarfile at the mount point and then lists the names of files inside. We were not able to print out the contents and then unmount the files. We tried a lot of different approaches to solve the problem. But we kept on getting a lot of segmentation faults. 

The mounting of the tar file is done inside function `mount_tarfile()`.  
