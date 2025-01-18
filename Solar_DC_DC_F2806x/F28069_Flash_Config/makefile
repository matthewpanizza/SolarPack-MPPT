################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./ADC_SOC_Cnf.obj" \
"./DSP2803x_CodeStartBranch.obj" \
"./DSP2803x_usDelay.obj" \
"./F2806x_ECan.obj" \
"./F2806x_GlobalVariableDefs.obj" \
"./HV_Solar_DC_DC-DPL-ISR.obj" \
"./HV_Solar_DC_DC-Main.obj" \
"./HV_Solar_DC_DC_DevInit_F2803x.obj" \
"./PWM_1ch_UpDwnCnt_Cnf.obj" \
"./PWM_ComplPairDB_Cnf.obj" \
"./SciCommsGui.obj" \
"./SolarHv_DCDC-Lin.obj" \
"../F28069.cmd" \
"../F2806x_Headers_nonBIOS.cmd" \
"C:/ti/HV_SOLAR_DC_DC/libs/math/IQmath/v160/lib/IQmath.lib" \
$(GEN_CMDS__FLAG) \

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
HV_Solar_DC_DC.out 

EXE_OUTPUTS__QUOTED += \
"HV_Solar_DC_DC.out" 


# All Target
all: HV_Solar_DC_DC.out

# Tool invocations
HV_Solar_DC_DC.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C2000 Linker'
	"C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=softlib --vcu_support=vcu0 --advice:performance=all --define=_DEBUG --define=FLASH --define=LARGE_MODEL -g --diag_warning=225 --abi=coffabi -z -m"HV_Solar_DC_DC.map" --stack_size=0x380 --warn_sections -i"C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/lib" -i"C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --reread_libs --xml_link_info="HV_Solar_DC_DC_linkInfo.xml" --rom_model -o "HV_Solar_DC_DC.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)
	-$(RM) "ADC_SOC_Cnf.obj" "DSP2803x_CodeStartBranch.obj" "DSP2803x_usDelay.obj" "F2806x_ECan.obj" "F2806x_GlobalVariableDefs.obj" "HV_Solar_DC_DC-DPL-ISR.obj" "HV_Solar_DC_DC-Main.obj" "HV_Solar_DC_DC_DevInit_F2803x.obj" "PWM_1ch_UpDwnCnt_Cnf.obj" "PWM_ComplPairDB_Cnf.obj" "SciCommsGui.obj" "SolarHv_DCDC-Lin.obj" 
	-$(RM) "ADC_SOC_Cnf.d" "F2806x_ECan.d" "F2806x_GlobalVariableDefs.d" "HV_Solar_DC_DC-Main.d" "HV_Solar_DC_DC_DevInit_F2803x.d" "PWM_1ch_UpDwnCnt_Cnf.d" "PWM_ComplPairDB_Cnf.d" "SciCommsGui.d" "SolarHv_DCDC-Lin.d" 
	-$(RM) "DSP2803x_CodeStartBranch.d" "DSP2803x_usDelay.d" "HV_Solar_DC_DC-DPL-ISR.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

