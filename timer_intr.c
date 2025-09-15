#include "xparameters.h"
#include "xscugic.h"
#include "xtmrctr.h"
#include "xil_exception.h"

#define TIMER_DEVICE_ID      0
#define INTC_DEVICE_ID       0
#define TIMER_IRPT_INTR      61

static XTmrCtr TimerInstance;
static XScuGic InterruptController;

volatile int TimerExpired = 0;

void TimerInterruptHandler(void *CallBackRef, u8 TmrCtrNumber) {
    XTmrCtr *InstancePtr = (XTmrCtr *)CallBackRef;
    
    if (XTmrCtr_IsExpired(InstancePtr, TmrCtrNumber)) {
        TimerExpired = 1;
        XTmrCtr_Reset(InstancePtr, TmrCtrNumber); // Clear interrupt
    }
}

int main() {
    int Status;

    // Initialize Timer
    Status = XTmrCtr_Initialize(&TimerInstance, TIMER_DEVICE_ID);
    if (Status != XST_SUCCESS) return XST_FAILURE;

    XTmrCtr_SetOptions(&TimerInstance, 0, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
    XTmrCtr_SetResetValue(&TimerInstance, 0, 0x0000000F);  // Adjust for desired time
    XTmrCtr_Start(&TimerInstance, 0);

    // Initialize Interrupt Controller
    XScuGic_Config *IntcConfig;
    IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
    if (IntcConfig == NULL) return XST_FAILURE;

    Status = XScuGic_CfgInitialize(&InterruptController, IntcConfig, IntcConfig->CpuBaseAddress);
    if (Status != XST_SUCCESS) return XST_FAILURE;

    // Connect Timer Interrupt Handler
    Status = XScuGic_Connect(&InterruptController, TIMER_IRPT_INTR,
                             (Xil_InterruptHandler)XTmrCtr_InterruptHandler,
                             (void *)&TimerInstance);
    if (Status != XST_SUCCESS) return XST_FAILURE;

    XScuGic_Enable(&InterruptController, TIMER_IRPT_INTR);

    // Set up exception handler
    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                                 (Xil_ExceptionHandler)XScuGic_InterruptHandler,
                                 &InterruptController);
    Xil_ExceptionEnable();

    // Enable Timer Interrupt in hardware
    XTmrCtr_SetHandler(&TimerInstance, TimerInterruptHandler, &TimerInstance);

    u32 reset_value = 0xFFFFFFFF - 100000000; // 1 second at 100 MHz
XTmrCtr_SetResetValue(&TimerInstance, 0, reset_value);



    print("Timer interrupt example started. Waiting for timer...\n");

    while (1) {
        if ((XTmrCtr_IsExpired(&TimerInstance, 0)) ||
        (TimerExpired)) {
            TimerExpired = 0;
            xil_printf("Timer interrupt occurred!\n");
            XTmrCtr_Reset(&TimerInstance, 0);
        }
    }

    return 0;
}
