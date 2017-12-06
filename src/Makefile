CMAKE=cmake

include Makefile.common

OS := $(shell uname)
ifeq ($(OS),Darwin)
CMAKE_ARGS += -DCMAKE_PREFIX_PATH=$(shell brew --prefix qt)
endif
