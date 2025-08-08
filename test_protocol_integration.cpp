#include "protocole/LPTF_Protocol.hpp"
#include <iostream>
#include <iomanip>

void print_hex(const std::vector<uint8_t>& data) {
    for (size_t i = 0; i < data.size(); ++i) {
        if (i % 16 == 0) std::cout << "\n" << std::setfill('0') << std::setw(4) << std::hex << i << ": ";
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)data[i] << " ";
    }
    std::cout << std::dec << std::endl;
}

int main() {
    std::cout << "=== LPTF Protocol Integration Test ===" << std::endl;
    
    // Test 1: Chat Message
    std::cout << "\n1. Testing Chat Message:" << std::endl;
    LPTF::LPTF_Packet chat_packet = LPTF::ChatMessage::create("bob", "Hello LPTF!", 1690123456789ULL);
    
    std::cout << "   Created packet: " << chat_packet.to_string() << std::endl;
    std::cout << "   Packet size: " << chat_packet.size() << " bytes" << std::endl;
    
    // Serialize
    std::vector<uint8_t> serialized = chat_packet.serialize();
    std::cout << "   Serialized size: " << serialized.size() << " bytes" << std::endl;
    std::cout << "   Hex dump:";
    print_hex(serialized);
    
    // Deserialize
    LPTF::LPTF_Packet received_packet;
    if (received_packet.deserialize(serialized)) {
        std::cout << "   ✓ Deserialization successful!" << std::endl;
        
        std::string username, message;
        uint64_t timestamp;
        if (LPTF::ChatMessage::parse(received_packet, username, message, timestamp)) {
            std::cout << "   ✓ Parsed: " << username << " says '" << message << "' at " << timestamp << std::endl;
        }
    }
    
    // Test 2: Protocol Info
    std::cout << "\n2. Testing Protocol Info:" << std::endl;
    LPTF::LPTF_Packet info_packet(LPTF::MessageType::PROTOCOL_INFO);
    info_packet.set_string("name", "LPTF");
    info_packet.set_uint32("version", 1);
    info_packet.set_string("description", "Educational Protocol");
    
    std::cout << "   " << info_packet.to_string() << std::endl;
    
    // Test 3: Ping/Pong
    std::cout << "\n3. Testing Ping Message:" << std::endl;
    LPTF::LPTF_Packet ping_packet(LPTF::MessageType::PING);
    ping_packet.set_uint64("timestamp", 1690123456789ULL);
    ping_packet.add_flag(LPTF::PacketFlags::REQUIRES_ACK);
    
    std::cout << "   " << ping_packet.to_string() << std::endl;
    std::cout << "   Has REQUIRES_ACK flag: " << (ping_packet.has_flag(LPTF::PacketFlags::REQUIRES_ACK) ? "Yes" : "No") << std::endl;
    
    std::cout << "\n=== All Tests Completed Successfully! ===" << std::endl;
    return 0;
}
