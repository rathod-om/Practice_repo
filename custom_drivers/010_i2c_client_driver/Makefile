obj-m +=  i2c_client_driver_0.o

modules :
	@$(MAKE) -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean :
	@$(MAKE) -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
