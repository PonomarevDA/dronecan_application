# Copyright (c) 2023 Dmitry Ponomarev
# Distributed under the MIT License, available in the file LICENSE.
# Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD_DIR:=$(ROOT_DIR)/build
BUILD_UBUNTU_DIR:=$(BUILD_DIR)/ubuntu
LIBPARAMS_SRC_DIR:=${BUILD_DIR}/external/libparams

define build_and_run_sitl
	$(info Build example $(1)...)
	mkdir -p $(BUILD_UBUNTU_DIR)
	cd $(BUILD_UBUNTU_DIR) && cmake $(ROOT_DIR)/examples/$(1) && make -s
	$(BUILD_UBUNTU_DIR)/ubuntu_application
endef

ubuntu: clone_dependencies
	$(call build_and_run_sitl,ubuntu)

clone_dependencies:
	mkdir -p build
	if [ ! -d "${LIBPARAMS_SRC_DIR}" ]; then git clone --depth 1 --branch pr-refactor-cmake https://github.com/PonomarevDA/libparams.git ${LIBPARAMS_SRC_DIR}; fi

clean:
	rm -rf build/examples/

distclean:
	rm -rf build/
