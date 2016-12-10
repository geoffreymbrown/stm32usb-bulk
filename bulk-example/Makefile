TEMPLATEROOT = ../

#  compiler flags

CFLAGS = -g -O1  -I./Inc 
AFLAGS = -g
LDFLAGS = -nostdlib

vpath %.c ./Src

# additional source files

CSOURCE =   usbd_bulk.c usbd_bulk_if.c bulk.c

ASMSOURCE =

# additional files to delete at clean (none)

CLEANOTHER= usbtest usbecho

all: usbtest usbecho

usbtest: usbtest.c
	gcc -I/usr/local/include/libusb-1.0 -L/usr/local/lib usbtest.c -o usbtest -lusb-1.0

usbecho: usbecho.c
	gcc -I/usr/local/include/libusb-1.0 -L/usr/local/lib usbecho.c -o usbecho -lusb-1.0

# hal files to exclude from build

CEXCLUDE = stm32f3xx_hal_i2c.c stm32f3xx_hal_i2c_ex.c
CEXCLUDE += stm32f3xx_hal_flash.c stm32f3xx_hal_flash_ex.c
CEXCLUDE += stm32f3xx_hal_pwr.c stm32f3xx_hal_pwr_ex.c
CEXCLUDE += stm32f3xx_hal_tim.c stm32f3xx_hal_tim_ex.c
CEXCLUDE += stm32f3xx_hal_dma.c stm32f3xx_hal_spi.c spi.c
CEXCLUDE += usbd_cdc.c usbd_cdc_if.c

# load template make file

-include $(TEMPLATEROOT)/gpdsc.mk
