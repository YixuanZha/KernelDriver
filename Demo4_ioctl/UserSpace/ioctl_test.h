#ifndef IOCTL_TEST_H
#define IOCTL_TEST_H

#include <linux/ioctl.h>

// 定义一个幻数为ioctl 命令号的唯一标识符
// 随便选取一个未被使用的字母
#define IOCTL_MAGIC 'k'

// 定义两个ioctl命令
// _IOW 表示从用户空间向内核空间写入数据
// 参数（幻数， 命令序号，传递的数据类型）
#define IOCTL_SET_VAL _IOW(IOCTL_MAGIC,1,int)

// _IOR 表示从内核空间向用户空间读取数据
// 参数 （幻数，命令序号，传递的数据类型）
#define IOCTL_GET_VAL _IOR(IOCTL_MAGIC,2,int)

#endif