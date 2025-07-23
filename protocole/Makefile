# Makefile pour le protocole LPTF

CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# Sources du protocole
PROTOCOL_SOURCES = LPTF_Protocol.cpp
PROTOCOL_OBJECTS = $(PROTOCOL_SOURCES:.cpp=.o)

# Headers
PROTOCOL_HEADERS = LPTF_Protocol.hpp

# Programmes de test
test_protocol: Binaire.cpp $(PROTOCOL_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilation des fichiers objets
%.o: %.cpp $(PROTOCOL_HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Test du protocole
run-test-protocol: test_protocol
	./test_protocol

# Nettoyage
clean-protocol:
	rm -f $(PROTOCOL_OBJECTS) test_protocol test_protocol.dSYM

# Validation du format RFC
validate-rfc:
	@echo "Validation du format RFC..."
	@wc -l LPTF_RFC.txt
	@echo "Lignes du document RFC: $$(wc -l < LPTF_RFC.txt)"

# Documentation
doc:
	@echo "=== Documentation du Protocole LPTF ==="
	@echo "Fichiers principaux:"
	@echo "  - LPTF_Protocol.hpp : Définitions et interface"
	@echo "  - LPTF_Protocol.cpp : Implémentation"
	@echo "  - LPTF_RFC.txt      : Spécification RFC"
	@echo "  - Binaire.cpp       : Exemple d'utilisation"
	@echo ""
	@echo "Compilation et test:"
	@echo "  make test_protocol     : Compile le test"
	@echo "  make run-test-protocol : Exécute le test"
	@echo ""
	@echo "Structure du protocole:"
	@echo "  - Header fixe de 12 bytes"
	@echo "  - Payload variable avec champs typés"
	@echo "  - Support de l'évolutivité et versioning"

# Analyse de la taille du protocole
analyze-size: test_protocol
	@echo "=== Analyse de la taille du protocole ==="
	@size test_protocol
	@echo ""
	@echo "Taille des fichiers sources:"
	@wc -c $(PROTOCOL_SOURCES) $(PROTOCOL_HEADERS)

# Test de performance basique
benchmark: test_protocol
	@echo "=== Test de performance basique ==="
	@time ./test_protocol

.PHONY: run-test-protocol clean-protocol validate-rfc doc analyze-size benchmark
