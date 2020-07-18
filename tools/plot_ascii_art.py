# pip3 install Pillow しておくこと
from PIL import Image, ImageDraw

# python3 plot_ascii_art.py < galaxy.txt とかすると plot.jpg に出力される
if __name__ == "__main__":
    lst = []
    while True:
        try:
            x = input()
            lst.append(x)
        except EOFError:
            break
    height = len(lst)
    width = len(lst[0])
    print(height, width)
    im = Image.new('RGB', (2*width, 2*height), (0, 0, 0))
    draw = ImageDraw.Draw(im)
    for i in range(height):
        for j in range(width):
            if lst[i][j] != '.':
                draw.rectangle((2*j, 2*i, 2*j+1, 2*i+1), fill=(255, 255,
                                                               255), outline=(255, 255, 255))
    im.save('plot.jpg')
