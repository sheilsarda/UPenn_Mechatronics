# --------------------------------------------------------
# Custom ATmega Makefile
# created by: Chao Liu(chaoliu@seas.upenn.edu)
# updated: Aug 31, 2017
# --------------------------------------------------------

# --------------------------------------------------------
# you shouldn't change anything below here,
# unless you really know what you're doing
# --------------------------------------------------------

# --------------------------------------------------------
# Specify the device you are using and its clock.
# --------------------------------------------------------

DEVICE		= atmega32u4
CLOCK		= 16000000

# --------------------------------------------------------
# Operating System Recognition
# --------------------------------------------------------

ifeq ($(OS), Windows_NT)
	OS_name :=$(OS)
else
	OS_name := $(shell uname)
endif

# --------------------------------------------------------
# if you are using JTAGICE mkII, let PROGRAMMER = jtag2isp;
# if you are using AVRISP mkII, let PROGRAMMER = avrispmkII;
# if you are using USB, let PROGRAMMER = USB.
# --------------------------------------------------------

PROGRAMMER	= USB

TARGET_DEVICE = m32u4
DEVICE_LABEL = ATmega32U4

COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

SRCDIR = src
INCDIR = inc	# directory for header files

INCLUDES = -I$(INCDIR)
INCLUDES += -I./

_SOURCES = $(wildcard $(SRCDIR)/*.c)
_SOURCES += $(wildcard ./*.c)
SOURCES = $(notdir $(_SOURCES))

OBJDIRS = obj_$(DEVICE)
OBJECTS := $(patsubst %.c,%.o, $(SOURCES))
OBJECTS_POS = $(addprefix $(OBJDIRS)/,$(OBJECTS))

vpath %.c $(dir $(_SOURCES))	# directory for source files
vpath %.o $(OBJDIRS)            # directory for object files
vpath %.elf $(OBJDIRS)
vpath %.hex .

# symbolic targets:
all:	main.hex
.PHONY : all

.c.o:
	@[ ! -e $@ ] && mkdir -p $(OBJDIRS)
	@$(COMPILE) $(INCLUDES) -c $< -o $(OBJDIRS)/$@
	@echo "[CC]  $^"

.S.o:
	@$(COMPILE) $(INCLUDES) -x assembler-with-cpp -c $< -o $(OBJDIRS)/$@
	@echo "[>-----Generate $@ Successfully-----<]"

.c.s:
	@$(COMPILE) $(INCLUDES) -S $< -o $(OBJDIRS)/$@
	@echo "[>-----Generate $@ Successfully-----<]"

install: flash

flash: all

ifeq ($(DEVICE), atmega32u4)
ifneq ($(PROGRAMMER), USB)
	@avrdude -p $(TARGET_DEVICE) -c $(PROGRAMMER) -P usb -e -U flash:w:main.hex -B9
	@echo "[>-----$(DEVICE_LABEL) Loaded-----<]"
else
ifeq ($(OS_name), Darwin)
	@echo "Loading Program from Mac OS"
	@./util/teensy_loader_cli_mac -mmcu=$(DEVICE) -v -w main.hex
	@echo "[>-----$(DEVICE_LABEL) Loaded and Running-----<]"
else ifeq ($(OS_name), Linux)
	@echo "Loading Program from $(OS_name)"
	@./util/teensy_loader_cli_linux -mmcu=$(DEVICE) -v -w main.hex
	@echo "[>-----$(DEVICE_LABEL) Loaded and Running-----<]"
else ifeq ($(OS_name), Windows_NT)
	@echo "Loading Program from $(OS_name)"
	@./util/teensy_loader_cli_win -mmcu=$(DEVICE) -v -w main.hex
	@echo "[>-----$(DEVICE_LABEL) Loaded and Running-----<]"
else
	@echo "$(OS_name)"
	@echo "Operating System is not supported."
endif
endif
endif

clean:
	rm -fr main.hex $(OBJDIRS)

# file targets:
main.elf: $(OBJECTS)
	@$(COMPILE) -o $(OBJDIRS)/main.elf $(OBJECTS_POS) -lm
	@echo "[ELF] $(OBJDIRS)/$@"

main.hex: main.elf
	@rm -f main.hex
	@avr-objcopy -j .text -j .data -O ihex $(OBJDIRS)/main.elf main.hex
	@avr-size --format=avr --mcu=$(DEVICE) $(OBJDIRS)/main.elf
	@echo "[>-----Generate $@ Successfully-----<]"
	@echo "[>-----Build Successfully-----<]"

# Targets for code debugging and analysis:
disasm:	main.elf
	avr-objdump -d $(OBJDIRS)/main.elf

cpp:
	$(COMPILE) -E main.c
