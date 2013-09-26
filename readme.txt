wmdp introduction:
This is a embedded system develop platform, the kernel of it is "FREERTOS" which is open-source and free to use in commercial products.

wmdp advantage:
1, friendly configurable interface
the source code is managed with Make and provide GUI configure interface with the help of python.
you can just use "make xconfig" to organize your project.
2, Object-Oriented Style
i want to bring in "OOP" to this platform, such as we can use "oop" to develop device driver, just like linux device driver.
......

arch principal:
1) the whole software is divided into 5 layers, boot/application, modules, rtos and kernel library which is reentrant
, driver and cpulib, cpu. In general, the rtos is fixed.
2) Each layer can and can only call the layers below it and the partners in the same layer.
For example, lib_basic is located at the bottom layer, it could be called by
every layer.

folders description:
/projects	ide project files
/scripts	script files
/src		source codes
/doc		documents

/src/include	header files
/src/drivers	common useful routines and device independed driver
/src/component	modules such as tcp/ip, fatfs, finsh shell
/src/libcpu		device drivers
/src/board		user application layer, related to real board


