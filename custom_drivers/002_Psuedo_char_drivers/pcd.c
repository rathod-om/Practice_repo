#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt, __func__ 
#define DEV_MEM_SIZE 512

/* Required Declaration */
loff_t pcd_lseek(struct file *filp, loff_t off, int whence);
ssize_t pcd_read(struct file *filp,char __user *buff,size_t count,loff_t *f_pos);
ssize_t pcd_write(struct file *filp,const char __user *buff,size_t count,loff_t *f_pos);
int inode_release(struct inode *inode,struct file *filp);
int inode_open(struct inode *inode,struct file *filp);

/* Psuedo Device Memory */
char device_buffer[DEV_MEM_SIZE];

dev_t device_number;

struct cdev pcd_cdev;
//struct class *class_create(const char *name);


loff_t pcd_lseek(struct file *filp,loff_t off,int whence){
	pr_info("lseek requested\n");
	pr_info("Current file position %lld :: BEFORE\n",filp->f_pos);

	switch(whence){
		case SEEK_SET:
			if ((off > DEV_MEM_SIZE) || (off < 0)){
				return -EINVAL;
			}
			filp->f_pos = off;
			break;
		case SEEK_CUR:
			loff_t temp = filp->f_pos+off;
			if ((temp > DEV_MEM_SIZE) || (temp < 0)){
				return -EINVAL;
			}
                        filp->f_pos = temp;
                        break;
		case SEEK_END:
			temp = DEV_MEM_SIZE+off;
                        if ((temp > DEV_MEM_SIZE) || (temp < 0)){
                                return -EINVAL;
                        }
                        filp->f_pos = temp;
                        break;
		default:
	 		return -EINVAL;		
	}
	pr_info("Current file position %lld :: AFTER\n",filp->f_pos);

	return filp->f_pos;
}

ssize_t pcd_read(struct file *filp,char __user *buff,size_t count,loff_t *f_pos){
        pr_info("read request of %zu bytes\n",count);
        pr_info("Current file position is %lld\n",*f_pos);
	
	/* checking is count is Within limit */
	if (*f_pos + count > DEV_MEM_SIZE){
		count = DEV_MEM_SIZE - *f_pos;
	}
	
	/* copy to user from kernel */
	if (copy_to_user(buff,device_buffer+*f_pos,count) ){
		return -EFAULT;	
	}

	/* Update the file position */
	*f_pos += count;
        pr_info("read by %zu bytes successfully\n",count);
        pr_info("Current file position is %lld after read\n",*f_pos);

	/* Number of bytes read */
	return count;
}
ssize_t pcd_write(struct file *filp,const char __user *buff,size_t count,loff_t *f_pos){
        pr_info("write request of %zu bytes\n",count);
        pr_info("Current file position is %lld\n",*f_pos);

        /* checking is count is Within limit */
        if (*f_pos + count > DEV_MEM_SIZE){
                count = DEV_MEM_SIZE - *f_pos;
        }
	
	if(!count){
		return -ENOMEM;
	}

        /* copy from user from kernel */
        if (copy_from_user(device_buffer+*f_pos,buff,count) ){
                return -EFAULT;
        }

        /* Update the file position */
        *f_pos += count;
        pr_info("Written by %zu bytes successfully\n",count);
        pr_info("Current file position is %lld after read\n",*f_pos);

        /* Number of bytes read */
        return count;
}
struct file_operations pcd_fops = {
        .write = pcd_write,
        .open = inode_open,
        .read = pcd_read,
        .llseek = pcd_lseek,
        .release = inode_release,
        .owner = THIS_MODULE
};
int inode_open(struct inode *inode,struct file *filp){
        pr_info("opended successfully\n");
	return 0;
}
int inode_release(struct inode *inode,struct file *filp){
        pr_info("closed sucessfully\n");
        return 0;
}

/*
struct file_operations pcd_fops = {
	.write = pcd_write,
	.open = pcd_open,
	.read = pcd_read,
	.llseek = pcd_lseek,
	.release = pcd_release,
	.owner = THIS_MODULE
};
*/


struct class *class_pcd;
struct device *device_pcd;


static int __init pcd_init(void){
      	
	int ret = alloc_chrdev_region(&device_number,0,1, "pcd_device");
	
	/* Error Handling */
	if (ret < 0){
		pr_err("Allocation failed");
		goto out;
	}
	
	pr_info("Device Number <Major> <minor> = <%d> <%d>\n",MAJOR(device_number),MINOR(device_number));

	cdev_init(&pcd_cdev,&pcd_fops);
        
	pcd_cdev.owner = THIS_MODULE;
	ret = cdev_add(&pcd_cdev,device_number,1);
	if (ret < 0){
		pr_err("Device ADD failed\n");
		goto unreg_chrdev;
	}
	/* create device class under sys/class/ */
	class_pcd = class_create("pcd_class");
	if (IS_ERR(class_pcd)){
		pr_err("class Create Failed\n");
		ret = PTR_ERR(class_pcd);
		goto cdev_del;
	}
	device_pcd = device_create(class_pcd,NULL,device_number,NULL,"pcd");
	
	if (IS_ERR(device_pcd)){
		pr_err("device Create Failed\n");
		ret = PTR_ERR(device_pcd);
		goto class_des;
	}

	pr_info("Module in start\n");
	
	return 0;



class_des:
	class_destroy(class_pcd);
cdev_del:
	cdev_del(&pcd_cdev); 
unreg_chrdev:
	unregister_chrdev_region(device_number,1);
out:
	pr_err("Allocatiob failde\n");
	return ret;

}


static void __exit pcd_exit(void){
	device_destroy(class_pcd,device_number);
	class_destroy(class_pcd);
	cdev_del(&pcd_cdev);
	unregister_chrdev_region(device_number,1);
	pr_info("Module unloaded\n");
}

module_init(pcd_init);
module_exit(pcd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Om Rathod");
MODULE_DESCRIPTION("Hello world kernel module ");

