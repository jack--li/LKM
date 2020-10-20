#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/zlib.h>
#include <linux/zutil.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/timer.h>

#include <linux/sched.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/time.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/poll.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>

extern void msleep(unsigned int msecs);
static z_stream  strm;
unsigned char out[100];
unsigned char read_buf[100];

//#define FILE_NAME "/home/l00424143/github/LKM/1.txt"
#define FILE_NAME "/data/1.txt.gz"

static int bnx2x_gunzip(const u8 *out, const u8 *zbuf, int len)
{
	int n, rc;

	/* check gzip header */
	if ((zbuf[0] != 0x1f) || (zbuf[1] != 0x8b) || (zbuf[2] != Z_DEFLATED)) {
		pr_err("Bad gzip header\n");
		return -EINVAL;
	}

	n = 10;

#define FNAME				0x8

	if (zbuf[3] & FNAME)
		while ((zbuf[n++] != 0) && (n < len));

	strm.next_in = (typeof(strm.next_in))zbuf + n;
	strm.avail_in = len - n;
	strm.next_out = out;
	strm.avail_out = 100;

	rc = zlib_inflateInit2(&strm, -MAX_WBITS);
	if (rc != Z_OK)
		return rc;

	rc = zlib_inflate(&strm, Z_FINISH);
	if ((rc != Z_OK) && (rc != Z_STREAM_END))
		pr_err("Firmware decompression error: %s\n", strm.msg);

	zlib_inflateEnd(&strm);
	if (rc == Z_STREAM_END)
		return 0;

	return rc;
}
static ssize_t mlog_test_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	struct file *filp;
	loff_t pos = 0;	
	int ret;
	int i = 0;
	filp = filp_open(FILE_NAME, O_RDONLY, 0);
	if ((filp == NULL) || IS_ERR(filp)) {
		pr_err("error\n");
		return -1;
	}
	pr_err("lpf open ok\n");
	ret = kernel_read(filp, read_buf, 100, &pos);
	if (ret < 0 ) {
	  return 0;
	}
	pr_err("lpf read ok %d\n", ret );
	for (; i < ret; i++) {
		pr_err("0x%x ", read_buf[i]);
	}
	
	ret = bnx2x_gunzip(out, read_buf, ret);
	 if (ret != 0) {
         pr_err("fail to decompress image , error code %d\n", ret);
         return ret;
     } else {
         pr_err("decompress image success. file length = 0x%x\n", (unsigned)ret);
     }
	pr_err("%s\n", out);
	return 0;
}

static const struct file_operations proc_mlog_log_operations = {
    .read        = mlog_test_read,
};

static int __init hello_init(void)
{
	proc_create("test", S_IRUSR, NULL, &proc_mlog_log_operations);
	strm.workspace = vmalloc(zlib_inflate_workspacesize());
	if (!strm.workspace)
		return -1;

	printk(KERN_ALERT "lipengfei, world\n");
	return 0;
}
static void __exit hello_exit(void)
{
printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("XJ");

