#include "usbtransfer.h"

void usage();
void getOptions(int argc, char **argv, uint8_t * configBuffer);
void setUpConfigBuffer(uint8_t * configBuffer);

void setDisableChip(uint8_t * configBuffer);
void setUpgradeLock(uint8_t * configBuffer);
void setUsePCConfig(uint8_t * configBuffer);
void setRegionLock(uint8_t * configBuffer);


int main(int argc, char **argv) {
    usb_dev_handle *dev = NULL; /* the device handle */
    uint8_t configBuffer[512];

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

    /* Get commandline options and set up configBuffer */
    getOptions(argc,argv,configBuffer);
    /* Write the configuration to the device */
    write10(configBuffer,0x1,0x0,dev);

    /* FIXME: inform the user about the new setting ;) */

    /* Clean up */
    usb_release_interface(dev, 0);
    usb_close(dev);
    exit(0);
}



void usage() {
	printf("Sundriver set config v 0.1: \n");
	printf("Usage: \n");
	printf("-d disable chip\n");
	printf("-u upgrade lock\n");
	printf("-p use pc config\n");
	printf("-l region lock\n");
	printf("-r region {pal,usa,jap,kor}\n");
	printf("-h print this text\n");
}


void setRegion(uint8_t * configBuffer,char region) {
    const uint8_t config_pal = 0;
    const uint8_t config_usa = 1<<4;
    const uint8_t config_jap = 1<<5;
    const uint8_t config_kor = 3<<4;

    switch (region) {
	    case 'p':
		    //nothing to do at all
			break;
	    case 'u':
		    configBuffer[3]=configBuffer[3] | config_usa;
			break;
	    case 'j':
		    configBuffer[3]=configBuffer[3] | config_jap;
			break;
	    case 'k':
		    configBuffer[3]=configBuffer[3] | config_kor;
			break;
	    }
}

void setDisableChip(uint8_t * configBuffer) {
    const uint8_t config_disable_chip = 1<< 3;
    configBuffer[2]=configBuffer[2]|config_disable_chip;
}

void setUpgradeLock(uint8_t * configBuffer) {
    const uint8_t config_upgrade_lock = 1<<2;
    configBuffer[2]=configBuffer[2]|config_upgrade_lock;
}

void setUsePCConfig(uint8_t * configBuffer) {
    const uint8_t config_use_pc_config = 1<< 3;
    configBuffer[3]=configBuffer[3]|config_use_pc_config;
}

void setRegionLock(uint8_t * configBuffer) {
    const uint8_t config_region_lock = 1<<2;
    configBuffer[4]=configBuffer[4]|config_region_lock;
}

void getOptions(int argc, char **argv, uint8_t * configBuffer) {
    int c;
	opterr = 0; //clear errors
	while ((c = getopt (argc, argv, "duplr:h")) != -1)
		switch (c) {
			case 'd':
				setDisableChip(configBuffer);
				break;
			case 'u':
				setUpgradeLock(configBuffer);
				break;
			case 'p':
				setUsePCConfig(configBuffer);
				break;
			case 'l':
				setRegionLock(configBuffer);
				break;
			case 'r':
                if (strcmp("pal",optarg) == 0)
                    setRegion(configBuffer,'p');
                else if (strcmp("usa",optarg) == 0)
                    setRegion(configBuffer,'u');
                else if (strcmp("jap",optarg) == 0)
                    setRegion(configBuffer,'j');
                else if (strcmp("kor",optarg) == 0)
                    setRegion(configBuffer,'k');
                else {
                    usage();
				    exit(0);
                    }
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
}

void setUpConfigBuffer(uint8_t * configBuffer) {
    // Set up the config to pal without any other options used
    //a5 5a 00 00 00 e0 00 00 - sniffed
    memset (configBuffer,0,512);

    //a5 5a 00 00 
    configBuffer[0] = 0xa5; //unknown option
    configBuffer[1] = 0x5a; //unknown option

    //00 e0 00 00
    configBuffer[5] =0xe0; //unknown option
}

