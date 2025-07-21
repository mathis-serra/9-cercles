#ifndef SERVER_HPP
#define SERVER_HPP

#include "LPTF_socket.hpp"
#include <string>
#include <memory>
#include <vector>
#include <map>

class Server {
private:
    std::unique_ptr<LPTF_Socket> server_socket_;
    std::vector<std::unique_ptr<LPTF_Socket>> client_sockets_;
    std::map<int, std::string> client_info_; // fd -> info client
    std::string bind_ip_;
    int bind_port_;
    bool is_running_;
    int max_clients_;

public:
    // Forme canonique de Coplien
    Server();
    Server(const std::string& bind_ip, int bind_port, int max_clients = 10);
    Server(const Server& other);
    Server& operator=(const Server& other);
    ~Server();
    
    // Constructeur et opérateur de move
    Server(Server&& other) noexcept;
    Server& operator=(Server&& other) noexcept;
    
    // Méthodes principales
    bool start_server();
    void stop_server();
    void run();
    
    // Méthodes de gestion des clients
    void handle_new_connection();
    void handle_client_message(LPTF_Socket& client_socket);
    void remove_client(int client_fd);
    void broadcast_message(const std::string& message, int sender_fd = -1);
    
    // Getters (const)
    const std::string& get_bind_ip() const;
    int get_bind_port() const;
    bool get_is_running() const;
    int get_max_clients() const;
    size_t get_client_count() const;
    
    // Setters
    void set_bind_info(const std::string& ip, int port);
    void set_max_clients(int max_clients);

private:
    void copy_from(const Server& other);
    void move_from(Server&& other) noexcept;
    void reset();
    void cleanup_disconnected_clients();
};

#endif // SERVER_HPP
