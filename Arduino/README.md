# Arduino
Programming the microcontroller was done using [PlatformIO](https://platformio.org/) - Arduino framework.

Before uploading the firmware to the microcrontroller, we have to set a few configuration parameters.
```bash
cd MainControllerNodeMCU-32S/config/
cp secret.ini.dist secret.ini
vim secret.ini and fill in the proper values
```

I use a custom made script that bundles a few of the PlatformIO together, though in short, these are the steps to be run to program the microcontroller:
```bash
platformio pkg update
platformio run
platformio run --target upload
platformio device monitor --port /dev/O_ttyUSB0 --baud 115200 | ts [%Y-%m-%d %H:%M:%S] | tee -a MainControllerNodeMCU-32S.log
```

