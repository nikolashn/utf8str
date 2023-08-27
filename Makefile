TEST_MAX_CAP = "536870912"
test:
	cc tests/munit/munit.c tests/*.c src/*.c -o tests/tests -g -std=gnu89 -O0 -Wall -Wshadow
	tests/tests --param maxCap $(TEST_MAX_CAP)

.PHONY: test

