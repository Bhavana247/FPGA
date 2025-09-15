#include "xgpio.h"
#include "sleep.h"

XGpio led_gpio;

int main() {
    int status;
    XGpio_Config *cfg;

    // Lookup configuration
    cfg = XGpio_LookupConfig(0);  // Device ID (usually 0 for LED GPIO)
    if (!cfg)
        return XST_FAILURE;

    // Initialize GPIO
    status = XGpio_CfgInitialize(&led_gpio, cfg, cfg->BaseAddress);
    if (status != XST_SUCCESS)
        return XST_FAILURE;

    // Set GPIO channel 1 as output
    XGpio_SetDataDirection(&led_gpio, 1, 0x00);

    while (1) {
        // Turn on LEDs
        XGpio_DiscreteWrite(&led_gpio, 1, 0x0F);  // Set lower 4 LEDs
        sleep(1);

        // Turn off LEDs
        XGpio_DiscreteWrite(&led_gpio, 1, 0x00);
        sleep(1);
    }

    return 0;
}
