#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <iostream>
#include <dirent.h>
#include <vector>
#include <unordered_set>

#include <sys/stat.h>

#define PORT 5802
#define BUFFER_LEN 1024 

std::string folder_str = "./server_test_files/";

int new_socket, server_fd;
const char *stop_filenames = "FN-STOP";
std::vector<std::string> filenames_vector;
std::vector<std::string> missing_files;
std::unordered_set<std::string> received_filenames;

void get_filenames() {

    int file_size = 0;
    int number_of_filenames = 0;
    ssize_t valread;

    /* Receive how many filenames for the program to expect. */
    if((valread = read(new_socket, &number_of_filenames, 4)) == 4) {
        number_of_filenames = ntohl(number_of_filenames);
    } else {
        std::cerr << "Wasn't able to receive the number of filenames" << std::endl;
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < number_of_filenames; i++) {
        if((valread = read(new_socket , &file_size, 4)) == 4) {

            std::string received_name;

            file_size = ntohl(file_size);
        
            /* Include null-byte */
            char buffer[file_size + 1];
            memset(buffer, 0, file_size + 1);
            /* TODO: Check if all bytes have been received */
            valread = read(new_socket, buffer, file_size);
            received_name = buffer;

            received_filenames.insert(received_name);
            file_size = 0;
        } else {
            std::cout << "Wasn't able to receive enough bytes!" << std::endl;
            break;
        }
    }
}

void find_missing_files() {
    for(std::string name : filenames_vector) {
        if(received_filenames.find(name) == received_filenames.end()) {
            missing_files.push_back(name);
        }
    }
}

void files_in_dir(std::string dir_path) {
    DIR *dir;
    struct dirent *ent;

    std::string filename;
    
    if((dir = opendir(dir_path.c_str())) != NULL) {
        while((ent = readdir(dir)) != NULL) {
            /* Ignore directories, only work with files */
            if(ent->d_type == DT_DIR) continue;
            
            filename = ent->d_name;
            filenames_vector.push_back(filename);
        }
    } else {
        std::cerr << "Could not open that directory" << std::endl;
    }
}

void send_file(const char* filename) {

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
    send(new_socket, &file_size, 4, 0);

    std::cout << "File Server: sending file - '" << filename << "'" << std::endl;

    /* Counters to print the progress */
    int total_sent = 0;
    file_size = ntohl(file_size);

    while((read_bytes = fread(file_buf, sizeof(char), BUFFER_LEN, file)) > 0) {
        /* TODO: Check if all bytes have been sent */
        sent = send(new_socket, file_buf, read_bytes, 0); 
        total_sent += read_bytes;
        std::cout << "\r" << "File Server: sending - " << std::to_string(total_sent) << " / " << std::to_string(file_size) << " bytes";
    }
    std::cout << std::endl;

    fclose(file);
}

void send_missing_files() {

    /* First sending how many files to send */
    int number_of_missing = missing_files.size(); 

    if(!number_of_missing) std::cout << "File Server: Your data is in sync." << std::endl;
    else std::cout << "File Server: number of files to send " << std::to_string(number_of_missing) << std::endl;

    number_of_missing = htonl(number_of_missing);
    send(new_socket, &number_of_missing, 4, 0);
    
    /* Sending length of filename, string of filename, 
     * and then sending the file itself consecutively. */
    for(auto filename : missing_files) {
        /* Sending the filename length */
        int filename_length = filename.size();
        filename_length = htonl(filename_length);
        send(new_socket, &filename_length, 4, 0);

        /* Sending the filename string */
        const char *filename_c = filename.c_str();
        send(new_socket, filename_c, filename.size(), 0);

        /* Send the file itself */
        std::string full_path = folder_str + filename;
        send_file(full_path.c_str());
    }
}


int main(int argc, char const *argv[]) 
{ 
    /* First get files in the directory */
    files_in_dir("./server_test_files");

    ssize_t valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[8] = {0}; 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(PORT); 
       
    // Attaching socket to the port 5802 
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 

    const char* filenames_start = "FN-STRT";

    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        } 

        if((valread = read(new_socket , buffer, 8)) > 0) {
            if(strcmp(buffer, filenames_start) == 0) {
                get_filenames();
                find_missing_files();
                send_missing_files();
                missing_files.clear(); 
                received_filenames.clear();

            }
            memset(&buffer, 0, sizeof(buffer));
        }
    } 

    return 0; 
} 
