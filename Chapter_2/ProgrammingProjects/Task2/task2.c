#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

#define PROC1 "jiffies"
#define PROC2 "seconds"
#define BUFF_SIZE 256

ssize_t jiffies_read (struct file *file, char __user *usr_buf, size_t count,
                      loff_t *pos);
ssize_t seconds_read (struct file *file, char __user *usr_buf, size_t count,
                      loff_t *pos);
static struct proc_ops jiffies_ops = {
    .proc_read = jiffies_read,
};
static struct proc_ops seconds_ops = {
    .proc_read = seconds_read,
};

static unsigned long jf0;
int task2_init (void)
{
    jf0 = jiffies;
    proc_create (PROC1, 0666, NULL, &jiffies_ops);
    proc_create (PROC2, 0666, NULL, &seconds_ops);
    return 0;
}

ssize_t jiffies_read (struct file *file, char __user *usr_buf, size_t count,
                      loff_t *pos)
{
    if (*pos > 0)
        return 0;
    char buffer[BUFF_SIZE / 2];
    *pos = sprintf (buffer, "The value of jiffies is %lu now\n", jiffies);
    if (copy_to_user (usr_buf, buffer, *pos))
        return EFAULT;
    else
        return *pos;
}
ssize_t seconds_read (struct file *file, char __user *usr_buf, size_t count,
                      loff_t *pos)
{
    if (*pos > 0)
        return 0;
    char buffer[BUFF_SIZE / 2];
    *pos = sprintf (buffer,
                    "It has been %lu seconds since the module was loaded.\n",
                    (jiffies - jf0) / HZ);
    if (copy_to_user (usr_buf, buffer, *pos))
        return EFAULT;
    else
        return *pos;
}
void task2_exit (void)
{
    remove_proc_entry (PROC1, NULL);
    remove_proc_entry (PROC2, NULL);
    printk (KERN_INFO "Exiting Task2 Module\n");
}

module_init (task2_init);
module_exit (task2_exit);
MODULE_LICENSE ("GPL");
MODULE_DESCRIPTION ("Task2 Module");
MODULE_AUTHOR ("Cellxi");
