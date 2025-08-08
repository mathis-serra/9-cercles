#include "LPTF_Protocol.hpp"
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <iomanip>

namespace LPTF {


bool ByteOrder::is_big_endian() {
    uint16_t test = 0x0102;
    uint8_t* bytes = reinterpret_cast<uint8_t*>(&test);
    return bytes[0] == 0x01;
}

uint16_t ByteOrder::hton16(uint16_t value) {
    if (is_big_endian()) return value;
    return ((value & 0xFF) << 8) | ((value >> 8) & 0xFF);
}

uint32_t ByteOrder::hton32(uint32_t value) {
    if (is_big_endian()) return value;
    return ((value & 0xFF) << 24) | 
           (((value >> 8) & 0xFF) << 16) | 
           (((value >> 16) & 0xFF) << 8) | 
           ((value >> 24) & 0xFF);
}

uint64_t ByteOrder::hton64(uint64_t value) {
    if (is_big_endian()) return value;
    return ((value & 0xFFULL) << 56) |
           (((value >> 8) & 0xFFULL) << 48) |
           (((value >> 16) & 0xFFULL) << 40) |
           (((value >> 24) & 0xFFULL) << 32) |
           (((value >> 32) & 0xFFULL) << 24) |
           (((value >> 40) & 0xFFULL) << 16) |
           (((value >> 48) & 0xFFULL) << 8) |
           ((value >> 56) & 0xFFULL);
}

uint16_t ByteOrder::ntoh16(uint16_t value) { return hton16(value); }
uint32_t ByteOrder::ntoh32(uint32_t value) { return hton32(value); }
uint64_t ByteOrder::ntoh64(uint64_t value) { return hton64(value); }


LPTF_Packet::LPTF_Packet() {
    header_.magic = 0x4C505446;
    header_.version = 1;
    header_.flags = 0;
    header_.message_type = 0;
    header_.payload_length = 0;
}

LPTF_Packet::LPTF_Packet(MessageType type) : LPTF_Packet() {
    header_.message_type = static_cast<uint16_t>(type);
}

LPTF_Packet::LPTF_Packet(const LPTF_Packet& other) {
    copy_from(other);
}

LPTF_Packet& LPTF_Packet::operator=(const LPTF_Packet& other) {
    if (this != &other) {
        copy_from(other);
    }
    return *this;
}

LPTF_Packet::~LPTF_Packet() {
   
}

LPTF_Packet::LPTF_Packet(LPTF_Packet&& other) noexcept {
    move_from(std::move(other));
}

LPTF_Packet& LPTF_Packet::operator=(LPTF_Packet&& other) noexcept {
    if (this != &other) {
        move_from(std::move(other));
    }
    return *this;
}

void LPTF_Packet::set_message_type(MessageType type) {
    header_.message_type = static_cast<uint16_t>(type);
}

void LPTF_Packet::set_flags(uint8_t flags) {
    header_.flags = flags;
}

void LPTF_Packet::add_flag(PacketFlags flag) {
    header_.flags |= static_cast<uint8_t>(flag);
}

void LPTF_Packet::remove_flag(PacketFlags flag) {
    header_.flags &= ~static_cast<uint8_t>(flag);
}

void LPTF_Packet::set_string(const std::string& name, const std::string& value) {
    fields_[name] = value;
}

void LPTF_Packet::set_uint32(const std::string& name, uint32_t value) {
    fields_[name] = value;
}

void LPTF_Packet::set_uint64(const std::string& name, uint64_t value) {
    fields_[name] = value;
}

void LPTF_Packet::set_binary(const std::string& name, const std::vector<uint8_t>& value) {
    fields_[name] = value;
}

std::string LPTF_Packet::get_string(const std::string& name) const {
    auto it = fields_.find(name);
    if (it != fields_.end()) {
        try {
            return std::get<std::string>(it->second);
        } catch (const std::bad_variant_access&) {
            throw DeserializationException("Field '" + name + "' is not a string");
        }
    }
    throw DeserializationException("Field '" + name + "' not found");
}

uint32_t LPTF_Packet::get_uint32(const std::string& name) const {
    auto it = fields_.find(name);
    if (it != fields_.end()) {
        try {
            return std::get<uint32_t>(it->second);
        } catch (const std::bad_variant_access&) {
            throw DeserializationException("Field '" + name + "' is not a uint32");
        }
    }
    throw DeserializationException("Field '" + name + "' not found");
}

uint64_t LPTF_Packet::get_uint64(const std::string& name) const {
    auto it = fields_.find(name);
    if (it != fields_.end()) {
        try {
            return std::get<uint64_t>(it->second);
        } catch (const std::bad_variant_access&) {
            throw DeserializationException("Field '" + name + "' is not a uint64");
        }
    }
    throw DeserializationException("Field '" + name + "' not found");
}

std::vector<uint8_t> LPTF_Packet::get_binary(const std::string& name) const {
    auto it = fields_.find(name);
    if (it != fields_.end()) {
        try {
            return std::get<std::vector<uint8_t>>(it->second);
        } catch (const std::bad_variant_access&) {
            throw DeserializationException("Field '" + name + "' is not binary data");
        }
    }
    throw DeserializationException("Field '" + name + "' not found");
}

bool LPTF_Packet::has_field(const std::string& name) const {
    return fields_.find(name) != fields_.end();
}

std::vector<std::string> LPTF_Packet::get_field_names() const {
    std::vector<std::string> names;
    for (const auto& pair : fields_) {
        names.push_back(pair.first);
    }
    return names;
}

MessageType LPTF_Packet::get_message_type() const {
    return static_cast<MessageType>(header_.message_type);
}

uint8_t LPTF_Packet::get_version() const {
    return header_.version;
}

uint8_t LPTF_Packet::get_flags() const {
    return header_.flags;
}

uint32_t LPTF_Packet::get_payload_length() const {
    return header_.payload_length;
}

bool LPTF_Packet::has_flag(PacketFlags flag) const {
    return (header_.flags & static_cast<uint8_t>(flag)) != 0;
}

DataType LPTF_Packet::get_data_type(const DataValue& value) const {
    return std::visit([](const auto& v) -> DataType {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, uint8_t>) return DataType::UINT8;
        else if constexpr (std::is_same_v<T, uint16_t>) return DataType::UINT16;
        else if constexpr (std::is_same_v<T, uint32_t>) return DataType::UINT32;
        else if constexpr (std::is_same_v<T, uint64_t>) return DataType::UINT64;
        else if constexpr (std::is_same_v<T, int8_t>) return DataType::INT8;
        else if constexpr (std::is_same_v<T, int16_t>) return DataType::INT16;
        else if constexpr (std::is_same_v<T, int32_t>) return DataType::INT32;
        else if constexpr (std::is_same_v<T, int64_t>) return DataType::INT64;
        else if constexpr (std::is_same_v<T, float>) return DataType::FLOAT;
        else if constexpr (std::is_same_v<T, double>) return DataType::DOUBLE;
        else if constexpr (std::is_same_v<T, std::string>) return DataType::STRING;
        else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) return DataType::BINARY;
        else return DataType::BINARY; // Fallback
    }, value);
}

size_t LPTF_Packet::get_serialized_size(const DataValue& value) const {
    return std::visit([](const auto& v) -> size_t {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, int8_t>) return 1;
        else if constexpr (std::is_same_v<T, uint16_t> || std::is_same_v<T, int16_t>) return 2;
        else if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, int32_t>) return 4;
        else if constexpr (std::is_same_v<T, uint64_t> || std::is_same_v<T, int64_t>) return 8;
        else if constexpr (std::is_same_v<T, float>) return 4;
        else if constexpr (std::is_same_v<T, double>) return 8;
        else if constexpr (std::is_same_v<T, std::string>) return v.length();
        else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) return v.size();
        else return 0;
    }, value);
}

std::vector<uint8_t> LPTF_Packet::serialize() const {
    std::vector<uint8_t> buffer;
    
    
    size_t payload_size = 0;
    for (const auto& field : fields_) {
        payload_size += 1; 
        payload_size += field.first.length(); 
        payload_size += 1; 
        payload_size += 2; 
        payload_size += get_serialized_size(field.second); 
    }
    
    
    const_cast<LPTF_Packet*>(this)->header_.payload_length = static_cast<uint32_t>(payload_size);
    
    
    buffer.reserve(sizeof(PacketHeader) + payload_size);
    
    
    serialize_header(buffer);
    
    
    for (const auto& field : fields_) {
        serialize_field(field.first, field.second, buffer);
    }
    
    return buffer;
}

void LPTF_Packet::serialize_header(std::vector<uint8_t>& buffer) const {
   
    uint32_t magic = ByteOrder::hton32(header_.magic);
    const uint8_t* magic_bytes = reinterpret_cast<const uint8_t*>(&magic);
    buffer.insert(buffer.end(), magic_bytes, magic_bytes + 4);
    
   
    buffer.push_back(header_.version);
    
    
    buffer.push_back(header_.flags);
    
    
    uint16_t msg_type = ByteOrder::hton16(header_.message_type);
    const uint8_t* type_bytes = reinterpret_cast<const uint8_t*>(&msg_type);
    buffer.insert(buffer.end(), type_bytes, type_bytes + 2);
    
   
    uint32_t payload_len = ByteOrder::hton32(header_.payload_length);
    const uint8_t* len_bytes = reinterpret_cast<const uint8_t*>(&payload_len);
    buffer.insert(buffer.end(), len_bytes, len_bytes + 4);
}

void LPTF_Packet::serialize_field(const std::string& name, const DataValue& value, std::vector<uint8_t>& buffer) const {
   
    buffer.push_back(static_cast<uint8_t>(name.length()));
    
    
    buffer.insert(buffer.end(), name.begin(), name.end());
    
  
    buffer.push_back(static_cast<uint8_t>(get_data_type(value)));
    
  
    uint16_t data_len = ByteOrder::hton16(static_cast<uint16_t>(get_serialized_size(value)));
    const uint8_t* len_bytes = reinterpret_cast<const uint8_t*>(&data_len);
    buffer.insert(buffer.end(), len_bytes, len_bytes + 2);
    
   
    std::visit([&buffer](const auto& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            buffer.insert(buffer.end(), v.begin(), v.end());
        } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
            buffer.insert(buffer.end(), v.begin(), v.end());
        } else if constexpr (std::is_arithmetic_v<T>) {
            T network_value = v;
            if constexpr (sizeof(T) == 2) {
                network_value = ByteOrder::hton16(v);
            } else if constexpr (sizeof(T) == 4) {
                if constexpr (std::is_floating_point_v<T>) {
                    uint32_t temp;
                    std::memcpy(&temp, &v, 4);
                    temp = ByteOrder::hton32(temp);
                    std::memcpy(&network_value, &temp, 4);
                } else {
                    network_value = ByteOrder::hton32(v);
                }
            } else if constexpr (sizeof(T) == 8) {
                if constexpr (std::is_floating_point_v<T>) {
                    uint64_t temp;
                    std::memcpy(&temp, &v, 8);
                    temp = ByteOrder::hton64(temp);
                    std::memcpy(&network_value, &temp, 8);
                } else {
                    network_value = ByteOrder::hton64(v);
                }
            }
            const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&network_value);
            buffer.insert(buffer.end(), bytes, bytes + sizeof(T));
        }
    }, value);
}

bool LPTF_Packet::deserialize(const std::vector<uint8_t>& data) {
    clear();
    
    if (data.size() < sizeof(PacketHeader)) {
        return false;
    }
    
    size_t offset = 0;
    
    // Désérialiser le header
    if (!deserialize_header(data, offset)) {
        return false;
    }
    
    // Vérifier la cohérence
    if (data.size() < sizeof(PacketHeader) + header_.payload_length) {
        return false;
    }
    
    // Désérialiser les fields
    size_t end_offset = sizeof(PacketHeader) + header_.payload_length;
    while (offset < end_offset) {
        if (!deserialize_field(data, offset)) {
            return false;
        }
    }
    
    return true;
}

bool LPTF_Packet::deserialize_header(const std::vector<uint8_t>& data, size_t& offset) {
    // Magic number
    if (offset + 4 > data.size()) return false;
    std::memcpy(&header_.magic, &data[offset], 4);
    header_.magic = ByteOrder::ntoh32(header_.magic);
    offset += 4;
    
    if (!validate_magic(header_.magic)) return false;
    
    // Version
    if (offset + 1 > data.size()) return false;
    header_.version = data[offset++];
    
    if (!validate_version(header_.version)) return false;
    
    // Flags
    if (offset + 1 > data.size()) return false;
    header_.flags = data[offset++];
    
    // Message type
    if (offset + 2 > data.size()) return false;
    std::memcpy(&header_.message_type, &data[offset], 2);
    header_.message_type = ByteOrder::ntoh16(header_.message_type);
    offset += 2;
    
    // Payload length
    if (offset + 4 > data.size()) return false;
    std::memcpy(&header_.payload_length, &data[offset], 4);
    header_.payload_length = ByteOrder::ntoh32(header_.payload_length);
    offset += 4;
    
    return true;
}

bool LPTF_Packet::deserialize_field(const std::vector<uint8_t>& data, size_t& offset) {
    // Name length
    if (offset + 1 > data.size()) return false;
    uint8_t name_len = data[offset++];
    
    // Name
    if (offset + name_len > data.size()) return false;
    std::string name(data.begin() + offset, data.begin() + offset + name_len);
    offset += name_len;
    
    // Data type
    if (offset + 1 > data.size()) return false;
    DataType data_type = static_cast<DataType>(data[offset++]);
    
    // Data length
    if (offset + 2 > data.size()) return false;
    uint16_t data_len;
    std::memcpy(&data_len, &data[offset], 2);
    data_len = ByteOrder::ntoh16(data_len);
    offset += 2;
    
    // Data value
    if (offset + data_len > data.size()) return false;
    
    switch (data_type) {
        case DataType::STRING: {
            std::string value(data.begin() + offset, data.begin() + offset + data_len);
            fields_[name] = value;
            break;
        }
        case DataType::UINT32: {
            if (data_len != 4) return false;
            uint32_t value;
            std::memcpy(&value, &data[offset], 4);
            value = ByteOrder::ntoh32(value);
            fields_[name] = value;
            break;
        }
        case DataType::UINT64: {
            if (data_len != 8) return false;
            uint64_t value;
            std::memcpy(&value, &data[offset], 8);
            value = ByteOrder::ntoh64(value);
            fields_[name] = value;
            break;
        }
        case DataType::BINARY: {
            std::vector<uint8_t> value(data.begin() + offset, data.begin() + offset + data_len);
            fields_[name] = value;
            break;
        }
        default:
            return false; // Type non supporté pour l'instant
    }
    
    offset += data_len;
    return true;
}

void LPTF_Packet::clear() {
    fields_.clear();
    header_.payload_length = 0;
}

size_t LPTF_Packet::size() const {
    return sizeof(PacketHeader) + header_.payload_length;
}

bool LPTF_Packet::is_valid() const {
    return validate_magic(header_.magic) && validate_version(header_.version);
}

std::string LPTF_Packet::to_string() const {
    std::ostringstream oss;
    oss << "LPTF Packet:\n";
    oss << "  Magic: 0x" << std::hex << header_.magic << std::dec << "\n";
    oss << "  Version: " << static_cast<int>(header_.version) << "\n";
    oss << "  Flags: 0x" << std::hex << static_cast<int>(header_.flags) << std::dec << "\n";
    oss << "  Message Type: " << header_.message_type << "\n";
    oss << "  Payload Length: " << header_.payload_length << "\n";
    oss << "  Fields (" << fields_.size() << "):\n";
    
    for (const auto& field : fields_) {
        oss << "    " << field.first << ": ";
        std::visit([&oss](const auto& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << v << "\"";
            } else if constexpr (std::is_arithmetic_v<T>) {
                oss << v;
            } else if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
                oss << "[" << v.size() << " bytes]";
            }
        }, field.second);
        oss << "\n";
    }
    
    return oss.str();
}

bool LPTF_Packet::is_compatible_version(uint8_t version) {
    return version == 1; // Pour l'instant, seule la v1 est supportée
}

std::string LPTF_Packet::get_protocol_info() {
    return "LPTF (La Plateforme Transport Format) v1.0";
}

void LPTF_Packet::copy_from(const LPTF_Packet& other) {
    header_ = other.header_;
    fields_ = other.fields_;
    raw_data_ = other.raw_data_;
}

void LPTF_Packet::move_from(LPTF_Packet&& other) noexcept {
    header_ = other.header_;
    fields_ = std::move(other.fields_);
    raw_data_ = std::move(other.raw_data_);
    other.reset();
}

void LPTF_Packet::reset() {
    header_ = PacketHeader();
    fields_.clear();
    raw_data_.clear();
}

bool LPTF_Packet::validate_magic(uint32_t magic) const {
    return magic == 0x4C505446;
}

bool LPTF_Packet::validate_version(uint8_t version) const {
    return version == 1;
}

// ============================================================================
// ChatMessage Helper Implementation
// ============================================================================

LPTF_Packet ChatMessage::create(const std::string& username, const std::string& message, uint64_t timestamp) {
    LPTF_Packet packet(MessageType::CHAT_MESSAGE);
    packet.set_string("username", username);
    packet.set_string("message", message);
    packet.set_uint64("timestamp", timestamp);
    return packet;
}

bool ChatMessage::parse(const LPTF_Packet& packet, std::string& username, std::string& message, uint64_t& timestamp) {
    if (packet.get_message_type() != MessageType::CHAT_MESSAGE) {
        return false;
    }
    
    try {
        username = packet.get_string("username");
        message = packet.get_string("message");
        timestamp = packet.get_uint64("timestamp");
        return true;
    } catch (const ProtocolException&) {
        return false;
    }
}

} // namespace LPTF
