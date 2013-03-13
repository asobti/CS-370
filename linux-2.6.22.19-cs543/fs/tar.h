#define TARFS_MAGIC 0x19980122
#define TMPSIZE 20


// define file operations


static int tarfs_open(struct inode *inode, struct file *filp);

static ssize_t tarfs_read_file(struct file *filp, char* buf, size_t count, loff_t *offset);

static ssize_t tarfs_write_file(struct file *filp, const char *buf, size_t count, loff_t *offset);

static struct inode *tarfs_make_inode(struct super_block *sb, int mode);

static struct dentry *tarfs_create_file (struct super_block *sb, struct dentry *dir, const char *name, atomic_t *counter);


static struct dentry *tarfs_create_dir (struct super_block *sb, struct dentry *parent, const char *name);


static void tarfs_create_files (struct super_block *sb, struct dentry *root);

static int tarfs_fill_super (struct super_block *sb, void *data, int silent);

static struct super_block *tarfs_get_super(struct file_system_type *fst, int flags, const char *devname, void *data);

static int __init init_tar_fs(void);

static void __exit exit_tar_fs(void);
