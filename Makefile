CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues
OBJ2HEX=avr-objcopy
UISP=avrdude
TARGET=board
MCU_CMP=-mmcu=atmega8
MCU_FLH=atmega8
OBJECTS11= main.o motor.o smbus_commands.o external/SMBSlave.c
OBJECTS12= main.o motor1.2.o smbus_commands.o external/SMBSlave.c

TARGET11=$(TARGET).v1.1
TARGET12=$(TARGET).v1.2

program : $(TARGET11).hex $(TARGET12).hex
	$(UISP) -c usbasp -p $(MCU_FLH) -U flash:w:$(TARGET).hex

%.o :   %.c
	$(CC) $(CFLAGS) $(MCU_CMP) -c $<

$(TARGET11).obj : $(OBJECTS11)
	$(CC) $(CFLAGS) $(MCU_CMP) $(OBJECTS11) -o $@

$(TARGET12).obj : $(OBJECTS12)
	$(CC) $(CFLAGS) $(MCU_CMP) $(OBJECTS12) -o $@

$(TARGET11).hex : $(TARGET11).obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

$(TARGET12).hex : $(TARGET12).obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

clean :
	rm -f *.hex *.obj *.o *.*~
