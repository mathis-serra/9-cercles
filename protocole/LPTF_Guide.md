# Protocole LPTF - Guide d'Implémentation

## Vue d'Ensemble

Le protocole LPTF (La Plateforme Transport Format) est un protocole binaire conçu pour supporter l'apprentissage progressif des systèmes distribués à travers 9 cercles de complexité croissante.

## Architecture du Protocole

### Structure des Messages

```
+------------------+
|   Header (12B)   |
+------------------+
|                  |
|   Payload        |
|   (Variable)     |
|                  |
+------------------+
```

### Header Format (12 bytes fixes)

| Offset | Taille | Champ           | Description                    |
|--------|--------|-----------------|--------------------------------|
| 0      | 4      | Magic Number    | 0x4C505446 ("LPTF")          |
| 4      | 1      | Version         | Version du protocole (1)      |
| 5      | 1      | Flags           | Flags de contrôle             |
| 6      | 2      | Message Type    | Type du message               |
| 8      | 4      | Payload Length  | Taille du payload en bytes    |

### Format des Champs (Payload)

Chaque champ dans le payload suit ce format :

```
+----------+---------------+----------+-------------+-----------+
| Name Len | Field Name    | Data Type| Data Length | Data Value|
| (1 byte) | (variable)    | (1 byte) | (2 bytes)   | (variable)|
+----------+---------------+----------+-------------+-----------+
```

## Types de Données Supportés

| Type ID | Nom      | Taille    | Description                        |
|---------|----------|-----------|------------------------------------|
| 0x01    | UINT8    | 1 byte    | Entier non signé 8 bits           |
| 0x02    | UINT16   | 2 bytes   | Entier non signé 16 bits          |
| 0x03    | UINT32   | 4 bytes   | Entier non signé 32 bits          |
| 0x04    | UINT64   | 8 bytes   | Entier non signé 64 bits          |
| 0x05    | INT8     | 1 byte    | Entier signé 8 bits               |
| 0x06    | INT16    | 2 bytes   | Entier signé 16 bits              |
| 0x07    | INT32    | 4 bytes   | Entier signé 32 bits              |
| 0x08    | INT64    | 8 bytes   | Entier signé 64 bits              |
| 0x09    | FLOAT    | 4 bytes   | Flottant simple précision IEEE 754|
| 0x0A    | DOUBLE   | 8 bytes   | Flottant double précision IEEE 754|
| 0x0B    | STRING   | Variable  | Chaîne UTF-8                      |
| 0x0C    | BINARY   | Variable  | Données binaires brutes           |

## Types de Messages par Cercle

### Cercle 1 - Communication Basique (0x0001-0x000F)
- `0x0001 HELLO` : Message de connexion initiale
- `0x0002 CHAT_MESSAGE` : Message de chat entre clients
- `0x0003 DISCONNECT` : Déconnexion gracieuse

### Cercle 2 - Protocole Binaire (0x0010-0x001F)
- `0x0010 PROTOCOL_INFO` : Échange d'informations sur le protocole
- `0x0011 CAPABILITY_EXCHANGE` : Négociation des fonctionnalités

### Cercles Futurs (Réservés)
- `0x0020-0x002F` : Authentification et autorisation
- `0x0030-0x003F` : Transfert de fichiers
- `0x0040-0x004F` : États de jeu temps réel
- `0x0050-0x005F` : Streaming média
- Et ainsi de suite jusqu'au cercle 9...

### Messages Système (0xFFFC-0xFFFF)
- `0xFFFF ERROR` : Notification d'erreur
- `0xFFFE ACK` : Accusé de réception
- `0xFFFD PING` : Sonde keep-alive
- `0xFFFC PONG` : Réponse keep-alive

## Flags de Contrôle

| Bit | Nom          | Description                    |
|-----|--------------|--------------------------------|
| 0   | COMPRESSED   | Payload compressé              |
| 1   | ENCRYPTED    | Payload chiffré                |
| 2   | FRAGMENTED   | Message fragmenté              |
| 3   | REQUIRES_ACK | Nécessite un accusé de réception|
| 4   | PRIORITY_HIGH| Priorité haute                 |
| 5   | PRIORITY_LOW | Priorité basse                 |
| 6-7 | RESERVED     | Réservé pour usage futur       |

## Évolutivité et Compatibilité

### Gestion des Versions
- Le champ version permet la détection de compatibilité
- Les implémentations doivent supporter au moins leur propre version
- Idéalement, support d'au moins une version précédente

### Extensibilité
- Les types de messages inconnus sont ignorés
- Les champs inconnus dans des messages connus sont ignorés
- Les types de données inconnus provoquent une erreur

### Migration
- Ajout de nouveaux champs : compatible
- Suppression de champs : nécessite gestion spéciale
- Changement de type : incompatible (nouvelle version)

## Exemple d'Utilisation

### Création d'un Message de Chat

```cpp
#include "LPTF_Protocol.hpp"

// Méthode 1 : Utilisation directe
LPTF::LPTF_Packet packet(LPTF::MessageType::CHAT_MESSAGE);
packet.set_string("username", "alice");
packet.set_string("message", "Hello World!");
packet.set_uint64("timestamp", 1690123456789ULL);

// Méthode 2 : Utilisation de l'helper
LPTF::LPTF_Packet packet2 = LPTF::ChatMessage::create("alice", "Hello World!", 1690123456789ULL);
```

### Sérialisation et Envoi

```cpp
// Sérialisation
std::vector<uint8_t> data = packet.serialize();

// Envoi via socket (exemple)
std::string str_data(data.begin(), data.end());
socket.send_data(str_data);
```

### Réception et Désérialisation

```cpp
// Réception
std::string received_data;
socket.receive_data(received_data);

// Désérialisation
std::vector<uint8_t> data(received_data.begin(), received_data.end());
LPTF::LPTF_Packet packet;
if (packet.deserialize(data)) {
    // Traitement du message
    if (packet.get_message_type() == LPTF::MessageType::CHAT_MESSAGE) {
        std::string username = packet.get_string("username");
        std::string message = packet.get_string("message");
        uint64_t timestamp = packet.get_uint64("timestamp");
        
        std::cout << username << ": " << message << std::endl;
    }
}
```

### Gestion des Erreurs

```cpp
try {
    LPTF::LPTF_Packet packet;
    packet.deserialize(data);
    
    std::string value = packet.get_string("username");
} catch (const LPTF::DeserializationException& e) {
    std::cerr << "Erreur de désérialisation: " << e.what() << std::endl;
} catch (const LPTF::ProtocolException& e) {
    std::cerr << "Erreur de protocole: " << e.what() << std::endl;
}
```

## Considérations de Performance

### Optimisations Recommandées
1. **Memory Pooling** : Réutiliser les buffers de sérialisation
2. **Zero-Copy** : Éviter les copies inutiles de données
3. **Compression** : Activer pour les gros messages (flag COMPRESSED)
4. **Batch Processing** : Traiter plusieurs messages ensemble

### Métriques à Surveiller
- Taille moyenne des messages sérialisés
- Temps de sérialisation/désérialisation
- Overhead du protocole (header/payload ratio)
- Throughput réseau

## Sécurité

### Validations Obligatoires
- Magic number du header
- Version du protocole
- Cohérence des tailles (payload length)
- Validation des types de données
- Bounds checking sur tous les accès mémoire

### Recommandations
- Limiter la taille maximale des messages
- Implémenter un rate limiting
- Valider tous les champs métier
- Utiliser TLS pour le transport si nécessaire

## Debugging et Diagnostic

### Outils Fournis
- `packet.to_string()` : Affichage lisible du contenu
- `packet.is_valid()` : Validation basique
- `LPTF::LPTF_Packet::get_protocol_info()` : Information sur le protocole

### Techniques de Debug
1. Dump hexadécimal des données sérialisées
2. Logs des types de messages échangés
3. Validation de round-trip (serialize -> deserialize)
4. Comparaison avec des traces de référence

## Extensions Futures

Le protocole est conçu pour évoluer. Les extensions prévues incluent :

1. **Compression** : Intégration d'algorithmes de compression
2. **Chiffrement** : Support du chiffrement au niveau protocole
3. **Fragmentation** : Support des messages volumineux
4. **Streaming** : Support des flux de données
5. **Multiplexing** : Plusieurs flux sur une connexion

## Conformité RFC

Ce protocole suit les conventions de la RFC 9999 (voir LPTF_RFC.txt) et respecte les standards d'ingénierie des protocoles réseau.

---

**Version du document** : 1.0  
**Date** : Juillet 2025  
**Auteur** : Équipe étudiante La Plateforme
