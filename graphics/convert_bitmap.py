import numpy as np
import os
from PIL import Image
import numpy as np

def convert_bitmap(bitmap_path):
    bitmap_name = "bitmap_" + os.path.splitext(os.path.basename(bitmap_path))[0]
    print(f"Processing bitmap from {bitmap_path}")
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
    c_bitmap_name = "BITMAP_" + bitmap_name.upper().replace("-", "_")
    
    image_size = (image.shape[1], image.shape[0])
    img_len_bits = image_size[0]*image_size[1]
    img_len_bytes = int(np.ceil(img_len_bits / 8))
    pad_width = img_len_bytes * 8 - img_len_bits
    output = ""
    output += f"#define {c_bitmap_name}_WIDTH {image_size[0]}\n"
    output += f"#define {c_bitmap_name}_HEIGHT {image_size[1]}\n"
    output += f"uint8_t {c_bitmap_name}[{img_len_bytes}] =" +  " {\n"

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
    bit_index = 0
    for N in range(image_size[1]):
        for M in range(image_size[0]):
            if bits[bit_index] == 1:
                print("#", end="")
            else:
                print(".", end="")
            bit_index += 1
        print()

    output += "\n};"

    with open(bitmap_file_output, 'w') as file:
        file.write(output)


    print(f"Saved bitmap header file to {bitmap_file_output}")
    print(f"bitmap size = {len(bytes)} bytes")



convert_bitmap("src/bitmaps/logo_64x64.png")