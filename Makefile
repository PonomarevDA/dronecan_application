# Copyright (c) 2023 Dmitry Ponomarev
# Distributed under the MIT License, available in the file LICENSE.
# Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

define build_example
	$(info Build example $(1)...)
	mkdir -p build/examples/$(1)
	cd build/examples/$(1) && cmake ../../../examples/$(1) && make -s
endef

ubuntu_minimal: clone_dependencies
	$(call build_example,ubuntu_minimal)
	./build/examples/ubuntu_minimal/application

clone_dependencies:
	mkdir -p build
	if [ ! -d "build/libparams" ]; then git clone https://github.com/PonomarevDA/libparams.git build/libparams; fi

clean:
	rm -rf build/examples/

distclean:
	rm -rf build/