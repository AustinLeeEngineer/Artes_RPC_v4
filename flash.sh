#! /bin/bash

/home/mrush/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI -c port=SWD mode=UR reset=HWrst freq=240 -ob rdp=0xAA -e all -d ./build/ARTES_Rack_Power_Controller_v3.hex 0x08000000 -v -rst

