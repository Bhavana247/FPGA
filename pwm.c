#include "xparameters.h"
#include "xtmrctr.h"
#include "xil_printf.h"
#include "xgpio.h"

#define TIMER_DEVICE_ID     0x42800000
#define LED_GPIO_DEVICE_ID  0x41200000
#define TIMER_COUNTER_0     0
#define PWM_PERIOD          0x00F00000   // Total period (in timer ticks)
#define DUTY_CYCLE          0.5       // 50% duty cycle

XTmrCtr TimerInstance;
u32 HighTime; // High time for the PWM signal
u32 LowTime;  // Low time for the PWM signal

XGpio led_gpio;

int main() {
    int status;
    u32 led_state = 0;
    xil_printf("PWM with AXI Timer\r\n");


	// Initialize GPIO
    status = XGpio_CfgInitialize(&led_gpio, NULL, LED_GPIO_DEVICE_ID);
    if (status != XST_SUCCESS)
        return XST_FAILURE;
		
	XGpio_SetDataDirection(&led_gpio, 1, 0x00);
		
    // Initialize timer
    status = XTmrCtr_Initialize(&TimerInstance, TIMER_DEVICE_ID);
    if (status != XST_SUCCESS) {
        xil_printf("Timer Init Failed\r\n");
        return XST_FAILURE;
    }

    // Set period (total PWM period)
    HighTime = PWM_PERIOD * DUTY_CYCLE;   // High time based on duty cycle
    LowTime = PWM_PERIOD - HighTime;      // Low time is the remainder of the period

    // Configure the timer for auto-reload (down-counting)
    XTmrCtr_SetOptions(&TimerInstance, TIMER_COUNTER_0, XTC_AUTO_RELOAD_OPTION | XTC_DOWN_COUNT_OPTION);

    // Set the reload value (period)
    XTmrCtr_SetResetValue(&TimerInstance, TIMER_COUNTER_0, PWM_PERIOD);

    // Start the timer
    XTmrCtr_Start(&TimerInstance, TIMER_COUNTER_0);

    xil_printf("PWM Running...\r\n");

    while (1) {
        if (XTmrCtr_IsExpired(&TimerInstance, TIMER_COUNTER_0)) {
            led_state = ~led_state;  // Toggle LED state
            XGpio_DiscreteWrite(&led_gpio, 1, led_state);
            XTmrCtr_Reset(&TimerInstance, TIMER_COUNTER_0); // Restart the timer
        }
    }

    return 0;
}
