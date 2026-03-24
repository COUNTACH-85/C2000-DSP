################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-534635602: ../c2000.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs2041/ccs/utils/sysconfig_1.26.0/sysconfig_cli.bat" -s "C:/ti/C2000Ware_26_00_00_00/.metadata/sdk.json" -d "F2837xD" --script "C:/Users/Amitesh/workspace_ccstheia/PI_Motor_Control/c2000.syscfg" -o "syscfg" --compiler ccs
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/board.c: build-534635602 ../c2000.syscfg
syscfg/board.h: build-534635602
syscfg/board.cmd.genlibs: build-534635602
syscfg/board.opt: build-534635602
syscfg/board.json: build-534635602
syscfg/pinmux.csv: build-534635602
syscfg/c2000ware_libraries.cmd.genlibs: build-534635602
syscfg/c2000ware_libraries.opt: build-534635602
syscfg/c2000ware_libraries.c: build-534635602
syscfg/c2000ware_libraries.h: build-534635602
syscfg/clocktree.h: build-534635602
syscfg: build-534635602

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2041/ccs/tools/compiler/ti-cgt-c2000_22.6.3.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -Ooff --include_path="C:/Users/Amitesh/workspace_ccstheia/PI_Motor_Control" --include_path="C:/Users/Amitesh/workspace_ccstheia/PI_Motor_Control/device" --include_path="C:/ti/C2000Ware_26_00_00_00/driverlib/f2837xd/driverlib/" --include_path="C:/ti/ccs2041/ccs/tools/compiler/ti-cgt-c2000_22.6.3.LTS/include" --define=DEBUG --define=_LAUNCHXL_F28379D --define=CPU1 --float_operations_allowed=all --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="C:/Users/Amitesh/workspace_ccstheia/PI_Motor_Control/CPU1_RAM/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2041/ccs/tools/compiler/ti-cgt-c2000_22.6.3.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -Ooff --include_path="C:/Users/Amitesh/workspace_ccstheia/PI_Motor_Control" --include_path="C:/Users/Amitesh/workspace_ccstheia/PI_Motor_Control/device" --include_path="C:/ti/C2000Ware_26_00_00_00/driverlib/f2837xd/driverlib/" --include_path="C:/ti/ccs2041/ccs/tools/compiler/ti-cgt-c2000_22.6.3.LTS/include" --define=DEBUG --define=_LAUNCHXL_F28379D --define=CPU1 --float_operations_allowed=all --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/Amitesh/workspace_ccstheia/PI_Motor_Control/CPU1_RAM/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


