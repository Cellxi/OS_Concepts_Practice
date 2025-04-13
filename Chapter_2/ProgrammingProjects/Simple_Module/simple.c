#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

//  You must declare the parameter as void explicitly rather than omit it
int simple_init(void) {
  printk(KERN_INFO "Loading Kernel Module\n");
  return 0;
}

void simple_exit(void) { printk(KERN_INFO "Removing Kernel Module\n"); }

//  These two macros are defined in init.h
module_init(simple_init);  //  it tells kernel where to start this module
module_exit(simple_exit);  //  it tells kernel where to exit it

//  These three are defined in module.h
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("Cellxi");
