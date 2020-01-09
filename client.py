import socket
import struct
import sys
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

    s.send("FN_STRT")
    for name in filenames:
        print("Filename: " + name)
        rand_num = len(name)
        rand_pack = struct.pack('!i', rand_num)

        num_sent = s.send(rand_pack)
        print("Sent num size: " + str(num_sent))
        str_sent = s.send(name)

def recv_file():

    filename = '../test_app_example.py'

    with open(filename, 'wb') as file:
        while True:
            data = s.recv(1024)
            if not data:
                break
            file.write(data)

#with open(filename, 'rb') as file:
#    data = file.read(1024)
#    while(data):
#        sent_bytes = s.send(data)
#        data = file.read(1024)
#print("Sent bytes: " + str(sent_bytes))

def send_integer():
    s.send("FN_STRT")
    test_string_1 = "app_example.py"
    rand_num = len(test_string_1)
    rand_pack = struct.pack('!i', rand_num)

    num_sent = s.send(rand_pack)
    print("Sent num size: " + str(num_sent))
    str_sent = s.send(test_string_1)
    print('Sent string length: ' + str(str_sent))

    send_file(test_string_1)

    #test_string_2 = "123123_Testing_testing_2.cpp"
    #rand_num = len(test_string_2)
    #rand_pack = struct.pack('!i', rand_num)

    #num_sent = s.send(rand_pack)
    #print("Sent num size: " + str(num_sent))
    #str_sent = s.send(test_string_2)
    #print('Sent string length: ' + str(str_sent))


def main():
    s.connect((host, port))
    #send_file_names(filenames_in_dir())
    #send_integer()
    recv_file()
    s.close()

main()
