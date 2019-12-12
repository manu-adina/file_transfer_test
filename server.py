#!/usr/bin/env python3
import sys
from ctypes import *
import socket
import struct

import os

HOST = '127.0.0.1'
PORT = 5802
MAX_FILE_NAME_LENGTH = 255

path = '/home/pi/Documents/file_transfer_test'
file_list_buffer = bytearray(1024)


def files_search():
    files = []
    offset = 0

    for r, d, f in os.walk(path):
        for file in f:
            files.append(file)

    print("PACKING THE FOLLOWING:")
    for f in files:
        #print(f)
        #struct.pack_into('21s', file_list_buffer, offset, f)
        #offset = offset + len(f)
        data = 'test'
        s = bytes(f, 'utf-8')
        struct.pack("I%ds" % (len(s),), len(s), s)
        (i,), data = struct.unpack("I", data[:4]), data[4:]
        s, data = data[:i], data[i:]
        print(data)

files_search()

# Basic server connection for now.
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen();
    conn, addr = s.accept();
    with conn:
        print("Connected by ", addr)
        while True:
            data = conn.recv(1024);
            if not data:
                break
            conn.sendall(data)

f_binary = open(filename, 'wb')
while True:
    f.write(l)


f.close()
s.close()

print("Done")
