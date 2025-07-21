#include "Server.hpp"
#include <iostream>
#include <algorithm>
#include <poll.h>
#include <unistd.h>

// Constructeur par défaut
Server::Server() 
    : server_socket_(nullptr), bind_ip_("0.0.0.0"), bind_port_(8080), 
      is_running_(false), max_clients_(10) {
}

// Constructeur paramétré
Server::Server(const std::string& bind_ip, int bind_port, int max_clients)
    : server_socket_(nullptr), bind_ip_(bind_ip), bind_port_(bind_port), 
      is_running_(false), max_clients_(max_clients) {
}

// Constructeur de copie
Server::Server(const Server& other) 
    : server_socket_(nullptr), bind_ip_(""), bind_port_(0), 
      is_running_(false), max_clients_(0) {
    copy_from(other);
}

// Opérateur d'assignation
Server& Server::operator=(const Server& other) {
    if (this != &other) {
        stop_server();
        copy_from(other);
    }
    return *this;
}

// Constructeur de move
Server::Server(Server&& other) noexcept 
    : server_socket_(nullptr), bind_ip_(""), bind_port_(0), 
      is_running_(false), max_clients_(0) {
    move_from(std::move(other));
}

// Opérateur d'assignation de move
Server& Server::operator=(Server&& other) noexcept {
    if (this != &other) {
        stop_server();
        move_from(std::move(other));
    }
    return *this;
}

// Destructeur
Server::~Server() {
    stop_server();
}

// Démarrage du serveur
bool Server::start_server() {
    if (is_running_) {
        std::cout << "Le serveur est déjà en cours d'exécution" << std::endl;
        return true;
    }
    
    server_socket_ = std::make_unique<LPTF_Socket>(bind_ip_, bind_port_, true);
    
    if (!server_socket_->create_socket()) {
        std::cerr << "Erreur lors de la création de la socket serveur" << std::endl;
        return false;
    }
    
    if (!server_socket_->bind_socket()) {
        std::cerr << "Erreur lors du bind de la socket serveur" << std::endl;
        return false;
    }
    
    if (!server_socket_->listen_socket()) {
        std::cerr << "Erreur lors de la mise en écoute de la socket serveur" << std::endl;
        return false;
    }
    
    // Configuration en mode non-bloquant pour accepter les connexions sans bloquer
    if (!server_socket_->set_non_blocking(true)) {
        std::cerr << "Erreur lors de la configuration non-bloquante" << std::endl;
        return false;
    }
    
    is_running_ = true;
    std::cout << "Serveur démarré sur " << bind_ip_ << ":" << bind_port_ << std::endl;
    std::cout << "En attente de connexions clients..." << std::endl;
    
    return true;
}

// Arrêt du serveur
void Server::stop_server() {
    if (!is_running_) {
        return;
    }
    
    is_running_ = false;
    
    // Fermer toutes les connexions clients
    client_sockets_.clear();
    client_info_.clear();
    
    // Fermer la socket serveur
    if (server_socket_) {
        server_socket_->close_socket();
        server_socket_.reset();
    }
    
    std::cout << "Serveur arrêté" << std::endl;
}

// Boucle principale du serveur
void Server::run() {
    if (!start_server()) {
        return;
    }
    
    while (is_running_) {
        // Préparer les descripteurs pour poll
        std::vector<struct pollfd> poll_fds;
        
        // Ajouter la socket serveur
        struct pollfd server_pfd;
        server_pfd.fd = server_socket_->get_socket_fd();
        server_pfd.events = POLLIN;
        server_pfd.revents = 0;
        poll_fds.push_back(server_pfd);
        
        // Ajouter toutes les sockets clients
        for (const auto& client : client_sockets_) {
            if (client && client->get_socket_fd() != -1) {
                struct pollfd client_pfd;
                client_pfd.fd = client->get_socket_fd();
                client_pfd.events = POLLIN;
                client_pfd.revents = 0;
                poll_fds.push_back(client_pfd);
            }
        }
        
        // Attendre des événements (timeout de 1 seconde)
        int poll_result = poll(poll_fds.data(), poll_fds.size(), 1000);
        
        if (poll_result == -1) {
            std::cerr << "Erreur lors du poll" << std::endl;
            break;
        }
        
        if (poll_result == 0) {
            // Timeout - continuer la boucle
            continue;
        }
        
        // Vérifier la socket serveur pour de nouvelles connexions
        if (poll_fds[0].revents & POLLIN) {
            handle_new_connection();
        }
        
        // Vérifier les sockets clients pour des messages
        for (size_t i = 1; i < poll_fds.size(); ++i) {
            if (poll_fds[i].revents & POLLIN) {
                // Trouver la socket client correspondante
                int client_fd = poll_fds[i].fd;
                auto it = std::find_if(client_sockets_.begin(), client_sockets_.end(),
                    [client_fd](const std::unique_ptr<LPTF_Socket>& socket) {
                        return socket && socket->get_socket_fd() == client_fd;
                    });
                
                if (it != client_sockets_.end()) {
                    handle_client_message(**it);
                }
            }
        }
        
        // Nettoyer les clients déconnectés
        cleanup_disconnected_clients();
    }
}

// Gestion d'une nouvelle connexion
void Server::handle_new_connection() {
    if (static_cast<int>(client_sockets_.size()) >= max_clients_) {
        std::cout << "Nombre maximum de clients atteint, connexion refusée" << std::endl;
        return;
    }
    
    auto new_client = server_socket_->accept_connection();
    if (new_client) {
        // Configurer le client en mode non-bloquant
        new_client->set_non_blocking(true);
        
        int client_fd = new_client->get_socket_fd();
        std::string client_info = new_client->get_ip() + ":" + std::to_string(new_client->get_port());
        
        client_sockets_.push_back(std::move(new_client));
        client_info_[client_fd] = client_info;
        
        std::cout << "Nouveau client connecté: " << client_info 
                  << " (Total: " << client_sockets_.size() << ")" << std::endl;
        
        // Envoyer un message de bienvenue
        std::string welcome_msg = "Bienvenue sur le serveur LPTF !";
        client_sockets_.back()->send_data(welcome_msg);
        
        // Notifier les autres clients
        std::string notification = "Un nouveau client s'est connecté: " + client_info;
        broadcast_message(notification, client_fd);
    }
}

// Gestion d'un message client
void Server::handle_client_message(LPTF_Socket& client_socket) {
    std::string message;
    ssize_t bytes_received = client_socket.receive_data(message);
    
    if (bytes_received > 0) {
        int client_fd = client_socket.get_socket_fd();
        std::string client_info = client_info_[client_fd];
        
        std::cout << "Message de " << client_info << ": " << message << std::endl;
        
        // Echo du message à tous les clients (y compris l'expéditeur)
        std::string echo_msg = "[" + client_info + "]: " + message;
        broadcast_message(echo_msg);
        
    } else if (bytes_received == 0) {
        // Client déconnecté
        int client_fd = client_socket.get_socket_fd();
        std::string client_info = client_info_[client_fd];
        
        std::cout << "Client déconnecté: " << client_info << std::endl;
        
        // Notifier les autres clients
        std::string notification = "Le client " + client_info + " s'est déconnecté";
        broadcast_message(notification, client_fd);
        
        remove_client(client_fd);
    }
}

// Suppression d'un client
void Server::remove_client(int client_fd) {
    // Supprimer de la map d'informations
    client_info_.erase(client_fd);
    
    // Supprimer du vecteur de sockets
    client_sockets_.erase(
        std::remove_if(client_sockets_.begin(), client_sockets_.end(),
            [client_fd](const std::unique_ptr<LPTF_Socket>& socket) {
                return socket && socket->get_socket_fd() == client_fd;
            }),
        client_sockets_.end()
    );
    
    std::cout << "Client supprimé (Total: " << client_sockets_.size() << ")" << std::endl;
}

// Diffusion d'un message à tous les clients
void Server::broadcast_message(const std::string& message, int sender_fd) {
    for (const auto& client : client_sockets_) {
        if (client && client->get_socket_fd() != -1 && client->get_socket_fd() != sender_fd) {
            client->send_data(message);
        }
    }
}

// Getters
const std::string& Server::get_bind_ip() const {
    return bind_ip_;
}

int Server::get_bind_port() const {
    return bind_port_;
}

bool Server::get_is_running() const {
    return is_running_;
}

int Server::get_max_clients() const {
    return max_clients_;
}

size_t Server::get_client_count() const {
    return client_sockets_.size();
}

// Setters
void Server::set_bind_info(const std::string& ip, int port) {
    if (is_running_) {
        std::cerr << "Impossible de changer les informations de bind pendant que le serveur fonctionne" << std::endl;
        return;
    }
    
    bind_ip_ = ip;
    bind_port_ = port;
}

void Server::set_max_clients(int max_clients) {
    max_clients_ = max_clients;
}

// Méthodes privées
void Server::copy_from(const Server& other) {
    bind_ip_ = other.bind_ip_;
    bind_port_ = other.bind_port_;
    max_clients_ = other.max_clients_;
    is_running_ = false; // Une copie n'est jamais en cours d'exécution
    // Ne pas copier les sockets actives
}

void Server::move_from(Server&& other) noexcept {
    server_socket_ = std::move(other.server_socket_);
    client_sockets_ = std::move(other.client_sockets_);
    client_info_ = std::move(other.client_info_);
    bind_ip_ = std::move(other.bind_ip_);
    bind_port_ = other.bind_port_;
    is_running_ = other.is_running_;
    max_clients_ = other.max_clients_;
    
    // Reset de l'objet source
    other.reset();
}

void Server::reset() {
    server_socket_.reset();
    client_sockets_.clear();
    client_info_.clear();
    bind_ip_ = "";
    bind_port_ = 0;
    is_running_ = false;
    max_clients_ = 0;
}

void Server::cleanup_disconnected_clients() {
    client_sockets_.erase(
        std::remove_if(client_sockets_.begin(), client_sockets_.end(),
            [this](const std::unique_ptr<LPTF_Socket>& socket) {
                if (!socket || socket->get_socket_fd() == -1) {
                    return true;
                }
                // Vérifier si la socket est encore valide
                if (!socket->get_is_connected()) {
                    int fd = socket->get_socket_fd();
                    client_info_.erase(fd);
                    return true;
                }
                return false;
            }),
        client_sockets_.end()
    );
}
