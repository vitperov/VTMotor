CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=atmega8
OBJ2HEX=avr-objcopy
UISP=avrdude
TARGET=prog
OBJECTS= main.o motor.o smbus_commands.o external/SMBSlave.c

program : $(TARGET).hex
	$(UISP) -c usbasp -p atmega8 -U flash:w:$(TARGET).hex

%.o :   %.c
	$(CC) $(CFLAGS) -c $<

$(TARGET).obj : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

$(TARGET).hex : $(TARGET).obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

clean :
	rm -f *.hex *.obj *.o *.*~
