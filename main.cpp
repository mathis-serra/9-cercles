#include "server/Server.hpp"
#include "client/Client.hpp"
#include <iostream>
#include <string>

void print_usage(const std::string& program_name) {
    std::cout << "Usage: " << program_name << " [server|client|demo] [options]" << std::endl;
    std::cout << "  server [ip] [port] [max_clients]" << std::endl;
    std::cout << "  client [server_ip] [server_port]" << std::endl;
    std::cout << "  demo   - Test remote control features locally" << std::endl;
}

int run_server(int argc, char* argv[]) {
    std::string bind_ip = "0.0.0.0";
    int bind_port = 8080;
    int max_clients = 10;
    
    if (argc >= 3) {
        bind_ip = argv[2];
    }
    if (argc >= 4) {
        try {
            bind_port = std::stoi(argv[3]);
        } catch (const std::exception& e) {
            std::cerr << "Invalid port: " << argv[3] << std::endl;
            return 1;
        }
    }
    if (argc >= 5) {
        try {
            max_clients = std::stoi(argv[4]);
        } catch (const std::exception& e) {
            std::cerr << "Invalid max clients: " << argv[4] << std::endl;
            return 1;
        }
    }
    
    std::cout << "Starting server on " << bind_ip << ":" << bind_port << std::endl;
    
    Server server(bind_ip, bind_port, max_clients);
    
    std::cout << "Press Ctrl+C to stop..." << std::endl;
    server.run();
    
    return 0;
}

int run_client(int argc, char* argv[]) {
    std::string server_ip = "127.0.0.1";
    int server_port = 8080;
    
    if (argc >= 3) {
        server_ip = argv[2];
    }
    if (argc >= 4) {
        try {
            server_port = std::stoi(argv[3]);
        } catch (const std::exception& e) {
            std::cerr << "Invalid port: " << argv[3] << std::endl;
            return 1;
        }
    }
    
    std::cout << "Connecting to " << server_ip << ":" << server_port << std::endl;
    
    Client client(server_ip, server_port);
    client.run_interactive();
    
    return 0;
}

int run_demo() {
    std::cout << "=== DEMO Mode - Remote Control Features ===" << std::endl;
    std::cout << "⚠️  ATTENTION: Ces fonctionnalités sont à des fins éducatives uniquement!" << std::endl;
    
    Client demo_client;
    demo_client.run_remote_control_demo();
    
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    std::string mode = argv[1];
    
    if (mode == "server") {
        return run_server(argc, argv);
    } else if (mode == "client") {
        return run_client(argc, argv);
    } else if (mode == "demo") {
        return run_demo();
    } else {
        std::cerr << "Invalid mode: " << mode << std::endl;
        print_usage(argv[0]);
        return 1;
    }
}
