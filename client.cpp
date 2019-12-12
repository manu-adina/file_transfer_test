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

#define PORT 5802

std::unordered_set<std::string> FileNamesInDirectory() {
    std::unordered_set<std::string> file_names;

    DIR *d;
    struct dirent *dir;

    d = opendir("/home/pi/Documents/file_transfer_test/");
    if(d) {
        while((dir = readdir(d)) != NULL) {
            std::string file_name(dir->d_name);
            if(file_name == "." || file_name == "..") continue;
            file_names.insert(file_name);
        }
        closedir(d);
    }
    return file_names;
}

void MissingFilesVector(std::unordered_set<std::string> ground_station_files,
                                        std::vector<std::string> &external_computer_files) {
    for(int i = external_computer_files.size() - 1; i >= 0; i--) {
        if(ground_station_files.find(external_computer_files[i]) == ground_station_files.end())
            continue;
        else external_computer_files.pop_back();
    }
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

    std::unordered_set<std::string> file_names = FileNamesInDirectory();
    std::vector<std::string> external_computer_files;
    external_computer_files.push_back("app_example.py");
    external_computer_files.push_back("client");
    external_computer_files.push_back("server");
    
    for(auto s : file_names) {
        std::cout << s << std::endl;
    }


    /* Wrong approach. There should be files left at the end */
    MissingFilesVector(file_names, external_computer_files);
    for(auto s : external_computer_files) {
        cout
    }

    return 0;
}
