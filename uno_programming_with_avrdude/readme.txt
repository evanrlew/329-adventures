Here are some files and programs I gathered to program the Arduino 'Uno' with hex files, such as those created by AVRStudio4.  This allows one to program without using the full Arduino environment and just make use of the board itself.

In this archive I put both the drivers, and the latest version (5.8) version of Avrdude.exe and a recent .conf file that contains the programming info to program the device.  Note that older <avrdude.conf> files don't have the entry for the Atmel 328p processor or the Uno programmer in it.

First you need to install the drivers.  Plug the USB cable into the Uno, it will attempt to find a driver and fail, then select the path to install and point it at the arduino_drivers\ directory in this archive (I got it from the drivers\ directory in the much larger 87MB arduino-0022.zip download from www.arduino.cc, copied here to save you time).  I found the driver installation to take a minute or two, a bit longer than I thought but be patient.

**Make sure to take note of the COM port that it installs on, it will inform you in the final message when it completes the driver installation.  

Then you need to create a .hex file.  I use AVR studio4 and the C compiler.  Then you need to make a batch file that contains 'prog_m328 filename.hex' (or you can not bother making a batch file and just run it from the command line).  Note that I made two examples that use 'blink.hex' and 'blink_fast.hex', they cause pin 13 (port pin PB5) to turn on and off.  I test it with a LED through a 1K resistor to GND.

These two batch files themselves call the batch file <prog_m328.bat> which uses the first argument as the hex name.  

You will have to change <prog_m328.bat> to the COM port that your system installs the Arduino Uno driver in.  Mine was COM 9, yours will likely be different.  

Thanks to several posts on internet that helped guide me, including http://hekilledmywire.wordpress.com.

Mark Fiala
mark.fiala@ryerson.ca