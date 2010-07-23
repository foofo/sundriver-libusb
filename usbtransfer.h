#ifndef USBTRANSFER_H
#define USBTRANSFER_H

#include <usb.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

/* Sundriver USB */
#define SUNDRIVER_VID 0x14ef
#define SUNDRIVER_PID 0x6689
/* the device's endpoints */
#define EP_IN 0x81
#define EP_OUT 0x02

typedef enum {READ10, WRITE10} _usbDirection;
usb_dev_handle *open_dev(void);
void read10(uint8_t * readBuffer, uint32_t numberOfBlocks,uint32_t startBlock, usb_dev_handle *dev);
void write10(uint8_t * writeBuffer, uint32_t numberOfBlocks,uint32_t startBlock, usb_dev_handle *dev);
#endif

 
