struct pcdev_platform_data{
	int size;
	int perm;
	const char *serial_number;

};

/* PERMISSION MACROS */
#define RDONLY 0x01
#define WRONLY 0x10
#define RDWR 0x11


/* DECLARATION */
int check_permission(int dev_perm,int access_mode);
int pcd_release(struct inode *inode,struct file *filp);
int pcd_open(struct inode *inode,struct file *filp);
ssize_t pcd_write(struct file *filp,const char __user *buff,size_t count,loff_t *f_pos);
ssize_t pcd_read(struct file *filp,char __user *buff,size_t count,loff_t *f_pos);
loff_t pcd_lseek(struct file *filp,loff_t off,int whence);
int pcd_platform_driver_probe(struct platform_device *pdev);
int pcd_platform_driver_remove(struct platform_device *pdev);
void pcdev_release (struct device *dev);

