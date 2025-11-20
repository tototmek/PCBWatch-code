#!/usr/bin/python

import argparse
import os
from PIL import Image
import numpy as np

def generate_font(font_image, N, char_size, quiet):
    font_name = os.path.splitext(os.path.basename(font_image))[0]
    if not quiet:
        print(f"Processing font from {font_image}")
    image = Image.open(font_image)

    # Convert to binary numpy array
    image = np.array(image, dtype=np.uint8)
    is_white = np.all(image == 255, axis=-1)
    image = np.zeros(image.shape[:-1], dtype=np.uint8)
    image[is_white] = 1

    char_spacing = int(np.floor((image.shape[1] - N * char_size) / (N-1)))
    char_size = (char_size, image.shape[0])

    # Extract characters
    chars = []
    for i in range(N):
        startx = i * (char_size[0] + char_spacing)
        # chars.append(image[startx : startx + char_size[0], 0 : char_size[1]])
        chars.append(image[0 : char_size[1], startx : startx + char_size[0]])

    # Generate font header file
    c_font_name = "FONT_" + font_name.upper().replace("-", "_")
    font_file_output = f"output/fonts/font_{font_name}.h"
    char_len_bits = char_size[0]*char_size[1]
    char_len_bytes = int(np.ceil(char_len_bits / 8))
    pad_width = char_len_bytes * 8 - char_len_bits
    font = ""
    font += f"#ifndef {c_font_name}_H\n"
    font += f"#define {c_font_name}_H\n\n"
    font += f"#include <stdint.h>\n\n"
    font += f"#define {c_font_name}_CHAR_WIDTH {char_size[0]}\n"
    font += f"#define {c_font_name}_CHAR_HEIGHT {char_size[1]}\n"
    font += f"#define {c_font_name}_CHAR_SPACING {char_spacing}\n\n"
    font += f"uint8_t {c_font_name}[{N}][{char_len_bytes}] =" +  " {\n"

    def bits_to_bytes_loop(bits):
        byte_list = []
        for i in range(0, len(bits), 8):
            byte_segment = bits[i : i + 8]
            binary_string = "".join(map(str, byte_segment))
            byte_value = int(binary_string, 2)
            byte_list.append(byte_value)
        return byte_list

    for i, char in enumerate(chars):
        char_list : np.ndarray = char.flatten()
        char_list = np.pad(char_list, pad_width=(0, pad_width), mode='constant', constant_values=0)
        bytes = bits_to_bytes_loop(char_list)
        font += "    {"
        font += ",".join([hex(byte) for byte in bytes])
        font += "},\n"
        bit_index = 0
        if not quiet:
            print(f"Character {i}:")
            for line in range(char_size[1]):
                for verse in range(char_size[0]):
                    if char_list[bit_index] == 1:
                        print("#", end="")
                    else:
                        print(".", end="")
                    bit_index += 1
                print()

    font = font[:-1]
    font += "\n};\n"+f"\n#endif // {c_font_name}_H\n"

    with open(font_file_output, "w") as file:
        file.write(font)

    print(f'Saved "{font_file_output}". Font size: {char_len_bytes * N} bytes.')

if __name__=="__main__":
    parser = argparse.ArgumentParser(
        description="Convert image files to C/C++ byte array font representations (header files)."
    )
    parser.add_argument(
        'file',
        type=str,
        help='Path to image file.'
    )
    parser.add_argument(
        'n_chars',
        type=int,
        help='Number of individual characters in font.'
    )
    parser.add_argument(
        'char_width',
        type=int,
        help='Pixel width of single character in font.'
    )
    parser.add_argument(
        '-q', '--quiet', 
        action='store_true', 
        help='Suppress preview.'
    )
    args = parser.parse_args()

    generate_font(args.file, args.n_chars, args.char_width, args.quiet)

