font_chars = "ABCDEIKLNOPRSTWZĄ.ŚV"
target_text = "PONIEDZ."

result = []
for letter in target_text:
    idx = font_chars.find(letter)
    if idx == -1:
        print(f"Couln't convert string: Character {letter} missing in font")
    result.append(idx)

print(target_text)
print(result)