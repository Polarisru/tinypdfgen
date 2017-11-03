#!/usr/bin/env python
# -*- coding: latin-1 -*-
# ****************************************************************************
# * Software: Image to C converter for TinyPDFGen                            *
# * Version:  0.0.1                                                          *
# * Date:     2017-11-03                                                     *
# * Last update: 2017-11-03                                                  *
# * License:  LGPL v3.0                                                      *
# *                                                                          *
# * Author:  Alex Kiselev                                                    *
# ****************************************************************************

import sys
import zlib
import struct
from PIL import Image

formats = ["png", "bmp", "jpg"]

if len(sys.argv) < 2:
    print("Usage: image2c.py <file_name.img>")
    print("Supported image formats: bmp, png")
    exit()
img_name = sys.argv[1]
names = img_name.split('.')
f_name = names[0] + ".c"
if names[1] in formats:
    img = Image.open(img_name).convert("RGB")
    width = img.width
    height = img.height
    dt = []
    for y in range(height):
        for x in range(width):
            dt.extend(img.getpixel((x, y)))
    data = bytearray(dt)
else:
    with open(img_name, "rb") as f:
        f_data = bytearray(f.read())
    # check BMP format
    bmp_id = "".join(map(chr, struct.unpack_from("BB", f_data, 0)))
    if bmp_id != "BM":
        print("Wrong BMP file: ", bmp_name)
        exit()
    offset = struct.unpack_from("<L", f_data, 10)[0]
    width = struct.unpack_from("<L", f_data, 18)[0]
    height = struct.unpack_from("<L", f_data, 22)[0]
    # read raw image data
    data = f_data[offset:]
    if len(data) != width*height*3:
        print("Wrong image data, file corrupted?")
        exit()
    i = 0
    # swap R and B components
    while i  < len(data):
        data[i], data[i + 2] = data[i + 2], data[i]
        i += 3
enc = zlib.compress(data)
print("Image size: {}x{} pixels".format(width, height))
print("Original size:   {} bytes".format(len(data)))
print("Compressed size: {} bytes".format(len(enc)))
# write *.c file
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
