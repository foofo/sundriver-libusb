#include "usbtransfer.h"

void usage();
void getOptions(int argc, char **argv, uint8_t * readBuffer,usb_dev_handle *dev);
void putSlotNo(char * filename,int slotno,usb_dev_handle *dev);
void updateMenu(char * filename,int slotno,usb_dev_handle *dev);

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
	printf("-f filename\n");
	printf("-h print this text\n");
}

void getOptions(int argc, char **argv, uint8_t * readBuffer,usb_dev_handle *dev) {
    int c;
	opterr = 0; //clear errors
    int slotno;
    char * filename=NULL;
	while ((c = getopt (argc, argv, "s:f:h")) != -1)
		switch (c) {
			case 's':
				slotno=atoi(optarg);
				break;
			case 'f':
				filename=optarg;
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
    if(slotno!=0 && filename!=NULL) {
        putSlotNo(filename,slotno,dev);
    }
    else {
        usage();
		exit(0);
    }
}

void putSlotNo(char * filename,int slotno,usb_dev_handle *dev) {
	int fd;
    int bytesRead=0;
    int byteReadCounter=0;
    int startblock = (slotno-1)*0x8c4000+0x380000;

    uint8_t readBuffer[4096];
	/* open64 file for read */
	fd = open64(filename,O_RDONLY);
	if (fd==-1) {
		perror("open failed\n");
		exit(EXIT_FAILURE);
	}

//        bytesRead = read(fd,readBuffer,4096);
//        write10(readBuffer,0x8,startblock,dev);
    int counter = 0;
    int blocksCounter =0;
    printf("Transfering ...\n");
    do {
        bytesRead = read(fd,readBuffer,4096);
        write10(readBuffer,0x8,startblock,dev);
        startblock=startblock+0x8;
        byteReadCounter=byteReadCounter+bytesRead;
        counter++;
        if(counter == 32) {
            counter=0;
            blocksCounter++;
            /* FIXME: use ncurses ;) or copy the string to a buffer*/
            printf("%i Blocks transfered to the Sundriver",blocksCounter);
	        printf("\r");
	        fflush(stdout);
        }
        //printf("byteReadCounter:\t%x\n",byteReadCounter);
    } while (bytesRead>0);
    printf("\n");
    /* clean up */
    updateMenu(filename,slotno,dev);

    if (close(fd) == -1) {
        perror("close(fd)\n");
        exit(EXIT_FAILURE);
	}
}

void updateMenu(char * filename,int slotno,usb_dev_handle *dev){
    //#if 0
    int i;    
    int startblock = (slotno-1)*0x8c4000+0x380000;
    uint8_t readBuffer[0x200*0x100];
    uint8_t nintendoBuffer[0x200];

    memset(nintendoBuffer,0,0x200);
    memset(readBuffer,0,0x200*0x100);
    /* Get the current Sundriver blocks */

    read10(readBuffer,0x100,0x0,dev);

    /* Read the Nintendo signature */
    read10(nintendoBuffer,0x1,startblock,dev);


    printf("SIGNEATURE:\n");
    i = 0;
    printf("Buffer: \n");
    for(i=0;i<0x06;i++) {
        if((i%4==0) && (i!=0))
            printf(" %x \n",nintendoBuffer[i]);
        else
            printf("%x ",nintendoBuffer[i]);
    }
    printf("\n");

    /* Copy the Nintendo signature */
    memcpy(&readBuffer[slotno*0x100+0x1f0],nintendoBuffer,0x06);

    /* Set magic */
    readBuffer[slotno*0x100+0x100]=0xa0;

    /* print */
    //printf("FOO: %i\n",(strlen(filename) <0xee)?strlen(filename):0xee);

    /* Copy the ISO file name*/
    memcpy(&readBuffer[slotno*0x100+0x101],filename,(strlen(filename) <0xee)?strlen(filename):0xee);

/*
    i = 0;
    printf("BUFFER:\n");
    printf("Buffer: \n");
    for(i=0;i<0x200*0x100;i++) {
        if((i%4==0) && (i!=0))
            printf(" %x \n",readBuffer[i]);
        else
            printf("%x ",readBuffer[i]);
    }
    printf("\n");
*/
    /* Update the current Sundriver blocks */
    /*FIXME: WTF why writing 0x100 to 0x0 or 0x80 to 0x0 is not possible but
     * writing 0x2 to start+0x2 is possible?? ~:/
     */
    printf("Updating menue ...\n");
    for(i=0;i<0x100;i=i+0x2) {
        //printf("writing to.. %x\n",i);
        write10(&readBuffer[i],0x2,i,dev);
    }
    //#endif
}
