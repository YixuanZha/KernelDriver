#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "ioctl_test.h"

#define FilePath "/dev/ioctldevice"

int main()
{
    int value_to_set = 123;
    int value_to_get;

    int fd;
    fd = open(FilePath,O_RDWR);
    if(fd < 0)
    {
        perror("failed to open the file");
        return -1;
    }

    printf("Open the file successfully,the fd is: %d\n",fd);
    
    printf("Calling ioctl to set value to %d\n",value_to_set);
    if(ioctl(fd,IOCTL_SET_VAL,&value_to_set) < 0)
    {
        perror("IOCTL_SET_VAL failed");
        close(fd);
        return -1;
    }

    printf("Calling ioctl to get value\n");
    if(ioctl(fd,IOCTL_GET_VAL,&value_to_get) < 0)
    {
        perror("IOCTL_GET_VAL failed");
        close(fd);
        return -1;
    }

    printf("Value read from the kernel is: %d\n",value_to_get);

    close(fd);
    return 0;
}