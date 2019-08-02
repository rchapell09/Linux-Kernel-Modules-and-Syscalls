export
CCPREFIX=arm-linux-gnueabihf-
sysroot := "/home/dylan/linux"
obj-m += procaccess.o

all:
	make ARCH=arm CROSS_COMPILE=${CCPREFIX} -C $(sysroot) M=$(PWD) modules

clean:
	make -C $(sysroot) M=$(PWD) clean
