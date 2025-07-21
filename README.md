# Projet LPTF Socket - Communication Client/Serveur

Ce projet implémente un système de communication réseau en C++ avec une classe `LPTF_Socket` qui encapsule tous les syscalls réseau nécessaires.

## Fonctionnalités

- **Classe LPTF_Socket** : Encapsulation complète des syscalls réseau
- **Serveur multi-clients** : Capable de gérer plusieurs clients simultanément sans threads/fork
- **Client interactif** : Interface console pour envoyer des messages
- **Communication bidirectionnelle** : Messages entre clients via le serveur
- **Respect des bonnes pratiques C++** :
  - Forme canonique de Coplien
  - Smart pointers
  - Variables const par défaut
  - Attributs privés avec getters/setters
  - Pas de variables globales

## Architecture

### Classes principales

1. **LPTF_Socket** : Classe de base pour la communication réseau
   - Encapsule socket(), bind(), listen(), accept(), connect(), send(), recv()
   - Support du mode non-bloquant
   - Gestion des erreurs intégrée

2. **Server** : Serveur multi-clients
   - Utilise poll() pour gérer plusieurs connexions
   - Diffusion de messages entre clients
   - Gestion des connexions/déconnexions

3. **Client** : Client avec interface interactive
   - Connexion au serveur
   - Envoi/réception de messages
   - Interface console conviviale

## Compilation

### Avec le Makefile (recommandé)
```bash
make
```

### Compilation manuelle
```bash
clang++ -std=c++11 -Wall -Wextra -g main.cpp server/LPTF_socket.cpp server/Server.cpp client/Client.cpp -o main
```

## Utilisation

### Lancer le serveur
```bash
# Serveur par défaut (0.0.0.0:8080, max 10 clients)
./main server

# Serveur personnalisé
./main server 127.0.0.1 9090 5
```

### Lancer un client
```bash
# Client par défaut (connexion à 127.0.0.1:8080)
./main client

# Client personnalisé
./main client 192.168.1.100 9090
```

## Test de fonctionnement

1. **Démarrer le serveur** :
   ```bash
   ./main server
   ```

2. **Connecter plusieurs clients** (dans des terminaux séparés) :
   ```bash
   ./main client
   ```

3. **Tester la communication** :
   - Tapez des messages dans un client
   - Vérifiez qu'ils apparaissent sur les autres clients
   - Testez les connexions/déconnexions

## Fonctionnalités avancées

### Gestion multi-clients sans threads
Le serveur utilise `poll()` pour surveiller simultanément :
- La socket serveur (nouvelles connexions)
- Toutes les sockets clients (messages entrants)

### Mode non-bloquant
- Les sockets sont configurées en mode non-bloquant
- Évite les blocages lors des accept() et recv()
- Permet une gestion fluide de multiples connexions

### Robustesse
- Gestion des erreurs réseau
- Nettoyage automatique des clients déconnectés
- Protection contre les buffers pleins

## Structure des fichiers

```
.
├── main.cpp                 # Point d'entrée principal
├── Makefile                 # Fichier de compilation
├── README.md               # Ce fichier
├── server/
│   ├── LPTF_socket.hpp     # Header de la classe socket
│   ├── LPTF_socket.cpp     # Implémentation de la classe socket
│   ├── Server.hpp          # Header de la classe serveur
│   └── Server.cpp          # Implémentation de la classe serveur
└── client/
    ├── Client.hpp          # Header de la classe client
    └── Client.cpp          # Implémentation de la classe client
```

## Respecte les contraintes

✅ **Forme canonique de Coplien** : Toutes les classes l'implémentent  
✅ **Références plutôt que pointeurs** : Utilisées quand possible  
✅ **Variables const** : Par défaut, sauf justification  
✅ **Pas de variables globales** : Aucune variable globale  
✅ **Attributs privés** : Avec getters/setters appropriés  
✅ **Smart pointers** : std::unique_ptr utilisé  
✅ **Encapsulation syscalls** : Tous dans LPTF_Socket  
✅ **Pas de threads/fork** : Utilisation de poll()  
✅ **Multi-clients** : Serveur gère plusieurs clients simultanément

## Exemples d'utilisation

### Session serveur
```
=== Serveur LPTF ===
Configuration:
  IP: 0.0.0.0
  Port: 8080
  Max clients: 10

Serveur démarré sur 0.0.0.0:8080
En attente de connexions clients...
Nouveau client connecté: 127.0.0.1:54321 (Total: 1)
Message de 127.0.0.1:54321: Bonjour tout le monde!
```

### Session client
```
=== Client LPTF ===
Configuration:
  Serveur IP: 127.0.0.1
  Serveur Port: 8080

Connecté au serveur 127.0.0.1:8080
Tapez vos messages (tapez 'quit' pour quitter):
> Bonjour tout le monde!
Message envoyé: Bonjour tout le monde!
Message reçu: [127.0.0.1:54321]: Bonjour tout le monde!
```
