#ifndef NETWORK_CONFIG_H
#define NETWORK_CONFIG_H

#include <string>

namespace Network {

    // Network configuration constants
    class NetworkConfig {
    public:
        // Default connection settings
        static const std::string DEFAULT_SERVER_ADDRESS;
        static const int DEFAULT_SERVER_PORT;
        static const int DEFAULT_TIMEOUT_MS;
        static const int MAX_RECONNECT_ATTEMPTS;
        static const int RECONNECT_DELAY_MS;
        
        // Buffer sizes
        static const int RECEIVE_BUFFER_SIZE;
        static const int SEND_BUFFER_SIZE;
        
        // Connection validation
        static bool IsValidIPAddress(const std::string& ip);
        static bool IsValidPort(int port);
        
        // Configuration loading/saving
        static bool LoadNetworkConfig(const std::string& configFile);
        static bool SaveNetworkConfig(const std::string& configFile);
        
        // Get current settings
        static std::string GetServerAddress();
        static int GetServerPort();
        static int GetTimeout();
        static int GetMaxReconnectAttempts();
        static int GetReconnectDelay();
        
        // Set settings
        static void SetServerAddress(const std::string& address);
        static void SetServerPort(int port);
        static void SetTimeout(int timeoutMs);
        static void SetMaxReconnectAttempts(int attempts);
        static void SetReconnectDelay(int delayMs);
        
    private:
        static std::string serverAddress;
        static int serverPort;
        static int timeoutMs;
        static int maxReconnectAttempts;
        static int reconnectDelayMs;
    };

    // Network utility functions
    class NetworkUtils {
    public:
        // Network diagnostics
        static bool PingServer(const std::string& address, int port, int timeoutMs = 5000);
        static std::string GetLocalIPAddress();
        static std::string GetHostname();
        
        // Data formatting
        static std::string FormatMessage(const std::string& type, const std::string& data);
        static bool ParseMessage(const std::string& message, std::string& type, std::string& data);
        
        // Error handling
        static std::string GetNetworkErrorDescription(int errorCode);
        static void LogNetworkEvent(const std::string& event, const std::string& details);
    };

} // namespace Network

#endif // NETWORK_CONFIG_H
