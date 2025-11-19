import os
from PIL import Image
import numpy as np

def generate_font(font_image, char_size, char_spacing, N):
    font_name = os.path.splitext(os.path.basename(font_image))[0]
    print(f"Processing font from {font_image}")
    image = Image.open(font_image)

    # Convert to binary numpy array
    image = np.array(image, dtype=np.uint8)
    is_white = np.all(image == 255, axis=-1)
    image = np.zeros(image.shape[:-1], dtype=np.uint8)
    image[is_white] = 1

    # Extract characters
    chars = []
    for i in range(N):
        startx = i * (char_size[0] + char_spacing)
        # chars.append(image[startx : startx + char_size[0], 0 : char_size[1]])
        chars.append(image[0 : char_size[1], startx : startx + char_size[0]])


    # --- Save Characters ---
    output_dir = f"output/fonts/{font_name}-chars"
    os.makedirs(output_dir, exist_ok=True) 

    for i, char in enumerate(chars):
        scaled_char = char * 255 
        
        char_img = Image.fromarray(scaled_char, mode='L')
        
        filename = os.path.join(output_dir, f"char_{i:02d}.png")
        char_img.save(filename)
        print(f"Saved: {filename}")


    # Generate font header file
    c_font_name = "FONT_" + font_name.upper().replace("-", "_")
    font_file_output = f"output/fonts/font_{font_name}.h"
    char_len_bits = char_size[0]*char_size[1]
    char_len_bytes = int(np.ceil(char_len_bits / 8))
    pad_width = char_len_bytes * 8 - char_len_bits
    font = ""
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
    font += "\n};"

    with open(font_file_output, "w") as file:
        file.write(font)


    print(f"Saved font header file to {font_file_output}")
    print(f"Font size = {char_len_bytes * N} bytes")



input_file_name = "src/fonts/big.png"
char_size = (37, 35)
char_spacing = 4
char_count = 10

# input_file_name = "src/fonts/small.png"
# char_size = (9, 14)
# char_spacing = 5
# char_count = 38 # 11 - just numbers and dash, 38 - with letters

generate_font(input_file_name, char_size, char_spacing, char_count)