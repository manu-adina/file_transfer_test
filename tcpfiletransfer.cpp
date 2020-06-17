/** 1.) Files exchange first.
 *  2.) Exhange folder names.
 *  3.) For each folder name, exchange files
 *  4.) Complete
**/

#include "tcpfiletransfer.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <dirent.h>
#include <iostream>

#include <sys/stat.h>   // File sizes.

#define PORT 5802
#define BUFFER_LEN 1024
#define MAX_FILENAME_LEN 255

/* Receive filenames or directory names */
#define RECV_FN 0
#define RECV_DN 1

TCPFileTransfer::TCPFileTransfer()
{

}

void TCPFileTransfer::Start() {

    /* First get files in the directory */
    FilesInDirectory(folder_str.c_str());

    int opt = 1;
    _addrlen = sizeof(_address);
    char buffer[8] = {0}; // Buffer for FN-STRT string to come

    // Creating socket file descriptor
    if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cerr << "File Server: creating a socket failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        std::cerr << "File Server: setting socket to reuse port and addr failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(PORT);

    // Attaching socket to the port 5802
    if (bind(_server_fd, (struct sockaddr *)&_address, sizeof(_address))<0)
    {
        std::cerr << "File Server: binding socket to port failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(_server_fd, 3) < 0)
    {
        std::cerr << "File Server: listen failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Successfully created a TCP server." << std::endl;
}

// Gather both files and dirnames in the 'scan_results' directory
void TCPFileTransfer::FilesInDirectory(std::string directory_path) {
    DIR *dir;
    struct dirent *ent;

    std::string filename;

    if((dir = opendir(directory_path.c_str())) != NULL) {
        while((ent = readdir(dir)) != NULL) {
            // Look for files and directories in the folder
            std::string dirname = ent->d_name;
            if(ent->d_type == DT_DIR) {
                if(dirname != ".." && dirname != ".") _dirnames_vector.push_back(ent->d_name);
            }
            else if(ent->d_type == DT_REG) _filenames_vector.push_back(ent->d_name); 
        }
    } else {
        std::cerr << "Could not open that directory" << std::endl;
        exit(EXIT_FAILURE);
    }
}

/* Mode = 0 - Recv Filenames
 * Mode = 1 - Recv Dirnames
 */
void TCPFileTransfer::RecvNames(int mode) {

    int file_size = 0;
    int number_of_filenames = 0;
    ssize_t valread;

    /* Receive how many filenames for the program to expect. */
    if((valread = read(_new_socket, &number_of_filenames, 4)) == 4) {
        number_of_filenames = ntohl(number_of_filenames);
    } else {
        std::cerr << "File Server: wasn't able to receive the number of filenames" << std::endl;
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < number_of_filenames; i++) {
        if((valread = read(_new_socket , &file_size, 4)) == 4) {

            std::string received_name;

            file_size = ntohl(file_size);

            /* Include null-byte */
            char buffer[file_size + 1];
            memset(buffer, 0, file_size + 1);
            /* TODO: Check if all bytes have been received */
            valread = read(_new_socket, buffer, file_size);
            received_name = buffer;
            std::cout << "Received name: " << received_name << std::endl;

            if(!mode) _received_filenames.insert(received_name);
            else _received_dirnames.insert(received_name);
            file_size = 0;
        } else {
            std::cerr << "File Server: wasn't able to receive enough bytes" << std::endl;
            break;
        }
    }
}

void TCPFileTransfer::SendFile(const char *filename) {

    ssize_t sent = 0;

    /* Open file to be read */
    FILE *file;
    file = fopen(filename, "rb");

    char file_buf[BUFFER_LEN];

    ssize_t read_bytes = 0;

    /* First send the size of the file */
    struct stat st;
    stat(filename, &st);
    unsigned int file_size = st.st_size;
    file_size = htonl(file_size);
    send(_new_socket, &file_size, 4, 0);

    printf("File Server: sending file - '%s'\n", filename);

    /* Counters to print the progress */
    //int total_sent = 0;
    file_size = ntohl(file_size);

    while((read_bytes = fread(file_buf, sizeof(char), BUFFER_LEN, file)) > 0) {
        /* TODO: Check if all bytes have been sent */
        int total = 0;
        int bytes_left = read_bytes;
        while(total < read_bytes) {
            sent = send(_new_socket, file_buf + total, bytes_left, 0);
            if(sent == -1) break;
            total += sent;
            bytes_left -= sent;
        }

        printf("File Server: total bytes sent - %d\n", total);
        //qDebug("File Server: sending - %d / %d bytes", total_sent, file_size);
    }

    fclose(file);
}

void TCPFileTransfer::SendMissingFiles(std::string dirname) {
    /* First sending how many files to send */
    int number_of_missing = _missing_files.size();
    std::cout << "Number of missing: " << std::to_string(number_of_missing) << std::endl;

    if(!number_of_missing) std::cout << "File Server: your data is in sync." << std::endl;
    else printf("File Server: number of files to send (%d)\n", number_of_missing);

    number_of_missing = htonl(number_of_missing);
    send(_new_socket, &number_of_missing, 4, 0);

    /* Sending length of filename, string of filename,
     * and then sending the file itself consecutively. */
    for(auto filename : _missing_files) {
        /* Sending the filename length */
        int filename_length = filename.size();
        printf("File Server: Sending filename length (%d)\n", filename_length);
        filename_length = htonl(filename_length);
        send(_new_socket, &filename_length, 4, 0);

        /* Sending the filename string */
        const char *filename_c = filename.c_str();
        send(_new_socket, filename_c, filename.size(), 0);

        /* Send the file itself */
        std::string full_path = folder_str + dirname + "/" + filename;
        SendFile(full_path.c_str());
    }
    /* Clear so that it doesn't persist for the next directory stage */
    _filenames_vector.clear();
}

void TCPFileTransfer::SendDirNames() {
    std::cout << "File Server: syncing directories..." << std::endl;
    std::cout << "File Server: number of dirs - " << std::to_string(_dirnames_vector.size()) << std::endl;

    /* Sending the number of directory names it should be expecting */
    int dir_num = _dirnames_vector.size();
    dir_num = htonl(dir_num);
    send(_new_socket, &dir_num, 4, 0); 
    
    for(auto dirname : _dirnames_vector) {
        /* Sending the dirname length */
        std::cout << "File Server: processing directory - " << dirname << std::endl;
        int dirname_length = dirname.size();
        dirname_length = htonl(dirname_length);
        send(_new_socket, &dirname_length, 4, 0);

        /* Sending the dirname string */
        const char *dirname_c = dirname.c_str();
        send(_new_socket, dirname_c, dirname.size(), 0);
        _received_filenames.clear();
        RecvNames(RECV_FN);
        _missing_files.clear();
        _filenames_vector.clear();
        std::string dir_path_str = folder_str + dirname + "/";
        FilesInDirectory(dir_path_str.c_str());
        FindMissingFiles();
        SendMissingFiles(dirname);
        std::cout << "Files Server: sent all the items" << std::endl;
    }
}

void TCPFileTransfer::FindMissingFiles() {
    for(std::string name : _filenames_vector) {
        if(_received_filenames.find(name) == _received_filenames.end()) {
            _missing_files.push_back(name);
        }
    }
}

void TCPFileTransfer::Run() {
    Start();

    for(;;) {
        if ((_new_socket = accept(_server_fd, (struct sockaddr *)&_address,
                       (socklen_t*)&_addrlen))<0)
        {
            std::cerr << "File Server: could not accept the connection" << std::endl;
            exit(EXIT_FAILURE);
        }

        if((_valread = read(_new_socket , _fn_start_buffer, 8)) > 0) {
            if(strcmp(_fn_start_buffer, _filenames_start) == 0) {
                // Find all the files in the "home" directory
                RecvNames(RECV_FN);
                FindMissingFiles();
                SendMissingFiles("");
                SendDirNames();

                // Start Receiving all the directory contents
                for(auto name : _missing_dirs) {
                    std::cout << "Missing Dir: " << name << std::endl;
                }
                // For all the files in the directory, do the steps above.
                //for(int i = 0; i < number_of_missing; i++) {
                //    RecvFile(RECV_FN);
                //    FindMissingFiles();
                //}

                _missing_files.clear();
                _received_filenames.clear();
            }
            memset(&_fn_start_buffer, 0, sizeof(_fn_start_buffer));
        }
    }
}

int main() {
    
    TCPFileTransfer *tcpsock = new TCPFileTransfer();
    
    tcpsock->Run();
    
    return 0;
}
