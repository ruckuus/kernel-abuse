#include <linux/vmalloc.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#include "file_write.h"

int file_write(char *filename)
{
	struct file *out;
	long fsize;
	int i;
	char *kbuf = "Hello World from the ground!\n";
	loff_t pos;

	out = filp_open(filename, O_WRONLY|O_CREAT, 0644);
	if (IS_ERR(out)) {
		printk(KERN_INFO "Unable to open '%s'.\n", filename);
	}
	fsize = out->f_path.dentry->d_inode->i_size;
	pos = 0;
	i = vfs_write(out, (char __user *)kbuf, 40, &pos);

	filp_close(out, current->files);

	return i;
}
