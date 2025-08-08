#include "LPTF_Protocol.hpp"
#include <string>
#include <iostream>
#include <cstring>
#include <algorithm>

using namespace std;
using namespace LPTF;

typedef struct {
    std::string username;
    std::string message;
    uint64_t timestamp; 
} ChatMessage_Legacy;

LPTF_Packet convert_to_lptf(const ChatMessage_Legacy& legacy) {
    LPTF_Packet packet(MessageType::CHAT_MESSAGE);
    
    packet.set_string("username", legacy.username);
    packet.set_string("message", legacy.message);
    packet.set_uint64("timestamp", legacy.timestamp);
    
    return packet;
}

ChatMessage_Legacy convert_from_lptf(const LPTF_Packet& packet) {
    ChatMessage_Legacy legacy;
    
    if (packet.get_message_type() == MessageType::CHAT_MESSAGE) {
        legacy.username = packet.get_string("username");
        legacy.message = packet.get_string("message");
        legacy.timestamp = packet.get_uint64("timestamp");
    }
    
    return legacy;
}

int main() {
    std::cout << "LPTF Protocol Test" << std::endl;
    
    ChatMessage_Legacy legacy_msg;
    legacy_msg.username = "alice";
    legacy_msg.message = "Hello world!";
    legacy_msg.timestamp = 1690123456789ULL;
    
    std::cout << "Original: " << legacy_msg.username << " - " << legacy_msg.message << std::endl;
    
    LPTF_Packet packet = convert_to_lptf(legacy_msg);
    
    std::cout << "Packet size: " << packet.size() << " bytes" << std::endl;
    
    std::vector<uint8_t> serialized = packet.serialize();
    std::cout << "Serialized: " << serialized.size() << " bytes" << std::endl;
    
    LPTF_Packet received_packet;
    if (received_packet.deserialize(serialized)) {
        std::cout << "Deserialization successful!" << std::endl;
        
        ChatMessage_Legacy received_legacy = convert_from_lptf(received_packet);
        
        std::cout << "Received: " << received_legacy.username << " - " << received_legacy.message << std::endl;
        
        bool success = (legacy_msg.username == received_legacy.username &&
                       legacy_msg.message == received_legacy.message &&
                       legacy_msg.timestamp == received_legacy.timestamp);
        
        std::cout << "Test: " << (success ? "PASS" : "FAIL") << std::endl;
    } else {
        std::cout << "Deserialization failed!" << std::endl;
    }
    
    return 0;
}




