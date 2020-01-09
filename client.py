import socket
import struct
import sys
from os import listdir
from os.path import isfile, join

host = "127.0.0.1"
port = 5802

s = socket.socket()

def filenames_in_dir():
    onlyfiles = [f for f in listdir("./test_dir") if isfile(join(".", f))]
    return onlyfiles

def send_file_names(filenames):

    # Send that you're about to start sending filenames.
    s.send("FN_STRT")

    # Send how many files the server should be expecting.
    number_of_filenames = len(filenames)
    number_of_filenames = struct.pack('!i', number_of_filenames)
    s.send(number_of_filenames)

    # Send integers with the respective filenames.
    for name in filenames:
        print("Filename: " + name)
        rand_num = len(name)
        rand_pack = struct.pack('!i', rand_num)

        num_sent = s.send(rand_pack)
        print("Sent num size: " + str(num_sent))
        str_sent = s.send(name)

        #recv_file()

    # Receive the number of files it will be receiving back.
    number_of_missing = s.recv(4)
    number_of_missing = struct.unpack('!i', number_of_missing)[0]
    print("Number of missing: " + str(number_of_missing))

    #for i in range(0, number_of_missing):
    #    recv_file()

    #print("Received all the files.")


def recv_file():

    filename = '../test_app_example.py'

    with open(filename, 'wb') as file:
        while True:
            data = s.recv(1024)
            if not data:
                break
            file.write(data)

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
    send_file_names(filenames_in_dir())
    #send_integer()
    #recv_file()
    s.close()

main()
