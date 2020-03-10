all:
	mkdir -p build
	cd build; cmake ..

clean:
	rm -rf build build-1 build-old cmake-builld*
	rm foo-output*
