#ifndef LPTF_PROTOCOL_HPP
#define LPTF_PROTOCOL_HPP

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <map>
#include <variant>

// LPTF Protocol - La Plateforme Transport Format
// Version 1.0 - Protocole binaire évolutif pour les 9 cercles

namespace LPTF {

// Types de données supportés
enum class DataType : uint8_t {
    UINT8 = 0x01,
    UINT16 = 0x02,
    UINT32 = 0x03,
    UINT64 = 0x04,
    INT8 = 0x05,
    INT16 = 0x06,
    INT32 = 0x07,
    INT64 = 0x08,
    FLOAT = 0x09,
    DOUBLE = 0x0A,
    STRING = 0x0B,
    BINARY = 0x0C,
    ARRAY = 0x0D,
    OBJECT = 0x0E
};

// Types de messages pour les différents cercles
enum class MessageType : uint16_t {
    // Cercle 1 - Communication basique
    HELLO = 0x0001,
    CHAT_MESSAGE = 0x0002,
    DISCONNECT = 0x0003,
    
    // Cercle 2 - Protocole binaire
    PROTOCOL_INFO = 0x0010,
    CAPABILITY_EXCHANGE = 0x0011,
    
    // Cercles futurs (réservés)
    AUTH_REQUEST = 0x0020,
    AUTH_RESPONSE = 0x0021,
    FILE_TRANSFER = 0x0030,
    GAME_STATE = 0x0040,
    MEDIA_STREAM = 0x0050,
    
    // Messages système
    ERROR = 0xFFFF,
    ACK = 0xFFFE,
    PING = 0xFFFD,
    PONG = 0xFFFC
};

// Flags pour le header
enum class PacketFlags : uint8_t {
    NONE = 0x00,
    COMPRESSED = 0x01,
    ENCRYPTED = 0x02,
    FRAGMENTED = 0x04,
    REQUIRES_ACK = 0x08,
    PRIORITY_HIGH = 0x10,
    PRIORITY_LOW = 0x20
};

// Structure du header LPTF (fixe 12 bytes)
struct PacketHeader {
    uint32_t magic;           // Magic number: 0x4C505446 ("LPTF")
    uint8_t version;          // Version du protocole
    uint8_t flags;            // Flags (PacketFlags)
    uint16_t message_type;    // Type du message (MessageType)
    uint32_t payload_length;  // Taille du payload en bytes
    
    PacketHeader() : magic(0x4C505446), version(1), flags(0), message_type(0), payload_length(0) {}
} __attribute__((packed));

// Valeur représentant une donnée typée
using DataValue = std::variant<
    uint8_t, uint16_t, uint32_t, uint64_t,
    int8_t, int16_t, int32_t, int64_t,
    float, double,
    std::string,
    std::vector<uint8_t>
>;

// Classe principale pour la gestion des paquets LPTF
class LPTF_Packet {
private:
    PacketHeader header_;
    std::map<std::string, DataValue> fields_;
    std::vector<uint8_t> raw_data_;
    
public:
    // Constructeurs - Forme canonique de Coplien
    LPTF_Packet();
    LPTF_Packet(MessageType type);
    LPTF_Packet(const LPTF_Packet& other);
    LPTF_Packet& operator=(const LPTF_Packet& other);
    ~LPTF_Packet();
    
    // Move semantics
    LPTF_Packet(LPTF_Packet&& other) noexcept;
    LPTF_Packet& operator=(LPTF_Packet&& other) noexcept;
    
    // Configuration du paquet
    void set_message_type(MessageType type);
    void set_flags(uint8_t flags);
    void add_flag(PacketFlags flag);
    void remove_flag(PacketFlags flag);
    
    // Ajout de données typées
    template<typename T>
    void set_field(const std::string& name, const T& value);
    
    // Méthodes spécialisées pour les types courants
    void set_string(const std::string& name, const std::string& value);
    void set_uint32(const std::string& name, uint32_t value);
    void set_uint64(const std::string& name, uint64_t value);
    void set_binary(const std::string& name, const std::vector<uint8_t>& value);
    
    // Extraction de données
    template<typename T>
    T get_field(const std::string& name) const;
    
    std::string get_string(const std::string& name) const;
    uint32_t get_uint32(const std::string& name) const;
    uint64_t get_uint64(const std::string& name) const;
    std::vector<uint8_t> get_binary(const std::string& name) const;
    
    // Vérification de l'existence des champs
    bool has_field(const std::string& name) const;
    std::vector<std::string> get_field_names() const;
    
    // Sérialisation/Désérialisation
    std::vector<uint8_t> serialize() const;
    bool deserialize(const std::vector<uint8_t>& data);
    
    // Getters pour le header
    MessageType get_message_type() const;
    uint8_t get_version() const;
    uint8_t get_flags() const;
    uint32_t get_payload_length() const;
    bool has_flag(PacketFlags flag) const;
    
    // Méthodes utilitaires
    void clear();
    size_t size() const;
    bool is_valid() const;
    std::string to_string() const; // Pour debug
    
    // Méthodes pour la compatibilité
    static bool is_compatible_version(uint8_t version);
    static std::string get_protocol_info();
    
private:
    // Méthodes privées pour la sérialisation
    void serialize_header(std::vector<uint8_t>& buffer) const;
    void serialize_field(const std::string& name, const DataValue& value, std::vector<uint8_t>& buffer) const;
    
    bool deserialize_header(const std::vector<uint8_t>& data, size_t& offset);
    bool deserialize_field(const std::vector<uint8_t>& data, size_t& offset);
    
    DataType get_data_type(const DataValue& value) const;
    size_t get_serialized_size(const DataValue& value) const;
    
    // Méthodes pour la forme canonique
    void copy_from(const LPTF_Packet& other);
    void move_from(LPTF_Packet&& other) noexcept;
    void reset();
    
    // Validation
    bool validate_magic(uint32_t magic) const;
    bool validate_version(uint8_t version) const;
};

// Classes spécialisées pour les messages courants
class ChatMessage {
public:
    static LPTF_Packet create(const std::string& username, const std::string& message, uint64_t timestamp);
    static bool parse(const LPTF_Packet& packet, std::string& username, std::string& message, uint64_t& timestamp);
};

class ProtocolInfo {
public:
    static LPTF_Packet create(uint8_t version, const std::vector<uint16_t>& supported_messages);
    static bool parse(const LPTF_Packet& packet, uint8_t& version, std::vector<uint16_t>& supported_messages);
};

// Utilitaires pour la conversion d'endianness
class ByteOrder {
public:
    static uint16_t hton16(uint16_t value);
    static uint32_t hton32(uint32_t value);
    static uint64_t hton64(uint64_t value);
    
    static uint16_t ntoh16(uint16_t value);
    static uint32_t ntoh32(uint32_t value);
    static uint64_t ntoh64(uint64_t value);
    
    static bool is_big_endian();
};

// Exceptions spécifiques au protocole
class ProtocolException : public std::exception {
private:
    std::string message_;
public:
    explicit ProtocolException(const std::string& message) : message_(message) {}
    const char* what() const noexcept override { return message_.c_str(); }
};

class SerializationException : public ProtocolException {
public:
    explicit SerializationException(const std::string& message) 
        : ProtocolException("Serialization error: " + message) {}
};

class DeserializationException : public ProtocolException {
public:
    explicit DeserializationException(const std::string& message) 
        : ProtocolException("Deserialization error: " + message) {}
};

} // namespace LPTF

#endif // LPTF_PROTOCOL_HPP
