#include <linux/kernel.h>
#include <asm/atomic.h>
#include <linux/mm.h>
#include <linux/file.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/mount.h>
#include <linux/pipe_fs_i.h>
#include <linux/uio.h>
#include <linux/highmem.h>
#include <linux/pagemap.h>
#include <linux/audit.h>
#include <linux/blkdev.h>
#include <asm/uaccess.h>
#include <asm/ioctls.h>

#include <asm/segment.h>
#include <linux/buffer_head.h>
#include "tar.h"

static struct super_operations tarfs_s_ops = {
        .statfs         = simple_statfs,
        .drop_inode     = generic_delete_inode,
};

static struct file_operations tarfs_file_ops = {
	.open 		=	tarfs_open,
	.read 		=	tarfs_read_file,
	.write 		=	tarfs_write_file,
};

static struct file_system_type tar_fs_type = {	
	.name		= "tarfs",
	.get_sb		= tarfs_get_super,
	.kill_sb	= kill_anon_super,
};

static struct vfsmount *tar_mnt __read_mostly;

static int tarfs_open(struct inode *inode, struct file *filp) {
	// assign index in our array to filp->private_data
	// for easy access later	
	filp->private_data = inode->i_private;
	return 0;
}

static ssize_t tarfs_read_file(struct file *filp, char* buf, size_t count, loff_t *offset) {
	char* filename;
	int j;
	int len;

	filename = filp->f_dentry->d_name.name;

	// use filename to find index for this file in our files array
	// for (j = 0; j < 2; j++)
	// 	if (strcmp(filename, files[j]->name) == 0)
	// 		break;
	
	// if (j == 2) {
	// 	// file not found
	// 	printk("File %s not found.\n", filename);
	// 	return 0;	
	// }

	// len = octalStringToInt(files[j]->size, 11);

	// if (*offset > len)
	// 	return 0;
	
	// if (count > len - *offset) 
	// 	count = len - *offset;

	// if (copy_to_user(buf, files[j]->contents, count))
	// 	return EFAULT;

	// *offset += count;
	// return count;

	return 0;
}


static ssize_t tarfs_write_file(struct file *filp, const char *buf,
		size_t count, loff_t *offset)
{
	atomic_t *counter = (atomic_t *) filp->private_data;
	char tmp[TMPSIZE];

	if (*offset != 0)
		return -EINVAL;
	if (count >= TMPSIZE)
		return -EINVAL;

	memset(tmp, 0, TMPSIZE);
	if (copy_from_user(tmp, buf, count))
		return -EFAULT;
	atomic_set(counter, simple_strtol(tmp, NULL, 10));
	return count;
}

static struct inode *tarfs_make_inode(struct super_block *sb, int mode, int file_idx)
{
	struct inode *ret = new_inode(sb);

	if (ret) {
		ret->i_mode = mode;
		ret->i_uid = 0;		
		ret->i_blkbits = blksize_bits(PAGE_CACHE_SIZE);
		ret->i_blocks = 0;
		ret->i_atime = ret->i_mtime = ret->i_ctime = CURRENT_TIME;

		if (file_idx < 0) {
			// not in our array
			ret->i_gid = 0;
			ret->i_size = 4096;
		} else {
			// set GID and size (bytes) for files in our array
			ret->i_gid = octalStringToInt(files[file_idx]->gid, 7);
			ret->i_size = octalStringToInt(files[file_idx]->size, 11);
		}
	}
	return ret;
}


static struct dentry *tarfs_create_file (struct super_block *sb,
		struct dentry *dir, const char *name, atomic_t *counter, int file_idx)
{
	struct dentry *dentry;
	struct inode *inode;
	struct qstr qname;
	/*
	 * Make a hashed version of the name to go with the dentry.
	 */
	qname.name = name;
	qname.len = strlen (name);
	qname.hash = full_name_hash(name, qname.len);
	
	// Now we can create our dentry and the inode to go with it.
	 
	dentry = d_alloc(dir, &qname);
	if (! dentry)
		goto out;
	inode = tarfs_make_inode(sb, S_IFREG | 0644, file_idx);
	if (! inode)
		goto out_dput;
	inode->i_fop = &tarfs_file_ops;
	inode->i_private = counter;
	/*
	 * Put it all into the dentry cache and we're done.
	 */
	d_add(dentry, inode);
	return dentry;
	/*
	 * Then again, maybe it didn't work.
	 */
  out_dput:
	dput(dentry);
  out:
	return 0;
}


static struct dentry *tarfs_create_dir (struct super_block *sb,
		struct dentry *parent, const char *name)
{
	struct dentry *dentry;
	struct inode *inode;
	struct qstr qname;

	qname.name = name;
	qname.len = strlen (name);
	qname.hash = full_name_hash(name, qname.len);
	dentry = d_alloc(parent, &qname);
	if (! dentry)
		goto out;

	inode = tarfs_make_inode(sb, S_IFDIR | 0644, -1);
	if (! inode)
		goto out_dput;
	inode->i_op = &simple_dir_inode_operations;
	inode->i_fop = &simple_dir_operations;

	d_add(dentry, inode);
	return dentry;

  out_dput:
	dput(dentry);
  out:
	return 0;
}


static atomic_t counter, subcounter;

static void tarfs_create_files (struct super_block *sb, struct dentry *root)
{
	struct dentry *subdir;
	int i;
	/*
	 * One counter in the top-level directory.
	 */
	// atomic_set(&counter, 0);

	for (i = 0; i < 2; i++) {
		tarfs_create_file(sb, root, files[i]->name, &counter, i);
	}

	/*
	 * And one in a subdirectory.
	 */
	// atomic_set(&subcounter, 0);
	// subdir = tarfs_create_dir(sb, root, "subdir");
	// if (subdir)
	// 	tarfs_create_file(sb, subdir, sourceFile, &subcounter);
}


static int tarfs_fill_super (struct super_block *sb, void *data, int silent)
{
	struct inode *root;
	struct dentry *root_dentry;

	// read in the headers from the mounted tarfile
	mount_tarfile();

	/*
	 * Basic parameters.
	 */
	sb->s_blocksize = PAGE_CACHE_SIZE;
	sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
	sb->s_magic = TARFS_MAGIC;
	sb->s_op = &tarfs_s_ops;

	/*
	 * We need to conjure up an inode to represent the root directory
	 * of this filesystem.  Its operations all come from libfs, so we
	 * don't have to mess with actually *doing* things inside this
	 * directory.
	 */
	root = tarfs_make_inode (sb, S_IFDIR | 0755, -1);
	if (! root)
		goto out;
	root->i_op = &simple_dir_inode_operations;
	root->i_fop = &simple_dir_operations;

	/*
	 * Get a dentry to represent the directory in core.
	 */
	root_dentry = d_alloc_root(root);
	if (! root_dentry)
		goto out_iput;
	sb->s_root = root_dentry;

	/*
	 * Make up the files which will be in this filesystem, and we're done.
	 */
	tarfs_create_files (sb, root_dentry);
	return 0;
	
  out_iput:
	iput(root);
  out:
	return -ENOMEM;
}


static struct super_block *tarfs_get_super(struct file_system_type *fst,
		int flags, const char *devname, void *data, struct vfsmount *mnt) {
	// copy filename to a global variable for later use
	sourceFile = devname;	
	return get_sb_single(fst, flags, data, tarfs_fill_super, mnt);
}


static int mount_tarfile() {
	if (sourceFile == NULL) {
		printk("No tar file specified.\n");
		return 1;
	}
	
	long filesize;
	int tarfile_size;
	int offset; 	// offset in tar file
	int i, j; 		// loop counter

	// file handler
	struct file* fh = file_open(sourceFile, O_RDWR, 0);

	if (fh != NULL) {
		filesize = 10240;
		filecontents = (char*)kmalloc(filesize + 1, GFP_KERNEL);		// +1 for null terminator
		
		file_read(fh, fh->f_pos, filecontents, filesize);
		filecontents[filesize] = 0;

		file_close(fh);

		offset = 0;

		files[0] = (struct tarfile*) kmalloc(sizeof(struct tarfile*), GFP_KERNEL);
		files[1] = (struct tarfile*) kmalloc(sizeof(struct tarfile*), GFP_KERNEL);
		
		for(j = 0; j < 2; j++) {
			// read in filename
			for (i = 0 + offset; i < 100 + offset; i++) {
				if (filecontents[i]) {				
					files[j]->name[i - (offset)] = filecontents[i];
				} else
					break;
			}
			// null terminate
			files[j]->name[i - offset] = 0;
			
			// read in owner's numeric id	
			for (i = 108 + offset; i < 116 + offset; i++) {			
				files[j]->uid[i-(108 + offset)] = filecontents[i];
			}
			// null terminate
			files[j]->uid[8] = 0;

			// read in group id
			for (i = 116 + offset; i < 124 + offset; i++) {
				files[j]->gid[i-(116 + offset)] = filecontents[i];
			}
			// null terminate
			files[j]->gid[8] = 0;
			
			
			// // read in file size
			// for (i = 124 + offset; i < 136 + offset; i++) {
			// 	size[i-(124 + offset)] = filecontents[i];
			// }
			
			files[j]->size = octalStringToInt(&filecontents[124+offset], 11);

			// store contents offset
			files[j]->contentOffset = 512 + offset;
			tarfile_size = files[j]->size;
			// tarfile_size = octalStringToInt(files[j]->size, 11);
			
			// update offset
			offset = (offset + ((tarfile_size/512) + 1) * 512) + 512;
		}

		for (j = 0; j < 2; j++) {
			printk("File name: %s\n", files[j]->name);
			// printk("UID Octal: %s\n", files[j]->uid);
			printk("UID: %d\n", octalStringToInt(files[j]->uid, 7));
			printk("GID: %d\n", octalStringToInt(files[j]->gid, 7));
			printk("File size: %d\n", files[j]->size);
			//printk("File contents: %s\n", files[j]->contents);
			printk("Contents begin at offset: %d\n", files[j]->contentOffset);
			printk("\n");		
		}

		return 0;
	} else {
		printk("Failed to open file");
		return 1;
	}
}

static int __init init_tar_fs(void) {
	return register_filesystem(&tar_fs_type);
}

static void __exit exit_tar_fs(void) {
	unregister_filesystem(&tar_fs_type);
	mntput(tar_mnt);
}

fs_initcall(init_tar_fs);
module_exit(exit_tar_fs);


// Utility function to convert an octal string to int
// Source: http://stackoverflow.com/questions/2505042/how-to-parse-a-tar-file-in-c

static int octalStringToInt(char *string, unsigned int size) {
	unsigned int output = 0;
	while(size > 0){
		output = output*8 + *string - '0';
		string++;
		size--;
	}
	return output;
}


// Utility functions to open / read files
// Source: http://stackoverflow.com/questions/1184274/how-to-read-write-files-within-a-linux-kernel-module

struct file* file_open(const char* path, int flags, int rights) {
    struct file* filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if(IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
}

int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}   

void file_close(struct file* file) {
    filp_close(file, NULL);
}