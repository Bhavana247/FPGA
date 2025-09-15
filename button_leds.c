#include "xgpio.h"
#include "sleep.h"

XGpio btn_gpio,led_gpio;

int main() {
    int status;
    int psb_check;

    status = XGpio_CfgInitialize(&btn_gpio, NULL, 0x41200000);
    if (status != XST_SUCCESS)
        return XST_FAILURE;

    status = XGpio_CfgInitialize(&led_gpio, NULL, 0x41210000);
    if (status != XST_SUCCESS)
        return XST_FAILURE;

    XGpio_SetDataDirection(&btn_gpio, 1, 0x0F);
    XGpio_SetDataDirection(&led_gpio, 1, 0x00);

    while (1) {

        psb_check = XGpio_DiscreteRead(&btn_gpio, 1);
	    xil_printf("Button Status is %x\r\n", psb_check);
        XGpio_DiscreteWrite(&led_gpio, 1, psb_check); 
        sleep(1);
    }
}