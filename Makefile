FLAGS=-g -O2 -Wall -Wextra -Werror --pedantic
SAN_FLAGS=-fsanitize=address -fno-omit-frame-pointer
C_FLAGS=-std=c11
CPP_FLAGS=-std=c++11

BUILD_DIR=build

.PHONY: test_uniq_counter release

build_dir:
	mkdir -p ${BUILD_DIR}

test_uniq_counter: build_dir
	clang++ ${FLAGS} ${CPP_FLAGS} ${SAN_FLAGS} -DTEST uniq_counter.cc \
		test_uniq_counter.cc -o ${BUILD_DIR}/test_uniq_counter
	./${BUILD_DIR}/test_uniq_counter

tinyprof: tinyprof.c tinyelf.c tinyelf.h uniq_counter.c uniq_counter.h
	clang ${C_FLAGS} ${FLAGS} ${SAN_FLAGS} tinyprof.c tinyelf.c uniq_counter.c \
		-o ${BUILD_DIR}/tinyprof

time_consumer: time_consumer.c
	clang ${C_FLAGS} ${FLAGS} time_consumer.c -o ${BUILD_DIR}/time_consumer

release:
	clang ${C_FLAGS} -O2 tinyprof.c tinyelf.c uniq_counter.c \
		-o ${BUILD_DIR}/tinyprof

format:
	clang-format -i *.c *.cc *.h

clean:
	rm -rf ${BUILD_DIR}/*
