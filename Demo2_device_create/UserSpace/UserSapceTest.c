#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define dev_name "/dev/mynewdev"

int main()
{
    char buffer[256];
    ssize_t bytes_read, bytes_write;
    int fd = open(dev_name,O_RDWR);
    if(fd == -1)
    {
        perror("Failed to open the dev file");
        return 1;
    }

    bytes_read = read(fd,buffer,sizeof(buffer) - 1);
    if(bytes_read == -1)
    {
        perror("Failed to read from the dev file");
        return 1;
    }
    else
    {
        buffer[bytes_read] = '\0';
        printf("Read %zd bytes from the dev, the context is: '%s' \n\n",bytes_read,buffer);
    }

    const char* write_data = "Hi kernel, this is user space\n";
    bytes_write = write(fd,write_data,strlen(write_data));
    if(bytes_write == -1)
    {
        perror("Failed to write to the dev file");
        return 1;
    }
    else
    {
        printf("Write %zd bytes to the dev file, the context is: '%s' \n\n",bytes_write,write_data);
    }

    if(close(fd) == -1)
    {
        perror("Failed to close the file");
        return 1;
    }

    return 0;
}