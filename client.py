import socket
import struct
from os import listdir
from os.path import isfile, join

host = "127.0.0.1"
port = 5802

s = socket.socket()

def filenames_in_dir():
    onlyfiles = [f for f in listdir(".") if isfile(join(".", f))]
    return onlyfiles

# TODO: Sending a string from here and sending it to the server.

def send_file_names(filenames):

    s.send("FN-STRT")
    for name in filenames:
        #Send the size first.
        print(len(name))
        size_val = struct.pack('!i', len(name))
        s.send(size_val)
        print(name)
        s.send(name)

    #s.send("FN-STOP")


def send_integer():
    s.send("FN_STRT")
    test_string_1 = "Testing_testing_1.cpp"
    rand_num = len(test_string_1)
    rand_pack = struct.pack('!i', rand_num)
    s.send(rand_pack)
    s.send(test_string_1)

    test_string_2 = "123123_Testing_testing_2.cpp"
    rand_num = len(test_string_2)
    rand_pack = struct.pack('!i', rand_num)
    s.send(rand_pack)
    s.send(test_string_2)


def main():
    s.connect((host, port))
    # send_file_names(filenames_in_dir())
    send_integer()
    s.close()

main()
