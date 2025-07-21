# Makefile pour le projet LPTF Socket

CXX = clang++
CXXFLAGS = -std=c++14 -Wall -Wextra -g
TARGET = main
SRCDIR = .
SERVERDIR = server
CLIENTDIR = client

# Sources
SOURCES = main.cpp \
          $(SERVERDIR)/LPTF_socket.cpp \
          $(SERVERDIR)/Server.cpp \
          $(CLIENTDIR)/Client.cpp

# Objects
OBJECTS = $(SOURCES:.cpp=.o)

# Headers
HEADERS = $(SERVERDIR)/LPTF_socket.hpp \
          $(SERVERDIR)/Server.hpp \
          $(CLIENTDIR)/Client.hpp

# Règle principale
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilation des fichiers objets
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -f $(OBJECTS) $(TARGET) $(TARGET).dSYM test_server test_client test_server.o test_client.o

# Nettoyage complet
fclean: clean
	rm -rf $(TARGET).dSYM/ test_server.dSYM/ test_client.dSYM/

# Reconstruction
re: fclean all

# Programmes de test
test_server: test_server.cpp $(SERVERDIR)/LPTF_socket.o $(SERVERDIR)/Server.o
	$(CXX) $(CXXFLAGS) -o $@ $^

test_client: test_client.cpp $(SERVERDIR)/LPTF_socket.o $(CLIENTDIR)/Client.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# Règles pour tester
run-test-server: test_server
	./test_server

run-test-client: test_client
	./test_client

test-server: $(TARGET)
	./$(TARGET) server

test-client: $(TARGET)
	./$(TARGET) client

# Installation (optionnel)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/lptf-socket

# Affichage de l'aide
help:
	@echo "Makefile pour le projet LPTF Socket"
	@echo ""
	@echo "Règles disponibles:"
	@echo "  all         - Compile le projet"
	@echo "  clean       - Supprime les fichiers objets et l'exécutable"
	@echo "  fclean      - Supprime tout (clean + dossiers de debug)"
	@echo "  re          - Reconstruction complète (fclean + all)"
	@echo "  test-server - Lance le serveur avec les paramètres par défaut"
	@echo "  test-client - Lance le client avec les paramètres par défaut"
	@echo "  install     - Installe l'exécutable dans /usr/local/bin"
	@echo "  help        - Affiche cette aide"
	@echo ""
	@echo "Usage:"
	@echo "  ./main server [ip] [port] [max_clients]"
	@echo "  ./main client [server_ip] [server_port]"

.PHONY: all clean fclean re test-server test-client install help
