#!/bin/bash

GPIO_PIN=18

# Program pin release on exit
trap "echo $GPIO_PIN > /sys/class/gpio/unexport" EXIT

# Export control of pin
echo $GPIO_PIN > /sys/class/gpio/export
# Set pin direction to output
echo out > /sys/class/gpio/gpio$GPIO_PIN/direction

# Blink once every second
while [ 1 ]; do
    echo 1 > /sys/class/gpio/gpio$GPIO_PIN/value
    sleep 1
    echo 0 > /sys/class/gpio/gpio$GPIO_PIN/value
    sleep 1
done
