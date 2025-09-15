from pynq import Overlay
from pynq.lib import AxiGPIO

import time
axi_gpio_design = Overlay("led_button.bit")

axi_gpio_design.ip_dict

led_instance = axi_gpio_design.ip_dict['leds']
led = AxiGPIO(led_instance).channel1

led[0:4].write(0x1)

from time import sleep


led[0:4].write(0x1)
sleep(1)
led[0:4].write(0x2)
sleep(1)
led[0:4].write(0x4)
sleep(1)
led[0:4].write(0x8)
sleep(1)

buttons_instance = axi_gpio_design.ip_dict['buttons']
buttons = AxiGPIO(buttons_instance).channel1
value = buttons.read()
print("Buttons status : ", value)