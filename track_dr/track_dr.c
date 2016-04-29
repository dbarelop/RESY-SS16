#include <linux/module.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <asm/uaccess.h>

static dev_t gpio_dev_number;
static struct cdev *driver_object;
static struct class *gpio_class;
static struct device *track_dev;


#define OUT1  11
#define OUT2  25
#define OUT3  8
#define OUT4  7

static int driver_open( struct inode *geraetedatei, struct file *instanz )
{
	int err = -1;
	
	err = gpio_request( OUT1, "rpi-gpio-echo" );
	if (err) {
		printk("gpio_request failed\n");
		return -EIO;
	}
	err = gpio_direction_input( OUT1 );
	if (err) {
		printk("gpio_direction_input failed\n");
		gpio_free( OUT1 );
		return -EIO;
	}

	err = gpio_request( OUT2, "rpi-gpio-echo" );
        if (err) {
                printk("gpio_request failed\n");
                return -EIO;
        }
        err = gpio_direction_input( OUT2 );
        if (err) {
                printk("gpio_direction_input failed\n");
                gpio_free( OUT1 );
		gpio_free( OUT2 );
                return -EIO;
        }

	err = gpio_request( OUT3, "rpi-gpio-echo" );
        if (err) {
                printk("gpio_request failed\n");
                return -EIO;
        }
        err = gpio_direction_input( OUT3 );
        if (err) {
                printk("gpio_direction_input failed\n");
                gpio_free( OUT1 );
		gpio_free( OUT2 );
		gpio_free( OUT3 );
                return -EIO;
        }

	err = gpio_request( OUT4, "rpi-gpio-echo" );
        if (err) {
                printk("gpio_request failed\n");
                return -EIO;
        }
        err = gpio_direction_input( OUT4 );
        if (err) {
                printk("gpio_direction_input failed\n");
                gpio_free( OUT1 );
		gpio_free( OUT2 );
		gpio_free( OUT3 );
		gpio_free( OUT4 );
                return -EIO;
        }

	printk("gpio %d, %d, %d and %d successfull configured\n",OUT1, OUT2, OUT3, OUT4);
	return 0;
}

static int driver_close( struct inode *geraete_datei, struct file *instanz )
{
	printk( "driver_close called\n");
	gpio_free( OUT1 );
	gpio_free( OUT2 );
	gpio_free( OUT3 );
	gpio_free( OUT4 );
	return 0;
}

static ssize_t driver_read( struct file *instanz, char __user *user,
	size_t count, loff_t *offset )
{
	int value;
	int to_copy, not_copied;

	printk( "driver_read\n" );

	// ECHO_PIN einlesen
	value = gpio_get_value( OUT1 );
	value = value + gpio_get_value( OUT2 )*10;
	value = value + gpio_get_value( OUT3 )*100;
	value = value + gpio_get_value( OUT4 )*1000;

	// Echopin zur Applikation kopieren
	to_copy = min( count, sizeof(value) );
	not_copied=copy_to_user( user, &value, to_copy );

	return to_copy-not_copied;
}

static struct file_operations fops = {
	.owner= THIS_MODULE,
	.read = driver_read,
	.open= driver_open,
	.release= driver_close,
};

static int __init mod_init( void )
{
	if( alloc_chrdev_region(&gpio_dev_number,0,1,"track")<0 )
		return -EIO;
	driver_object = cdev_alloc(); /* Anmeldeobjekt reservieren */
	if( driver_object==NULL )
		goto free_device_number;
	driver_object->owner = THIS_MODULE;
	driver_object->ops = &fops;
	if( cdev_add(driver_object,gpio_dev_number,1) )
		goto free_cdev;
	/* Eintrag im Sysfs, damit Udev den Geraetedateieintrag erzeugt. */
	gpio_class = class_create( THIS_MODULE, "track" );
	if( IS_ERR( gpio_class ) ) {
		pr_err( "gpio: no udev support\n");
		goto free_cdev;
	}
	track_dev = device_create( gpio_class, NULL, gpio_dev_number,
		NULL, "%s", "track" );
	dev_info(track_dev, "mod_init");
	return 0;
free_cdev:
	kobject_put( &driver_object->kobj );
free_device_number:
	unregister_chrdev_region( gpio_dev_number, 2 );
	return -EIO;
}

static void __exit mod_exit( void )
{
	dev_info(track_dev, "mod_exit");
	/* Loeschen des Sysfs-Eintrags und damit der Geraetedatei */
	device_destroy( gpio_class, gpio_dev_number );
	class_destroy( gpio_class );
	/* Abmelden des Treibers */
	cdev_del( driver_object );
	unregister_chrdev_region( gpio_dev_number, 1 );
	return;
}

module_init( mod_init );
module_exit( mod_exit );

MODULE_LICENSE("GPL");
