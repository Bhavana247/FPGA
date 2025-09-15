#include "xparameters.h"
#include "xiicps.h"
#include "xil_printf.h"

#define I2C_DEVICE_ID  0
#define I2C_SCLK_RATE  100000  // 100kHz

// For example, LM75A I2C temperature sensor
#define TEMP_SENSOR_ADDR  0x48  // 7-bit address

XIicPs Iic;

int main() {
    XIicPs_Config *Config;
    u8 RecvBuffer[2];
    int Status;
    s16 raw_temp;
    float temperature;

    xil_printf("Initializing I2C...\n");

    Config = XIicPs_LookupConfig(I2C_DEVICE_ID);
    if (Config == NULL) return XST_FAILURE;

    Status = XIicPs_CfgInitialize(&Iic, Config, Config->BaseAddress);
    if (Status != XST_SUCCESS) return XST_FAILURE;

    XIicPs_SetSClk(&Iic, I2C_SCLK_RATE);

    xil_printf("Reading Temperature...\n");

    // LM75: temperature register is at address 0x00
    u8 temp_reg = 0x00;
    XIicPs_MasterSendPolled(&Iic, &temp_reg, 1, TEMP_SENSOR_ADDR);
    while (XIicPs_BusIsBusy(&Iic));

    XIicPs_MasterRecvPolled(&Iic, RecvBuffer, 2, TEMP_SENSOR_ADDR);
    while (XIicPs_BusIsBusy(&Iic));

    raw_temp = (RecvBuffer[0] << 8) | RecvBuffer[1];
    raw_temp >>= 5;  // For 11-bit resolution in LM75
    temperature = raw_temp * 0.125;

    xil_printf("Temperature: %.2f C\n", temperature);

    return 0;
}
