#include "usbtransfer.h"

void printCBWBuffer(uint8_t * cbwBuffer) {
    int i = 0;
    printf("cbwBuffer: \n");
    for(i=0;i<32;i++) {
        if((i%4==0) && (i!=0))
            printf(" %x ",cbwBuffer[i]);
        else
            printf("%x ",cbwBuffer[i]);
    }
    printf("\n");
}

void uint32_tToUint8_tArray(uint8_t * array , uint32_t i) {
    /* convert uint32_t to uint8_t array */
    array[0]=i;
    array[1]=i>>8;
    array[2]=i>>16;
    array[3]=i>>24;
}

void setUpCBWBuffer(uint8_t * cbwBuffer,
                    _usbDirection usbDirection,
                    uint32_t dCBWDataDtransferLength,
                    uint32_t numberOfBlocks,
                    uint32_t startBlock
                    ) {
    /* Prepare the CBW Message to be send later 
     * 
     */
    memset (cbwBuffer,0,32);
    uint32_t dCBWTag;
    uint8_t tmp[4];
    int i = 0;
    int j = 0;
    //set dCBWSignature to 55 53 42 43 - sniffed
    cbwBuffer[0]=0x55;
    cbwBuffer[1]=0x53;
    cbwBuffer[2]=0x42;
    cbwBuffer[3]=0x43;
    //set dCBWTag
    dCBWTag = 1;//rand() %1000;
    /* FIXME use memcpy*/
    uint32_tToUint8_tArray(tmp,dCBWTag);
    i = 4;
    j = 0;
    while(j<4)  {
        cbwBuffer[i++]=tmp[j++];
    }
    //set dCBWDataDtransferLength
    uint32_tToUint8_tArray(tmp,dCBWDataDtransferLength);
    i = 8;
    j = 0;
    while(j<4)  {
        cbwBuffer[i++]=tmp[j++];
    }
    switch(usbDirection) {
        case READ10:  cbwBuffer[12]=0x80; //0x80 (data in) usb -> host
                    cbwBuffer[15]=0x28; //set read 10;
                    break;
        case WRITE10: cbwBuffer[12]=0x0; //0x00 (data out) host -> usb
                    cbwBuffer[15]=0x2a; //set write 10;
                    break;
    }
    //set up bCBWLUN - sniffed
    cbwBuffer[14]=0x0a;
    //start block
    cbwBuffer[17]=startBlock>>24;
    cbwBuffer[18]=startBlock>>16;
    cbwBuffer[19]=startBlock>>8;
    cbwBuffer[20]=startBlock;
    //no of blocks
    cbwBuffer[22]=numberOfBlocks>>8;
    cbwBuffer[23]=numberOfBlocks;
}

void read10(uint8_t * readBuffer, uint32_t numberOfBlocks,uint32_t startBlock, usb_dev_handle *dev) {
    int bytesRead=0;
    int byteReadCounter=0;

    _usbDirection usbDirection = READ10;
    const int blockSize = 512; //FIXME: read block size from device!
    uint32_t lengthInByte=blockSize*numberOfBlocks;

    uint8_t cbwBuffer[31];
    uint8_t cswBuffer[13];
    //Set up Buffers
    memset (cswBuffer,0,13);

    setUpCBWBuffer(cbwBuffer,usbDirection,lengthInByte,numberOfBlocks,startBlock);

    //FIXME: remove later!
    //printCBWBuffer(cbwBuffer);

    //request read
    if ( (usb_bulk_write(dev, EP_OUT, cbwBuffer, 31, 5000)) <0 ) {
		perror("usb bulk write failed\n");
		exit(EXIT_FAILURE);
    }
    //read
    while (byteReadCounter!=lengthInByte)  {
        bytesRead = usb_bulk_read(dev,
                        EP_IN,
                        readBuffer,
                        ((lengthInByte-byteReadCounter > 4096) ? 4096: lengthInByte-byteReadCounter),
                        5000);
        if (bytesRead  <0 ) {
		    perror("usb bulk read failed\n");
			exit(EXIT_FAILURE);
        }
        //FIXME: remove this line
        //printf("READ: %i\n",((lengthInByte-byteReadCounter > 4096) ? 4096: lengthInByte-byteReadCounter));

        //update counter
        byteReadCounter=byteReadCounter+bytesRead;
        readBuffer=readBuffer+bytesRead*sizeof(uint8_t);
    
        //FIXME: check CSW!
        bytesRead = usb_bulk_read(dev, EP_IN, cswBuffer, 13, 5000);
        //printf("bytes Read %i \n",bytesRead);
        if ( bytesRead <0 ) {
			perror("usb read failed\n");
			exit(EXIT_FAILURE);
        }
    }
}

void write10(uint8_t * writeBuffer, uint32_t numberOfBlocks,uint32_t startBlock, usb_dev_handle *dev) {
    int bytesWrite=0;
    int bytesRead=0;
    int byteWriteCounter=0;

    _usbDirection usbDirection = WRITE10;
    const int blockSize = 512; //FIXME: read block size from device!
    uint32_t lengthInByte=blockSize*numberOfBlocks;

    uint8_t cbwBuffer[31];
    uint8_t cswBuffer[13];
    //Set up Buffers
    memset (cswBuffer,0,13);

    setUpCBWBuffer(cbwBuffer,usbDirection,lengthInByte,numberOfBlocks,startBlock);

    //FIXME: remove later!
    //printCBWBuffer(cbwBuffer);

    //request write
    if ( (usb_bulk_write(dev, EP_OUT, cbwBuffer, 31, 5000)) <0 ) {
		perror("usb bulk write failed\n");
		exit(EXIT_FAILURE);
    }
    //read
    while (byteWriteCounter!=lengthInByte)  {
        bytesWrite = usb_bulk_write(dev,
                        EP_OUT,
                        writeBuffer,
                        ((lengthInByte-byteWriteCounter > 4096) ? 4096: lengthInByte-byteWriteCounter),
                        5000);
        if (bytesWrite  <0 ) {
		    perror("usb bulk write failed\n");
			exit(EXIT_FAILURE);
        }
        //FIXME: remove this line
        //printf("WRITE: %i\n",((lengthInByte-byteWriteCounter > 4096) ? 4096: lengthInByte-byteWriteCounter));

        //update counter
        byteWriteCounter=byteWriteCounter+bytesWrite;
        writeBuffer=writeBuffer+bytesWrite*sizeof(uint8_t);
    
        //FIXME: check CSW!
        bytesRead = usb_bulk_read(dev, EP_IN, cswBuffer, 13, 5000);
        //printf("bytes Read %i \n",bytesRead);
        if ( bytesRead <0 ) {
			perror("usb read failed\n");
			exit(EXIT_FAILURE);
        }
    }
}

usb_dev_handle *open_dev(void) {
    struct usb_bus *bus;
    struct usb_device *dev; 
    for (bus = usb_get_busses(); bus; bus = bus->next){
        for (dev = bus->devices; dev; dev = dev->next){
            if (dev->descriptor.idVendor == SUNDRIVER_VID && dev->descriptor.idProduct == SUNDRIVER_PID){
                return usb_open(dev);
            }
        }
    }
    return NULL;
}
