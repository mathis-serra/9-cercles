#ifndef BINAIRE_HPP
#define BINAIRE_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <cstring>

class Binaire {
public:
    Binaire();
    Binaire(const Binaire& other) = default;
    Binaire& operator=(const Binaire& other) = default;
    ~Binaire();

    // Méthodes pour encoder et décoder des données
    std::vector<uint8_t> encoder(const std::string& message);
    std::string decoder(const std::vector<uint8_t>& data);

private:
    // Méthodes utilitaires
    uint32_t to_uint32(const uint8_t* bytes);
    void from_uint32(uint32_t value, uint8_t* bytes);
};

#endif // BINAIRE_HPP