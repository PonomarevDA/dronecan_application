# Copyright (c) 2023 Dmitry Ponomarev
# Distributed under the MIT License, available in the file LICENSE.
# Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD_DIR:=$(ROOT_DIR)/build/examples

define build_and_run_sitl
	$(info Build example $(1)...)
	mkdir -p $(BUILD_DIR)/$(1)
	cd $(BUILD_DIR)/$(1) && cmake $(ROOT_DIR)/examples/$(1) && make -s
	$(BUILD_DIR)/$(1)/application
endef

ubuntu: clone_dependencies
	$(call build_and_run_sitl,ubuntu)

ubuntu_minimal: clone_dependencies
	$(call build_and_run_sitl,ubuntu/minimal)

ubuntu_publisher_circuit_status: clone_dependencies
	$(call build_and_run_sitl,ubuntu/publisher/circuit_status)

clone_dependencies:
	mkdir -p build
	if [ ! -d "build/libparams" ]; then git clone --depth 1 --branch v0.8.4 https://github.com/PonomarevDA/libparams.git build/libparams; fi

clean:
	rm -rf build/examples/

distclean:
	rm -rf build/
