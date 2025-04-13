#This Makefile is for hello module.
#Change "hello.o" to "simple.o" if you need one for simple module.
obj-m += hello.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

