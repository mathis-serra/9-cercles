#include "../server/LPTF_socket.hpp"
#include "LPTF_Protocol.hpp"
#include <iostream>

// Exemple d'intégration du protocole LPTF avec votre socket
class LPTF_Server {
private:
    std::unique_ptr<LPTF_Socket> server_socket_;
    std::vector<std::unique_ptr<LPTF_Socket>> clients_;
    
public:
    LPTF_Server(const std::string& ip, int port) {
        server_socket_ = std::make_unique<LPTF_Socket>(ip, port, true);
    }
    
    bool start() {
        if (!server_socket_->create_socket()) return false;
        if (!server_socket_->bind_socket()) return false;
        if (!server_socket_->listen_socket()) return false;
        
        std::cout << "Serveur LPTF démarré avec protocole binaire" << std::endl;
        std::cout << "Protocole: " << LPTF::LPTF_Packet::get_protocol_info() << std::endl;
        
        return true;
    }
    
    // Envoyer un paquet LPTF via socket
    bool send_packet(LPTF_Socket& socket, const LPTF::LPTF_Packet& packet) {
        try {
            std::vector<uint8_t> data = packet.serialize();
            std::string str_data(data.begin(), data.end());
            return socket.send_data(str_data) > 0;
        } catch (const LPTF::ProtocolException& e) {
            std::cerr << "Erreur protocole: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Recevoir un paquet LPTF via socket
    bool receive_packet(LPTF_Socket& socket, LPTF::LPTF_Packet& packet) {
        std::string str_data;
        ssize_t received = socket.receive_data(str_data);
        
        if (received <= 0) return false;
        
        std::vector<uint8_t> data(str_data.begin(), str_data.end());
        return packet.deserialize(data);
    }
    
    // Diffuser un message chat à tous les clients
    void broadcast_chat(const std::string& username, const std::string& message, uint64_t timestamp) {
        LPTF::LPTF_Packet packet = LPTF::ChatMessage::create(username, message, timestamp);
        
        for (auto& client : clients_) {
            if (client && client->get_is_connected()) {
                send_packet(*client, packet);
            }
        }
    }
    
    // Envoyer information sur le protocole
    void send_protocol_info(LPTF_Socket& client) {
        LPTF::LPTF_Packet packet(LPTF::MessageType::PROTOCOL_INFO);
        packet.set_uint8("version", 1);
        packet.set_string("protocol_name", "LPTF");
        packet.set_string("description", "La Plateforme Transport Format");
        
        std::vector<uint16_t> supported_types = {
            static_cast<uint16_t>(LPTF::MessageType::HELLO),
            static_cast<uint16_t>(LPTF::MessageType::CHAT_MESSAGE),
            static_cast<uint16_t>(LPTF::MessageType::PROTOCOL_INFO),
            static_cast<uint16_t>(LPTF::MessageType::PING),
            static_cast<uint16_t>(LPTF::MessageType::PONG)
        };
        
        // Convertir en binary data
        std::vector<uint8_t> types_data;
        for (uint16_t type : supported_types) {
            types_data.push_back((type >> 8) & 0xFF);
            types_data.push_back(type & 0xFF);
        }
        packet.set_binary("supported_types", types_data);
        
        send_packet(client, packet);
    }
};

// Exemple d'utilisation
int main() {
    std::cout << "=== Exemple d'Intégration LPTF avec Socket ===" << std::endl;
    
    // Créer différents types de paquets
    
    // 1. Message de chat
    LPTF::LPTF_Packet chat_packet = LPTF::ChatMessage::create("bob", "Hello LPTF!", 1690123456789ULL);
    std::cout << "\n" << chat_packet.to_string() << std::endl;
    
    // 2. Message d'information protocole
    LPTF::LPTF_Packet info_packet(LPTF::MessageType::PROTOCOL_INFO);
    info_packet.set_string("protocol", "LPTF");
    info_packet.set_uint8("version", 1);
    std::cout << info_packet.to_string() << std::endl;
    
    // 3. Message de ping
    LPTF::LPTF_Packet ping_packet(LPTF::MessageType::PING);
    ping_packet.set_uint64("timestamp", 1690123456789ULL);
    ping_packet.add_flag(LPTF::PacketFlags::REQUIRES_ACK);
    std::cout << ping_packet.to_string() << std::endl;
    
    // Test de sérialisation/désérialisation
    std::vector<uint8_t> serialized = chat_packet.serialize();
    std::cout << "Taille sérialisée: " << serialized.size() << " bytes" << std::endl;
    
    LPTF::LPTF_Packet deserialized;
    if (deserialized.deserialize(serialized)) {
        std::cout << "Désérialisation réussie!" << std::endl;
        
        // Vérifier les données
        std::string username, message;
        uint64_t timestamp;
        if (LPTF::ChatMessage::parse(deserialized, username, message, timestamp)) {
            std::cout << "Message parsé: " << username << " dit '" << message << "' à " << timestamp << std::endl;
        }
    }
    
    return 0;
}
