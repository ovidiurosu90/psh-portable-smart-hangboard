# [PSH Portable Smart Hangboard](https://github.com/ovidiurosu90/psh-portable-smart-hangboard)

## Abstract
### Problem
Help people get better at climbing by improving their finger strength

### Solution
Provide a portable smart hangboard to use for finger strength training

### How
- the whole project is open-source, and can be replicated by interested parties (note it requires a variety of skills like setting up a web server, 3d printing, soldering & crimping)
- the hangboard measures the pressure applied to the hanghold by using a couple of load cells
- the training plans are set in Google Sheets

## Images

![CAD vs Real World](./Images/PSH_Portable_Smart_Hangboard_May_2024_v3_2024-05-13_CAD_vs_real_world.PNG "CAD vs Real World")

## Usage
- Power the microcontroller via its micro-USB port (+5V)
- Wait for the microcontroller to finish setting things up (it's ready when it says 'idle' on the lower left side of analog meter)
- There are 3 big black momentary switches (labeled SW1, SW2 and SW3)
- There are 3 small toggle switches (labeled SW4, SW5 and SW6)
- The functionality of the switches is described below

### SW1
If SW4 is toggled ON: start training

Else: decrease volume

### SW2
If SW4 is toggled ON: start calibrating

Else: store volume into NVS (only if SW5 is toggled ON)

### SW3
If SW4 is toggled ON: end training

Else: increase volume

### SW4
If toggled ON: show train menu

Else: show volume menu

### SW5
If toggled ON: allow NVS to be updated (for calibration and for volume)

Else: don't allow NVS to be updated

### SW6
Not used at the moment


## Wiring

![NodeMCU 32S Pinout](./Images/NodeMCU-32S_pinout.png "NodeMCU 32S Pinout")

![MainControllerNodeMCU-32S Schematic Editor](./Images/MainControllerNodeMCU-32S_Schematic_Editor.PNG "MainControllerNodeMCU-32S Schematic Editor")

| [NodeMcu ESP-32S Lua ESP-WROOM-32](https://opencircuit.shop/product/nodemcu-esp-32s-lua-esp-wroom-32-wifi) | [SparkFun HX711 Load Cell Amplifier](https://eu.robotshop.com/products/hx711-load-cell-amplifier?variant=42600661942440) => NOTE we need to solder male pins |
| ---------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| 5, 4                                                                                                       | DAT1, CLK1                                                                                                                                                   |
| 16, 15                                                                                                     | DAT2, CLK2                                                                                                                                                   |
| +3.3V                                                                                                      | VDD, VCC                                                                                                                                                     |
| GND                                                                                                        | GND                                                                                                                                                          |

| [NodeMcu ESP-32S Lua ESP-WROOM-32](https://opencircuit.shop/product/nodemcu-esp-32s-lua-esp-wroom-32-wifi) | [AZDelivery ST7735 SPI TFT Display 1.8 inch 128 x 160 pixels](https://www.amazon.nl/dp/B078J5TS2G/ref=pe_28126711_487805961_TE_item?th=1) =>  [guide](https://www.az-delivery.de/en/blogs/azdelivery-blog-fur-arduino-und-raspberry-pi/1-8-toll-tft-am-esp-32-dev-kit-c-betreiben) |
| ---------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| +5V                                                                                                        | VCC                                                                                                                                                                                                                                                                                |
| GND                                                                                                        | GND                                                                                                                                                                                                                                                                                |
| G17                                                                                                        | CS                                                                                                                                                                                                                                                                                 |
| G14                                                                                                        | Reset / RST                                                                                                                                                                                                                                                                        |
| G2                                                                                                         | A0 / DC                                                                                                                                                                                                                                                                            |
| G23                                                                                                        | Sda / MOSI                                                                                                                                                                                                                                                                         |
| G18                                                                                                        | Sck / SCLK                                                                                                                                                                                                                                                                         |
| 3.3V                                                                                                       | LED                                                                                                                                                                                                                                                                                |

| [NodeMcu ESP-32S Lua ESP-WROOM-32](https://opencircuit.shop/product/nodemcu-esp-32s-lua-esp-wroom-32-wifi) | [AZDelivery ST7735 SPI TFT Display 1.8 inch 128 x 160 pixels](https://www.amazon.nl/dp/B078J5TS2G/ref=pe_28126711_487805961_TE_item?th=1) => [guide](https://randomnerdtutorials.com/guide-to-1-8-tft-display-with-arduino/) |
| ---------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| G33                                                                                                        | CS                                                                                                                                                                                                                           |
| G23                                                                                                        | MOSI (shared with display)                                                                                                                                                                                                   |
| G19                                                                                                        | MISO                                                                                                                                                                                                                         |
| G18                                                                                                        | SCK / SCLK (shared with display                                                                                                                                                                                              |

NOTE At the moment, these connections need to be done manually, as in: 4 wires should be soldered directly on the PCB. This is dirty, error prone, and hopefully temporary.
With the new custom PCB version for MainControllerNodeMCU-32S, we'll have a proper JST-XH 4-pin connector that will be used for this purpose.
Check [KiCad/README.md](KiCad).


| [NodeMcu ESP-32S Lua ESP-WROOM-32](https://opencircuit.shop/product/nodemcu-esp-32s-lua-esp-wroom-32-wifi) | [SparkFun MAX98357A I2S Audio Breakout](https://opencircuit.shop/product/sparkfun-i2s-audio-breakout-max98357a) => [guide](https://learn.sparkfun.com/tutorials/i2s-audio-breakout-hookup-guide/all) |
| ---------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| +5V (if we use +3.3V the volume fluctuates)                                                                | VDD                                                                                                                                                                                                  |
| GND                                                                                                        | GND                                                                                                                                                                                                  |
| \-                                                                                                         | SD                                                                                                                                                                                                   |
| GND                                                                                                        | GAIN (+12dB, without this it will be +9dB)                                                                                                                                                           |
| G22                                                                                                        | DIN / DOUT                                                                                                                                                                                           |
| G26                                                                                                        | BCLK                                                                                                                                                                                                 |
| G25                                                                                                        | LRCLK                                                                                                                                                                                                |

| [NodeMcu ESP-32S Lua ESP-WROOM-32](https://opencircuit.shop/product/nodemcu-esp-32s-lua-esp-wroom-32-wifi) | SwitchesModule (my custom PCB) |
| ---------------------------------------------------------------------------------------------------------- | ------------------------------ |
| G21                                                                                                        | SW1                            |
| G27                                                                                                        | SW2                            |
| G13                                                                                                        | SW3                            |
| GND                                                                                                        | GND                            |
| +3.3V                                                                                                      | ~ pull-up resistors            |
| G36                                                                                                        | SW4 with 10k resistor to +3.3V |
| G39                                                                                                        | SW5 with 10k resistor to +3.3V |
| G35                                                                                                        | SW6 with 10k resistor to +3.3V |
| GND                                                                                                        | GND                            |

| [NodeMcu ESP-32S Lua ESP-WROOM-32](https://opencircuit.shop/product/nodemcu-esp-32s-lua-esp-wroom-32-wifi) | ?         |
| ---------------------------------------------------------------------------------------------------------- | --------- |
| G32 (input-output pin)                                                                                     | Available |
| G34 (only output pin)                                                                                      | Available |


## Others

| Name | README |
| --------------------- | ------ |
| BOM Bill Of Materials | [BOM.md](BOM.md) |

