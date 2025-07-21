#include "server/Server.hpp"
#include "client/Client.hpp"
#include <iostream>
#include <string>

void print_usage(const std::string& program_name) {
    std::cout << "Usage: " << program_name << " [server|client] [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Mode serveur:" << std::endl;
    std::cout << "  " << program_name << " server [ip] [port] [max_clients]" << std::endl;
    std::cout << "  Exemple: " << program_name << " server 0.0.0.0 8080 10" << std::endl;
    std::cout << std::endl;
    std::cout << "Mode client:" << std::endl;
    std::cout << "  " << program_name << " client [server_ip] [server_port]" << std::endl;
    std::cout << "  Exemple: " << program_name << " client 127.0.0.1 8080" << std::endl;
    std::cout << std::endl;
    std::cout << "Valeurs par défaut:" << std::endl;
    std::cout << "  Serveur: IP=0.0.0.0, Port=8080, Max clients=10" << std::endl;
    std::cout << "  Client: IP=127.0.0.1, Port=8080" << std::endl;
}

int run_server(int argc, char* argv[]) {
    std::string bind_ip = "0.0.0.0";
    int bind_port = 8080;
    int max_clients = 10;
    
    // Parser les arguments
    if (argc >= 3) {
        bind_ip = argv[2];
    }
    if (argc >= 4) {
        try {
            bind_port = std::stoi(argv[3]);
        } catch (const std::exception& e) {
            std::cerr << "Port invalide: " << argv[3] << std::endl;
            return 1;
        }
    }
    if (argc >= 5) {
        try {
            max_clients = std::stoi(argv[4]);
        } catch (const std::exception& e) {
            std::cerr << "Nombre maximum de clients invalide: " << argv[4] << std::endl;
            return 1;
        }
    }
    
    std::cout << "=== Serveur LPTF ===" << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  IP: " << bind_ip << std::endl;
    std::cout << "  Port: " << bind_port << std::endl;
    std::cout << "  Max clients: " << max_clients << std::endl;
    std::cout << std::endl;
    
    Server server(bind_ip, bind_port, max_clients);
    
    std::cout << "Appuyez sur Ctrl+C pour arrêter le serveur..." << std::endl;
    server.run();
    
    return 0;
}

int run_client(int argc, char* argv[]) {
    std::string server_ip = "127.0.0.1";
    int server_port = 8080;
    
    // Parser les arguments
    if (argc >= 3) {
        server_ip = argv[2];
    }
    if (argc >= 4) {
        try {
            server_port = std::stoi(argv[3]);
        } catch (const std::exception& e) {
            std::cerr << "Port invalide: " << argv[3] << std::endl;
            return 1;
        }
    }
    
    std::cout << "=== Client LPTF ===" << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Serveur IP: " << server_ip << std::endl;
    std::cout << "  Serveur Port: " << server_port << std::endl;
    std::cout << std::endl;
    
    Client client(server_ip, server_port);
    client.run_interactive();
    
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    std::string mode = argv[1];
    
    if (mode == "server") {
        return run_server(argc, argv);
    } else if (mode == "client") {
        return run_client(argc, argv);
    } else {
        std::cerr << "Mode invalide: " << mode << std::endl;
        std::cerr << "Utilisez 'server' ou 'client'" << std::endl;
        print_usage(argv[0]);
        return 1;
    }
}
