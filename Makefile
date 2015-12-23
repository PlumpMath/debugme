test: build/libdebugme.a test.swift
	swiftc -g test.swift -I SwiftLib/Debugme -ldebugme -L./build/

build/libdebugme.a:
	mkdir -p build
	gcc -c -g -o build/debugme.o debugme.c
	ar rcs build/libdebugme.a build/debugme.o

clean:
	rm -rf build test
