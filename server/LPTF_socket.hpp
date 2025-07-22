#ifndef LPTF_SOCKET_HPP
#define LPTF_SOCKET_HPP

#include <string>
#include <memory>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

class LPTF_Socket {
private:
    int socket_fd_;
    struct sockaddr_in address_;
    bool is_server_;
    bool is_connected_;
    
public:

    LPTF_Socket(); 
    LPTF_Socket(const std::string& ip, int port, bool is_server = false); 
    LPTF_Socket(const LPTF_Socket& other);
    LPTF_Socket& operator=(const LPTF_Socket& other); 
    ~LPTF_Socket(); // Destructeur
    
    LPTF_Socket(LPTF_Socket&& other) noexcept;
    LPTF_Socket& operator=(LPTF_Socket&& other) noexcept;
    
    bool create_socket();
    bool bind_socket();
    bool listen_socket(int backlog = 5);
    std::unique_ptr<LPTF_Socket> accept_connection();
    bool connect_to_server();
   
    ssize_t send_data(const std::string& data) const;
    ssize_t receive_data(std::string& data, size_t buffer_size = 1024) const;
   
    bool set_non_blocking(bool non_blocking);
    bool is_ready_to_read() const;
    bool is_ready_to_write() const;
   
    int get_socket_fd() const;
    const std::string get_ip() const;
    int get_port() const;
    bool get_is_server() const;
    bool get_is_connected() const;
    
  
    void set_address(const std::string& ip, int port);
    
   
    void close_socket();
    
   
    static std::vector<std::unique_ptr<LPTF_Socket>> poll_sockets(
        const std::vector<std::unique_ptr<LPTF_Socket>>& sockets, 
        int timeout_ms = -1
    );

private:
    
    void copy_from(const LPTF_Socket& other);
    void move_from(LPTF_Socket&& other) noexcept;
    void reset();
};

#endif 