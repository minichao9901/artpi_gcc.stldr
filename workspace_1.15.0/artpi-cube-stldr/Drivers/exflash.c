#include "exflash.h"
#include "qspi.h"


/* W25Qϵ��Flash�ڲ��Ĵ�����ַ */
#define W25X_WriteEnable		0x06
#define W25X_WriteDisable		0x04
#define W25X_ReadStatusReg1		0x05
#define W25X_ReadStatusReg2		0x35
#define W25X_ReadStatusReg3		0x15
#define W25X_WriteStatusReg1    0x01
#define W25X_WriteStatusReg2    0x31
#define W25X_WriteStatusReg3    0x11
#define W25X_ReadData			0x03
#define W25X_FastReadData		0x0B
#define W25X_FastReadDual		0x3B
#define W25X_PageProgram		0x02
#define W25X_BlockErase			0xD8
#define W25X_SectorErase		0x20
#define W25X_ChipErase			0xC7
#define W25X_PowerDown			0xB9
#define W25X_ReleasePowerDown	0xAB
#define W25X_DeviceID			0xAB
#define W25X_ManufactDeviceID	0x90
#define W25X_JedecDeviceID		0x9F
#define W25X_Enable4ByteAddr    0xB7
#define W25X_Exit4ByteAddr      0xE9
#define W25X_SetReadParam		0xC0
#define W25X_EnterQPIMode       0x38
#define W25X_ExitQPIMode        0xFF
#define W25X_EnableReset		0x66
#define W25X_ResetDevice		0x99
#define W25X_FastReadQuadIO		0xEB

/* W25Xϵ��оƬ����ID */
typedef enum
{
	eW25ERR = 0x0000,
	eW25Q08 = 0xEF13,
	eW25Q16 = 0xEF14,
	eW25Q32 = 0xEF15,
	eW25Q64 = 0xEF16,
	eW25Q128 = 0xEF17,
	eW25Q256 = 0xEF18,
} eFlashType_t;

typedef enum
{
	QSPI_1_LINE,
	QSPI_2_LINES,
	QSPI_4_LINES,
} eFlashLines_t;

/* �������� */
static eFlashType_t exFlashType = eW25ERR;
static eFlashLines_t exFlashLines = QSPI_1_LINE;
#if defined(EXFLASH_WRITE) && defined(EXFLASH_SECTOR_WRITE_BUFFER)
static unsigned char exFlashSecteroWriteBuff[EXFLASH_SECTOR_SIZE] = {0};
#endif


/****************************************************************************
 * descri: ��exFlash�Ĵ�����ֵ.
 * inputs: regadr: �Ĵ�����ַ
 *         regval: �Ĵ�������
 * return: 0:OK,[-1,-2,-3]:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
static int exflash_reg_read(unsigned char regadr, unsigned char *regval)
{
	if (regval == NULL)
	{
		return -1;
	}
	
	if (exFlashLines == QSPI_4_LINES)
	{
		if (qspi_command(regadr, 0, 0, QSPI_INSTRUCTION_4_LINES, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_4_LINES) != 0)
		{
			return -2;
		}
	}
	else if (exFlashLines == QSPI_2_LINES)
	{
		if(qspi_command(regadr, 0, 0, QSPI_INSTRUCTION_2_LINES, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_2_LINES) != 0)
		{
			return -2;
		}
	}
	else
	{
		if (qspi_command(regadr, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_1_LINE) != 0)
		{
			return -2;
		}
	}
	
	if (qspi_read(regval, sizeof(unsigned char)) != 0)
	{
		return -3;
	}
	
	return 0;
}

/****************************************************************************
 * descri: дexFlash�Ĵ�����ֵ.
 * inputs: regadr: �Ĵ�����ַ
 *         regval: �Ĵ�������
 * return: 0:OK,[-1,-2]:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
static int exflash_reg_write(unsigned char regadr, unsigned char regval)
{
	if (exFlashLines == QSPI_4_LINES)
	{
		if (qspi_command(regadr, 0, 0, QSPI_INSTRUCTION_4_LINES, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_4_LINES) != 0)
		{
			return -1;
		}
	}
	else if(exFlashLines == QSPI_2_LINES)
	{
		if (qspi_command(regadr, 0, 0, QSPI_INSTRUCTION_2_LINES, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_2_LINES) != 0)
		{
			return -1;
		}
	}
	else
	{
		if (qspi_command(regadr, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_1_LINE) != 0)
		{
			return -1;
		}
	}
	
	if (qspi_write(&regval, sizeof(unsigned char)) != 0)
	{
		return -2;
	}
	
	return 0;
}

/****************************************************************************
 * descri: exFlashдʹ������.
 * inputs: state: ����״̬(ENABLE | DISABLE)
 * return: 0:OK,[-1]:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
static int exflash_write_mode_cfg(FunctionalState state)
{
	unsigned int cmd;
	
	if (state == ENABLE)
	{
		cmd = W25X_WriteEnable; // ��S1�Ĵ�����WEL��λ
	}
	else
	{
		cmd = W25X_WriteDisable; // ��S1�Ĵ�����WEL����
	}
	
	if (exFlashLines == QSPI_4_LINES)
	{
		if (qspi_command(cmd, 0, 0, QSPI_INSTRUCTION_4_LINES, QSPI_ADDRESS_NONE, QSPI_ADDRESS_24_BITS, QSPI_DATA_NONE) != 0)
		{
			return -1;
		}
	}
	else if (exFlashLines == QSPI_2_LINES)
	{
		if (qspi_command(cmd, 0, 0, QSPI_INSTRUCTION_2_LINES, QSPI_ADDRESS_NONE, QSPI_ADDRESS_24_BITS, QSPI_DATA_NONE) != 0)
		{
			return -1;
		}
	}
	else
	{
		if (qspi_command(cmd, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_24_BITS, QSPI_DATA_NONE) != 0)
		{
			return -1;
		}
	}
	
	return 0;
}

/****************************************************************************
 * descri: exFlash QSPIģʽ����.
 * inputs: state: ����״̬(ENABLE | DISABLE)
 * return: 0:OK,[-1,-2,-3,-4,-5]:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
static int exflash_qspi_mode_cfg(FunctionalState state)
{
	if (state == ENABLE)
	{
		unsigned char regval = 0;
		if (exflash_reg_read(W25X_ReadStatusReg2, &regval) != 0)
		{
			return -1;
		}
		
		if ((regval & 0x02) == 0) // QEλδʹ��
		{
			if (exflash_write_mode_cfg(ENABLE) != 0)
			{
				return -2;
			}
			
			regval |= 0x1 << 1; // ʹ��QEλ
			if (exflash_reg_write(W25X_WriteStatusReg2, regval) != 0)
			{
				return -3;
			}
		}
		
		if (qspi_command(W25X_EnterQPIMode, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE) != 0)
		{
			return -4;
		}
		
		exFlashLines = QSPI_4_LINES; // ���QSPI����ģʽ
	}
	else
	{
		if (qspi_command(W25X_ExitQPIMode, 0, 0, QSPI_INSTRUCTION_4_LINES, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE) != 0)
		{
			return -5;
		}
		
		exFlashLines = QSPI_1_LINE; // ���QSPI����ģʽ
	}
	
	return 0;
}

/****************************************************************************
 * descri: exFlash æ�ȴ�.
 * inputs: void.
 * return: 0:IDLE,1:BUSY.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
static int exflash_busy_wait(void)
{
	unsigned char regval = 0;
	unsigned long timeout = 0;
	
	while (timeout++ < 0x7FFFFFFFUL)
	{
		if (exflash_reg_read(W25X_ReadStatusReg1, &regval) == 0)
		{
			if ((regval & 0x01) != 0x01)
			{
				return 0;
			}
		}
	}
	
	return 1;
}

/****************************************************************************
 * descri: exFlash����ID��ȡ.
 * inputs: void.
 * return: ��������.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
static eFlashType_t exflash_type_read(void)
{
	unsigned char regval[2] = {0};
	unsigned short deviceid = 0;
	
	do
	{
		if (exFlashLines == QSPI_4_LINES)
		{
			if (qspi_command(W25X_ManufactDeviceID, 0, 0, QSPI_INSTRUCTION_4_LINES, QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_24_BITS, QSPI_DATA_4_LINES) == 0)
			{
				break;
			}
			
			if (qspi_command(W25X_ManufactDeviceID, 0, 0, QSPI_INSTRUCTION_4_LINES, QSPI_ADDRESS_4_LINES, QSPI_ADDRESS_32_BITS, QSPI_DATA_4_LINES) == 0)
			{
				break;
			}
		}
		else
		{
			if (qspi_command(W25X_ManufactDeviceID, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE) == 0)
			{
				break;
			}
			
			if (qspi_command(W25X_ManufactDeviceID, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_32_BITS, QSPI_DATA_1_LINE) == 0)
			{
				break;
			}
		}
		
		return eW25ERR;
	} while (0);
	
	/* Read Flash ID */
	if (qspi_read(regval, sizeof(regval)) != 0)
	{
		return eW25ERR;
	}
	
	deviceid = (deviceid << 8) + regval[0];
	deviceid = (deviceid << 8) + regval[1];
	switch (deviceid)
	{
		case 0xEF13: return eW25Q08;
		case 0xEF14: return eW25Q16;
		case 0xEF15: return eW25Q32;
		case 0xEF16: return eW25Q64;
		case 0xEF17: return eW25Q128;
		case 0xEF18: return eW25Q256;
		default: break;
	}
	
	return eW25ERR;
}

#if defined(EXFLASH_WRITE)

/****************************************************************************
 * descri: exFlashҳд����.
 * inputs: addr: ����ҳ��ʼ��ַ
 *         pbuf: ���ݻ����ַ
 *         size: ����д���ֽ���
 * return: 0:OK,[-1,-2,-3,-4,-5]:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
int exflash_write_page(unsigned int addr, const void *pbuf, int size)
{
	unsigned int addrbit;
	
	if (!pbuf || size < 1)
	{
		return -1;
	}
	
	addrbit = QSPI_ADDRESS_24_BITS;
	if (exFlashType == eW25Q256)
	{
		addrbit = QSPI_ADDRESS_32_BITS;
	}
	
	if (exflash_write_mode_cfg(ENABLE) != 0)
	{
		return -2;
	}
	
	if (qspi_command(W25X_PageProgram, addr, 0, QSPI_INSTRUCTION_4_LINES, QSPI_ADDRESS_4_LINES, addrbit, QSPI_DATA_4_LINES) != 0)
	{
		return -3;
	}
	
	if (qspi_write(pbuf, size) != 0)
	{
		return -4;
	}
	
	if (exflash_busy_wait() != 0)
	{
		return -5;
	}
	
	return 0;
}

/****************************************************************************
 * descri: exFlash����д����(�Զ���ҳ).
 * inputs: addr: ������ʼ��ַ
 *         pbuf: ���ݻ����ַ
 *         size: ����д���ֽ���
 * return: 0:OK,[-1,-2]:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
int exflash_write_sector(unsigned int addr, const void *pbuf, int size)
{
	unsigned int pageremain;
	unsigned char *wbuf = (unsigned char *)pbuf;
	
	if (!pbuf || !wbuf || size < 1)
	{
		return -1;
	}
	
	pageremain = EXFLASH_PAGE_SIZE - (addr % EXFLASH_PAGE_SIZE);
	if (size <= pageremain)
	{
		pageremain = size;
	}
	
	while (1)
	{
		if (exflash_write_page(addr, wbuf, pageremain) != 0)
		{
			break;
		}
		
		if (size <= pageremain)
		{
			return 0;
		}
	 	
		wbuf += pageremain;
		addr += pageremain;
		size -= pageremain;
		pageremain = (size >= EXFLASH_PAGE_SIZE)? EXFLASH_PAGE_SIZE : size;
	}
	
	return -2;
}

/****************************************************************************
 * descri: exFlash����д����.
 * inputs: addr: д����ʼ��ַ
 *         pbuf: ���ݻ����ַ
 *         size: ����д���ֽ���
 * return: 0:OK,[-1,-2]:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
int exflash_write(unsigned int addr, const void *pbuf, int size)
{
	unsigned int sector_addr = 0;
	unsigned int sector_offset = 0;
	unsigned int sector_remain = 0;
	
#if defined(EXFLASH_SECTOR_WRITE_BUFFER)
	unsigned char erase_flag;
	unsigned char *rbuf = &exFlashSecteroWriteBuff[0];
#endif
	
	unsigned char *wbuf = (unsigned char *)pbuf;
	
	if (!pbuf || !wbuf || size < 1)
	{
		return -1;
	}
	
 	sector_addr = addr / EXFLASH_SECTOR_SIZE; // ������ַ
	sector_offset = addr % EXFLASH_SECTOR_SIZE; // �������ڵ�ƫ��
	sector_remain = EXFLASH_SECTOR_SIZE - sector_offset; // ����ʣ��ռ��С
	
 	if(size <= sector_remain)
	{
		sector_remain = size;
	}
	
	while(1)
	{
#if defined(EXFLASH_SECTOR_WRITE_BUFFER)
		erase_flag = 0;
		if(exflash_read(sector_addr * EXFLASH_SECTOR_SIZE, rbuf, EXFLASH_SECTOR_SIZE) != 0)
		{
			break;
		}
		
		for(int i=0; i<sector_remain; i++)
		{
			if(rbuf[sector_offset + i] != 0xFF)
			{
				erase_flag = 1;
				break;
			}
		}
		
		if(erase_flag != 0)
		{
			if(exflash_erase_sector(sector_addr * EXFLASH_SECTOR_SIZE) != 0)
			{
				break;
			}
			
			for(int i=0; i<sector_remain; i++)
			{
				rbuf[i + sector_offset] = wbuf[i];
			}
			
			if(exflash_write_sector(sector_addr * EXFLASH_SECTOR_SIZE, rbuf, EXFLASH_SECTOR_SIZE) != 0)
			{
				break;
			}
		}
		else
#endif
		{
			if(exflash_write_sector(addr, wbuf, sector_remain) != 0)
			{
				break;
			}
		}
		
		if(size <= sector_remain)
		{
			return 0;
		}
		
		sector_addr++; // ������ַ��1
		sector_offset = 0; // ƫ��λ��Ϊ0

		wbuf += sector_remain; // ָ��ƫ��
		addr += sector_remain; // д��ַƫ��
		size -= sector_remain; // �ֽ����ݼ�
		sector_remain = (size >= EXFLASH_SECTOR_SIZE)? EXFLASH_SECTOR_SIZE : size;
	}
	
	return -2;
}

#endif // EXFLASH_WRITE

/****************************************************************************
 * descri: exFlash���ݶ�ȡ.
 * inputs: addr: ��ȡ��ʼ��ַ
 *         pbuf: ���ݻ����ַ
 *         size: ���ݶ�ȡ�ֽ���
 * return: 0:OK,[-1,-2]:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
int exflash_read(unsigned int addr, void * const pbuf, int size)
{
	unsigned int addrbit = 0;
	
	addrbit = QSPI_ADDRESS_24_BITS;
	if (exFlashType == eW25Q256)
	{
		addrbit = QSPI_ADDRESS_32_BITS;
	}
	
	if (qspi_command(W25X_FastReadData, addr, 8, QSPI_INSTRUCTION_4_LINES, QSPI_ADDRESS_4_LINES, addrbit, QSPI_DATA_4_LINES) != 0)
	{
		return -1;
	}
	
	if (qspi_read(pbuf, size) != 0)
	{
		return -2;
	}
	
	return 0;
}

/****************************************************************************
 * descri: exFlash��������оƬ.
 * inputs: void
 * return: 0:OK,[-1,-2,-3,-4]:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
int exflash_erase_chip(void)
{
    if (exflash_write_mode_cfg(ENABLE) != 0)
	{
		return -1;
	}
	
    if (exflash_busy_wait() != 0)
	{
		return -2;
	}
	
	if (qspi_command(W25X_ChipErase, 0, 0, QSPI_INSTRUCTION_4_LINES, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE) != 0)
	{
		return -3;
	}
	
	if (exflash_busy_wait() != 0)
	{
		return -4;
	}
	
	return 0;
}

/****************************************************************************
 * descri: exFlash��������(����һ����������ʱ��:150ms).
 * inputs: addr: ������ʼ��ַ.
 * return: 0:OK,[-1,-2,-3,-4]:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
int exflash_erase_sector(unsigned int addr)
{
	unsigned int addrbit = 0;
	
	addrbit = QSPI_ADDRESS_24_BITS;
	if (exFlashType == eW25Q256)
	{
		addrbit = QSPI_ADDRESS_32_BITS;
	}
	
	if (exflash_write_mode_cfg(ENABLE) != 0)
	{
		return -1;
	}
	
	if (exflash_busy_wait() != 0)
	{
		return -2;
	}
	
	if (qspi_command(W25X_SectorErase, addr, 0, QSPI_INSTRUCTION_4_LINES, QSPI_ADDRESS_4_LINES, addrbit, QSPI_DATA_NONE) != 0)
	{
		return -3;
	}
	
	if (exflash_busy_wait() != 0)
	{
		return -4;
	}
	
	return 0;
}

/****************************************************************************
 * descri: exFlash��ʼ��.
 * inputs: void
 * return: 0:OK,[-1,-2,-3,-4,-5,-6,-7,-8,-9]:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
int exflash_init(void)
{
	unsigned char regval;
	
	/* ������ʼ�� */
	exFlashType = eW25ERR;
	exFlashLines = QSPI_1_LINE;
	
	/* QSPI���� */
#if defined (FLASH_MEM) // MDK ��д�㷨
	if (qspi_init(__CLZ(__RBIT(EXFLASH_SIZE))) != 0)
#else
	if (qspi_init(__CLZ(__RBIT(EXFLASH_SIZE)) - 1) != 0)
#endif
	{
		return -1;
	}
	
	/* QSPIģʽ���� */
	if (exflash_qspi_mode_cfg(ENABLE) != 0)
	{
		return -2;
	}
	
	/* Read Flash ID */
	if ((exFlashType = exflash_type_read()) == eW25ERR)
	{
		return -3;
	}
	
	regval = 0;
	if (exflash_reg_read(W25X_ReadStatusReg3, &regval) != 0)
	{
		return -4;
	}
	
	if ((regval & 0x01) == 0x00)
	{
		if (exflash_write_mode_cfg(ENABLE) != 0)
		{
			return -5;
		}
		
		if (qspi_command(W25X_Enable4ByteAddr, 0, 0, QSPI_INSTRUCTION_4_LINES, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE) != 0)
		{
			return -6;
		}
	}
	
	if (exflash_write_mode_cfg(ENABLE) != 0)
	{
		return -7;
	}
	
	if (qspi_command(W25X_SetReadParam, 0, 0, QSPI_INSTRUCTION_4_LINES, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_4_LINES) != 0)
	{
		return -8;
	}
	
	regval = 0x3 << 4; // ����bit4,5=11,8��dummy clocks,104M
	if (qspi_write(&regval, sizeof(regval)) != 0)
	{
		return -9;
	}
	
	return 0;
}

/****************************************************************************
 * descri: exFlash����ʼ��.
 * inputs: void
 * return: 0:OK,-1:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
int exflash_deinit(void)
{
	exFlashType = eW25ERR;
	exFlashLines = QSPI_1_LINE;
	return qspi_deinit();
}

/****************************************************************************
 * descri: exFlash�ڴ�ӳ��ģʽ����.
 * inputs: void
 * return: 0:OK,-1:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
int exflash_memory_mapped_config(void)
{
	unsigned int addrbit = 0;
	
	addrbit = QSPI_ADDRESS_24_BITS;
	if (exFlashType == eW25Q256)
	{
		addrbit = QSPI_ADDRESS_32_BITS;
	}
	
	return qspi_memory_mapped_config(W25X_FastReadQuadIO, addrbit);
}


