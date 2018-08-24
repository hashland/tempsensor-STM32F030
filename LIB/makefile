#----------------------------------------------------------------------------
# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
EXTRAINCDIRS =    \
            .     \
            inc
#----------------------------------------------------------------------------
DEP_DIR = .deps
#----------------------------------------------------------------------------
# Flags for C and C++ (arm-elf-gcc/arm-elf-g++)
CFLAGS =  -DSTM32F042x6 -mthumb -mcpu=cortex-m0
CFLAGS += -D__USE_CMSIS -DUSE_HAL_DRIVER
CFLAGS += -O0 -g3 -std=gnu99 
CFLAGS += -ffunction-sections -fno-strict-aliasing -fno-builtin
CFLAGS += -Wall
CFLAGS += -MD -MP -MF $(DEP_DIR)/$(@F).d
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
# ---------------------------------------------------------------------------
# Define programs and commands.
CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size
NM = arm-none-eabi-nm
REMOVE = rm -f
REMOVEDIR = rm -r
COPY = cp
#----------------------------------------------------------------------------
SRC  =  \
        src/stm32f0xx_hal.c		\
        src/stm32f0xx_hal_adc.c		\
        src/stm32f0xx_hal_adc_ex.c	\
        src/stm32f0xx_hal_can.c		\
        src/stm32f0xx_hal_cec.c		\
        src/stm32f0xx_hal_comp.c	\
        src/stm32f0xx_hal_cortex.c	\
        src/stm32f0xx_hal_crc.c		\
        src/stm32f0xx_hal_crc_ex.c	\
        src/stm32f0xx_hal_dac.c		\
        src/stm32f0xx_hal_dac_ex.c	\
        src/stm32f0xx_hal_dma.c		\
        src/stm32f0xx_hal_flash.c	\
        src/stm32f0xx_hal_flash_ex.c	\
        src/stm32f0xx_hal_gpio.c	\
        src/stm32f0xx_hal_i2c.c		\
        src/stm32f0xx_hal_i2c_ex.c	\
        src/stm32f0xx_hal_i2s.c		\
        src/stm32f0xx_hal_irda.c	\
        src/stm32f0xx_hal_iwdg.c	\
        src/stm32f0xx_hal_pcd.c		\
        src/stm32f0xx_hal_pcd_ex.c	\
        src/stm32f0xx_hal_pwr.c		\
        src/stm32f0xx_hal_pwr_ex.c	\
        src/stm32f0xx_hal_rcc.c		\
        src/stm32f0xx_hal_rcc_ex.c	\
        src/stm32f0xx_hal_rtc.c		\
        src/stm32f0xx_hal_rcc_ex.c	\
        src/stm32f0xx_hal_smartcard.c	\
        src/stm32f0xx_hal_smartcard_ex.c \
        src/stm32f0xx_hal_smbus.c	\
        src/stm32f0xx_hal_spi.c		\
        src/stm32f0xx_hal_tim.c		\
        src/stm32f0xx_hal_tim_ex.c	\
        src/stm32f0xx_hal_tsc.c		\
        src/stm32f0xx_hal_uart.c	\
        src/stm32f0xx_hal_uart_ex.c	\
        src/stm32f0xx_hal_usart.c	\
        src/stm32f0xx_hal_wwdg.c
#----------------------------------------------------------------------------
OUT = libLib_STM32F0xx.a
#----------------------------------------------------------------------------
OBJ = $(SRC:.c=.o)
DEPS = $(SRC:.c=.d)
#----------------------------------------------------------------------------
.SUFFIXES: .c
#----------------------------------------------------------------------------
default: $(OUT)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	$(AR) rcs $(OUT) $(OBJ)
#----------------------------------------------------------------------------
clean:
	$(REMOVE) $(OBJ) $(OUT) $(DEPS) Makefile.bak 
#----------------------------------------------------------------------------
include $($(DEP_DIR) 2 > /dev/null) $(wildcard $(DEP_DIR)/*)
#----------------------------------------------------------------------------

