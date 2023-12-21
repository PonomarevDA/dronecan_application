# Copyright (c) 2023 Dmitry Ponomarev
# Distributed under the MIT License, available in the file LICENSE.
# Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

define build_example
	$(info Build example $(1)...)
	mkdir -p build/examples/$(1)
	cd build/examples/$(1) && cmake ../../../examples/$(1) && make -s
endef

ubuntu_minimal:
	$(call build_example,ubuntu_minimal)
	./build/examples/ubuntu_minimal/application

generage_dsdl:
	if [ ! -d "build/nunavut_out" ]; then echo "not exist"; fi
	mkdir -p build/nunavut_out
	./scripts/nnvg_generate_c_headers.sh build/nunavut_out

clean:
	rm -rf build/examples/

distclean:
	rm -rf build/
