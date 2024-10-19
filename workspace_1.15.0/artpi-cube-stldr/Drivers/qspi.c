#include "qspi.h"

QSPI_HandleTypeDef hqspi;

/****************************************************************************
 * descri: QSPI MSP Initialization.
 * inputs: hqspi: QSPI Handle.
 * return: void.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
void HAL_QSPI_MspInit(QSPI_HandleTypeDef *hqspi)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	
	if (hqspi->Instance == QUADSPI)
	{
		/* Initializes the peripherals clock */
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_QSPI;
		PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_D1HCLK;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
		{
			while (1);
		}

		/* Peripheral clock enable */
		__HAL_RCC_QSPI_CLK_ENABLE();
		__HAL_RCC_GPIOG_CLK_ENABLE();
		__HAL_RCC_GPIOF_CLK_ENABLE();
		
		// QUADSPI GPIO Configuration
		// QUADSPI_BK1_NCS --> PG6
		// QUADSPI_BK1_IO3 --> PF6
		// QUADSPI_BK1_IO2 --> PF7
		// QUADSPI_BK1_IO0 --> PF8
		// QUADSPI_BK1_IO1 --> PF9
		// QUADSPI_CLK     --> PF10
		
		GPIO_InitStruct.Pin = GPIO_PIN_6;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
		HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	}
}

/****************************************************************************
 * descri: QSPI MSP De-Initialization.
 * inputs: hqspi: QSPI Handle.
 * return: void.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef *hqspi)
{
	if(hqspi->Instance == QUADSPI)
	{
		/* Peripheral clock disable */
		__HAL_RCC_QSPI_CLK_DISABLE();

		// QUADSPI GPIO Configuration
		// QUADSPI_BK1_NCS --> PG6
		// QUADSPI_BK1_IO3 --> PF6
		// QUADSPI_BK1_IO2 --> PF7
		// QUADSPI_BK1_IO0 --> PF8
		// QUADSPI_BK1_IO1 --> PF9
		// QUADSPI_CLK     --> PF10
		
		HAL_GPIO_DeInit(GPIOG, GPIO_PIN_6);
		HAL_GPIO_DeInit(GPIOF, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_10|GPIO_PIN_9);
	}
}

/****************************************************************************
 * descri: QSPI Reset memory.
 * inputs: hqspi: QSPI Handle.
 * return: 0:OK,[-1]:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
static void qspi_memory_reset(QSPI_HandleTypeDef *hqspi)
{
	if (hqspi)
	{
		if (hqspi->Instance == QUADSPI)
		{
			/* Reset memory config, Cmd in 1 line */
			/* Send RESET ENABLE command (0x66) to be able to reset the memory registers */
			while(hqspi->Instance->SR & QSPI_FLAG_BUSY);  /* Wait for busy flag to be cleared */
			hqspi->Instance->CCR = 0x2166;
			hqspi->Instance->AR = 0;
			hqspi->Instance->ABR = 0;
			hqspi->Instance->DLR = 0;
			__DSB(); 

			/* Send RESET command (0x99) to reset the memory registers */
			while(hqspi->Instance->SR & QSPI_FLAG_BUSY);  /* Wait for busy flag to be cleared */
			hqspi->Instance->CCR = 0x2199;
			hqspi->Instance->AR = 0;
			hqspi->Instance->ABR = 0;
			hqspi->Instance->DLR = 0;
			__DSB();

			/* Reset memory config, Cmd in 2 lines*/
			/* Send RESET ENABLE command (0x66) to be able to reset the memory registers */
			while(hqspi->Instance->SR & QSPI_FLAG_BUSY);  /* Wait for busy flag to be cleared */
			hqspi->Instance->CCR = 0x2266;
			hqspi->Instance->AR = 0;
			hqspi->Instance->ABR = 0;
			hqspi->Instance->DLR = 0;
			__DSB(); 

			/* Send RESET command (0x99) to reset the memory registers */
			while(hqspi->Instance->SR & QSPI_FLAG_BUSY);  /* Wait for busy flag to be cleared */
			hqspi->Instance->CCR = 0x2299;
			hqspi->Instance->AR = 0;
			hqspi->Instance->ABR = 0;
			hqspi->Instance->DLR = 0;
			__DSB();

			/* Reset memory config, Cmd in 4 lines*/
			/* Send RESET ENABLE command (0x66) to be able to reset the memory registers */
			while(hqspi->Instance->SR & QSPI_FLAG_BUSY);  /* Wait for busy flag to be cleared */
			hqspi->Instance->CCR = 0x2366;
			hqspi->Instance->AR = 0;
			hqspi->Instance->ABR = 0;
			hqspi->Instance->DLR = 0;
			__DSB(); 

			/* Send RESET command (0x99) to reset the memory registers */
			while(hqspi->Instance->SR & QSPI_FLAG_BUSY);  /* Wait for busy flag to be cleared */
			hqspi->Instance->CCR = 0x2399;
			hqspi->Instance->AR = 0;
			hqspi->Instance->ABR = 0;
			hqspi->Instance->DLR = 0;
			__DSB();
		}
	}
}

/****************************************************************************
 * descri: QSPI Initialization.
 * inputs: flashsize: Specifies the Flash Size.
 * return: 0:OK,[-1,-2]:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
int qspi_init(int flashsize)
{
	/* Zero Init structs */
	uint8_t *ptr = (uint8_t *)&hqspi;
	for (int i=0; i<sizeof(hqspi); i++)
	{
		ptr[i] = 0;
	}
	
	/* QSPI Initialization */
	hqspi.Instance = QUADSPI;
	if (HAL_QSPI_DeInit(&hqspi) != HAL_OK)
	{
		return -1;
	}
	
	hqspi.Init.ClockPrescaler = 1; // 2��Ƶ��ʱ��Ϊ240/(1+1)=120MHz
	hqspi.Init.FifoThreshold = 4; // FIFO��ֵΪ4���ֽ�
	hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE; // ������λ�������
	hqspi.Init.FlashSize = flashsize; // Flash������С
	hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_7_CYCLE; // Ƭѡ�ߵ�ƽʱ��Ϊ7��ʱ��(8.3*7=58.3ns),���ֲ������tSHSL����
	hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0; // ʱ��ģʽѡ��ģʽ0��nCSΪ�ߵ�ƽ��Ƭѡ�ͷţ�ʱ��CLK���뱣�ֵ͵�ƽ
	hqspi.Init.FlashID = QSPI_FLASH_ID_1; // ����Ӳ������ѡ���һƬFlash
	hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE; // ��ֹ˫����ģʽ
	
	if (HAL_QSPI_Init(&hqspi) != HAL_OK)
	{
		return -2;
	}
	
	/* QSPI memory reset */
	qspi_memory_reset(&hqspi);
	return 0;
}

/****************************************************************************
 * descri: QSPI De-Initialization.
 * inputs: void.
 * return: 0:OK,-1:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
int qspi_deinit(void)
{
	if (HAL_QSPI_DeInit(&hqspi) != HAL_OK)
	{
		return -1;
	}
	
	return 0;
}

/****************************************************************************
 * descri: QSPI����ָ��.
 * inputs: ins: Ҫ���͵�ָ��
 *         addr: ���͵���Ŀ�ĵ�ַ
 *         dummy: ��ָ�������� (0 ~ 31)
 *         insmode: ָ��ģʽ; (QSPI_INSTRUCTION_NONE, QSPI_INSTRUCTION_1_LINE, QSPI_INSTRUCTION_2_LINE, QSPI_INSTRUCTION_4_LINE)
 *         addrmode: ��ַģʽ; (QSPI_ADDRESS_NONE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_2_LINE, QSPI_ADDRESS_4_LINE)
 *         addrsize: ��ַ����; (QSPI_ADDRESS_8_BITS, QSPI_ADDRESS_16_BITS, QSPI_ADDRESS_24_BITS, QSPI_ADDRESS_32_BITS)
 *         datamode: ����ģʽ; (QSPI_DATA_NONE, QSPI_DATA_1_LINE, QSPI_DATA_2_LINE, QSPI_DATA_4_LINE)
 * return: 0:OK,-1:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
int qspi_command(unsigned int ins, unsigned int addr, unsigned int dummy, unsigned int insmode, unsigned int addrmode, unsigned int addrsize, unsigned int datamode)
{
	QSPI_CommandTypeDef QSPI_CommandStruct = {0};
	
	QSPI_CommandStruct.Instruction = ins; // ָ��
	QSPI_CommandStruct.InstructionMode = insmode; // ָ��ģʽ
	
	QSPI_CommandStruct.Address = addr; // ��ַ
	QSPI_CommandStruct.AddressSize = addrsize; // ��ַ����
	QSPI_CommandStruct.AddressMode = addrmode; // ��ַģʽ
	
	QSPI_CommandStruct.NbData = 0;
	QSPI_CommandStruct.DataMode = datamode; // ����ģʽ
	QSPI_CommandStruct.DummyCycles = dummy; // ���ÿ�ָ��������
	
	QSPI_CommandStruct.AlternateBytes = 0; // �����ֽ�
	QSPI_CommandStruct.AlternateBytesSize = 0; // �����ֽڳ���
	QSPI_CommandStruct.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; // �����ֽ�ģʽ
	
	QSPI_CommandStruct.SIOOMode = QSPI_SIOO_INST_EVERY_CMD; // ÿ�ζ�����ָ��
	QSPI_CommandStruct.DdrMode = QSPI_DDR_MODE_DISABLE; // �ر�DDRģʽ
	QSPI_CommandStruct.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY; // DDR��������
	
	if (HAL_QSPI_Command(&hqspi, &QSPI_CommandStruct, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return -1;
	}
	
	return 0;
}

/****************************************************************************
 * descri: exFlash�ڴ�ӳ��ģʽ����.
 * inputs: command: Flash command.
 *         addrbit: address lines.
 * return: 0:OK,-1:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
int qspi_memory_mapped_config(unsigned int command, unsigned int addrbit)
{
	QSPI_CommandTypeDef cmd = {0};
	QSPI_MemoryMappedTypeDef cfg = {0};
	
	/* Configure the command for the read instruction */
	cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
	cmd.Instruction = command;
	cmd.AddressMode = QSPI_ADDRESS_4_LINES;
	cmd.AddressSize = addrbit;
	cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode = QSPI_DATA_4_LINES;
	cmd.DummyCycles = 8;
	cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	
	/* Configure the memory mapped mode */
	cfg.TimeOutPeriod = 0;
	cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
	
	if (HAL_QSPI_MemoryMapped(&hqspi, &cmd, &cfg) != HAL_OK)
	{
		return -1;
	}
	
	return 0;
}

/****************************************************************************
 * descri: QSPI��������.
 * inputs: pbuf: ���ݻ����ַ
 *         size: �����ֽ���
 * return: 0:OK,[-1,-2]:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
int qspi_write(const void *pbuf, int size)
{
	if (!pbuf || size < 1)
	{
		return -1;
	}
	
	hqspi.Instance->DLR = size - 1; // �������ݳ���
    if (HAL_QSPI_Transmit(&hqspi, (uint8_t *)pbuf, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return -2;
	}
    
	return 0;
}

/****************************************************************************
 * descri: QSPI��������.
 * inputs: pbuf: ���ݻ����ַ
 *         size: �����ֽ���
 * return: 0:OK,[-1,-2]:ERR.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
int qspi_read(void * const pbuf, int size)
{
	if (!pbuf || size < 1)
	{
		return -1;
	}
	
	hqspi.Instance->DLR = size - 1; // �������ݳ���
    if (HAL_QSPI_Receive(&hqspi, (uint8_t *)pbuf, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return -2;
	}
    
	return 0;
}


/************************ (C) COPYRIGHT WANGZC ***** END OF FILE ********************/
