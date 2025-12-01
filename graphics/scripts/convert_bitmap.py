#!/usr/bin/python

import argparse
import glob
import numpy as np
import os
from PIL import Image
import numpy as np

def include_file(path):
    with open(path, 'r') as file:
        return file.read() + "\n"

def convert_bitmap(bitmap_path, quiet):
    bitmap_name = "bitmap_" + os.path.splitext(os.path.basename(bitmap_path))[0]
    if not quiet:
        print(f'Processing bitmap from file "{bitmap_path}"')
    image = Image.open(bitmap_path)

    # Convert to binary numpy array
    image = np.array(image, dtype=np.uint8)
    if len(image.shape) == 3:
        is_white = np.all(image == 255, axis=-1)
        image = np.zeros(image.shape[:-1], dtype=np.uint8)
        image[is_white] = 1
    if len(image.shape) == 2:
        image = image.clip(0, 1)

    # Generate font header file
    bitmap_file_output = f"output/bitmaps/{bitmap_name}.h"
    c_bitmap_name = bitmap_name.upper().replace("-", "_")
    
    image_size = (image.shape[1], image.shape[0])
    img_len_bits = image_size[0]*image_size[1]
    img_len_bytes = int(np.ceil(img_len_bits / 8))
    pad_width = img_len_bytes * 8 - img_len_bits
    output = ""
    output += f"#ifndef {c_bitmap_name}_H\n"
    output += f"#define {c_bitmap_name}_H\n\n"
    output += include_file("src/include/bitmap.h")
    output += f"\nstatic const uint8_t {c_bitmap_name}_DATA[{img_len_bytes}] =" +  " {\n"

    def bits_to_bytes_loop(bits):
        byte_list = []
        for i in range(0, len(bits), 8):
            byte_segment = bits[i : i + 8]
            binary_string = "".join(map(str, byte_segment))
            byte_value = int(binary_string, 2)
            byte_list.append(byte_value)
        return byte_list
    bits : np.ndarray = image.flatten()
    bits = np.pad(bits, pad_width=(0, pad_width), mode='constant', constant_values=0)
    bytes = bits_to_bytes_loop(bits)
    output += ",".join([hex(byte) for byte in bytes])
    output += "\n};\n"

    output += f"\nconst bitmap_t {c_bitmap_name} " + "{\n"
    output += f"    .width = {image_size[0]},\n"
    output += f"    .height = {image_size[1]},\n"
    output += f"    .datalen = {img_len_bytes},\n"
    output += f"    .data = {c_bitmap_name}_DATA,\n"
    output += "};\n"

    if not quiet:
        bit_index = 0
        for N in range(image_size[1]):
            for M in range(image_size[0]):
                if bits[bit_index] == 1:
                    print("#", end="")
                else:
                    print(".", end="")
                bit_index += 1
            print()

    output += "\n#endif // {c_bitmap_name}_H\n"

    with open(bitmap_file_output, 'w') as file:
        file.write(output)


    print(f'Saved "{bitmap_file_output}". Bitmap size: {len(bytes)} bytes')



if __name__=="__main__":
    parser = argparse.ArgumentParser(
        description="Convert image files to C/C++ byte arrays (header files)."
    )
    parser.add_argument(
        'files',
        nargs='+',
        help='Path to image file(s).'
    )
    parser.add_argument(
        '-q', '--quiet', 
        action='store_true', 
        help='Suppress previews'
    )
    args = parser.parse_args()

    all_files = []
    for file_pattern in args.files:
        expanded = glob.glob(file_pattern)
        if not expanded:
            all_files.append(file_pattern)
        else:
            all_files.extend(expanded)
    
    unique_files = sorted(list(set(all_files)))

    if not unique_files:
        print("No files to process.")
    else:
        for f in unique_files:
            convert_bitmap(f, args.quiet)


