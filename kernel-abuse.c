/*
 * Inspired by:
 * http://www.linuxjournal.com/article/8110?page=0,1
 *
 * Copyright (C) 2011 Dwi Sasongko S <ruckuus@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

#define MAXSIZ 524288

char *evil;

module_param(evil, charp, 0000);
MODULE_PARM_DESC(evil, "file you want to sup to kernel");

static int read_file(char *filename)
{
	struct file *filp;
	long fsize;
	int i;
	char *kbuf;
	loff_t pos;

	filp = filp_open(filename, 0, 0);
	if (IS_ERR(filp)) {
		printk(KERN_INFO "Unable to load '%s'.\n", filename);
	}
	fsize = filp->f_path.dentry->d_inode->i_size;

	/*FIXME: what is the max size of n in vmalloc(n)
	 * should this be dynamic?
	 * vmalloc = <size> 
	 */
	if (fsize <= 0 || fsize > MAXSIZ) {
		printk(KERN_INFO "File size must be less than 512KB '%s'\n", filename);
		filp_close(filp, current->files);
		return 0;
	}

	kbuf = vmalloc(fsize);
	if (kbuf == NULL) {
		printk(KERN_INFO "Unable to allocate memory '%s'.\n", filename);
		filp_close(filp, current->files);
		return 0;
	}
	pos = 0;
	if (vfs_read(filp, kbuf, fsize, &pos) != fsize) {
		printk(KERN_INFO "Unable to read '%s'.\n", filename);
		vfree(kbuf);
		filp_close(filp, current->files);
		return 0;
	} else {
		for (i = 0; i < fsize; i++) {
			printk("%c", kbuf[i]);
		}
		vfree(kbuf);
	}
	filp_close(filp, current->files);
	return 0;

}

static int __init init(void)
{
	int rv;
	mm_segment_t old_fs = get_fs();

	printk(KERN_INFO "Init kernel abuse module.\n");
	if (evil != NULL) {
		set_fs(get_ds());
		rv = read_file(evil);
		set_fs(old_fs);
	} else {
		printk (KERN_INFO "Sup a file!");
	}
	return -EAGAIN;
}

static void __exit exit(void)
{
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dwi Sasongko S");
MODULE_DESCRIPTION("Don't Do That!");
module_init(init);
module_exit(exit);
