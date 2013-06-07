CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
AS=arm-none-eabi-as
OBJCOPY=arm-none-eabi-objcopy

# Put your STM32F4 library code directory here
STM_COMMON=./STM32F4-Discovery_FW_V1.1.0


CFLAGS  = -g -O2 -Wall -Tstm32_flash.ld 
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=soft 
#CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -I. -Iarch/arm/include
# Include files from STM libraries
CFLAGS += -I$(STM_COMMON)/Utilities/STM32F4-Discovery
CFLAGS += -I$(STM_COMMON)/Libraries/CMSIS/Include -I$(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Include
CFLAGS += -I$(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/inc



AFLAGS=
LDFLAGS=

OBJ := kern/main.o kern/sched.o kern/sysLib.o kern/taskLib.o arch/arm/boot_stm32f4xx.o arch/arm/system_stm32f4xx.o


.PHONY: kernel clean

kernel: kernel.elf


kernel.elf: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ 
	$(OBJCOPY) -O ihex $@ $(@:.elf=.hex)
	$(OBJCOPY) -O binary $@ $(@:.elf=.bin)

#%.o: %.c
#	$(CC) $(CFLAGS) -o $@ -c $<

#%.o: %.S
#	$(AS) $(AFLAGS) -o $@ -c $<	

burn: kernel.bin
	sudo `which st-flash` write $< 0x8000000


clean:
	rm -f $(OBJ) kernel.elf kernel.hex kernel.bin
