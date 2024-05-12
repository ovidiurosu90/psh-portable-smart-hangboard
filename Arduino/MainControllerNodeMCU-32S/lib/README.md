# Libraries
This folder stores both private and third party libraries.

## Third party libraries
### schreibfaul1/ESP32-audioI2S
Source: [GitHub](https://github.com/schreibfaul1/ESP32-audioI2S) (last change on April 22nd, 2024)
Note: I modified this library as its original footprint was too big. Without any changes, it occupies 95% of the flash memory with the 'Simple Internet Radio' Demo. I basically removed everything that wasn't mp3. In Audio.cpp I removed all occurrences of aac & mp4a & m4a & ts_parsePacket, flac, opus, vorbis. I also dropped the respective directories: aac_decoder, flac_decoder, opus_decoder and vorbis_decoder.

### bogde/HX711
Source: [GitHub](https://github.com/bogde/HX711) (last change on April 13th, 2024)
Note: Unchanged.

### Bodmer/TFT_eSPI
Source: [GitHub](https://github.com/Bodmer/TFT_eSPI) (last change on March 19th, 2024)
Note: I modified User_Setup.h to configure the pins used to interface with the TFT.

