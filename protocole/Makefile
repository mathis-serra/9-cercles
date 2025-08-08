CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

PROTOCOL_SOURCES = LPTF_Protocol.cpp
PROTOCOL_OBJECTS = $(PROTOCOL_SOURCES:.cpp=.o)
PROTOCOL_HEADERS = LPTF_Protocol.hpp
test_protocol: Binaire.cpp $(PROTOCOL_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(PROTOCOL_HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run-test-protocol: test_protocol
	./test_protocol

clean-protocol:
	rm -f $(PROTOCOL_OBJECTS) test_protocol test_protocol.dSYM

validate-rfc:
	@wc -l LPTF_RFC.txt

doc:
	@echo "LPTF Protocol Documentation"
	@echo "Files: LPTF_Protocol.hpp, LPTF_Protocol.cpp, LPTF_RFC.txt"
	@echo "Usage: make test_protocol && make run-test-protocol"

analyze-size: test_protocol
	@size test_protocol
	@wc -c $(PROTOCOL_SOURCES) $(PROTOCOL_HEADERS)

benchmark: test_protocol
	@time ./test_protocol

.PHONY: run-test-protocol clean-protocol validate-rfc doc analyze-size benchmark
