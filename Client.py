import socket
import struct
import sys
from os import listdir, mkdir
from os.path import isfile, isdir, join

host = "127.0.0.1"
port = 5802
folder_path = ""

s = socket.socket()

onlyfiles = []

def filenames_in_dir(dir_path):
    try:
        onlyfiles = [f for f in listdir(dir_path) if isfile(join(dir_path, f)) and not f == sys.argv[0]]
    except OSError:
        print("File Client: could not find the specified directory")
        return onlyfiles
    else:
        return onlyfiles

def send_names(names, mode):
    # Send how many files the server should be expecting.
    number_of_names = len(names)
    number_of_names = struct.pack('!i', number_of_names)
    s.send(number_of_names)
    # Send integers with the respective filenames.
    for name in names:
        name_len = len(name)
        name_len = struct.pack('!i', name_len)
        s.send(name_len)
        s.send(name.encode())

def recv_files():
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
        print("File Client: Receiving Filename Length - " + str(filename_length))
        filename = s.recv(filename_length)
        filename_str = filename.decode()
        # Recv file
        recv_file(filename_str)
    if number_of_missing == 0:
        print("File Client: files are already in sync")
    else:
        print("File Client: succesfully received all the files")

def recv_dirs():
    # Recv the number of directories
    dir_num = s.recv(4)
    dir_num = struct.unpack('!i', dir_num)[0]

    print("File Client: receiving (" + str(dir_num) + ") dir names")

    for i in range(0, dir_num):
        # Recv the len of the dir name
        dirname_length = s.recv(4)
        dirname_length = struct.unpack('!i', dirname_length)[0]
        dirname = s.recv(dirname_length)
        dirname_str = dirname.decode()
        print("File Client: received - " + dirname_str)
        sync_dir(dirname_str)

def sync_dir(dirname):
    dir_path = folder_path + "/" + dirname
    if isdir(dir_path) == False:
        print("File Client: created a folder - " + dirname)
        mkdir(folder_path + "/" + dirname)

    print("File Client: folder path created")
    number_of_missing_in_dir = send_names(filenames_in_dir(dir_path), 0)
    print("Number of missing in dir: " + str(number_of_missing_in_dir))

def recv_file(filename):
    file_size = s.recv(4)
    file_size = struct.unpack('!I', file_size)[0]
    path = folder_path + '/' + filename
    data_to_recv = file_size
    data_chunk = 1024
    print("File Client: downloading file - " + filename)
    with open(path, 'wb') as file:
        while data_to_recv:
            if data_to_recv < data_chunk:
                data_chunk = data_to_recv

            data_recv = 0
            while data_recv != data_chunk:
                data = s.recv(data_chunk - data_recv)
                file.write(data)
                data_recv = data_recv + len(data)
                print("File Client: RECEIVED - " + str(data_recv))

            data_to_recv = data_to_recv - data_recv

def main():
    n_arguments = len(sys.argv) - 1
    global folder_path
    if n_arguments > 0:
        print("File Client: syncing with the folder '%s'" % (sys.argv[1]))
        folder_path = sys.argv[1]
    else:
        print("File Client: syncing with the current directory")
        folder_path = "./ExternalPC"
    try:
        s.connect((host, port))
    except socket.error:
        print("File Client: could not connect to the ground station")
    else:
        print("File Client: successfully connected to the ground station")

        # Send that you're about to start sending filenames.
        send_str = "FN-STRT"
        s.send(send_str.encode())

        # 1. Downloading the missing files in project directory.
        number_of_missing = send_names(filenames_in_dir(folder_path), 0)
        recv_files()
        recv_dirs()
        # 2. Receive directory names.

        # 2. Downloading folder contents
        # number_of_missing = send_names(dirnames_in_dir(folder_path), 1)

        s.close()

main()
