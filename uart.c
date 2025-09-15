#include "xparameters.h"
#include "xuartps.h"
#include "xil_printf.h"

#define UART_DEVICE_ID 0  // UART1

XUartPs Uart_PS;

int main() {
    XUartPs_Config *Config;
    int Status;
    u8 RecvChar;
    u8 LineBuffer[64];  // Buffer to store input
    int Index = 0;

    // Initialize UART1
    Config = XUartPs_LookupConfig(UART_DEVICE_ID);
    Status = XUartPs_CfgInitialize(&Uart_PS, Config, Config->BaseAddress);
    if (Status != XST_SUCCESS) {
        xil_printf("UART1 initialization failed\n");
        return XST_FAILURE;
    }

    xil_printf("UART1 ready. Type something:\n");

    while (1) {
        // Blocking receive one character
        RecvChar = XUartPs_RecvByte(Uart_PS.Config.BaseAddress);

        // Echo it back (optional)
        XUartPs_SendByte(Uart_PS.Config.BaseAddress, RecvChar);

        if (RecvChar == '\r' || RecvChar == '\n') {
            // ENTER key pressed, terminate and process
            LineBuffer[Index] = '\0';
            xil_printf("\nYou typed: %s\n", LineBuffer);
            Index = 0;  // Reset buffer for next input
        } else if (Index < sizeof(LineBuffer) - 1) {
            LineBuffer[Index++] = RecvChar;
        } else {
            // Overflow - reset buffer
            Index = 0;
            xil_printf("\nInput too long! Resetting.\n");
        }
    }

    return 0;
        
}
