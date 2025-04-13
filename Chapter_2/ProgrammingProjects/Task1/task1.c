//	This is an update based on simple.c to implement guide tasks in 2.17.II
//of the book 	The next step is to merge it with proc VFS to create files in
///proc
#include <asm-generic/param.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/module.h>
u64 JF0;
int task1_init(void) {
  JF0 = jiffies;
  printk(KERN_INFO "Jiffies is %llu now\n", JF0);
  printk(KERN_INFO "Loading Task1 Kernel Module\n");
  printk(KERN_INFO "The HZ is %d/s or %dms each time\n", HZ, 1000 / HZ);
  return 0;
}

void task1_exit(void) {
  printk(KERN_INFO "Exiting Task1 Kernel Module\n");
  printk(KERN_INFO "Jiffies is %lu now\n", jiffies);
  printk(KERN_INFO "The module has been running for %llu seconds",
         (jiffies - JF0) / HZ);
}

module_init(task1_init);
module_exit(task1_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Task1 Module");
MODULE_AUTHOR("Cellxi");
