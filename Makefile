
# Makefile to make using cmake easier

CMAKE ?= cmake

# If ninja is installed, use it. Otherwise, use make.
ifneq (,$(shell which ninja))
  CMAKE += -G Ninja
  BUILD_TOOL ?= ninja
else
  BUILD_TOOL ?= make
endif

all: build/kms_image

build: CMakeLists.txt
	mkdir -p build	
	cd build && $(CMAKE) ..

build/kms_image: build
	cd build && $(BUILD_TOOL)

clean:
	rm -rf build

install:
	cd build && $(BUILD_TOOL) install

.PHONY: all clean install
