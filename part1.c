#include</usr/include/linux/kernel.h>
#include</usr/include/arm-linux-gnueabihf/sys/syscall.h>
#include</usr/include/stdio.h>
#include<stdlib.h>

int main (int argc, const char* argv[]){
	int pid = atoi(argv[1]);
	syscall(400, pid);


}
