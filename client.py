import socket
import struct
import sys
from os import listdir
from os.path import isfile, join

host = "127.0.0.1"
port = 5802

s = socket.socket()

def filenames_in_dir(dir_path):
    onlyfiles = [f for f in listdir(dir_path) if isfile(join(".", f))]
    return onlyfiles

def send_file_names(filenames):

    # Send that you're about to start sending filenames.
    s.send("FN_STRT")

    # Send how many files the server should be expecting.
    number_of_filenames = len(filenames)
    print("Number of filenames to send: " + str(number_of_filenames))
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

    # Start receiving filename lengths, filename strings, and then files themselves.

    for i in range(0, number_of_missing):
        # Recv size of filename.
        filename_length = s.recv(4)
        filename_length = struct.unpack('!i', filename_length)[0]
        print("Incoming filename length: " + str(filename_length))

        # Recv filename string
        filename = s.recv(filename_length)
        print("Filename Received: " + filename)

        folder = './client_test_files'
        # Recv file
        recv_file(folder, filename)


    print("Received all the files.")


def recv_file(folder, filename):

    file_size = s.recv(4);
    file_size = struct.unpack('!I', file_size)[0]
    print("Receiving file size of: " + str(file_size))

    path = folder + '/' + filename;
    print(path)

    data_to_send = file_size
    data_chunk = 1024

    with open(path, 'wb') as file:
        while data_to_send:
            if data_to_send < data_chunk:
                data_chunk = data_to_send;

            data = s.recv(data_chunk)
            if not data:
                print("No data received")
                break
            file.write(data)
            #print("Bytes written: " + str(file.write(data)))
            data_to_send = data_to_send - data_chunk;

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
    send_file_names(filenames_in_dir('./client_test_files'))
    #send_integer()
    #recv_file()
    s.close()

main()
