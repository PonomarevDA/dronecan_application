# Copyright (c) 2023 Dmitry Ponomarev
# Distributed under the MIT License, available in the file LICENSE.
# Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

define build_and_run_sitl
	$(info Build example $(1)...)
	mkdir -p build/examples/$(1)
	cd build/examples/$(1) && cmake ../../../examples/$(1) && make -s
	./build/examples/$(1)/application
endef

ubuntu_minimal: clone_dependencies
	$(call build_and_run_sitl,ubuntu_minimal)

ubuntu_publisher_circuit_status: clone_dependencies
	$(call build_and_run_sitl,ubuntu_publisher_circuit_status)

clone_dependencies:
	mkdir -p build
	if [ ! -d "build/libparams" ]; then git clone https://github.com/PonomarevDA/libparams.git build/libparams; fi

clean:
	rm -rf build/examples/

distclean:
	rm -rf build/
