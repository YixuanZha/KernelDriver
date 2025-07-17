#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>    // For open()
#include <unistd.h>   // For close(), read(), write()
#include <string.h>   // For strlen()

#define DEVICE_FILE "/dev/dev_num"

int main() {
    int fd;
    char buffer[256];
    ssize_t bytes_read, bytes_written;

    // 打开设备文件
    fd = open(DEVICE_FILE, O_RDWR); // O_RDWR 表示读写模式
    if (fd == -1) {
        perror("Failed to open the device file");
        return EXIT_FAILURE;
    }
    printf("Successfully opened device: %s\n", DEVICE_FILE);

    // 尝试写入数据
    const char *write_data = "Hello from user space!";
    bytes_written = write(fd, write_data, strlen(write_data));
    if (bytes_written == -1) {
        perror("Failed to write to the device");
    } else {
        printf("Wrote %zd bytes to device: \"%s\"\n", bytes_written, write_data);
    }

    // 尝试读取数据
    bytes_read = read(fd, buffer, sizeof(buffer) - 1); // -1 为空终止符留出空间
    if (bytes_read == -1) {
        perror("Failed to read from the device");
    } else {
        buffer[bytes_read] = '\0'; // 确保字符串空终止
        printf("Read %zd bytes from device: \"%s\"\n", bytes_read, buffer);
    }

    // 关闭设备文件
    if (close(fd) == -1) {
        perror("Failed to close the device file");
        return EXIT_FAILURE;
    }
    printf("Successfully closed device: %s\n", DEVICE_FILE);

    return EXIT_SUCCESS;
}