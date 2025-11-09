# Huffman Text Compressor

A simple and efficient text compression system using Huffman coding algorithm.

## Building the Program

To compile the program, use:
```bash
gcc huffman.c -o huffman.exe
```

## Usage Instructions

### Compression
To compress a file:
```bash
./huffman.exe -c <input_file> <compressed_file.bin>
```

For example:
```bash
# Create a test file
echo "This is a test message" > input.txt

# Compress it
./huffman.exe -c input.txt compressed.bin
```

### Decompression
To decompress a file:
```bash
./huffman.exe -d <compressed_file.bin> <output_file>
```

For example:
```bash
./huffman.exe -d compressed.bin decompressed.txt
```

### Quick Test
Here's a complete example to test both compression and decompression:
```bash
# Create a test file
echo "Hello World! This is a test message" > test.txt

# Compress the file
./huffman.exe -c test.txt test.bin

# Decompress the file
./huffman.exe -d test.bin output.txt

# View results
Get-Content test.txt
Get-Content output.txt
```

## Project Files
- `huffman.c` - Main implementation containing compression/decompression logic
- `huffman.h` - Header file with type definitions and function declarations
- `huffman.exe` - Compiled executable program