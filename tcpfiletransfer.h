#ifndef TCPFILETRANSFER_H
#define TCPFILETRANSFER_H

#include <vector>
#include <unordered_set>

#include <netinet/in.h>

class TCPFileTransfer
{
    public:
        TCPFileTransfer();
        void Start();
        void FilesInDirectory(std::string directory_path);
        void RecvNames(int mode);
        void SendDirNames();
        void SendFile(const char *filename);
        void SendMissingFiles();
        void FindMissingFiles();
        void Run();

    private:
        int _server_fd, _new_socket;

        ssize_t _valread;
        struct sockaddr_in _address;
        int _addrlen;

        // Need to change the folder later.
        std::string folder_str = "./GroundStationFiles/";

        const char* _filenames_start = "FN-STRT";
        char _fn_start_buffer[8] = {0};

        std::vector<std::string> _filenames_vector;
        std::vector<std::string> _dirnames_vector;
        std::vector<std::string> _missing_files;
        std::vector<std::string> _missing_dirs;
        std::unordered_set<std::string> _received_filenames;
        std::unordered_set<std::string> _received_dirnames;
};

#endif // TCPFILETRANSFER_H
