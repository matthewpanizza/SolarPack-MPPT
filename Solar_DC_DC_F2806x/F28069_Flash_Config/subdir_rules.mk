################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
ADC_SOC_Cnf.obj: C:/ti/SolarPack-MPPT/HV_SOLAR_DC_DC/libs/app_libs/digital_power/f2803x_v3.3/C/ADC_SOC_Cnf.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=softlib --vcu_support=vcu0 --include_path="C:/ti/SolarPack-MPPT/Solar_DC_DC_F2806x" --include_path="../../HV_Solar_DC_DC/libs/math/IQmath/v160/include" --include_path="../../HV_Solar_DC_DC/libs/app_libs/solar/v1.1/IQ" --include_path="../../HV_Solar_DC_DC/libs/app_libs/digital_power/f2803x_v3.3/include" --include_path="../../HV_Solar_DC_DC/libs/app_libs/digital_power/f2803x_v3.3/asm" --include_path="../../HV_Solar_DC_DC/device_support/f2806x/v151/F2806x_common/include" --include_path="../../HV_Solar_DC_DC/device_support/f2806x/v151/F2806x_headers/include" --include_path="C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --include_path="../../HV_Solar_DC_DC/device_support/f2803x/v125/DSP2803x_common/include" --include_path="../../HV_Solar_DC_DC/device_support/f2803x/v125/DSP2803x_headers/include" --include_path="../../HV_Solar_DC_DC/development_kits/~SupportFiles/source" --advice:performance=all --define=_DEBUG --define=FLASH --define=LARGE_MODEL -g --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

DSP2803x_CodeStartBranch.obj: C:/ti/SolarPack-MPPT/HV_SOLAR_DC_DC/device_support/f2803x/v125/DSP2803x_common/source/DSP2803x_CodeStartBranch.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=softlib --vcu_support=vcu0 --include_path="C:/ti/SolarPack-MPPT/Solar_DC_DC_F2806x" --include_path="../../HV_Solar_DC_DC/libs/math/IQmath/v160/include" --include_path="../../HV_Solar_DC_DC/libs/app_libs/solar/v1.1/IQ" --include_path="../../HV_Solar_DC_DC/libs/app_libs/digital_power/f2803x_v3.3/include" --include_path="../../HV_Solar_DC_DC/libs/app_libs/digital_power/f2803x_v3.3/asm" --include_path="../../HV_Solar_DC_DC/device_support/f2806x/v151/F2806x_common/include" --include_path="../../HV_Solar_DC_DC/device_support/f2806x/v151/F2806x_headers/include" --include_path="C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --include_path="../../HV_Solar_DC_DC/device_support/f2803x/v125/DSP2803x_common/include" --include_path="../../HV_Solar_DC_DC/device_support/f2803x/v125/DSP2803x_headers/include" --include_path="../../HV_Solar_DC_DC/development_kits/~SupportFiles/source" --advice:performance=all --define=_DEBUG --define=FLASH --define=LARGE_MODEL -g --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

DSP2803x_usDelay.obj: C:/ti/SolarPack-MPPT/HV_SOLAR_DC_DC/device_support/f2803x/v125/DSP2803x_common/source/DSP2803x_usDelay.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=softlib --vcu_support=vcu0 --include_path="C:/ti/SolarPack-MPPT/Solar_DC_DC_F2806x" --include_path="../../HV_Solar_DC_DC/libs/math/IQmath/v160/include" --include_path="../../HV_Solar_DC_DC/libs/app_libs/solar/v1.1/IQ" --include_path="../../HV_Solar_DC_DC/libs/app_libs/digital_power/f2803x_v3.3/include" --include_path="../../HV_Solar_DC_DC/libs/app_libs/digital_power/f2803x_v3.3/asm" --include_path="../../HV_Solar_DC_DC/device_support/f2806x/v151/F2806x_common/include" --include_path="../../HV_Solar_DC_DC/device_support/f2806x/v151/F2806x_headers/include" --include_path="C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --include_path="../../HV_Solar_DC_DC/device_support/f2803x/v125/DSP2803x_common/include" --include_path="../../HV_Solar_DC_DC/device_support/f2803x/v125/DSP2803x_headers/include" --include_path="../../HV_Solar_DC_DC/development_kits/~SupportFiles/source" --advice:performance=all --define=_DEBUG --define=FLASH --define=LARGE_MODEL -g --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=softlib --vcu_support=vcu0 --include_path="C:/ti/SolarPack-MPPT/Solar_DC_DC_F2806x" --include_path="../../HV_Solar_DC_DC/libs/math/IQmath/v160/include" --include_path="../../HV_Solar_DC_DC/libs/app_libs/solar/v1.1/IQ" --include_path="../../HV_Solar_DC_DC/libs/app_libs/digital_power/f2803x_v3.3/include" --include_path="../../HV_Solar_DC_DC/libs/app_libs/digital_power/f2803x_v3.3/asm" --include_path="../../HV_Solar_DC_DC/device_support/f2806x/v151/F2806x_common/include" --include_path="../../HV_Solar_DC_DC/device_support/f2806x/v151/F2806x_headers/include" --include_path="C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --include_path="../../HV_Solar_DC_DC/device_support/f2803x/v125/DSP2803x_common/include" --include_path="../../HV_Solar_DC_DC/device_support/f2803x/v125/DSP2803x_headers/include" --include_path="../../HV_Solar_DC_DC/development_kits/~SupportFiles/source" --advice:performance=all --define=_DEBUG --define=FLASH --define=LARGE_MODEL -g --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

F2806x_GlobalVariableDefs.obj: C:/ti/SolarPack-MPPT/HV_SOLAR_DC_DC/device_support/f2806x/v151/F2806x_headers/source/F2806x_GlobalVariableDefs.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=softlib --vcu_support=vcu0 --include_path="C:/ti/SolarPack-MPPT/Solar_DC_DC_F2806x" --include_path="../../HV_Solar_DC_DC/libs/math/IQmath/v160/include" --include_path="../../HV_Solar_DC_DC/libs/app_libs/solar/v1.1/IQ" --include_path="../../HV_Solar_DC_DC/libs/app_libs/digital_power/f2803x_v3.3/include" --include_path="../../HV_Solar_DC_DC/libs/app_libs/digital_power/f2803x_v3.3/asm" --include_path="../../HV_Solar_DC_DC/device_support/f2806x/v151/F2806x_common/include" --include_path="../../HV_Solar_DC_DC/device_support/f2806x/v151/F2806x_headers/include" --include_path="C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --include_path="../../HV_Solar_DC_DC/device_support/f2803x/v125/DSP2803x_common/include" --include_path="../../HV_Solar_DC_DC/device_support/f2803x/v125/DSP2803x_headers/include" --include_path="../../HV_Solar_DC_DC/development_kits/~SupportFiles/source" --advice:performance=all --define=_DEBUG --define=FLASH --define=LARGE_MODEL -g --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=softlib --vcu_support=vcu0 --include_path="C:/ti/SolarPack-MPPT/Solar_DC_DC_F2806x" --include_path="../../HV_Solar_DC_DC/libs/math/IQmath/v160/include" --include_path="../../HV_Solar_DC_DC/libs/app_libs/solar/v1.1/IQ" --include_path="../../HV_Solar_DC_DC/libs/app_libs/digital_power/f2803x_v3.3/include" --include_path="../../HV_Solar_DC_DC/libs/app_libs/digital_power/f2803x_v3.3/asm" --include_path="../../HV_Solar_DC_DC/device_support/f2806x/v151/F2806x_common/include" --include_path="../../HV_Solar_DC_DC/device_support/f2806x/v151/F2806x_headers/include" --include_path="C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --include_path="../../HV_Solar_DC_DC/device_support/f2803x/v125/DSP2803x_common/include" --include_path="../../HV_Solar_DC_DC/device_support/f2803x/v125/DSP2803x_headers/include" --include_path="../../HV_Solar_DC_DC/development_kits/~SupportFiles/source" --advice:performance=all --define=_DEBUG --define=FLASH --define=LARGE_MODEL -g --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

HV_Solar_DC_DC-Main.obj: ../HV_Solar_DC_DC-Main.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=softlib --vcu_support=vcu0 --include_path="C:/ti/SolarPack-MPPT/Solar_DC_DC_F2806x" --include_path="../../HV_Solar_DC_DC/libs/math/IQmath/v160/include" --include_path="../../HV_Solar_DC_DC/libs/app_libs/solar/v1.1/IQ" --include_path="../../HV_Solar_DC_DC/libs/app_libs/digital_power/f2803x_v3.3/include" --include_path="../../HV_Solar_DC_DC/libs/app_libs/digital_power/f2803x_v3.3/asm" --include_path="../../HV_Solar_DC_DC/device_support/f2806x/v151/F2806x_common/include" --include_path="../../HV_Solar_DC_DC/device_support/f2806x/v151/F2806x_headers/include" --include_path="C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --include_path="../../HV_Solar_DC_DC/device_support/f2803x/v125/DSP2803x_common/include" --include_path="../../HV_Solar_DC_DC/device_support/f2803x/v125/DSP2803x_headers/include" --include_path="../../HV_Solar_DC_DC/development_kits/~SupportFiles/source" --advice:performance=all --define=FLASH --define=_DEBUG --define=LARGE_MODEL -g --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

SciCommsGui.obj: C:/ti/SolarPack-MPPT/HV_SOLAR_DC_DC/development_kits/~SupportFiles/source/SciCommsGui.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=softlib --vcu_support=vcu0 --include_path="C:/ti/SolarPack-MPPT/Solar_DC_DC_F2806x" --include_path="../../HV_Solar_DC_DC/libs/math/IQmath/v160/include" --include_path="../../HV_Solar_DC_DC/libs/app_libs/solar/v1.1/IQ" --include_path="../../HV_Solar_DC_DC/libs/app_libs/digital_power/f2803x_v3.3/include" --include_path="../../HV_Solar_DC_DC/libs/app_libs/digital_power/f2803x_v3.3/asm" --include_path="../../HV_Solar_DC_DC/device_support/f2806x/v151/F2806x_common/include" --include_path="../../HV_Solar_DC_DC/device_support/f2806x/v151/F2806x_headers/include" --include_path="C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --include_path="../../HV_Solar_DC_DC/device_support/f2803x/v125/DSP2803x_common/include" --include_path="../../HV_Solar_DC_DC/device_support/f2803x/v125/DSP2803x_headers/include" --include_path="../../HV_Solar_DC_DC/development_kits/~SupportFiles/source" --advice:performance=all --define=_DEBUG --define=FLASH --define=LARGE_MODEL -g --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


