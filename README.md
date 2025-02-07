A easy to use port of the POO-POO CodeVisionAVR alcd.h

Tools > External Tools 
  Set Name
  Command: /path/to/avrdude.exe
  Arguments: -c usbasp -p m32 -P usb -U flash:w:"$(ProjectDir)\Debug\$(TargetName).hex":i -vvvv -B 93.75 -b 9600

[change -p (Part Number) with your MCU, since BRACU EEE373 uses Atmel Atmega32A (m32)]

Consolidated version of GitHub User: [efthymios-ks](https://github.com/efthymios-ks)
