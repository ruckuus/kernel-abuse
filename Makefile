obj-m += kabuse.o
kabuse-objs += file_write.o kernel-abuse.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	rm -rf *.o *.mod.c *.ko *~ modules.order Module.symvers .*cmd  .tmp_versions/           
