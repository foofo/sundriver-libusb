#include "usbtransfer.h"

void printConfig(uint8_t * readBuffer);

int main(void) {
    usb_dev_handle *dev = NULL; /* the device handle */
    uint8_t readBuffer[512];

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

    read10(readBuffer,0x1,0x0,dev);
    printConfig(readBuffer);
    /*
    
    int i = 0;
    printf("readBuffer: \n");
    for(i=0;i<512;i++) {
        if((i%4==0) && (i!=0))
            printf(" %x ",readBuffer[i]);
        else
            printf("%x ",readBuffer[i]);
    }
    printf("\n");
*/
    /* Clean up */
    usb_release_interface(dev, 0);
    usb_close(dev);
    exit(0);
}

void printConfig(uint8_t * readBuffer){
    const uint8_t config_disable_chip = 1<< 3;
    const uint8_t config_upgrade_lock = 1<<2;
    const uint8_t config_use_pc_config = 1<< 3;
    const uint8_t config_region_lock = 1<<2;

    const uint8_t config_pal = 0;
    const uint8_t config_usa = 1<<4;
    const uint8_t config_jap = 1<<5;
    const uint8_t config_kor = 3<<4;

	printf("Disable chip:\t%s\n",
        ((readBuffer[2]&config_disable_chip)==config_disable_chip)?"YES":"NO");
	printf("Upgrade lock:\t%s\n",
        ((readBuffer[2]&config_upgrade_lock)==config_upgrade_lock)?"YES":"NO");
    printf("Use pc config:\t%s\n",
        ((readBuffer[3]&config_use_pc_config)==config_use_pc_config)?"YES":"NO");
    //check region
    if( (readBuffer[3] &config_kor) == config_kor)
	    printf("Region:\t\tKOR\n");
    else if( (readBuffer[3] &config_jap) == config_jap)
	    printf("Region:\t\tJAP\n");
    else if( (readBuffer[3] &config_usa) == config_usa)
	    printf("Region:\t\tUSA\n");
    else
        printf("Region:\t\tPAL\n");
	printf("Region lock:\t%s\n",
        ((readBuffer[4]&config_region_lock)==config_region_lock)?"YES":"NO");

}
