# Copyright (c) 2023 Dmitry Ponomarev
# Distributed under the MIT License, available in the file LICENSE.
# Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD_DIR:=$(ROOT_DIR)/build
BUILD_EXAMPLES_DIR:=$(BUILD_DIR)/src/examples
LIBPARAMS_DIR:=${BUILD_DIR}/external/libparams

define build_and_run_sitl
	$(info Build example $(1)...)
	mkdir -p $(BUILD_EXAMPLES_DIR)/$(1)
	cd $(BUILD_EXAMPLES_DIR)/$(1) && cmake $(ROOT_DIR)/examples/$(1) && make -s
	$(BUILD_EXAMPLES_DIR)/$(1)/application
endef

ubuntu: clone_dependencies
	$(call build_and_run_sitl,ubuntu)

clone_dependencies:
	mkdir -p build
	if [ ! -d "${LIBPARAMS_DIR}" ]; then git clone --depth 1 --branch v0.8.4 https://github.com/PonomarevDA/libparams.git ${LIBPARAMS_DIR}; fi

clean:
	rm -rf build/examples/

code_style: cpplint cppcheck crlf
astyle:
	./scripts/code_style/check_astyle.py src include --astylerc scripts/code_style/astylerc
cpplint:
	cpplint src/*.cpp \
			include/dcnode/*.h \
			include/dcnode/serialization/*.h \
			include/dcnode/serialization/*/*/*.h \
			include/dcnode/serialization/*/*/*/*.h
cppcheck: clone_dependencies
	cppcheck --enable=all \
			 --inconclusive \
			 --std=c++17 \
			 --addon=misra \
			 --suppress=constParameterCallback \
			 --suppress=constParameterPointer \
			 --suppress=misra-c2012-12.3 \
			 --check-level=exhaustive \
			 --error-exitcode=1 \
			 --suppress=missingIncludeSystem \
			 --suppress=unusedFunction \
			 -I build/external/libparams/libparams \
			 -I libs \
			 -I include \
			 -I include/dcnode/serialization \
			 -I platform_specific/socketcan/ \
			 -Dnullptr=0 \
			 src/
crlf:
	./scripts/code_style/check_crlf.sh
distclean:
	rm -rf build/
