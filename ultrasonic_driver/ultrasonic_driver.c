#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/timekeeping.h>
#include <linux/slab.h>

#define INTERRUPT

#ifdef INTERRUPT
#include <linux/interrupt.h>
#include <linux/mutex.h>
#endif

static dev_t gpio_dev_number;
static struct cdev *driver_object;
static struct class *gpio_class;
static struct device *ultrasonic_dev;

#ifdef INTERRUPT
static DEFINE_MUTEX(read_lock);
#endif

#define HIGH 1
#define LOW 0

#define GPIO_TRIGGER 3
#define GPIO_ECHO 2

static int driver_open(struct inode *device_file, struct file *instance) {
    // Request control of GPIO pins
    if (gpio_request(GPIO_TRIGGER, "rpi_gpio_ultrasonic_trigger")) {
        printk("failed requesting control for trigger GPIO\n");
        return -EIO;
    }
    if (gpio_request(GPIO_ECHO, "rpi_gpio_ultrasonic_echo")) {
        printk("failed requesting control for echo GPIO\n");
        return -EIO;
    }
    // Set the direction of the GPIO pins
    if (gpio_direction_output(GPIO_TRIGGER, 0)) {
        printk("failed setting output direction for trigger GPIO\n");
        return -EIO;
    }
    if (gpio_direction_input(GPIO_ECHO)) {
        printk("failed setting input direction for echo GPIO\n");
        return -EIO;
    }
    // Make sure the trigger pin is set low and wait for it to settle
    gpio_set_value(GPIO_TRIGGER, LOW);
    printk("GPIO pins (%d and %d) succesfully configured\n", GPIO_TRIGGER, GPIO_ECHO);

    return 0;
}

static int driver_close(struct inode *device_file, struct file *instance) {
    printk("driver_close called\n");
    gpio_free(GPIO_TRIGGER);
    gpio_free(GPIO_ECHO);

    return 0;
}

#ifdef INTERRUPT
static irq_handler_t irq_read_echo_handler(unsigned int irq, void *dev_id, struct pt_regs *regs) {
    free_irq(irq, dev_id);
    mutex_unlock(&read_lock);
    return (irq_handler_t) IRQ_HANDLED;
}
#endif

static ssize_t driver_read(struct file *instance, char __user *user, size_t count, loff_t *offset) {
    ktime_t start, end, delta;
    u64 travel_time;
    #ifdef INTERRUPT
    int irq_number, irq_request_result;
    irq_number = gpio_to_irq(GPIO_ECHO);
    #endif

    // Send trigger pulse
    gpio_set_value(GPIO_TRIGGER, HIGH);
    udelay(20);
    gpio_set_value(GPIO_TRIGGER, LOW);

    // Wait for echo to start
    #ifdef INTERRUPT
    irq_request_result = request_irq(irq_number, (irq_handler_t) irq_read_echo_handler, IRQF_TRIGGER_RISING, "ultrasonic_echo_handler", NULL);
    mutex_lock(&read_lock);
    printk("rising edge detected for ultrasonic sensor's ECHO line\n");
    //free_irq(irq_number, NULL);
    #else
    while (gpio_get_value(GPIO_ECHO) == LOW);
    #endif

    // Wait for echo to end
    start = ktime_get();
    #ifdef INTERRUPT
    irq_request_result = request_irq(irq_number, (irq_handler_t) irq_read_echo_handler, IRQF_TRIGGER_FALLING, "ultrasonic_echo_handler", NULL);
    mutex_lock(&read_lock);
    printk("falling edge detected for ultrasonic sensor's ECHO line\n");
    //free_irq(irq_number, NULL);
    #else
    while (gpio_get_value(GPIO_ECHO) == HIGH);
    #endif
    end = ktime_get();

    // Calculate elapsed time
    delta = ktime_sub(end, start);
    travel_time = ktime_to_ns(delta);
    printk("travelTime = %llu ns\n", travel_time);

    // Copy result into user space
    return copy_to_user(user, &travel_time, sizeof(travel_time));
}

static ssize_t driver_write(struct file *instance, const char __user *user, size_t count, loff_t *offset) {
    // Do nothing
    return count;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = driver_read,
    .write = driver_write,
    .open = driver_open,
    .release = driver_close
};

static int __init mod_init(void) {
    if (alloc_chrdev_region(&gpio_dev_number, 0, 1, "ultrasonic")) {
        printk("alloc_chrdev_region failed\n");
        return -EIO;
    }
    if ((driver_object = cdev_alloc()) == NULL) {
        unregister_chrdev_region(gpio_dev_number, 1);
        return -EIO;
    }
    driver_object->owner = THIS_MODULE;
    driver_object->ops = &fops;
    if (cdev_add(driver_object, gpio_dev_number, 1)) {
        kobject_put(&driver_object->kobj);
        unregister_chrdev_region(gpio_dev_number, 1);
        return -EIO;
    }
    gpio_class = class_create(THIS_MODULE, "ultrasonic");
    if (IS_ERR(gpio_class)) {
        printk("gpio: no udev support\n");
        kobject_put(&driver_object->kobj);
        unregister_chrdev_region(gpio_dev_number, 1);
        return -EIO;
    }
    ultrasonic_dev = device_create(gpio_class, NULL, gpio_dev_number, NULL, "%s", "ultrasonic");
    dev_info(ultrasonic_dev, "mod_init completed succesfully");

    return 0;
}

static void __exit mod_exit(void) {
    dev_info(ultrasonic_dev, "mod_exit called");
    device_destroy(gpio_class, gpio_dev_number);
    class_destroy(gpio_class);
    cdev_del(driver_object);
    unregister_chrdev_region(gpio_dev_number, 1);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
