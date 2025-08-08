CXX = clang++
CXXFLAGS = -std=c++14 -Wall -Wextra -g
TARGET = main
SRCDIR = .
SERVERDIR = server
CLIENTDIR = client

SOURCES = main.cpp \
          $(SERVERDIR)/LPTF_socket.cpp \
          $(SERVERDIR)/Server.cpp \
          $(CLIENTDIR)/Client.cpp

OBJECTS = $(SOURCES:.cpp=.o)

HEADERS = $(SERVERDIR)/LPTF_socket.hpp \
          $(SERVERDIR)/Server.hpp \
          $(CLIENTDIR)/Client.hpp

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) $(TARGET).dSYM *.o server/*.o client/*.o

fclean: clean
	rm -rf $(TARGET).dSYM/ test_server.dSYM/ test_client.dSYM/

re: fclean all

test_server: test_server.cpp $(SERVERDIR)/LPTF_socket.o $(SERVERDIR)/Server.o
	$(CXX) $(CXXFLAGS) -o $@ $^

test_client: test_client.cpp $(SERVERDIR)/LPTF_socket.o $(CLIENTDIR)/Client.o
	$(CXX) $(CXXFLAGS) -o $@ $^

run-test-server: test_server
	./test_server

run-test-client: test_client
	./test_client

test-server: $(TARGET)
	./$(TARGET) server

test-client: $(TARGET)
	./$(TARGET) client

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/lptf-socket

help:
	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "  all, clean, fclean, re"
	@echo "  test-server, test-client"
	@echo "  install"

.PHONY: all clean fclean re test-server test-client install help
