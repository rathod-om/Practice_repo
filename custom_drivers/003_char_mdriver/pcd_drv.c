#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt, __func__ 
/* REQUIRED MACROS */

#define NO_OF_DEVICES 4

#define MEM_SIZE_MAX_PCDEV1 1024
#define MEM_SIZE_MAX_PCDEV2 1024
#define MEM_SIZE_MAX_PCDEV3 1024
#define MEM_SIZE_MAX_PCDEV4 1024

/* PERMISSION MACROS */
#define RDONLY  0X01
#define WRONLY  0X10
#define RDWR    0X11

/* Required Declaration */
loff_t pcd_lseek(struct file *filp, loff_t off, int whence);
ssize_t pcd_read(struct file *filp,char __user *buff,size_t count,loff_t *f_pos);
ssize_t pcd_write(struct file *filp,const char __user *buff,size_t count,loff_t *f_pos);
int pcd_release(struct inode *inode,struct file *filp);
int pcd_open(struct inode *inode,struct file *filp);
int check_permission(int dev_perm,int access_mode);

/* Psuedo Device Memory */
char device_buffer_pcdev1[MEM_SIZE_MAX_PCDEV1];
char device_buffer_pcdev2[MEM_SIZE_MAX_PCDEV2];
char device_buffer_pcdev3[MEM_SIZE_MAX_PCDEV3];
char device_buffer_pcdev4[MEM_SIZE_MAX_PCDEV4];

/* Device private data struct */
struct pcdev_private_data{
	char *buffer;
	unsigned size;
	const char *serial_number;
	int perm;
	struct cdev cdev;
};

/* Drivers private data struct */
struct pcdrv_private_data{
	int total_devices;
	dev_t device_number;
	struct class *class_pcd;
	struct device *device_pcd;
	struct pcdev_private_data pcdev_data[NO_OF_DEVICES];
};


/* Allocating data to the struct drivers */
struct pcdrv_private_data pcdrv_data = {
	.total_devices = NO_OF_DEVICES,
	.pcdev_data = {
		[0] = {
			.buffer = device_buffer_pcdev1,
			.size = MEM_SIZE_MAX_PCDEV1,
			.serial_number = "PCDEVXYZ001",
			.perm = 0x1,/* READONLY*/
		},
		[1] = {
			.buffer = device_buffer_pcdev2,
			.size = MEM_SIZE_MAX_PCDEV2,
			.serial_number = "PCDEVXYZ002",
			.perm = 0x10,/* WRONLY*/
		},
		[2] = {
			.buffer = device_buffer_pcdev3,
			.size = MEM_SIZE_MAX_PCDEV3,
			.serial_number = "PCDEVXYZ003",
			.perm = 0x11,/* RDWRONLY*/
		},
		[3] = {
			.buffer = device_buffer_pcdev4,
			.size = MEM_SIZE_MAX_PCDEV4,
			.serial_number = "PCDEVXYZ004",
			.perm = 0x11,/* RDWRONLY*/
		}
	}
};

loff_t pcd_lseek(struct file *filp,loff_t off,int whence){
	struct pcdev_private_data *pcdev_data = (struct pcdev_private_data *)filp->private_data;
        int max_size = pcdev_data->size;

	pr_info("lseek requested\n");
	pr_info("Current file position %lld :: BEFORE\n",filp->f_pos);

	switch(whence){
		case SEEK_SET:
			if ((off > max_size) || (off < 0)){
				return -EINVAL;
			}
			filp->f_pos = off;
			break;
		case SEEK_CUR:
			loff_t temp = filp->f_pos+off;
			if ((temp > max_size) || (temp < 0)){
				return -EINVAL;
			}
			filp->f_pos = temp;
			break;
		case SEEK_END:
			temp = max_size+off;
			if ((temp > max_size) || (temp < 0)){
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
	struct pcdev_private_data *pcdev_data = (struct pcdev_private_data *)filp->private_data;
        int max_size = pcdev_data->size;

	pr_info("read request of %zu bytes\n",count);
	pr_info("Current file position is %lld\n",*f_pos);

	/* checking is count is Within limit */
	if (*f_pos + count > max_size){
		count = max_size - *f_pos;
	}

	/* copy to user from kernel */
	if (copy_to_user(buff,pcdev_data->buffer+(*f_pos),count) ){
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
	struct pcdev_private_data *pcdev_data = (struct pcdev_private_data *)filp->private_data;
	int max_size = pcdev_data->size;

	pr_info("write request of %zu bytes\n",count);
	pr_info("Current file position is %lld\n",*f_pos);

	/* checking is count is Within limit */
	if (*f_pos + count > max_size){
		count = max_size - *f_pos;
	}

	if(!count){
		return -ENOMEM;
	}

	/* copy from user from kernel */
	if (copy_from_user(pcdev_data->buffer+(*f_pos),buff,count) ){
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
	.open = pcd_open,
	.read = pcd_read,
	.llseek = pcd_lseek,
	.release = pcd_release,
	.owner = THIS_MODULE
};

#define RDONLY 	0X01
#define WRONLY 	0X10
#define RDWR 	0X11


int check_permission(int dev_perm,int access_mode){
	if(dev_perm == RDWR ){
		return 0;
	}
	if ((dev_perm == RDONLY) && (access_mode & FMODE_READ) && !(access_mode & FMODE_WRITE) ){
		return 0;
	}
	if ((dev_perm == WRONLY) && (access_mode & FMODE_READ) && !(access_mode & FMODE_WRITE) ){
                return 0;
        }
	return -EPERM;
}


int pcd_open(struct inode *inode,struct file *filp){
	int ret;
	int minor_n;
	struct pcdev_private_data *pcdev_data;
	/* Find out on which device field open was first attempted by user space */
	minor_n = MINOR(inode->i_rdev);
	pr_info("Minor Number is %d\n",minor_n);

	/* Get device private Data struct */	
	pcdev_data = container_of(inode->i_cdev,struct pcdev_private_data,cdev);
	
	/* To supply device private data to other methods of driver */
	filp->private_data = pcdev_data;

	/* Check Permission */
	ret = check_permission(pcdev_data->perm,filp->f_mode);
	pr_info("opended successfully\n");
	
	(!ret)?pr_info("open was successfull\n"):pr_info("open was not successfull\n");
	return ret;
}
int pcd_release(struct inode *inode,struct file *filp){
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
static int __init pcd_init(void){

	/* 1.Alocating the Device Number */	
	int ret = alloc_chrdev_region(&pcdrv_data.device_number,0,NO_OF_DEVICES, "pcdevs_1");

	/* Error Handling */
	if (ret < 0){
		pr_err("Allocation failed");
		goto out;
	}


	/* 4.create device class under sys/class/ */
	pcdrv_data.class_pcd = class_create("pcd_class_for_multiple");
	if (IS_ERR(pcdrv_data.class_pcd)){
		pr_err("class Create Failed\n");
		ret = PTR_ERR(pcdrv_data.class_pcd);
		goto unreg_chrdev;
	}


	for (int i = 0; i < NO_OF_DEVICES;i++){
		pr_info("Device Number <Major> <minor> = <%d> <%d>\n",MAJOR(pcdrv_data.device_number+i),MINOR(pcdrv_data.device_number+i));




		/* 2.Initialize cdev struct with file object */
		cdev_init(&pcdrv_data.pcdev_data[i].cdev,&pcd_fops);



		/* 3.Register a device with VFS */
		pcdrv_data.pcdev_data[i].cdev.owner = THIS_MODULE;
		ret = cdev_add(&pcdrv_data.pcdev_data[i].cdev,pcdrv_data.device_number+i,1);
		if (ret < 0){
			pr_err("Device ADD failed\n");
			goto cdev_del;
		}
		/* 5.Populate sysfs with the device information */
		pcdrv_data.device_pcd = device_create(pcdrv_data.class_pcd,NULL,pcdrv_data.device_number+i,NULL,"pcdevm-%d",i+1);

		if (IS_ERR(pcdrv_data.device_pcd)){
			pr_err("device Create Failed\n");
			ret = PTR_ERR(pcdrv_data.device_pcd);
			goto class_des;
		}
	}
	pr_info("Module in start\n");

	return 0;



cdev_del:
class_des:
	int i;
	for (;i<0;i--){
		device_destroy(pcdrv_data.class_pcd,pcdrv_data.device_number+i);
		cdev_del(&pcdrv_data.pcdev_data[i].cdev);
	}
	class_destroy(pcdrv_data.class_pcd);
unreg_chrdev:
	unregister_chrdev_region(pcdrv_data.device_number,NO_OF_DEVICES);
out:
	pr_err("Allocatiob failed\n");
	return ret;

}


static void __exit pcd_exit(void){
	for (int i=0;i<NO_OF_DEVICES;i++){
                device_destroy(pcdrv_data.class_pcd,pcdrv_data.device_number+i);
                cdev_del(&pcdrv_data.pcdev_data[i].cdev);
        }
        class_destroy(pcdrv_data.class_pcd);
	
	unregister_chrdev_region(pcdrv_data.device_number,NO_OF_DEVICES);
	pr_info("Module unloaded\n");
}

module_init(pcd_init);
module_exit(pcd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Om Rathod");
MODULE_DESCRIPTION("pc-multiple-dev kernel module ");

