CC := sdcc
CFLAGS := -mstm8 --std-c99

all: led.ihx serial.ihx serial_interrupt.ihx sound.ihx

%.ihx: %.c
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.asm *.ihx *.cdb *.lst *.map *.lk *.rel *.rst *.sym
