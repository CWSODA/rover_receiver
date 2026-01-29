# release commands
cmake-r:
	cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release

clean-r:
	rm -rf build-release/

build-r:
	cmake --build build-release

run-r:
	build-release/rover_receiver

# debug commands (default)
cmake:
	cmake -S . -B build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug

build-debug:
	make cmake

build: build-debug
	cmake --build build-debug

clean:
	rm -rf build-debug/

run:
	build-debug/rover_receiver

br:
	make build
	make run