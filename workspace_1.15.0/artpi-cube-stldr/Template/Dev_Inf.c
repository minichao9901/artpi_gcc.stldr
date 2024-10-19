#include "Dev_Inf.h"
#include "exflash.h"

/* This structure containes information used by ST-LINK Utility to program and erase the device */
#if defined (__ICCARM__)
__root struct StorageInfo const StorageInfo = {
#else
__attribute__((section(".storage_info"))) __attribute__((aligned (8)))
struct StorageInfo const  StorageInfo = {
#endif
	"ARTPI_H750_QSPI",		/* Device Name */
	SPI_FLASH,				/* Device Type */
	0x90000000,				/* Device Start Address */
	(EXFLASH_SIZE),			/* Device Size in Bytes */
	(EXFLASH_SECTOR_SIZE),	/* Programming Page Size 4KBytes */
	0xFF,					/* Initial Content of Erased Memory */
							/* Specify Size and Address of Sectors (view example below) */
	(EXFLASH_SIZE/EXFLASH_SECTOR_SIZE), (EXFLASH_SECTOR_SIZE),		/* Sector Num:2048, Sector Size: 4KBytes */
	0x00000000, 0x00000000,
};


