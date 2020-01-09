#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <iostream>
#define PORT 5802


int new_socket;
const char *stop_filenames = "FN-STOP";


void get_filenames() {
    //int num = 0;
    //valread = read(new_socket, &num, 4);
    //num = htonl(num);
    //char buffer_str[num + 1];
    //memset(buffer_str, 0, sizeof(buffer_str));
    //valread = read(new_socket, buffer_str, num);
    //std::cout << "Number: " << std::to_string(num) << std::endl;
    //std::cout << "Filename: " << buffer_str << std::endl;
    
    
    int file_size;
    ssize_t valread;

    int i = 0;

    while(1) {
        if((valread = read(new_socket , &file_size, 4)) > 0) {
            file_size = ntohl(file_size);
            std::cout << "Filename size: " << std::to_string(file_size) << std::endl;
        
            // Include null-byte
            file_size++;
            char buffer[file_size];
            memset(buffer, 0, file_size);
            valread = read(new_socket, buffer, file_size);
            printf("Filename: %s\n", buffer);
            file_size = 0;
            //if(strcmp(buffer, stop_filenames)) {
            //    std::cout << "Stopping" << std::endl;
            //}
            //i++;
            //if( i = 10 ) break;
            } else {
            break;
        }
    }
}


int main(int argc, char const *argv[]) 
{ 
    int server_fd, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[8] = {0}; 
    char *hello = "Hello from server"; 
       
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
    address.sin_port = htons( PORT ); 
       
    // Attaching socket to the port 5802 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 

    const char* filenames_start = "FN-STRT";

    while(1) {
        if((valread = read(new_socket , buffer, 8)) > 0) {
            if(strcmp(buffer, filenames_start)) {
                std::cout << "Receiving filenames" << std::endl;
                get_filenames();
            }
            memset(&buffer, 0, sizeof(buffer));
        }
        break;
   }
   
    


    return 0; 
} 
