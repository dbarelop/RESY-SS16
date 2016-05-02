

#include <linux/module.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

static dev_t gpio_dev_number;
static struct cdev *driver_object;
static struct class *gpio_class;
static struct device *motorl_dev, *motorr_dev;

// ToDo: Hier muessen die verwendeten GPIOs eingetragen werden
#define ML1   14
#define ML2   4
#define MR1   18
#define MR2   15

static int driver_open( struct inode *geraetedatei, struct file *instanz )
{
	int err;
	int motor_in1, motor_in2;

	// Ein Treiber bedient zwei Motoren.
	if (iminor(geraetedatei)==0) { // motor_left
		motor_in1 = ML1;
		motor_in2 = ML2;
	} else { // motor_right
		motor_in1 = MR1;
		motor_in2 = MR2;
	}

	// GPIO für motor_in1 reservieren
	// Inklusive Fehlerbehandlung
	err = gpio_request( motor_in1, "rpi_gpio_motor_in1" );
	if (err!=0) {
		return -EIO;
	}
	// GPIO für motor_in1 auf Output schalten
	// Fehlerbehandlung: Falls Fehlschlag muss der GPIO wieder
	//                   freigegeben werden.
	err = gpio_direction_output( motor_in1, 0 );
	if (err!=0) {
		gpio_free( motor_in1 );
		return -EIO;
	}
	// GPIO für motor_in2 reservieren
	// Inklusive Fehlerbehandlung
	err = gpio_request( motor_in2, "rpi_gpio_motor_in2" );
	if (err!=0) {
		gpio_free( motor_in1 );
		return -EIO;
	}
	// GPIO für motor_in2 auf Output schalten
	// Fehlerbehandlung: Falls Fehlschlag müssen die GPIOs wieder
	//                   freigegeben werden.
	err = gpio_direction_output( motor_in2, 0 );
	if (err!=0) {
		gpio_free( motor_in1 );
		gpio_free( motor_in2 );
		return -EIO;
	}

	printk("gpio %d and  %d successfull configured\n",motor_in1,motor_in2);
	return 0;
}

static int driver_close( struct inode *geraete_datei, struct file *instanz )
{
	int motor_in1, motor_in2;

	if (iminor(instanz->f_inode)==0) { // motor_left
		motor_in1 = ML1;
		motor_in2 = ML2;
	} else { // motor_right
		motor_in1 = MR1;
		motor_in2 = MR2;
	}

	printk( "driver_close called\n");
	gpio_free( motor_in1 );
	gpio_free( motor_in2 );
	return 0;
}

static ssize_t driver_read( struct file *instanz, char __user *user,
		size_t count, loff_t *offset )
{
	dev_info( motorl_dev, "driver_read\n" );
	return 0;
}

static ssize_t driver_write( struct file *instanz, const char __user *user,
		size_t count, loff_t *offset )
{
	unsigned long not_copied, to_copy;
	int motor_in1, motor_in2;
	int value=0;

	if (iminor(instanz->f_inode)==0) { // motor_left
		motor_in1 = ML1;
		motor_in2 = ML2;
	} else { // motor_right
		motor_in1 = MR1;
		motor_in2 = MR2;
	}

	to_copy = min( count, sizeof(value) );
	not_copied=copy_from_user(&value, user, to_copy);
	//dev_info( motorl_dev, "driver_write: value %x\n", value );
	if (value > 0) {
		//printk("setting forward\n");
	 	//Motor vorwärts drehen lassen
		gpio_set_value( motor_in1, 1 );
		gpio_set_value( motor_in2, 0 );
	} else if (value < 0) {
		//printk("setting backward\n");
		//Motor rückwärts drehen lassen
		gpio_set_value( motor_in1, 0 );
		gpio_set_value( motor_in2, 1 );
	} else {
		//printk("motor stop\n");
		//Motor anhalten
		gpio_set_value( motor_in1, 0 );
		gpio_set_value( motor_in2, 0 );
	}
	return to_copy-not_copied;
}

static struct file_operations fops = {
	.owner= THIS_MODULE,
	.read= driver_read,
	.write= driver_write,
	.open= driver_open,
	.release= driver_close,
};

static int __init mod_init( void )
{
	if( alloc_chrdev_region(&gpio_dev_number,0,2,"motor")<0 )
		return -EIO;
	driver_object = cdev_alloc(); /* Anmeldeobjekt reservieren */
	if( driver_object==NULL )
		goto free_device_number;
	driver_object->owner = THIS_MODULE;
	driver_object->ops = &fops;
	if( cdev_add(driver_object,gpio_dev_number,2) )
		goto free_cdev;
	/* Eintrag im Sysfs, damit Udev den Geraetedateieintrag erzeugt. */
	gpio_class = class_create( THIS_MODULE, "motor" );
	if( IS_ERR( gpio_class ) ) {
		pr_err( "gpio: no udev support\n");
		goto free_cdev;
	}
	motorl_dev = device_create( gpio_class, NULL, gpio_dev_number,
		NULL, "%s", "motor-left" );
	motorr_dev = device_create( gpio_class, NULL, gpio_dev_number+1,
		NULL, "%s", "motor-right" );
	dev_info(motorl_dev, "mod_init");
	return 0;
free_cdev:
	kobject_put( &driver_object->kobj );
free_device_number:
	unregister_chrdev_region( gpio_dev_number, 2 );
	return -EIO;
}

static void __exit mod_exit( void )
{
	dev_info(motorl_dev, "mod_exit");
	/* Loeschen des Sysfs-Eintrags und damit der Geraetedatei */
	device_destroy( gpio_class, gpio_dev_number+1 );
	device_destroy( gpio_class, gpio_dev_number );
	class_destroy( gpio_class );
	/* Abmelden des Treibers */
	cdev_del( driver_object );
	unregister_chrdev_region( gpio_dev_number, 2 );
	return;
}

module_init( mod_init );
module_exit( mod_exit );

/* Metainformation */
MODULE_LICENSE("GPL");


