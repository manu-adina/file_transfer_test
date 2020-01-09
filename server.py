#!/usr/bin/env python3
import sys
from ctypes import *
import socket
import struct

import os

i = 1

print(sys.getsizeof(i))

HOST = '127.0.0.1'
PORT = 5802

# Basic server connection for now.
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen();
    conn, addr = s.accept();
    with conn:
        print("Connected by ", addr)
        while True:
            data = conn.recv(1024);
            print(sys.getsizeof(data))
            if not data:
                break
            conn.sendall(data);
            recv_string = struct.unpack('!' + 'c' * 100, data);
            print(recv_string)

    s.close()

print("Done")
