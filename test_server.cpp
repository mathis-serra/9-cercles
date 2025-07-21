#include "server/Server.hpp"
#include <iostream>
#include <signal.h>

Server* global_server = nullptr;

void signal_handler(int signal) {
    if (signal == SIGINT && global_server) {
        std::cout << "\nArrêt du serveur..." << std::endl;
        global_server->stop_server();
    }
}

int main() {
    std::cout << "=== Test Serveur Simple ===" << std::endl;
    
    // Créer le serveur
    Server server("0.0.0.0", 8080, 5);
    global_server = &server;
    
    // Gérer Ctrl+C
    signal(SIGINT, signal_handler);
    
    // Lancer le serveur
    std::cout << "Démarrage du serveur..." << std::endl;
    server.run();
    
    std::cout << "Serveur arrêté." << std::endl;
    return 0;
}
