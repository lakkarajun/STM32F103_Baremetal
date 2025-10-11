#include "vl53l1_platform.h"
#include "stm32f1xx_hal.h"

extern I2C_HandleTypeDef hi2c1;

int8_t VL53L1_WriteMulti(VL53L1_Dev_t *pdev, uint16_t index, uint8_t *pdata, uint32_t count)
{
    return HAL_I2C_Mem_Write(&hi2c1, pdev->i2c_slave_address, index, I2C_MEMADD_SIZE_16BIT, pdata, count, 100);
}

int8_t VL53L1_ReadMulti(VL53L1_Dev_t *pdev, uint16_t index, uint8_t *pdata, uint32_t count)
{
    return HAL_I2C_Mem_Read(&hi2c1, pdev->i2c_slave_address, index, I2C_MEMADD_SIZE_16BIT, pdata, count, 100);
}

int8_t VL53L1_WrByte(VL53L1_Dev_t *pdev, uint16_t index, uint8_t data)
{
    return HAL_I2C_Mem_Write(&hi2c1, pdev->i2c_slave_address, index, I2C_MEMADD_SIZE_16BIT, &data, 1, 100);
}

int8_t VL53L1_RdByte(VL53L1_Dev_t *pdev, uint16_t index, uint8_t *data)
{
    return HAL_I2C_Mem_Read(&hi2c1, pdev->i2c_slave_address, index, I2C_MEMADD_SIZE_16BIT, data, 1, 100);
}

VL53L1_Error VL53L1_RdWord(
	VL53L1_Dev_t *pdev,
	uint16_t      index,
	uint16_t     *pdata)
{
	VL53L1_Error status         = VL53L1_ERROR_NONE;
	uint8_t  buffer[2];

	status = VL53L1_ReadMulti(
					pdev,
					index,
					buffer,
					VL53L1_BYTES_PER_WORD);

	*pdata = (uint16_t)(((uint16_t)(buffer[0])<<8) + (uint16_t)buffer[1]);

	return status;
}


VL53L1_Error VL53L1_RdDWord(
	VL53L1_Dev_t *pdev,
	uint16_t      index,
	uint32_t     *pdata)
{
	VL53L1_Error status = VL53L1_ERROR_NONE;
	uint8_t  buffer[4];

	status = VL53L1_ReadMulti(
					pdev,
					index,
					buffer,
					VL53L1_BYTES_PER_DWORD);

	*pdata = ((uint32_t)buffer[0]<<24) + ((uint32_t)buffer[1]<<16) + ((uint32_t)buffer[2]<<8) + (uint32_t)buffer[3];

	return status;
}

VL53L1_Error VL53L1_GetTickCount(VL53L1_Dev_t *pDev, uint32_t *pTickCountMs)
{
    if (pTickCountMs != NULL) {
        *pTickCountMs = HAL_GetTick(); // Uses SysTick by default
        return VL53L1_ERROR_NONE;
    }
    return VL53L1_ERROR_INVALID_PARAMS;
}

VL53L1_Error VL53L1_WaitMs(VL53L1_Dev_t *pDev, int32_t wait_ms)
{
    HAL_Delay(wait_ms);
    return VL53L1_ERROR_NONE;
}
