#include "Loader_Src.h"
#include "exflash.h"
#include "qspi.h"

#define ADDRESS_OFFSET	((uint32_t)0x90000000U)


/****************************************************************************
 * descri: configures the source of the time base.
 * inputs: TickPriority: Tick interrupt priority.
 * return: HAL status.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{ 
	return HAL_OK;
}

/****************************************************************************
 * descri: 粗略延时函数(强阻塞)
 * inputs: nus:延时时间(us)
 * return: void
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
static void bsp_delay(unsigned int nus)
{
	for(unsigned int i = 0; i < nus; i++)
	{
		for(unsigned int k = 0; k < 257; k++)
		{
			// 此延时参数是针对STM32H7的480MHz主频进行粗略延时
		}
	}
}

/****************************************************************************
 * descri: 重定向HAL库延时函数
 * inputs: Delay: 延时时间(ms)
 * return: void
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
void HAL_Delay(uint32_t Delay)
{
	for(uint32_t i = 0; i < Delay; i++)
	{
		bsp_delay(1000);
	}
}

/****************************************************************************
 * descri: System Clock Configuration
 * inputs: clock: System Clock(MHz)
 * return: 0:OK,[-1,-2]:Failed.
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
static int SystemClock_Config(int clock)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/* Supply configuration update enable */
	HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

	/* Configure the main internal regulator output voltage */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

	while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

	/* Initializes the RCC Oscillators according to the specified parameters in the RCC_OscInitTypeDef structure. */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 5;
	RCC_OscInitStruct.PLL.PLLN = (clock == 480)? 192 : 160;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		return -1;
	}

	/* Initializes the CPU, AHB and APB buses clocks */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
								| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
								| RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		return -2;
	}
	
	return 0;
}

/****************************************************************************
 * descri: Initilize the MCU Clock, the GPIO Pins corresponding to the
 *         device and initilize the FSMC with the chosen configuration.
 * inputs: configureMemoryMappedMode: 0表示配置为内存映射模式
 * return: 1:OK,0:ERR
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
__attribute__((section("used")))
int Init (uint8_t configureMemoryMappedMode)
{
	__disable_irq();
	
	SystemInit();
	
	if(HAL_Init() != 0)
	{
		return 0;
	}
	
	SystemClock_Config(400);
	
	if(exflash_init() != 0)
	{
		return 0;
	}
	
	if(configureMemoryMappedMode == 0)
	{
		if(exflash_memory_mapped_config() != 0)
		{
			return 0;
		}
	}
	
	return 1;
}

/****************************************************************************
 * descri: Write data to QSPI.
 * inputs: Address: Write Start Address(offset:0x90000000)
 *         Size: Write Buffer Size
 *         buffer: Data Buffer
 * return: 1:OK,0:ERR
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
__attribute__((section("used")))
int Write (uint32_t Address, uint32_t Size, uint8_t *buffer)
{
	Address -= ADDRESS_OFFSET;
	if(exflash_write(Address, buffer, Size) != 0)
	{
		return 0;
	}
	
	return 1;
}

/****************************************************************************
 * descri: Erase a full sector in the device.
 * inputs: EraseStartAddress: Start of sector Address(offset:0x90000000)
 *         EraseEndAddress: End of sector Address(offset:0x90000000)
 * return: 1:OK,0:ERR
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
__attribute__((section("used")))
int SectorErase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress)
{
	EraseEndAddress -= ADDRESS_OFFSET;
	EraseStartAddress -= ADDRESS_OFFSET;
	
	while(EraseStartAddress <= EraseEndAddress)
	{
		if(exflash_erase_sector(EraseStartAddress) != 0)
		{
			return 0;
		}
		
		EraseStartAddress += EXFLASH_SECTOR_SIZE;
	}
	
	return 1;
}

/****************************************************************************
 * descri: Erase full chip in the device.
 * inputs: void
 * return: 1:OK,0:ERR
 * author: wangzc.
 * record: 2022/05/20 first version.
 ****************************************************************************/
__attribute__((section("used")))
int MassErase (void)
{
	if(exflash_erase_chip() != 0)
	{
		return 0;
	}
	
	return 1;
}

//__attribute__((section("used")))
//int Read(uint32_t Address, uint32_t Size, uint8_t *Buffer)
//{
//
//		exflash_memory_mapped_config();
//    for (volatile int i = 0; i < Size; i++)
//    {
//        *(uint8_t *)Buffer = *(uint8_t *)Address;
//        Address++;
//        Buffer++;
//    }
//    return 1;
//}
