#include "usbtransfer.h"

void usage();
void getOptions(int argc, char **argv, uint8_t * readBuffer,usb_dev_handle *dev);
void getSlotNo(int slotno,usb_dev_handle *dev);
void getFilename(char *filename,int slotno,usb_dev_handle *dev);

int main(int argc, char **argv) {
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

    getOptions(argc,argv,readBuffer,dev);
    /* Clean up */
    usb_release_interface(dev, 0);
    usb_close(dev);
    exit(0);
}



void usage() {
	printf("Sundriver get iso v 0.1: \n");
	printf("Usage: \n");
	printf("-s slotno 0..66\n");
	printf("-h print this text\n");
}

void getOptions(int argc, char **argv, uint8_t * readBuffer,usb_dev_handle *dev) {
    int c;
	opterr = 0; //clear errors
    int slotno;
	while ((c = getopt (argc, argv, "s:h")) != -1)
		switch (c) {
			case 's':
				slotno=atoi(optarg);
				break;
			case 'h':
				usage();
				exit(0);
				break;
			case '?':
				usage();
				exit(0);
			default:
				abort ();
		}
    getSlotNo(slotno,dev);
}

void getSlotNo(int slotno,usb_dev_handle *dev){
    char filename[0xee];
    uint8_t readBuffer[0x80*0x200];
    printf("options %i\n",slotno,filename);
    getFilename(filename,slotno,dev);
    //open device for write
	int fd;
	/* open64 file for write */
	fd = open64( filename,O_CREAT|O_WRONLY|O_TRUNC, 0777);
	if (fd==-1) {
		perror("open( filename,O_CREAT|O_WRONLY|O_TRUNC, 0777)\n");
		exit(EXIT_FAILURE);
	}

    int i;
    int bytesRead = 0;
    //0x380000 start of isos
    //slotno -1 *0x8c4000 + start
    int startblock = (slotno-1)*0x8c4000+0x380000;
    int end = slotno*0x8c4000+0x380000;
    for(i=startblock;i<=end;i=i+0x80){
        read10(readBuffer,0x80,i,dev);
        write(fd,readBuffer,0x80*0x200);
    }
/*int j=0;
for (j=0;j<1;j++) {
    //numberOfBlocks,uint32_t startBlock
    read10(0x80,0x0,dev,fd);
}
*/
	if (close(fd) == -1) {
		perror("close(fd)\n");
		exit(EXIT_FAILURE);
	}
/*
		bytesWrite =  write(fd,tmpBuffer,bytesRead);
		if (bytesWrite == -1) {
			perror("write to file\n");
			exit(EXIT_FAILURE);
		}
*/


}

void getFilename(char *filename,int slotno,usb_dev_handle *dev){
    uint8_t readBuffer[0x200*0x100];
    read10(readBuffer,0x100,0x0,dev);
    memset(filename,0,0xee);
    memcpy(filename,&readBuffer[slotno*0x100+0x1],0xee);
    printf("Slot:\t%i\tISO name: %s\n",slotno,filename);
}
