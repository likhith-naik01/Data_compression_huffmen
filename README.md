# Simple Text Compressor (Huffman)

This tiny project implements a minimal Huffman-based compressor and decompressor in pure Python.

Files:

- `compress.py` — read `input.txt`, build Huffman tree, encode, and write `compressed.bin`.
- `decompress.py` — read `compressed.bin`, decode and write `decompressed.txt`.
- `input.txt` — sample input file (you can replace it with your text).
- `compressed.bin` — produced by `compress.py`.
- `decompressed.txt` — produced by `decompress.py`.

How it stores data in `compressed.bin`:

- First a pickled metadata object written with `pickle.dump(metadata, file)` where
  `metadata = {'codes': codes, 'padding': padding}`.
- After the pickle data, the raw compressed bytes are appended.

Why this is simple:
- No external dependencies — uses Python stdlib only (`heapq`, `pickle`, `collections`).
- Clear separation of metadata and compressed bytes.

Usage (PowerShell / Windows):

```powershell
# create or edit input.txt with the text you want to compress
python compress.py
python decompress.py
# verify
fc input.txt decompressed.txt
```

Notes:

- This is educational and not optimized for production. For real uses consider using `zlib` or `lzma`.
- Empty files and single-character files are handled.
- Improvements: store original filename, store frequency map instead of codes, add CLI args, support binary input.
