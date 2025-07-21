#include "client/Client.hpp"
#include <iostream>

int main() {
    std::cout << "=== Test Client Simple ===" << std::endl;
    
    // Créer un client
    Client client("127.0.0.1", 8080);
    
    // Se connecter au serveur
    if (!client.connect_to_server()) {
        std::cerr << "Impossible de se connecter au serveur" << std::endl;
        return 1;
    }
    
    // Envoyer un message
    std::string message = "Hello from test client!";
    if (client.send_message(message)) {
        std::cout << "Message envoyé avec succès" << std::endl;
    }
    
    // Attendre une réponse
    std::string response;
    if (client.receive_message(response)) {
        std::cout << "Réponse reçue avec succès" << std::endl;
    }
    
    // Envoyer un second message
    message = "Second message";
    client.send_message(message);
    
    // Attendre une réponse
    client.receive_message(response);
    
    // Déconnexion propre
    client.disconnect();
    
    return 0;
}
