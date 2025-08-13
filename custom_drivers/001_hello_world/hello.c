#include<linux/module.h>

static int __init helloworld_init(void)
{
	pr_alert("Hello my name is om\n");
	return 0;
}

static void __exit helloworld_cleanup(void){

	pr_alert("Good bye\n");
}

module_init(helloworld_init);
module_exit(helloworld_cleanup);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Om Rathod");
MODULE_DESCRIPTION("Hello world kernel module ");
MODULE_INFO(board,"beagle bone");


