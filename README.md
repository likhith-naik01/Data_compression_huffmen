# Huffman Text Compression System

An efficient implementation of text compression using Huffman coding algorithm with a web-based interface. This project combines the power of C for compression algorithms with a modern web interface using Flask and JavaScript.

## Project Overview

The project consists of three main components:
1. **Core Algorithm (C)**: Efficient Huffman coding implementation
2. **Backend Server (Python/Flask)**: RESTful API for handling compression requests
3. **Frontend Interface (HTML/JS)**: User-friendly web interface

## Time and Space Complexity

### Huffman Algorithm Complexity
- **Time Complexity**: 
  - Building Huffman Tree: O(n log n) where n is the number of unique characters
  - Encoding: O(N) where N is the length of the text
  - Decoding: O(N)
- **Space Complexity**:
  - Huffman Tree: O(k) where k is the number of unique characters
  - Encoding Table: O(k)
  - Compressed Data: O(N) in worst case

## Features

- Lossless text compression
- Real-time compression statistics
- Support for large files (up to 10MB)
- Progress tracking during compression/decompression
- Clean and intuitive web interface
- Efficient C implementation of core algorithms
- Cross-platform compatibility

## Makefile Explanation

The project includes a Makefile that automates the build process for the C components. Here's what each part does:

### Makefile Components
```makefile
CC = gcc                    # Specifies the C compiler to use
CFLAGS = -Wall -Wextra -O2 # Compiler flags for warnings and optimization
```

### Makefile Targets
- `make all`: Builds both compression and decompression executables
- `make clean`: Removes compiled executables and test files
- `make test`: Runs automated tests for compression/decompression

### Compiler Flags Explained
- `-Wall`: Enable all common warning messages
- `-Wextra`: Enable additional warning messages
- `-O2`: Level 2 optimization for better performance

### Building Process
The Makefile handles:
1. Compiling C source files with appropriate flags
2. Dependency management for header files
3. Automated testing of compression/decompression
4. Cleanup of temporary and compiled files

## Setup and Running

1. **Build the C Program**:
```bash
cd algorithm
make         # Builds the executables
make test    # Optional: Run tests
```

2. **Setup Python Backend**:
```bash
cd backend
pip install -r requirements.txt
python server.py
```

3. **Start Frontend**:
```bash
cd frontend
python -m http.server 8000
```

4. Open your browser and visit: http://localhost:8000

## Implementation Details

### Compression Process
1. Text file is uploaded through web interface
2. Backend processes file and builds Huffman tree
3. Data is compressed using variable-length codes
4. Compressed binary file is generated
5. Statistics are calculated and displayed

### Decompression Process
1. Compressed .bin file is uploaded
2. Huffman tree is reconstructed from header data
3. Binary data is decoded using the tree
4. Original text is recovered
5. File is available for download

## API Endpoints

- `POST /api/compress`: Compress uploaded text file
- `POST /api/decompress`: Decompress uploaded binary file
- `GET /api/download/<filename>`: Download processed files
- `GET /`: Health check endpoint

## Performance

- Average compression ratio: 40-60% for text files
- Processing speed: ~1MB/second (depending on hardware)
- Minimal memory footprint due to C implementation
- Efficient handling of repeated patterns

## Technical Requirements

- C Compiler (gcc recommended)
- Python 3.7+
- Modern web browser
- Required Python packages in requirements.txt

## Error Handling

- Validates file types (.txt for compression, .bin for decompression)
- Handles empty files and single-character files
- Provides detailed error messages
- Automatic cleanup of temporary files
- Secure file handling

## Development and Testing

### Automated Testing
The project includes automated tests that can be run using:
```bash
cd algorithm
make test
```

This will:
1. Create a test input file with sample text
2. Compress the test file
3. Decompress the compressed file
4. Compare input and output files
5. Report test results

### Test Cases Include
- Empty files
- Single character repeated
- Mixed text content
- Special characters
- Large files
- Binary files

### Development Tools
- **Makefile**: Automates building and testing
- **GCC**: C compiler with optimization flags
- **Python**: Backend server implementation
- **Flask**: Web framework for API
- **JavaScript**: Frontend implementation

### Debugging
- Use `make clean` to remove all compiled files
- Compiler warnings are enabled (-Wall -Wextra)
- Backend logging provides detailed operation info
- Frontend console shows detailed error messages
