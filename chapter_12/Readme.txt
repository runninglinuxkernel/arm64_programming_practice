第12章 中断处理 实验说明

本章讲述的是如何使用树莓派4B的传统中断控制器来实现中断处理。
下面分两种情况来说明。

1. 使用QEMU
我们提供的虚拟机内置的QEMU版本为4.2.50_BenOS，这个版本默认是支持 GICv2控制器的。
我们需要安装另外一个qemu deb安装包。

$ sudo dpkg -i qemu-system-arm-pi4_4.1.50-3_without_GIC_amd64.deb

此时QEMU的版本为4.1.50_BenOS，这个版本的QEMU的树莓派4B支持传统中断器模式。


2. 使用树莓派4B开发板

如果想在树莓派4B开发板里默认使用传统中断控制器，可以采用如下暴力的方法。
1．需要修改MicroSD卡boot分区里的bcm2711-rpi-4-b.dtb文件。使用hexedit工具来修改，把里面的“arm,gic-400”字符串暴力修改为“not,gic-400”。其实，只要把这个字符串修改了就行。这样，树莓派4b的固件就不能找到设备树上关于GIC的描述，它就会自动切换到传统的中断控制器中。
2．	修改MicroSD卡boot分区的config.txt文件，新增一个选项“enable_gic=0”。
   [pi4]
    kernel=benos4.bin
    enable_gic=0

请读者尽量自行使用hexedit工具修改，我们把修改后文件放在hack-rpi4文件夹下面仅供参考。把bcm2711-rpi-4-b-nogic.dtb拷贝到MicroSD卡boot分区，并修改名为：bcm2711-rpi-4-b.dtb，原来的bcm2711-rpi-4-b.dtb请备份。

