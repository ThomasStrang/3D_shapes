# -*- encoding: utf-8 -*-
import struct
import sys

if len(sys.argv) != 2:
    print("Usage: python BinaryToASCII.py stl_to_convert.stl")
    sys.exit(1)

FILE_NAME=sys.argv[1]

data = None
with open(FILE_NAME, 'rb') as f:
    data = f.read()

data = data[80:]

def get_vector(data, offset):
    return (struct.unpack('<f', data[0+offset:4+offset])[0], struct.unpack('<f', data[4+offset:8+offset])[0], struct.unpack('<f', data[8+offset:12+offset])[0])

with open(FILE_NAME.replace(".", "-ascii."), "w")  as out:
    out.write("solid\n")
    faces = data[0]+data[1]*256 +data[2] *256*256+data[3]*256*256*256
    data=data[4:]
    for f_num in range(0,faces):
        triangle_output=""
        triangle_output=triangle_output+"facet normal "
        x,y,z=get_vector(data, f_num*50)
        triangle_output=triangle_output+(str(x) + " " + str(y) + " " + str(z) + "\n")
        triangle_output=triangle_output+("outer loop\n")
        for v_num in range(3):
            triangle_output=triangle_output+("vertex ")
            x,y,z=get_vector(data, f_num*50 + v_num*12+12)
            triangle_output=triangle_output+(str(x) + " " + str(y) + " " + str(z) + "\n")
        triangle_output=triangle_output+("endloop\n")
        triangle_output=triangle_output+("endfacet\n")
        out.write(triangle_output)
        if f_num%50==0:
            print("\r"+str(f_num), flush=True, end="\r")
    out.write("endsolid")
print("Conversion complete!")