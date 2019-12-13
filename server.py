#!/usr/bin/env python3
import sys
from ctypes import *
import socket
import struct

import os

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
            if not data:
                break
    s.close()

print("Done")
