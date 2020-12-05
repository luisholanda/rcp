build:
	@cd build && cmake .. && make

clean:
	rm build

test: build
	@./build/tests/rcp_test

.PHONY: build clean

