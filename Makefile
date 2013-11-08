# Cross compiling:

TARGET	= main

CROSS_COMPILE	= arm-none-eabi-
MCU	= cortex-m3

LDSCRIPT = stm32f100rb_flash.ld

AS	= $(CROSS_COMPILE)as
LD	= $(CROSS_COMPILE)ld
CC	= $(CROSS_COMPILE)gcc
CPP	= $(CC) -E
AR	= $(CROSS_COMPILE)ar
NM	= $(CROSS_COMPILE)nm
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY	= $(CROSS_COMPILE)objcopy
OBJDUMP	= $(CROSS_COMPILE)objdump

CFLAGS  = -c
CFLAGS += -Wall -Wextra -Wno-sign-compare
#CFLAGS += -Wstrict-prototypes
#CFLAGS += -Wmissing-prototypes
CFLAGS += -mcpu=$(MCU)
CFLAGS += -mthumb -DTHUMB
CFLAGS += -mno-thumb-interwork -DTHUMB_NO_INTERWORKING
CFLAGS += -ggdb
CFLAGS += -ffunction-sections -fdata-sections -fno-common
CFLAGS += -fomit-frame-pointer
CFLAGS += -O2
CFLAGS += -MD -MP -MF .dep/$(@F).d

#ASFLAGS += -g -Wall
#ASFLAGS += -Wa,-amhls=$(<:.S=.lst)
ASFLAGS += -mthumb

LDFLAGS = -mcpu=$(MCU)
LDFLAGS += -T $(LDSCRIPT)
LDFLAGS += -Wl,-Map=$(TARGET).map,--cref,--gc-sections
LDFLAGS += -mthumb
LDFLAGS += -mno-thumb-interwork
#LDFLAGS += -nostdlib -Xlinker

INCLUDES = -I .

SRC = $(wildcard *.c)
ASM_SRC = startup_stm32f10x_md_vl.S

OBJDIR = obj

OBJ = $(addprefix $(OBJDIR)/, $(notdir $(SRC:.c=.o)))
OBJ += $(addprefix $(OBJDIR)/, $(notdir $(ASM_SRC:.S=.o)))

# Our target
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $^ $@

$(TARGET).elf: $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

$(OBJDIR)/%.o:%.S
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf *.bin *.elf *.o *.lst .dep/* *.map obj/*
