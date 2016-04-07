all:
	avr-gcc -mmcu=atmega32u4 -Wall hello.c -o main.out -O1
	avr-objcopy -O ihex main.out main.hex
deploy:
	avrdude -p m32u4 -P /dev/cu.usbmodem1411 -c avr109 -Uflash:w:main.hex:i
