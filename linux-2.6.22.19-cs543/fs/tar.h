#define TARFS_MAGIC 0x19980122
#define TMPSIZE 20

struct tarfile
{                              /* byte offset */
  char name[100];               /*   0 */
  char mode[8];                 /* 100 */
  char uid[8];                  /* 108 */
  char gid[8];                  /* 116 */
  char size[12];                /* 124 */
  char mtime[12];               /* 136 */
  char chksum[8];               /* 148 */
  char typeflag;                /* 156 */
  char linkname[100];           /* 157 */
  char magic[6];                /* 257 */
  char version[2];              /* 263 */
  char uname[32];               /* 265 */
  char gname[32];               /* 297 */
  char devmajor[8];             /* 329 */
  char devminor[8];             /* 337 */
  char prefix[155];             /* 345 */
                                /* 500 */
  char* contents;
};

// define file operations
const char* sourceFile = NULL;

// files array
struct tarfile* files[2];

static int tarfs_open(struct inode *inode, struct file *filp);

static ssize_t tarfs_read_file(struct file *filp, char* buf, size_t count, loff_t *offset);

static ssize_t tarfs_write_file(struct file *filp, const char *buf, size_t count, loff_t *offset);

static struct inode *tarfs_make_inode(struct super_block *sb, int mode, int file_idx);

static struct dentry *tarfs_create_file (struct super_block *sb, struct dentry *dir, const char *name, atomic_t *counter, int file_idx);


static struct dentry *tarfs_create_dir (struct super_block *sb, struct dentry *parent, const char *name);


static void tarfs_create_files (struct super_block *sb, struct dentry *root);

static int tarfs_fill_super (struct super_block *sb, void *data, int silent);

static struct super_block *tarfs_get_super(struct file_system_type *fst, int flags, const char *devname, void *data, struct vfsmount *mnt);

static int __init init_tar_fs(void);

static void __exit exit_tar_fs(void);

static int mount_tarfile();

static int octalStringToInt(char *string, unsigned int size);

struct file* file_open(const char* path, int flags, int rights);
int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size);
void file_close(struct file* file);