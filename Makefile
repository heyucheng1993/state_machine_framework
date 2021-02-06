##################################################
# PROJECT SETTINGS
##################################################

# project directories
PROJDIR := $(CURDIR)
SOURCEDIR := $(PROJDIR)/src
BUILDDIR := $(PROJDIR)/build

# final executable name
TARGETNAME := state_machine_template
TARGET := $(addprefix $(BUILDDIR)/, $(TARGETNAME)).elf
MAP := $(patsubst %.elf,%.map,$(TARGET))
HEX := $(patsubst %.elf,%.hex,$(TARGET))

# show commands during build
VERBOSE := false

##################################################
# COMPILE SETTINGS
##################################################

# include flag
INCLUDES := $(addprefix -I, $(SOURCEDIR))

# source files
SOURCES := $(wildcard $(SOURCEDIR)/*.c)

# objects
OBJS := $(SOURCES:$(SOURCEDIR)/%.c=$(BUILDDIR)/%.o)

# dependencies
DEPS := $(OBJS:.o=.d)

# compiler
CC := avr-gcc

# chip name
MCU_NAME := atmega328p

# chip model
MMCU := -mmcu=$(MCU_NAME)

# CPU frequency
CPU_FREQ := -DF_CPU=16000000UL

# debugging format
DEBUG := -g

# optimization level
OPT := -Og

# C standard
CSTANDARD := -std=gnu11

# compiler warning configuration
CWARN := -Wall -Wstrict-prototypes

# other user definitions
CDEFS :=

# other user includes
CINCS :=

# compiler flags
CFLAGS := $(DEBUG) $(CDEFS) $(CINCS) $(OPT) $(CWARN) $(CSTANDARD) $(MMCU) $(CPU_FREQ) $(INCLUDES)

##################################################
# LINK SETTINGS
##################################################

# map file options
MAP_GEN := -Wl,-Map=$(MAP),--cref

# linker flags
LDFLAGS := $(MAP_GEN) -ffunction-sections -fdata-sections

##################################################
# OBJCOPY SETTINGS
##################################################

# object copy utility
OBJCOPY := avr-objcopy

# executable format
FORMAT := ihex

# section select for hex
SECTION_HEX := -j .text -j .data

##################################################
# SIZE CALCULATION SETTINGS
##################################################

# size calculator
SIZE_CALCULATOR = avr-size

# option
SIZE_CALCULATION_OPTION := --mcu=$(MCU_NAME) -C

##################################################
# FLASH SETTINGS
##################################################

# flash configuration file
CONFGIG_FILE := -C "C:\Program Files (x86)\Arduino\hardware\tools\avr/etc/avrdude.conf"

# flash tool
AVRDUDE := avrdude

# programmer
PROGRAMMER := -c arduino

# device
DEVICE := -p $(MCU_NAME)

# port
PORT := -P COM4

# baud rate
BAUD := -b 57600

# auto erase
AUTO_ERASE := -D

# memory operation specification
MEMORY := -U flash:w:$(subst /,\,$(HEX)):i

# verbose output
AVRDUDEVERBOSE := -v

# fuse section
FUSESECTION := -U lfuse:r:-:i -U hfuse:r:-:i -U efuse:r:-:i

# flash flags
FLASHFLAGS := $(CONFGIG_FILE) $(PROGRAMMER) $(DEVICE) $(PORT) $(BAUD) $(AUTO_ERASE) $(MEMORY)

# check fuse
FUSECHECK := $(CONFGIG_FILE) $(PROGRAMMER) $(DEVICE) $(PORT) $(BAUD) $(AVRDUDEVERBOSE) $(FUSESECTION)

##################################################
# GENERAL SETTINGS
##################################################

# OS specific part
ifeq ($(OS),Windows_NT)
	RM := del /F /Q 
	RMDIR := -RMDIR /S /Q
	MKDIR := -mkdir
	ERRIGNORE := 2>NUL
	SEP2:=\ 
else
	RM := rm -rf 
	RMDIR := rm -rf 
	MKDIR := mkdir -p
	ERRIGNORE := 2>/dev/null
	SEP2:=/
endif
SEP := $(strip $(SEP2))

# verbose build output
ifeq ($(VERBOSE),true)
	HIDE :=  
else
	HIDE := @
endif

##################################################
# MAKE TARGETS
##################################################

# default target
.PHONY: all
all: create_build $(TARGET) $(HEX)
	$(HIDE)echo Make done.

# compile and assemble to objects
$(OBJS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.c
	$(HIDE)echo -----------------------------------------------
	$(HIDE)echo Building $@
	$(HIDE)echo -----------------------------------------------
	$(HIDE)$(CC) $(CFLAGS) -c $(subst /,$(SEP),$<) -o $(subst /,$(SEP),$@)

# link objects to target
$(TARGET): $(OBJS)
	$(HIDE)echo -----------------------------------------------
	$(HIDE)echo Linking $@
	$(HIDE)echo -----------------------------------------------
	$(HIDE)$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(TARGET)

# generate hex
$(HEX): $(TARGET)
	$(HIDE)echo -----------------------------------------------
	$(HIDE)echo Generating $@
	$(HIDE)echo -----------------------------------------------
	$(HIDE)$(OBJCOPY) $(SECTION_HEX) -O $(FORMAT) $< $@

# create build directory
.PHONY: create_build
create_build: 
	$(HIDE)$(MKDIR) $(subst /,$(SEP),$(BUILDDIR)) $(ERRIGNORE) || echo Build folder alread exists.

# remove all objects, dependencies and executable files
.PHONY: clean
clean:
	$(HIDE)$(RMDIR) $(subst /,$(SEP),$(BUILDDIR)) $(ERRIGNORE) || echo Nothing to clean
	@echo Cleaning done ! 

# calculate program size and usage
.PHONY: size
size:
	$(SIZE_CALCULATOR) $(SIZE_CALCULATION_OPTION) $(TARGET)

# flash hex in device
.PHONY: flash
flash:
	$(HIDE)$(AVRDUDE) $(FLASHFLAGS)

.PHONY: fuse
fuse:
	$(HIDE)$(AVRDUDE) $(FUSECHECK)
