################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./DSP2803x_CodeStartBranch.obj" \
"./DSP2803x_DefaultIsr.obj" \
"./DSP2803x_ECan.obj" \
"./DSP2803x_GlobalVariableDefs.obj" \
"./DSP2803x_PieCtrl.obj" \
"./DSP2803x_PieVect.obj" \
"./DSP2803x_SysCtrl.obj" \
"./DSP2803x_usDelay.obj" \
"./Example_2803xECanBack2Back.obj" \
"C:/ti/HV_SOLAR_DC_DC/device_support/f2803x/v125/DSP2803x_common/cmd/28035_RAM_lnk.cmd" \
"C:/ti/HV_SOLAR_DC_DC/device_support/f2803x/v125/DSP2803x_headers/cmd/DSP2803x_Headers_nonBIOS.cmd" \
$(GEN_CMDS__FLAG) \
-lrts2800_ml.lib \
-lIQmath.lib \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
Example_2803xECanBack2Back.out \

EXE_OUTPUTS__QUOTED += \
"Example_2803xECanBack2Back.out" \

BIN_OUTPUTS += \
Example_2803xECanBack2Back.hex \

BIN_OUTPUTS__QUOTED += \
"Example_2803xECanBack2Back.hex" \


# All Target
all: Example_2803xECanBack2Back.out

# Tool invocations
Example_2803xECanBack2Back.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C2000 Linker'
	"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --define=_DEBUG --define=LARGE_MODEL -g --diag_warning=225 --issue_remarks --verbose_diagnostics --quiet --abi=coffabi --output_all_syms -z -m"Example_2803xECanBack2Back.map" --stack_size=0x380 --warn_sections -i"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/lib" -i"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/include" -i"C:/ti/HV_SOLAR_DC_DC/device_support/f2803x/v125/DSP2803x_examples_ccsv4/ecan_back2back" -i"C:/ti/HV_SOLAR_DC_DC/device_support/f2803x/v125/DSP2803x_common/lib" -i"C:/ti/HV_SOLAR_DC_DC/libs/math/IQmath/v15c/lib" --reread_libs --issue_remarks --verbose_diagnostics --xml_link_info="Example_2803xECanBack2Back_linkInfo.xml" --entry_point=code_start --rom_model -o "Example_2803xECanBack2Back.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

Example_2803xECanBack2Back.hex: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: C2000 Hex Utility'
	"C:/ti/ccs1040/ccs/tools/compiler/ti-cgt-c2000_20.2.5.LTS/bin/hex2000" --diag_wrap=off -o "Example_2803xECanBack2Back.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building secondary target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "DSP2803x_CodeStartBranch.obj" "DSP2803x_DefaultIsr.obj" "DSP2803x_ECan.obj" "DSP2803x_GlobalVariableDefs.obj" "DSP2803x_PieCtrl.obj" "DSP2803x_PieVect.obj" "DSP2803x_SysCtrl.obj" "DSP2803x_usDelay.obj" "Example_2803xECanBack2Back.obj" 
	-$(RM) "DSP2803x_DefaultIsr.d" "DSP2803x_ECan.d" "DSP2803x_GlobalVariableDefs.d" "DSP2803x_PieCtrl.d" "DSP2803x_PieVect.d" "DSP2803x_SysCtrl.d" "Example_2803xECanBack2Back.d" 
	-$(RM) "DSP2803x_CodeStartBranch.d" "DSP2803x_usDelay.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

