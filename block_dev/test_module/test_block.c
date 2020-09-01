#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/vmalloc.h>
#include <linux/hdreg.h>
#include <linux/blkdev.h>
#include <linux/blkpg.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/initrd.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/major.h>
#include <linux/blkdev.h>
#include <linux/bio.h>
#include <linux/highmem.h>
#include <linux/mutex.h>
#include <linux/radix-tree.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/backing-dev.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>

#include <linux/uaccess.h>

#define VBLOCK_DEV_NAME "plct_vblock" 
#define VBLOCK_DEV_MAJOR 220  
#define VBLOCK_SECTOR_SIZE 512   
uint32_t len;

typedef struct
{
    unsigned char *data;
    struct request_queue *queue;
    struct gendisk *gd;
}vblock_device;

static vblock_device device;

static unsigned int vblock_make_request(struct request_queue *q, struct bio *bio)
{
    vblock_device *pdevice;    
    char *pVHDDData;
    char *pBuffer;
    struct bio_vec bvec;
    struct bvec_iter iter;
       
    if(((bio->bi_iter.bi_sector*VBLOCK_SECTOR_SIZE) + bio->bi_iter.bi_size) > len * VBLOCK_SECTOR_SIZE)
    {
        bio_io_error(bio);
        return 0;
    }
    else
    {        
        pdevice = (vblock_device *) bio-> bi_disk-> private_data;
        pVHDDData = pdevice->data + (bio->bi_iter.bi_sector*VBLOCK_SECTOR_SIZE);
        
        bio_for_each_segment(bvec, bio, iter)
        {            
            pBuffer = kmap(bvec.bv_page) + bvec.bv_offset;
            
            switch(bio_data_dir(bio))
            {   
                case READ : memcpy(pBuffer, pVHDDData, bvec.bv_len);
                    break;
                case WRITE : memcpy(pVHDDData, pBuffer, bvec.bv_len);
                    break;
                default : kunmap(bvec.bv_page);
                    bio_io_error(bio);
                    return 0;            
            }
            
            kunmap(bvec.bv_page);
            pVHDDData += bvec.bv_len;
        }
        
        bio_endio(bio );
        return 0;
    }    
}

int vblock_open(struct block_device *bdev, fmode_t mode)
{
    return 0;
}

void vblock_release (struct gendisk *disk, fmode_t mode)
{
    return;
}

int vblock_ioctl(struct block_device *bdev, fmode_t mode, unsigned int cmd,unsigned long arg)
{
    
    int error;
    if(cmd!= BLKFLSBUF)
    {
        return -ENOTTY;
    }
    error = -EBUSY;

    if(bdev->bd_openers <= 2)
    {
        truncate_inode_pages(bdev->bd_inode->i_mapping,0);
        error = 0;
    }

    return error;
}

static struct block_device_operations vrd_fops =
{
    .owner = THIS_MODULE,
    .open = vblock_open,
    .release = vblock_release,
    .ioctl = vblock_ioctl,
};

static int vblock_probe(struct platform_device *pdev)
{
    struct resource *res_mem; 
    struct class *myclass;
    of_property_read_s32(pdev->dev.of_node, "len", &len);
    res_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    devm_request_mem_region(&pdev->dev, res_mem->start, resource_size(res_mem),
                     res_mem->name);
    unsigned char *addr_start = (unsigned char *)ioremap(res_mem->start, resource_size(res_mem));
    int err = -ENOMEM;
   
    
    if(register_blkdev(VBLOCK_DEV_MAJOR, VBLOCK_DEV_NAME))
    {
        err = -EIO;
            goto out;
    }

    device.data = addr_start;    
    
    device.gd = alloc_disk(1);

    if (!device.gd)
        goto out;

    device.queue = blk_alloc_queue(vblock_make_request, NUMA_NO_NODE);
    if (!device.queue)
    {
        put_disk(device.gd);
        goto out;
    }

    
    

    device.gd->major = VBLOCK_DEV_MAJOR;
    device.gd->first_minor = 0;
    device.gd->fops = &vrd_fops;
    device.gd->queue = device.queue;
    device.gd->private_data = &device;
    sprintf(device.gd->disk_name, "vblock");
    set_capacity(device.gd,len);
        
    add_disk(device.gd);
    myclass = class_create(THIS_MODULE,"test_block"); 
    if(IS_ERR(myclass))
	{
		printk("Failed to class_create.\n");
    }
    struct device *pdevice;
    pdevice = device_create(myclass, NULL, MKDEV(VBLOCK_DEV_MAJOR,0), NULL, "vblock0");  
    if(IS_ERR(pdevice))
	{
		printk("Failed to device_create.\n");
    }
    printk("VBLOCK driver initialized!");
    return 0;
out:
    put_disk(device.gd);
    blk_cleanup_queue(device.queue);
   return err;
}

static int vblock_remove(struct platform_device *pdev)
{

    del_gendisk(device.gd);
    put_disk(device.gd);
    blk_cleanup_queue(device.queue);

    unregister_blkdev(VBLOCK_DEV_MAJOR, VBLOCK_DEV_NAME);
}


static const struct of_device_id of_match_devs[] = {
    { .compatible = "plct,vblock", .data = NULL },
    {  }
};

struct platform_driver vblock_drv = {
    .probe      = vblock_probe,
    .remove     = vblock_remove,
    .driver     = {
        .name   = "plct_vblock",
        .of_match_table = of_match_devs, 
    }
};


static int vblock_init(void)
{
    platform_driver_register(&vblock_drv);
    return 0;
}

static void vblock_exit(void)
{
    platform_driver_unregister(&vblock_drv);
}

module_init(vblock_init);
module_exit(vblock_exit);

MODULE_LICENSE("GPL");