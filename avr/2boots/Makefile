# 2boots Makefile
# (c) 2011 thomas seiler      (thseiler@gmail.com)
#

# 'all' is the default target: 
.PHONY: all 

# .hex files are considered intermediary files, but we want to keep them, so
# we declare secondary, which tells make to keep intermediary files...
.SECONDARY:

# setup avr-gcc to compile very space efficient
CC       = avr-gcc
OBJCOPY  = avr-objcopy
OBJDUMP  = avr-objdump
OPTIMIZE = -Os -funsigned-char -fno-split-wide-types -fno-inline-small-functions -mcall-prologues -ffunction-sections -fdata-sections -ffreestanding -combine -fwhole-program -fno-jump-tables
AVRSIZE  = avr-size

override CFLAGS  = -g -Wall $(OPTIMIZE) -mmcu=$(MCU) -DF_CPU=$(FCPU) $(DEFS)
override LDFLAGS = -Wl,$(LDSECTION) -Wl,--relax -nostartfiles -Wl,-gc-sections

# these are our actual source files
# the BOARD variable will be replaced later, depending on which board is beeing compiled
SRC = $(BOARD).c mmc_fat.c stk500v1.c prog_flash.c

# the 'all' rule should build 2boots for all supported boards
# to create one target per board-*.c file we use a $(wildcard... ) rule
all: buildfolder $(subst .c,,$(wildcard board-*.c)) size-report 

# here we create the 'build' folder in case it does not exist
buildfolder:
	[ -d build ] || mkdir build

#each board can list all possible combinations for FCPUS, MCUS, and MMC_CS here
#these variables are target specific, so each board can define a different value
#(see http://www.gnu.org/software/make/manual/make.html#Target_002dspecific)
#
# To compile 2boot twice for a given board (i.e. two frequencies), simply
# separate the arguments with a space

#default for any unknown board
board-%: FCPUS = 16000000L
board-%: MCUS = atmega328p
board-%: MMC_CSS = disabled

#arduino
board-arduino: FCPUS = 8000000L 16000000L
board-arduino: MCUS = atmega168 atmega328p
board-arduino: MMC_CSS = disabled PD4

#lilypad
board-lilypad: FCPUS = 8000000L
board-lilypad: MCUS = atmega168
board-lilypad: MMC_CSS = disabled

#stalker:
board-stalker: FCPUS = 8000000L
board-stalker: MCUS = atmega328p
board-stalker: MMC_CSS = PB1 PB2

#ftthhead
board-ftthhead: FCPUS = 8000000L
board-ftthhead: MCUS = atmega328p
board-ftthhead: MMC_CSS = disabled

#ftthcont
board-ftthcont: FCPUS = 8000000L
board-ftthcont: MCUS  = atmega328p
board-ftthcont: MMC_CSS = PB2



# set certain variables per MCU or MCU-FREQ:

#atmega168:
2boots-atmega168%:  LDSECTION := --section-start=.text=0x3800

#atmega328p:
2boots-atmega328p%:  LDSECTION := --section-start=.text=0x7800 
2boots-atmega328p%:  DEFS += -DBAUD_RATE=57600
2boots-atmega328p-8000000L%: DEFS += -DDOUBLE_SPEED

#atmega1280:
2boots-atmega1280%: LDSECTION := --section-start=.text=0x1F000 


# This rule finally builds 2boots.
# The idea here is to learn the configuration from the make target name
# This is done with the help of good old shell
2boots-%: MCU =  $(shell echo $@ | cut -d - -f 2 )
2boots-%: FCPU = $(shell echo $@ | cut -d - -f 3 )
2boots-%: MMC_CS = $(shell echo $@ | cut -d - -f 4)
2boots-%: BOARD = $(shell echo $(subst .hex,,$@) | awk 'BEGIN {FS="-"} { for (f=5;f<=NF;f++) { printf($$f); if (f != NF) { printf("-") } } }' )
2boots-%: DEFS += $(if $(findstring disabled,$(MMC_CS)) , , -DMMC_CS=$(MMC_CS) -DMMC_PORT=PORT$(shell echo $(MMC_CS) | cut -c 2) -DMMC_DDR=DDR$(shell echo $(MMC_CS) | cut -c 2 )) 
2boots-%:
	@echo 
	@echo ----------[$@]-------------------
	@echo Compiling 2boots for $(BOARD) with $(MCU) @ $(subst 000L,,$(FCPU))kHz and MMC $(MMC_CS)...
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o build/$@ $(LIBS)
	@echo
	@echo sucessfully compiled to build/$@
	@$(AVRSIZE) build/$@
	@echo 


# here come a few handy pattern rules for objcopy / objdump, 
# just in case someone wants a different format...

%.lst: %.elf
	$(OBJDUMP) -h -S build/$< > build/$@

%.hex: %.elf %.lst
	$(OBJCOPY) -j .text -j .data -O ihex build/$< build/$@	

%.srec: %.elf
	$(OBJCPOY) -j .text -j .data -O srec build/$< build/$@

%.bin: %.elf
	$(OBJCPOY) -j .text -j .data -O binary build/$< build/$@

# this one is handy, too
clean:
	rm -rf build/*.elf build/*.lst build/*.srec build/*.bin build/*.hex

# this helps to keep track of used / free space
size-report:
	@echo -----[Size reports]----------------------------------------
	@echo 
	@$(AVRSIZE) build/*.hex

# This section finally creates targets for all combinations of boards, mcus, mmc-cs and frequencies:
# because of evil make magic (secondary expansion) it should be the last rule
board-%: TARGETS = $(foreach currentmcu, $(MCUS) , $(foreach currentfreq, $(FCPUS), $(foreach currentmmccs, $(MMC_CSS), 2boots-$(currentmcu)-$(currentfreq)-$(currentmmccs)-$@.hex ) ) )
.SECONDEXPANSION:
board-%:  $$(TARGETS) ;
