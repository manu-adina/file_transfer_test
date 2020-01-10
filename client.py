import socket
import struct
import sys
from os import listdir
from os.path import isfile, join

host = "127.0.0.1"
port = 5802

s = socket.socket()

def filenames_in_dir(dir_path):
    onlyfiles = [f for f in listdir(dir_path) if isfile(join(dir_path, f))]
    return onlyfiles

def send_file_names(filenames):

    # Send that you're about to start sending filenames.
    s.send("FN-STRT")

    # Send how many files the server should be expecting.
    number_of_filenames = len(filenames)
    number_of_filenames = struct.pack('!i', number_of_filenames)
    s.send(number_of_filenames)

    # Send integers with the respective filenames.
    for name in filenames:
        rand_num = len(name)
        rand_pack = struct.pack('!i', rand_num)

        num_sent = s.send(rand_pack)
        str_sent = s.send(name)

    # Receive the number of files it will be receiving back.
    number_of_missing = s.recv(4)
    number_of_missing = struct.unpack('!i', number_of_missing)[0]
    print("File Client: number of files to download " + str(number_of_missing))

    # Start receiving filename lengths, filename strings, and then files themselves.

    for i in range(0, number_of_missing):
        # Recv size of filename.
        filename_length = s.recv(4)
        filename_length = struct.unpack('!i', filename_length)[0]

        # Recv filename string
        filename = s.recv(filename_length)

        folder = './client_test_files'
        # Recv file
        recv_file(folder, filename)

    s.close()
    if number_of_missing == 0:
        print("File Client: files are already in sync")
    else:
        print("File Client: succesfully received all the files")


def recv_file(folder, filename):

    file_size = s.recv(4);
    file_size = struct.unpack('!I', file_size)[0]

    path = folder + '/' + filename;

    data_to_recv = file_size
    data_chunk = 1024

    print("File Client: downloading file - " + filename)

    with open(path, 'wb') as file:
        while data_to_recv:
            if data_to_recv< data_chunk:
                data_chunk = data_to_recv;

            data = s.recv(data_chunk)
            if not data:
                print("No data received")
                break
            file.write(data)
            data_to_recv = data_to_recv - data_chunk;
            sys.stdout.write("\rFile Client: downloading - " + str(file_size - data_to_recv) + " / " + str(file_size))
            sys.stdout.flush()

    print()

def main():
    s.connect((host, port))
    send_file_names(filenames_in_dir('./client_test_files'))
    s.close()

main()
