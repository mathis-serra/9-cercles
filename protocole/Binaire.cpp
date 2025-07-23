#include "LPTF_Protocol.hpp"
#include <string>
#include <iostream>
#include <cstring>
#include <algorithm>

using namespace std;
using namespace LPTF;

// Exemple d'utilisation du protocole LPTF avec votre structure ChatMessage
typedef struct {
    std::string username;
    std::string message;
    uint64_t timestamp; 
} ChatMessage_Legacy;

// Démonstration de conversion Legacy -> LPTF
LPTF_Packet convert_to_lptf(const ChatMessage_Legacy& legacy) {
    LPTF_Packet packet(MessageType::CHAT_MESSAGE);
    
    packet.set_string("username", legacy.username);
    packet.set_string("message", legacy.message);
    packet.set_uint64("timestamp", legacy.timestamp);
    
    return packet;
}

// Démonstration de conversion LPTF -> Legacy
ChatMessage_Legacy convert_from_lptf(const LPTF_Packet& packet) {
    ChatMessage_Legacy legacy;
    
    if (packet.get_message_type() == MessageType::CHAT_MESSAGE) {
        legacy.username = packet.get_string("username");
        legacy.message = packet.get_string("message");
        legacy.timestamp = packet.get_uint64("timestamp");
    }
    
    return legacy;
}

// Exemple d'utilisation
int main() {
    std::cout << "=== Démonstration du Protocole LPTF ===" << std::endl;
    
    // Créer un message legacy
    ChatMessage_Legacy legacy_msg;
    legacy_msg.username = "alice";
    legacy_msg.message = "Bonjour le monde!";
    legacy_msg.timestamp = 1690123456789ULL;
    
    std::cout << "Message original:" << std::endl;
    std::cout << "  Username: " << legacy_msg.username << std::endl;
    std::cout << "  Message: " << legacy_msg.message << std::endl;
    std::cout << "  Timestamp: " << legacy_msg.timestamp << std::endl;
    
    // Conversion en LPTF
    LPTF_Packet packet = convert_to_lptf(legacy_msg);
    
    std::cout << "\nInformations du paquet LPTF:" << std::endl;
    std::cout << "  Type: " << static_cast<uint16_t>(packet.get_message_type()) << std::endl;
    std::cout << "  Version: " << static_cast<uint16_t>(packet.get_version()) << std::endl;
    std::cout << "  Taille: " << packet.size() << " bytes" << std::endl;
    
    // Sérialisation
    std::vector<uint8_t> serialized = packet.serialize();
    std::cout << "  Données sérialisées: " << serialized.size() << " bytes" << std::endl;
    
    // Désérialisation
    LPTF_Packet received_packet;
    if (received_packet.deserialize(serialized)) {
        std::cout << "\nDésérialisation réussie!" << std::endl;
        
        // Conversion retour
        ChatMessage_Legacy received_legacy = convert_from_lptf(received_packet);
        
        std::cout << "Message reçu:" << std::endl;
        std::cout << "  Username: " << received_legacy.username << std::endl;
        std::cout << "  Message: " << received_legacy.message << std::endl;
        std::cout << "  Timestamp: " << received_legacy.timestamp << std::endl;
        
        // Vérification
        bool success = (legacy_msg.username == received_legacy.username &&
                       legacy_msg.message == received_legacy.message &&
                       legacy_msg.timestamp == received_legacy.timestamp);
        
        std::cout << "\nTest de cohérence: " << (success ? "SUCCÈS" : "ÉCHEC") << std::endl;
    } else {
        std::cout << "Erreur de désérialisation!" << std::endl;
    }
    
    return 0;
}




