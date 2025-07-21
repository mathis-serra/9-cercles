#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../server/LPTF_socket.hpp"
#include <string>
#include <memory>

class Client {
private:
    std::unique_ptr<LPTF_Socket> socket_;
    std::string server_ip_;
    int server_port_;
    bool is_connected_;

public:
    // Forme canonique de Coplien
    Client();
    Client(const std::string& server_ip, int server_port);
    Client(const Client& other);
    Client& operator=(const Client& other);
    ~Client();
    
    // Constructeur et opérateur de move
    Client(Client&& other) noexcept;
    Client& operator=(Client&& other) noexcept;
    
    // Méthodes principales
    bool connect_to_server();
    bool send_message(const std::string& message);
    bool receive_message(std::string& message);
    void disconnect();
    
    // Getters (const)
    const std::string& get_server_ip() const;
    int get_server_port() const;
    bool get_is_connected() const;
    
    // Setters
    void set_server_info(const std::string& ip, int port);
    
    // Méthode pour exécuter le client en mode interactif
    void run_interactive();

private:
    void copy_from(const Client& other);
    void move_from(Client&& other) noexcept;
    void reset();
};

#endif // CLIENT_HPP