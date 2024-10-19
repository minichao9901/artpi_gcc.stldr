#ifndef __EX_FLASH_H
#define __EX_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Flash���� */
#define EXFLASH_SIZE			((unsigned int)(8*1024*1024)) // Flash��С(MB)
#define EXFLASH_PAGE_SIZE		((unsigned int)0x100)	// ҳ��С: 256B
#define EXFLASH_SECTOR_SIZE		((unsigned int)0x1000)	// ������С: 4KB
#define EXFLASH_BLOCK_SIZE		((unsigned int)0x10000)	// ���С: 64KB

/* ʹ��Flashд���� */
#define EXFLASH_WRITE
/* ʹ������д���� */
//#define EXFLASH_SECTOR_WRITE_BUFFER


int exflash_init(void);
int exflash_deinit(void);

int exflash_erase_chip(void);
int exflash_erase_sector(unsigned int addr);

int exflash_read(unsigned int addr, void * const pbuf, int size);

#if defined(EXFLASH_WRITE)
int exflash_write(unsigned int addr, const void *pbuf, int size);
int exflash_write_page(unsigned int addr, const void *pbuf, int size);
int exflash_write_sector(unsigned int addr, const void *pbuf, int size);
#endif

int exflash_memory_mapped_config(void);

#ifdef __cplusplus
}
#endif

#endif
