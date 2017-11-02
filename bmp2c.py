import sys
import zlib
import struct

if len(sys.argv) < 2:
    print("Usage: bmp2c.py <file_name.bmp>")
    exit()
bmp_name = sys.argv[1]
names = bmp_name.split('.')
f_name = names[0] + ".c"
with open(bmp_name, "rb") as f:
    f_data = bytearray(f.read())
bmp_id = "".join(map(chr, struct.unpack_from("BB", f_data, 0)))
if bmp_id != "BM":
    print("Wrong BMP file: ", bmp_name)
    exit()
offset = struct.unpack_from("<L", f_data, 10)[0]
width = struct.unpack_from("<L", f_data, 18)[0]
height = struct.unpack_from("<L", f_data, 22)[0]
data = f_data[offset+2:]
enc = zlib.compress(data)
f = open(f_name, 'w')
f.write("#include <stdint.h>\n")
f.write("#include \"pdf.h\"\n\n")
f.write("const uint8_t {}_data[] = {{\n  ".format(names[0]))
counter = 0
for i in enc:
    f.write("{}, ".format(format(i, '#04x')))
    counter += 1
    if counter % 16 == 0:
        f.write("\n  ")
f.write("\n};\n\n")
f.write("const TPdfImage {}_img = {{\n".format(names[0]))
f.write("  {}, // image width\n".format(width))
f.write("  {}, // image height\n".format(height))
f.write("  {}, // image length\n".format(len(enc)))
f.write("  {}_data, // image data\n".format(names[0]))
f.write("};\n")
f.close()
