#----------------------------------------------------------------------------
# STM32F042 * TEMP-SENSOR-TESTER
#----------------------------------------------------------------------------
CPU = cortex-m0
#----------------------------------------------------------------------------
# Directories
#
MAIN_DIR  = ./main
TOOL_DIR  = ./tools
USB_DIR   = ./usb
LIB_DIR   = ../lib
SYS_DIR   = ../
OUTDIR    = .obj
DEPDIR    = $(OUTDIR)/.dep
#----------------------------------------------------------------------------
# Filenames
#
LINKERFILE = bios.ld
TARGET     = _bios
#----------------------------------------------------------------------------
# Output format. (can be ihex or binary or both)
#
FORMAT  = ihex
#FORMAT = binary
#FORMAT = both
#----------------------------------------------------------------------------
# List C source files here
#
SRC  =  \
        $(MAIN_DIR)/cr_startup_stm32f042.c  \
        $(LIB_DIR)/system_stm32f0xx.c       \
\
        $(TOOL_DIR)/tools.c                 \
        $(TOOL_DIR)/timer.c                 \
        $(TOOL_DIR)/erdebug.c               \
        $(TOOL_DIR)/ds18x20.c               \
        $(TOOL_DIR)/onewire.c               \
\
        $(USB_DIR)/usb_main.c               \
        $(USB_DIR)/usb_core.c               \
        $(USB_DIR)/usb_ll.c                 \
        $(USB_DIR)/usb_desc.c               \
        $(USB_DIR)/usb_hid.c                \
\
        $(MAIN_DIR)/main.c
#----------------------------------------------------------------------------
# List C source files here which must be compiled in ARM-Mode (no -mthumb)
#
SRCARM =
#----------------------------------------------------------------------------
# List C++ source files here.
#
CPPSRC = 
#----------------------------------------------------------------------------
# List C++ source files here which must be compiled in ARM-Mode.
#
CPPSRCARM = 
#----------------------------------------------------------------------------
# List Assembler source files here.
#
ASRC =
#----------------------------------------------------------------------------
# List Assembler source files here which must be assembled in ARM-Mode
#
ASRCARM = 
#----------------------------------------------------------------------------
# List any extra directories to look for include files here.
#
EXTRAINCDIRS =  $(MAIN_DIR)     \
								$(TOOL_DIR)     \
								$(USB_DIR)      \
								$(LIB_DIR)      \
								$(LIB_DIR)/inc  \
								$(SYS_DIR)
#----------------------------------------------------------------------------
# List any extra directories to look for library files here.
#
EXTRA_LIBDIRS = $(LIB_DIR)
#----------------------------------------------------------------------------
# Extra libraries
#
EXTRA_LIBS =
#----------------------------------------------------------------------------
# Place project-specific -D (define)
#
CDEFS = -DERDEBUG
#----------------------------------------------------------------------------
# Place project-specific -D and/or -U options for 
#
ADEFS =
#----------------------------------------------------------------------------
# Compiler flag to set the C Standard level.
#
CSTANDARD = -std=gnu11
#----------------------------------------------------------------------------
CDEFS += -DSTM32F042 -D__USE_CMSIS -DUSE_STDPERIPH_DRIVER
#----------------------------------------------------------------------------
CFLAGS =  -mcpu=$(CPU) -mthumb
CFLAGS += $(CDEFS)
CFLAGS += -O1 -g3
CFLAGS += -ffunction-sections -fno-strict-aliasing -fno-builtin
CFLAGS += -Wall 
CFLAGS += -I. $(patsubst %,-I%,$(EXTRAINCDIRS))
CFLAGS += -Wa,-adhlns=$(addprefix $(OUTDIR)/, $(notdir $(addsuffix .lst, $(basename $<))))
CFLAGS += -MD -MP -MF $(DEPDIR)/$(@F).d
#----------------------------------------------------------------------------
# flags only for C
#
CONLYFLAGS += $(CSTANDARD)
#----------------------------------------------------------------------------
# flags only for C++ (arm-elf-g++)
#
CPPFLAGS = 
#----------------------------------------------------------------------------
# Assembler flags.
#  -Wa,...:    tell GCC to pass this to the assembler.
#  -ahlns:     create listing
#  -g$(DEBUG): have the assembler create line number information
#
ASFLAGS  = -mcpu=$(CPU) -mthumb -I. -x assembler-with-cpp
ASFLAGS += $(ADEFS)
ASFLAGS += -Wa,-adhlns=$(addprefix $(OUTDIR)/, $(notdir $(addsuffix .lst, $(basename $<))))
ASFLAGS += -Wa,-g$(DEBUG)
ASFLAGS += -I. $(patsubst %,-I%,$(EXTRAINCDIRS))
#----------------------------------------------------------------------------
# Linker flags.
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
#
LDFLAGS  = -mcpu=$(CPU) -mthumb
LDFLAGS += -nostartfiles -Wl,-Map=$(OUTDIR)/$(TARGET).map,--cref,--gc-sections
LDFLAGS += $(patsubst %,-L%,$(EXTRA_LIBDIRS))
LDFLAGS += $(patsubst %,-l%,$(EXTRA_LIBS))
#LDFLAGS += -lc -lgcc 
#----------------------------------------------------------------------------
# Set linker-script name 
#
LDFLAGS +=-T$(LINKERFILE)
# ---------------------------------------------------------------------------
# Define programs and commands.
#
SHELL = cmd
CC = arm-none-eabi-gcc
CPP = arm-none-eabi-g++
AR = arm-none-eabi-ar
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size
NM = arm-none-eabi-nm
REMOVE = rm -f
REMOVEDIR = rm -r
COPY = cp
# ---------------------------------------------------------------------------
# Define Messages
# 
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before: 
MSG_SIZE_AFTER = Size after:
MSG_LOAD_FILE = Creating load file:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling C:
MSG_COMPILING_ARM = "Compiling C (ARM-only):"
MSG_COMPILINGCPP = Compiling C++:
MSG_COMPILINGCPP_ARM = "Compiling C++ (ARM-only):"
MSG_ASSEMBLING = Assembling:
MSG_ASSEMBLING_ARM = "Assembling (ARM-only):"
MSG_CLEANING = Cleaning project:
MSG_FORMATERROR = Can not handle output-format
MSG_ASMFROMC = "Creating asm-File from C-Source:"
MSG_ASMFROMC_ARM = "Creating asm-File from C-Source (ARM-only):"
# ---------------------------------------------------------------------------
# List of all source files.
#
ALLSRC     = $(ASRCARM) $(ASRC) $(SRCARM) $(SRC) $(CPPSRCARM) $(CPPSRC)
# ---------------------------------------------------------------------------
# List of all source files without directory and file-extension.
#
ALLSRCBASE = $(notdir $(basename $(ALLSRC)))
# ---------------------------------------------------------------------------
# Define all object files.
#
ALLOBJ     = $(addprefix $(OUTDIR)/, $(addsuffix .o, $(ALLSRCBASE)))
# ---------------------------------------------------------------------------
# Define all listing files (used for make clean).
#
LSTFILES   = $(addprefix $(OUTDIR)/, $(addsuffix .lst, $(ALLSRCBASE)))
# ---------------------------------------------------------------------------
# Define all depedency-files (used for make clean).
#
DEPFILES   = $(addprefix $(DEPDIR)/, $(addsuffix .o.d, $(ALLSRCBASE)))
# ---------------------------------------------------------------------------
# Default target.
all: begin build sizeafter finished end
# ---------------------------------------------------------------------------
elf: $(OUTDIR)/$(TARGET).elf
lss: $(OUTDIR)/$(TARGET).lss 
sym: $(OUTDIR)/$(TARGET).sym
hex: $(OUTDIR)/$(TARGET).hex
bin: $(OUTDIR)/$(TARGET).bin
# ---------------------------------------------------------------------------
ifeq ($(FORMAT),ihex)
build: elf hex lss sym
else 
ifeq ($(FORMAT),binary)
build: elf bin lss sym
else 
ifeq ($(FORMAT),both)
build: elf hex bin lss sym
else 
$(error "$(MSG_FORMATERROR) $(FORMAT)")
endif
endif
endif
# ---------------------------------------------------------------------------
# Eye candy.
begin:
	@echo""
	@echo $(MSG_BEGIN)
	@gmkdir -p $(OUTDIR)
	@gmkdir -p $(DEPDIR)

finished:
	@echo $(MSG_ERRORS_NONE)

end:
	@echo $(MSG_END)
	@echo""
# ---------------------------------------------------------------------------
sizeafter:
	@echo""
	@echo $(MSG_SIZE_AFTER)
	@$(SIZE) -A $(OUTDIR)/$(TARGET).elf
# ---------------------------------------------------------------------------
# Display compiler version information.
gccversion : 
	@$(CC) --version
# ---------------------------------------------------------------------------
# Create final output file (.hex) from ELF output file.
%.hex: %.elf
	@echo""
	@echo $(MSG_LOAD_FILE) $@
	$(OBJCOPY) -O ihex $< $@
# ---------------------------------------------------------------------------
# Create final output file (.bin) from ELF output file.
%.bin: %.elf
	@echo""
	@echo $(MSG_LOAD_FILE) $@
	$(OBJCOPY) -O binary $< $@
# ---------------------------------------------------------------------------
# Create extended listing file from ELF output file.
# testing: option -C
%.lss: %.elf
	@echo""
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -d $< > $@
# ---------------------------------------------------------------------------
# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo""
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) -n $< > $@
# ---------------------------------------------------------------------------
# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(ALLOBJ)
%.elf:  $(ALLOBJ)
	@echo""
	@echo $(MSG_LINKING) $@
	$(CC) $(THUMB) $(CFLAGS) $(ALLOBJ) --output $@ $(LDFLAGS)
# ---------------------------------------------------------------------------
# Assemble: create object files from assembler source files.
define ASSEMBLE_TEMPLATE
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
	@echo""
	@echo $(MSG_ASSEMBLING) $$< == $$@
	$(CC) -c $(THUMB) $$(ASFLAGS) $$< -o $$@ 
endef
$(foreach src, $(ASRC), $(eval $(call ASSEMBLE_TEMPLATE, $(src)))) 
# ---------------------------------------------------------------------------
# Assemble: create object files from assembler source files. ARM-only
define ASSEMBLE_ARM_TEMPLATE
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
	@echo""
	@echo $(MSG_ASSEMBLING_ARM) $$< == $$@
	$(CC) -c $$(ASFLAGS) $$< -o $$@ 
endef
$(foreach src, $(ASRCARM), $(eval $(call ASSEMBLE_ARM_TEMPLATE, $(src)))) 
# ---------------------------------------------------------------------------
# Compile: create object files from C source files.
define COMPILE_C_TEMPLATE
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
	@echo""
	@echo $(MSG_COMPILING) $$< == $$@
	$(CC) -c $(THUMB) $$(CFLAGS) $$(CONLYFLAGS) $$< -o $$@ 
endef
$(foreach src, $(SRC), $(eval $(call COMPILE_C_TEMPLATE, $(src)))) 
# ---------------------------------------------------------------------------
# Compile: create object files from C source files. ARM-only
define COMPILE_C_ARM_TEMPLATE
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
	@echo""
	@echo $(MSG_COMPILING_ARM) $$< == $$@
	$(CC) -c $$(CFLAGS) $$(CONLYFLAGS) $$< -o $$@ 
endef
$(foreach src, $(SRCARM), $(eval $(call COMPILE_C_ARM_TEMPLATE, $(src)))) 
# ---------------------------------------------------------------------------
# Compile: create object files from C++ source files.
define COMPILE_CPP_TEMPLATE
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
	@echo""
	@echo $(MSG_COMPILINGCPP) $$< == $$@
	$(CC) -c $(THUMB) $$(CFLAGS) $$(CPPFLAGS) $$< -o $$@ 
endef
$(foreach src, $(CPPSRC), $(eval $(call COMPILE_CPP_TEMPLATE, $(src)))) 
# ---------------------------------------------------------------------------
# Compile: create object files from C++ source files. ARM-only
define COMPILE_CPP_ARM_TEMPLATE
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
	@echo""
	@echo $(MSG_COMPILINGCPP_ARM) $$< == $$@
	$(CC) -c $$(CFLAGS) $$(CPPFLAGS) $$< -o $$@ 
endef
$(foreach src, $(CPPSRCARM), $(eval $(call COMPILE_CPP_ARM_TEMPLATE, $(src)))) 
# ---------------------------------------------------------------------------
# Compile: create assembler files from C source files. ARM/Thumb
$(SRC:.c=.s) : %.s : %.c
	@echo $(MSG_ASMFROMC) $< == $@
	$(CC) $(THUMB) -S $(CFLAGS) $(CONLYFLAGS) $< -o $@
# ---------------------------------------------------------------------------
# Compile: create assembler files from C source files. ARM only
$(SRCARM:.c=.s) : %.s : %.c
	@echo $(MSG_ASMFROMC_ARM) $< == $@
	$(CC) -S $(CFLAGS) $(CONLYFLAGS) $< -o $@
# ---------------------------------------------------------------------------
# Target: clean project.
clean: begin clean_list finished end

clean_list :
	@echo""
	@echo $(MSG_CLEANING)
	$(REMOVE) $(OUTDIR)/$(TARGET).map
	$(REMOVE) $(OUTDIR)/$(TARGET).elf
	$(REMOVE) $(OUTDIR)/$(TARGET).hex
	$(REMOVE) $(OUTDIR)/$(TARGET).bin
	$(REMOVE) $(OUTDIR)/$(TARGET).sym
	$(REMOVE) $(OUTDIR)/$(TARGET).lss
	$(REMOVE) $(ALLOBJ)
	$(REMOVE) $(LSTFILES)
	$(REMOVE) $(DEPFILES)
	$(REMOVE) $(SRC:.c=.s)
	$(REMOVE) $(SRCARM:.c=.s)
	$(REMOVE) $(CPPSRC:.cpp=.s) 
	$(REMOVE) $(CPPSRCARM:.cpp=.s) 
# ---------------------------------------------------------------------------
# Include the dependency files.
-include $($(DEPDIR) 2 > /dev/null) $(wildcard $(DEPDIR)/*)
# ---------------------------------------------------------------------------
# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter gccversion \
         build elf hex bin lss sym clean clean_list program
# ---------------------------------------------------------------------------
