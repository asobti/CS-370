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
#include "tar.h"

//#define TARFS_MAGIC 0x19980122
//#define TMPSIZE 20

static struct super_operations tarfs_s_ops = {
        .statfs         = simple_statfs,
        .drop_inode     = generic_delete_inode,
};

static struct file_operations tarfs_file_ops = {
	.open 		=	tarfs_open,
	.read 		=	tarfs_read_file,
	.write 		=	tarfs_write_file,
};

static struct vfsmount *tar_mnt __read_mostly;

static int tarfs_open(struct inode *inode, struct file *filp) {
	 filp->private_data = inode->i_private;
	return 0;
}


static ssize_t tarfs_read_file(struct file *filp, char* buf, size_t count, loff_t *offset) {

	atomic_t *counter = (atomic_t *) filp->private_data;
	int v = atomic_read(counter);
	int len;
	atomic_inc(counter);
	char tmp[TMPSIZE];

	len = snprintf(tmp, TMPSIZE, "%d\n", v);
	if (*offset > len)
		return 0;
	if (count > len - *offset)
		count = len - *offset;

	if (copy_to_user(buf, tmp + *offset, count))
		return -EFAULT;
	*offset += count;
	return count;
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

// static int tarfs_fill_super (struct super_block *sb, 
//                                void *data, int silent) {
// 	sb->s_blocksize = PAGE_CACHE_SIZE;
// 	sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
// 	sb->s_magic = TARFS_MAGIC;
// 	sb->s_op = &tarfs_s_ops;

// 	// called at mount time
// }

static struct inode *tarfs_make_inode(struct super_block *sb, int mode)
{
	struct inode *ret = new_inode(sb);

	if (ret) {
		ret->i_mode = mode;
		ret->i_uid = ret->i_gid = 0;
		ret->i_blkbits = blksize_bits(PAGE_CACHE_SIZE);
		ret->i_blocks = 0;
		ret->i_atime = ret->i_mtime = ret->i_ctime = CURRENT_TIME;
	}
	return ret;
}


// static struct dentry *tarfs_create_file (struct super_block *sb,
// 		struct dentry *dir, const char *name,
// 		atomic_t *counter)
// {
// 	struct dentry *dentry;
// 	struct inode *inode;
// 	struct qstr qname;
// 	/*
// 	 * Make a hashed version of the name to go with the dentry.
// 	 */
// 	qname.name = name;
// 	qname.len = strlen (name);
// 	qname.hash = full_name_hash(name, qname.len);
	
// 	 * Now we can create our dentry and the inode to go with it.
	 
// 	dentry = d_alloc(dir, &qname);
// 	if (! dentry)
// 		goto out;
// 	inode = tarfs_make_inode(sb, S_IFREG | 0644);
// 	if (! inode)
// 		goto out_dput;
// 	inode->i_fop = &tarfs_file_ops;
// 	inode->i_private = counter;
// 	/*
// 	 * Put it all into the dentry cache and we're done.
// 	 */
// 	d_add(dentry, inode);
// 	return dentry;
// 	/*
// 	 * Then again, maybe it didn't work.
// 	 */
//   out_dput:
// 	dput(dentry);
//   out:
// 	return 0;
// }


// static struct dentry *tarfs_create_dir (struct super_block *sb,
// 		struct dentry *parent, const char *name)
// {
// 	struct dentry *dentry;
// 	struct inode *inode;
// 	struct qstr qname;

// 	qname.name = name;
// 	qname.len = strlen (name);
// 	qname.hash = full_name_hash(name, qname.len);
// 	dentry = d_alloc(parent, &qname);
// 	if (! dentry)
// 		goto out;

// 	inode = tarfs_make_inode(sb, S_IFDIR | 0644);
// 	if (! inode)
// 		goto out_dput;
// 	inode->i_op = &simple_dir_inode_operations;
// 	inode->i_fop = &simple_dir_operations;

// 	d_add(dentry, inode);
// 	return dentry;

//   out_dput:
// 	dput(dentry);
//   out:
// 	return 0;
// }


static atomic_t counter, subcounter;

// static void tarfs_create_files (struct super_block *sb, struct dentry *root)
// {
// 	struct dentry *subdir;

// 	/*
// 	 * One counter in the top-level directory.
// 	 */
// 	atomic_set(&counter, 0);
// 	tarfs_create_file(sb, root, "counter", &counter);

// 	/*
// 	 * And one in a subdirectory.
// 	 */
// 	// atomic_set(&subcounter, 0);
// 	// subdir = tarfs_create_dir(sb, root, "subdir");
// 	// if (subdir)
// 	// 	tarfs_create_file(sb, subdir, "subcounter", &subcounter);
// }


static int tarfs_fill_super (struct super_block *sb, void *data, int silent)
{
	struct inode *root;
	struct dentry *root_dentry;

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
	root = tarfs_make_inode (sb, S_IFDIR | 0755);
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
	// tarfs_create_files (sb, root_dentry);
	return 0;
	
  out_iput:
	iput(root);
  out:
	return -ENOMEM;
}


static struct super_block *tarfs_get_super(struct file_system_type *fst,
		int flags, const char *devname, void *data) {
	return get_sb_single(fst, flags, data, tarfs_fill_super, tar_mnt);
}

static struct file_system_type tar_fs_type = {
	.owner		= THIS_MODULE,
	.name		= "tarfs",
	.get_sb		= tarfs_get_super,
	.kill_sb	= kill_anon_super,
};

static int __init init_tar_fs(void) {
	return register_filesystem(&tar_fs_type);
}

static void __exit exit_tar_fs(void) {
	unregister_filesystem(&tar_fs_type);
}


fs_initcall(init_tar_fs);
module_exit(exit_tar_fs);