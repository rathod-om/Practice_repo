obj-m +=i2c_client_driver_LM75A.o



target :
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean :
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
