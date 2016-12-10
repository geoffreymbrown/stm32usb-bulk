V = 1

# name of executable

ELF=$(notdir $(CURDIR)).elf                    
BIN=$(notdir $(CURDIR)).bin
MKFRAG=$(notdir $(CURDIR)).mk
GPDSC=$(notdir $(CURDIR)).gpdsc

# Tool path

TOOLROOT=/l/arm2/devtools/bin/

# Tools

GPDSC2MK=python $(TEMPLATEROOT)/gpdsc2make.py

CC_0 = @echo -n "Compiling $< "; $(CC_1)
CC_1 = $(TOOLROOT)/arm-none-eabi-gcc
CC = $(CC_$(V))

GAS_0 = @echo -n "Assembling $< "; $(GAS_1)
GAS_1 = $(TOOLROOT)/arm-none-eabi-gcc
GAS = $(GAS_$(V))


DD_0 = @echo " "; $(DD_1)
DD_1 = $(TOOLROOT)/arm-none-eabi-gcc
DD = $(DD_$(V))

LD_0 = @echo "Linking $@"; $(LD_1)
LD_1=$(TOOLROOT)/arm-none-eabi-gcc
LD = $(LD_$(V))

DL_0 = @echo -n "Downloading"; $(DL_1)
DL_1=st-flash 
DL = $(DL_$(V))

OC_0 = @echo "Creating Bin Downloadable File"; $(OC_1)
OC_1=$(TOOLROOT)/arm-none-eabi-objcopy
OC = $(OC_$(V))

GP_0 = @echo " "; $(GP_1)
GP_1=grep
GP = $(GP_$(V))

RM_0 = @echo "Cleaning Directory"; $(RM_1)
RM_1=rm
RM = $(RM_$(V))

AR=$(TOOLROOT)/arm-none-eabi-ar
AS=$(TOOLROOT)/arm-none-eabi-as
OBJCOPY=$(TOOLROOT)/arm-none-eabi-objcopy

# Version

GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always --tags)
CFLAGS += -DVERSION=\"$(GIT_VERSION)\"

# Code Paths

vpath %.c $(TEMPLATEROOT)/Src


#  Processor specific

ifndef LDSCRIPT
LDSCRIPT = $(TEMPLATEROOT)/stm32f3xx.ld
endif

# Compilation Flags

ifdef PROCESSOR
CFLAGS += -D$(PROCESSOR)
else
CFLAGS += -DSTM32F303xC
endif

# LDLIBS = --specs=nosys.specs -lm 

LDLIBS = #-lm 
LDFLAGS+= -T$(LDSCRIPT) -mthumb -mcpu=cortex-m4
CFLAGS+= -mcpu=cortex-m4 -mthumb 
CFLAGS+= -Wno-multichar

# Generate project specific makefile

all:

$(MKFRAG): $(GPDSC)
	$(GPDSC2MK) $< > $@

-include $(MKFRAG)

CSOURCE := $(sort $(filter-out $(CEXCLUDE),$(CSOURCE)))
#CSOURCE := $(filter-out $(CEXCLUDE),$(CSOURCE))
OBJS += $(addprefix build/, $(CSOURCE:.c=.o) $(ASMSOURCE:.s=.o))
CFLAGS += $(INC)
CFLAGS += -I$(TEMPLATEROOT)/Inc

# Build executable 

$(ELF) : $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)
	echo $(OBJS:.o=.d)

# compile and generate dependency info

build:
	@mkdir -p $@

build/%.o: %.c | build
	$(CC) -c $(CFLAGS) $< -o $@
	$(DD) -MM $(CFLAGS) $< > build/$*.d.tmp
	@sed -e 's|.*:|build/$*.o:|' < build/$*.d.tmp > build/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < build/$*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> build/$*.d
	@rm -f build/$*.d.tmp

build/%.o: %.s | build
	$(GAS) -c $(CFLAGS) $< -o $@
	$(DD) -MM $(CFLAGS) $< > build/$*.d.tmp
	@sed -e 's|.*:|build/$*.o:|' < build/$*.d.tmp > build/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < build/$*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> build/$*.d
	@rm -f build/$*.d.tmp

%.bin: %.elf
	$(OC) -O binary $< $@

clean:
	$(RM) -rf build  $(ELF) $(CLEANOTHER) $(BIN) $(MKFRAG) st-flash.log

debug: $(ELF)
	arm-none-eabi-gdb $(ELF)

download: $(BIN)
	$(DL)write $(BIN) 0x8000000 > st-flash.log 2>&1
	$(GP) -o "jolly" st-flash.log | sed 's/jolly/Success/'
	$(GP) -o "Couldn" st-flash.log | sed 's/Couldn/Fail/'

all: $(ELF)

# pull in dependencies

-include $(OBJS:.o=.d)





