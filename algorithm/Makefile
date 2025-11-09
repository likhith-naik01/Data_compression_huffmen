CC = gcc
CFLAGS = -Wall -Wextra -O2

all: compress decompress

compress: compress.c huffman.h
	$(CC) $(CFLAGS) -o compress.exe compress.c

decompress: decompress.c huffman.h
	$(CC) $(CFLAGS) -o decompress.exe decompress.c

clean:
	if exist "*.exe" del *.exe
	if exist "test_*.*" del test_*.*

test: compress decompress
	@echo Testing Huffman compression...
	@echo likhith likhith abcd abcd naik naik > test_input.txt
	@compress.exe test_input.txt test_compressed.bin
	@decompress.exe test_compressed.bin test_output.txt
	@fc /b test_input.txt test_output.txt > nul && echo Test passed! || echo Test failed!

test: compress decompress
	@echo "Testing compression..."
	@echo "likhith likhith abcd abcd naik naik" > test_input.txt
	./compress test_input.txt test_compressed.bin
	./decompress test_compressed.bin test_output.txt
	@echo "Input file contents:"
	@cat test_input.txt
	@echo "Output file contents:"
	@cat test_output.txt
	@diff test_input.txt test_output.txt && echo "✅ Test passed!" || echo "❌ Test failed!"
	rm -f test_input.txt test_compressed.bin test_output.txt

.PHONY: all clean test