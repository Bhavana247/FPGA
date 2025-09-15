#include "xgpio.h"
#include "xil_printf.h"
#include "xil_types.h"
#include "xil_io.h"

#define GPIO_BTN_BASEADDR  0x41200000
#define GPIO_LED_BASEADDR  0x41210000

#define DEBOUNCE_DELAY     0xFFFF    // Adjust this for stronger debounce

XGpio btn_gpio,led_gpio;

// Simple delay function (software-based)
void debounce_delay() {
    for (volatile int i = 0; i < DEBOUNCE_DELAY; i++);
    for (volatile int i = 0; i < DEBOUNCE_DELAY; i++);
    for (volatile int i = 0; i < DEBOUNCE_DELAY; i++);
}

int main() {
    int Status;
    u32 current_btn_state = 0;
    u32 last_btn_state = 0;
    u32 stable_btn_state = 0;

    // Initialize GPIOs directly with base address
    Status = XGpio_CfgInitialize(&led_gpio, NULL, GPIO_LED_BASEADDR);
    if (Status != XST_SUCCESS) {
        xil_printf("LED GPIO init failed\n");
        return XST_FAILURE;
    }

    Status = XGpio_CfgInitialize(&btn_gpio, NULL, GPIO_BTN_BASEADDR);
    if (Status != XST_SUCCESS) {
        xil_printf("Button GPIO init failed\n");
        return XST_FAILURE;
    }

    // LED: output; Button: input
    XGpio_SetDataDirection(&led_gpio, 1, 0x00);
    XGpio_SetDataDirection(&btn_gpio, 1, 0xFF);

    xil_printf("Button-to-LED with Debounce Example\n");

while (1) {
        // Read current state
        current_btn_state = XGpio_DiscreteRead(&btn_gpio, 1);

        // If changed, debounce
        if (current_btn_state != last_btn_state) {
            debounce_delay();  // Wait for bounce to settle
            current_btn_state = XGpio_DiscreteRead(&btn_gpio, 1);

            if (current_btn_state == last_btn_state) {
                // Still unstable – skip
                continue;
            }
            // Button state is now stable
            stable_btn_state = current_btn_state;
            XGpio_DiscreteWrite(&led_gpio, 1, stable_btn_state);
        }

        last_btn_state = current_btn_state;
    }

    return 0;
}
