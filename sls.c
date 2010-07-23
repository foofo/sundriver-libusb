#include "usbtransfer.h"

void printReadBuffer(uint8_t * readBuffer);

int main(void) {
    usb_dev_handle *dev = NULL; /* the device handle */
    uint8_t readBuffer[0x200*0x100];

    /* Set up USB */
    usb_init();
    usb_find_busses();
    usb_find_devices();

    if (!(dev = open_dev())) {
        perror("device not found!\n");
		exit(EXIT_FAILURE);
    }
/*
    if (usb_set_configuration(dev, 1) < 0) {
        perror("setting config 1 failed\n");
        usb_close(dev);
        exit(EXIT_FAILURE);
    }
*/
    if (usb_claim_interface(dev, 0) < 0)
    {
        perror("claiming interface failed\n");
        usb_close(dev);
        exit(EXIT_FAILURE);
    }
    //FIXME: sniffed
    read10(readBuffer,0x100,0x0,dev);
    printReadBuffer(readBuffer);

    /* Clean up */
    usb_release_interface(dev, 0);
    usb_close(dev);
    exit(0);
}

void printReadBuffer(uint8_t * readBuffer){
    int i = 0;
    char isoStringBuffer[0xee];
    char nintendoBuffer[0x7];
    printf("Sundriver: \n");
    //FIXME: read storage size and calculate max slot number!
    //FIXME: handle 8.5 GB iso
    for(i=2;i<(0x44);i++) {
        //printf("%x\n",readBuffer[i*0x100]);
        //if(readBuffer[i*0x100]==0xa0) {
            memset(isoStringBuffer,0,0xee);
            memset(nintendoBuffer,0,0x07);
            memcpy(isoStringBuffer,&readBuffer[i*0x100+0x1],0xee);
            memcpy(nintendoBuffer,&readBuffer[i*0x100+0xf0],0x06);
            /* FIXME: The original Sundriver software reads 0x100 x times
             * from starting point 0x3800000 and adds 0x8c4000 each time
             * to the address. Afterwards it reads 1 Block (0x200 Bytes)
             * At +0x20 a String begins containing the Game name.
             * TODO: Add a struct and collect every available data and
             * print it out.
             * For now we only print the iso name.
             */
            printf("Slot:\t%i\tNintendo Nr.:%s\tISO name: %s\n",i-1,nintendoBuffer,isoStringBuffer);
        //}
    }
    printf("\n");
}
