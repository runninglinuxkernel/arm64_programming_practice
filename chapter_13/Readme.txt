
第13章 GIC-V2 实验说明

本章讲述的是如何使用树莓派4B的GICv2中断控制器来实现中断处理。
下面分两种情况来说明。

1. 使用QEMU
请确认QEMU版本，如果不是4.2.50_BenOS，请重新安装。
$ qemu-system-aarch64 --version

$ sudo dpkg -i qemu-system-arm-pi4_4.2.50-3_with_GIC_amd64.deb

2. 使用树莓派4B开发板
如果在第12章实验中，已经把 bcm2711-rpi-4-b.dtb和config.txt做了暴力修改，请还原回来。
