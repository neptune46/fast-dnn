
img = []
with open('t10k-images.idx3-ubyte', 'rb') as f:
    prefix = f.read(16)
    num = 28*28
    for i in range(num):
        n = f.read(1)
        a = '0x' + n.hex()
        img.append(a)

with open('test_data.h', 'a') as f:
    count = 0
    f.write('static char test_img[] = {\n')
    for d in img:
        f.write(d+", ")
        count = count + 1
        if count % 16 == 0:
            f.write('\n')
    f.write('};\n')

print('done')