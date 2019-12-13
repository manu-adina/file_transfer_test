#include <unistd.h>
#include <iostream>
#include <errno.h>

#include <stdio.h>
#include <string.h>

/* For socket networking */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* To get filenames */
#include <dirent.h>

#include <unordered_set>
#include <vector>

#define PORT 5802

std::vector<std::string> FileNamesInDirectory() {
    std::vector<std::string> file_names;

    DIR *d;
    struct dirent *dir;

    d = opendir("/home/pi/Documents/file_transfer_test/");
    if(d) {
        while((dir = readdir(d)) != NULL) {
            std::string file_name(dir->d_name);
            if(file_name == "." || file_name == "..") continue;
            file_names.push_back(file_name);
        }
        closedir(d);
    }
    return file_names;
}

/* Find the missing files that the ground station may have, and download those missing files. */
std::vector<std::string> MissingFilesVector(std::unordered_set<std::string> ground_station_files,
                                        std::vector<std::string> external_computer_files) {

    std::vector<std::string> missing_files;
    for(int i = external_computer_files.size() - 1; i >= 0; i--) {
        if(ground_station_files.find(external_computer_files[i]) == ground_station_files.end()) {
            missing_files.push_back(external_computer_files[i]);
        }
    }

    return missing_files;
}

int SendMissingFiles(std::vector<std::string> external_computer_files) {
    for(int i = 0; i < external_computer_files.size(); i++) {
        //TODO: Send Files Here
    }
}

int main(int argc, char **argv) {
    int sockfd; 
    struct sockaddr_in server_addr;
    char buffer[1024] = {0};

    char *hello = "hello";

    /* Creating socket */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Unable to create the socket: " << strerror(errno) << std::endl;
        return -1; 
    }

    char *addr = "127.0.0.1";

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(addr);
    server_addr.sin_port = htons(PORT);

    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) << 0) {
        std::cerr << "Unable to connect to the server " << strerror(errno) << std::endl;
        return -2;
    }

    send(sockfd, hello, strlen(hello), 0);
    read(sockfd, buffer, 1024); 

    std::cout << "Sent 'Hello' String" << std::endl;
    std::cout << sizeof(hello) << std::endl;
    std::cout << "Read " << buffer << std::endl;

    std::vector<std::string> file_names = FileNamesInDirectory();
    std::vector<std::string> missing_files; 
    std::unordered_set<std::string> external_computer_files;
    //external_computer_files.insert("app_example.py");
    //external_computer_files.insert("client");
    //external_computer_files.insert("server");

    for(auto s : file_names) {
        std::cout << "File in Dir: "<< s << std::endl;
    }

    missing_files = MissingFilesVector(external_computer_files, file_names);
    for(auto s : missing_files) {
        std::cout << "Left over files: " << s << std::endl;
    }

    return 0;
}
