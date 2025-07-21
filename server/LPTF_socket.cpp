#include "LPTF_socket.hpp"
#include <iostream>
#include <cstring>
#include <errno.h>

// Constructeur par défaut
LPTF_Socket::LPTF_Socket() 
    : socket_fd_(-1), is_server_(false), is_connected_(false) {
    std::memset(&address_, 0, sizeof(address_));
}

// Constructeur paramétré
LPTF_Socket::LPTF_Socket(const std::string& ip, int port, bool is_server)
    : socket_fd_(-1), is_server_(is_server), is_connected_(false) {
    std::memset(&address_, 0, sizeof(address_));
    set_address(ip, port);
    create_socket();
}

// Constructeur de copie
LPTF_Socket::LPTF_Socket(const LPTF_Socket& other) 
    : socket_fd_(-1), is_server_(false), is_connected_(false) {
    copy_from(other);
}

// Opérateur d'assignation
LPTF_Socket& LPTF_Socket::operator=(const LPTF_Socket& other) {
    if (this != &other) {
        close_socket();
        copy_from(other);
    }
    return *this;
}

// Constructeur de move
LPTF_Socket::LPTF_Socket(LPTF_Socket&& other) noexcept 
    : socket_fd_(-1), is_server_(false), is_connected_(false) {
    move_from(std::move(other));
}

// Opérateur d'assignation de move
LPTF_Socket& LPTF_Socket::operator=(LPTF_Socket&& other) noexcept {
    if (this != &other) {
        close_socket();
        move_from(std::move(other));
    }
    return *this;
}

// Destructeur
LPTF_Socket::~LPTF_Socket() {
    close_socket();
}

// Création de la socket
bool LPTF_Socket::create_socket() {
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ == -1) {
        std::cerr << "Erreur lors de la création de la socket: " << strerror(errno) << std::endl;
        return false;
    }
    
    // Option pour réutiliser l'adresse
    int opt = 1;
    if (setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Erreur lors du setsockopt: " << strerror(errno) << std::endl;
        close_socket();
        return false;
    }
    
    return true;
}

// Liaison de la socket (pour le serveur)
bool LPTF_Socket::bind_socket() {
    if (socket_fd_ == -1) {
        std::cerr << "Socket non créée" << std::endl;
        return false;
    }
    
    if (bind(socket_fd_, reinterpret_cast<struct sockaddr*>(&address_), sizeof(address_)) == -1) {
        std::cerr << "Erreur lors du bind: " << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}

// Écoute des connexions (pour le serveur)
bool LPTF_Socket::listen_socket(int backlog) {
    if (socket_fd_ == -1) {
        std::cerr << "Socket non créée" << std::endl;
        return false;
    }
    
    if (listen(socket_fd_, backlog) == -1) {
        std::cerr << "Erreur lors du listen: " << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}

// Acceptation d'une connexion (pour le serveur)
std::unique_ptr<LPTF_Socket> LPTF_Socket::accept_connection() {
    if (socket_fd_ == -1) {
        std::cerr << "Socket non créée" << std::endl;
        return nullptr;
    }
    
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    int client_fd = accept(socket_fd_, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
    if (client_fd == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            std::cerr << "Erreur lors de l'accept: " << strerror(errno) << std::endl;
        }
        return nullptr;
    }
    
    auto client_socket = std::make_unique<LPTF_Socket>();
    client_socket->socket_fd_ = client_fd;
    client_socket->address_ = client_addr;
    client_socket->is_server_ = false;
    client_socket->is_connected_ = true;
    
    return client_socket;
}

// Connexion au serveur (pour le client)
bool LPTF_Socket::connect_to_server() {
    if (socket_fd_ == -1) {
        std::cerr << "Socket non créée" << std::endl;
        return false;
    }
    
    if (connect(socket_fd_, reinterpret_cast<struct sockaddr*>(&address_), sizeof(address_)) == -1) {
        std::cerr << "Erreur lors de la connexion: " << strerror(errno) << std::endl;
        return false;
    }
    
    is_connected_ = true;
    return true;
}

// Envoi de données
ssize_t LPTF_Socket::send_data(const std::string& data) const {
    if (socket_fd_ == -1 || !is_connected_) {
        std::cerr << "Socket non connectée" << std::endl;
        return -1;
    }
    
    ssize_t bytes_sent = send(socket_fd_, data.c_str(), data.length(), 0);
    if (bytes_sent == -1) {
        std::cerr << "Erreur lors de l'envoi: " << strerror(errno) << std::endl;
    }
    
    return bytes_sent;
}

// Réception de données
ssize_t LPTF_Socket::receive_data(std::string& data, size_t buffer_size) const {
    if (socket_fd_ == -1 || !is_connected_) {
        std::cerr << "Socket non connectée" << std::endl;
        return -1;
    }
    
    std::vector<char> buffer(buffer_size);
    ssize_t bytes_received = recv(socket_fd_, buffer.data(), buffer_size - 1, 0);
    
    if (bytes_received == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            std::cerr << "Erreur lors de la réception: " << strerror(errno) << std::endl;
        }
        return -1;
    } else if (bytes_received == 0) {
        // Connexion fermée par le pair
        return 0;
    }
    
    buffer[bytes_received] = '\0';
    data = std::string(buffer.data(), bytes_received);
    
    return bytes_received;
}

// Configuration du mode non-bloquant
bool LPTF_Socket::set_non_blocking(bool non_blocking) {
    if (socket_fd_ == -1) {
        return false;
    }
    
    int flags = fcntl(socket_fd_, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Erreur lors de fcntl F_GETFL: " << strerror(errno) << std::endl;
        return false;
    }
    
    if (non_blocking) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }
    
    if (fcntl(socket_fd_, F_SETFL, flags) == -1) {
        std::cerr << "Erreur lors de fcntl F_SETFL: " << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}

// Vérification si la socket est prête pour la lecture
bool LPTF_Socket::is_ready_to_read() const {
    if (socket_fd_ == -1) {
        return false;
    }
    
    struct pollfd pfd;
    pfd.fd = socket_fd_;
    pfd.events = POLLIN;
    pfd.revents = 0;
    
    int result = poll(&pfd, 1, 0);
    return result > 0 && (pfd.revents & POLLIN);
}

// Vérification si la socket est prête pour l'écriture
bool LPTF_Socket::is_ready_to_write() const {
    if (socket_fd_ == -1) {
        return false;
    }
    
    struct pollfd pfd;
    pfd.fd = socket_fd_;
    pfd.events = POLLOUT;
    pfd.revents = 0;
    
    int result = poll(&pfd, 1, 0);
    return result > 0 && (pfd.revents & POLLOUT);
}

// Getters
int LPTF_Socket::get_socket_fd() const {
    return socket_fd_;
}

const std::string LPTF_Socket::get_ip() const {
    return inet_ntoa(address_.sin_addr);
}

int LPTF_Socket::get_port() const {
    return ntohs(address_.sin_port);
}

bool LPTF_Socket::get_is_server() const {
    return is_server_;
}

bool LPTF_Socket::get_is_connected() const {
    return is_connected_;
}

// Setter pour l'adresse
void LPTF_Socket::set_address(const std::string& ip, int port) {
    address_.sin_family = AF_INET;
    address_.sin_port = htons(port);
    
    if (ip.empty() || ip == "0.0.0.0") {
        address_.sin_addr.s_addr = INADDR_ANY;
    } else {
        if (inet_aton(ip.c_str(), &address_.sin_addr) == 0) {
            std::cerr << "Adresse IP invalide: " << ip << std::endl;
        }
    }
}

// Fermeture de la socket
void LPTF_Socket::close_socket() {
    if (socket_fd_ != -1) {
        close(socket_fd_);
        socket_fd_ = -1;
        is_connected_ = false;
    }
}

// Méthode statique pour gérer plusieurs sockets avec poll
std::vector<std::unique_ptr<LPTF_Socket>> LPTF_Socket::poll_sockets(
    const std::vector<std::unique_ptr<LPTF_Socket>>& sockets, 
    int timeout_ms) {
    
    std::vector<std::unique_ptr<LPTF_Socket>> ready_sockets;
    
    if (sockets.empty()) {
        return ready_sockets;
    }
    
    std::vector<struct pollfd> poll_fds;
    poll_fds.reserve(sockets.size());
    
    for (const auto& socket : sockets) {
        if (socket && socket->get_socket_fd() != -1) {
            struct pollfd pfd;
            pfd.fd = socket->get_socket_fd();
            pfd.events = POLLIN;
            pfd.revents = 0;
            poll_fds.push_back(pfd);
        }
    }
    
    int result = poll(poll_fds.data(), poll_fds.size(), timeout_ms);
    
    if (result > 0) {
        for (size_t i = 0; i < poll_fds.size() && i < sockets.size(); ++i) {
            if (poll_fds[i].revents & POLLIN) {
                // Note: On ne peut pas déplacer les sockets car elles sont const
                // Cette méthode devrait plutôt retourner les indices ou références
                // Pour cette implémentation, on retourne une copie des sockets prêtes
            }
        }
    }
    
    return ready_sockets;
}

// Méthodes privées utilitaires
void LPTF_Socket::copy_from(const LPTF_Socket& other) {
    // On ne copie pas le file descriptor car il ne peut pas être partagé
    // On copie seulement les informations de configuration
    address_ = other.address_;
    is_server_ = other.is_server_;
    is_connected_ = false; // Une copie n'est jamais connectée
    socket_fd_ = -1;
    
    // Si nécessaire, recréer la socket
    if (other.socket_fd_ != -1) {
        create_socket();
    }
}

void LPTF_Socket::move_from(LPTF_Socket&& other) noexcept {
    socket_fd_ = other.socket_fd_;
    address_ = other.address_;
    is_server_ = other.is_server_;
    is_connected_ = other.is_connected_;
    
    // Reset de l'objet source
    other.reset();
}

void LPTF_Socket::reset() {
    socket_fd_ = -1;
    is_server_ = false;
    is_connected_ = false;
    std::memset(&address_, 0, sizeof(address_));
}