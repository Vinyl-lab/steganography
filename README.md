# Image Steganography in C

A command-line application written in C that hides and extracts secret text messages inside 24-bit BMP images using Least Significant Bit (LSB) steganography.

This project was developed as part of my Embedded C learning journey to strengthen my understanding of file handling, bit manipulation, pointers, structures, and modular programming in C.

---

## Features

- Hide a secret text file inside a BMP image
- Extract the hidden text from the encoded image
- Supports 24-bit BMP image format
- Preserves the visual quality of the original image
- Modular implementation using multiple source and header files
- Command-line interface for encoding and decoding

---

## Technologies Used

- C Programming
- GCC Compiler
- File Handling
- Bitwise Operations
- Structures
- Pointers
- Command Line Arguments

---

## Project Files

| File | Description |
|------|-------------|
| `main.c` | Program entry point |
| `encode.c` | Encoding implementation |
| `decode.c` | Decoding implementation |
| `common.h` | Common definitions and declarations |
| `encode.h` | Encoding function declarations |
| `decode.h` | Decoding function declarations |

---

## Build

Compile the project using GCC:

```bash
gcc *.c -o steganography
```

---

## Usage

### Encode a Secret File

```bash
./steganography -e beautiful.bmp secret.txt stego.bmp
```

### Decode the Hidden File

```bash
./steganography -d stego.bmp output.txt
```

---

## Concepts Demonstrated

- File I/O
- Bit Manipulation
- Image Processing Basics
- Modular Programming
- Error Handling
- Command Line Argument Parsing

---

## Future Improvements

- Password-protected encoding
- Support for additional image formats
- Encrypt the secret data before embedding
- Graphical User Interface (GUI)

---

## Author

**Vinay L**

GitHub: https://github.com/Vinyl-lab