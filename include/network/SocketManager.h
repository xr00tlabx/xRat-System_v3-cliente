#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include "WebSocketClient.h"
#include "ReconnectionManager.h"

// Link with Winsock library
#pragma comment(lib, "ws2_32.lib")

namespace Network {

    // Socket connection states
    enum SocketState {
        SOCKET_DISCONNECTED = 0,
        SOCKET_CONNECTING = 1,
        SOCKET_CONNECTED = 2,
        SOCKET_ERROR_STATE = 3,
        SOCKET_RECONNECTING = 4
    };

    // Socket connection data
    struct ConnectionData {
        WebSocket::WebSocketClient* webSocketClient;
        std::string serverAddress;
        int serverPort;
        SocketState state;
        DWORD lastConnectionAttempt;
        int reconnectAttempts;
        bool autoReconnect;
        HANDLE connectionThread;
        HANDLE receiveThread;
        bool shouldStop;
    };

    class SocketManager {
    private:
        static SocketManager* instance;
        ConnectionData connectionData;
        CRITICAL_SECTION criticalSection;
        bool wsaInitialized;
        ReconnectionManager* reconnectionManager;

        // Private constructor for singleton
        SocketManager();
        
        // Private methods
        bool InitializeWinsock();
        void CleanupWinsock();
        static DWORD WINAPI ConnectionThreadProc(LPVOID lpParam);
        static DWORD WINAPI ReceiveThreadProc(LPVOID lpParam);
        bool AttemptConnection();
        void HandleConnectionError(const std::string& errorMsg);
        void ProcessReceivedMessage(const std::string& message);
        
        // Callback para ReconnectionManager
        static bool ConnectionCallback();
        
    public:
        // Singleton instance
        static SocketManager* GetInstance();
        static void DestroyInstance();
        
        // Destructor
        ~SocketManager();
        
        // Main connection methods
        bool Initialize();
        bool Connect(const std::string& serverAddress, int port);
        bool ConnectFromConfig(); // Nova função para conectar usando config.json
        bool Disconnect();
        void Shutdown();
        
        // Data transmission
        bool SendData(const char* data, int length);
        bool SendString(const std::string& message);
        int ReceiveData(char* buffer, int bufferSize);
        std::string ReceiveString();
        
        // Connection management
        SocketState GetConnectionState() const;
        bool IsConnected() const;
        void SetAutoReconnect(bool enable);
        bool StartReconnectProcess();
        
        // Status information
        std::string GetServerAddress() const;
        int GetServerPort() const;
        int GetReconnectAttempts() const;
        DWORD GetLastConnectionTime() const;
        
        // Utility methods
        std::string GetLastSocketError() const;
        void LogConnectionStatus(const std::string& status);
        bool ParseWebSocketUrl(const std::string& url, std::string& host, int& port); // Nova função para parser de URL
        
        // =============================================================================
        // MÉTODOS DE RECONEXÃO AUTOMÁTICA
        // =============================================================================
        
        /**
         * @brief Inicializa sistema de reconexão automática
         * @return true se inicializado com sucesso
         */
        bool InitializeReconnectionSystem();
        
        /**
         * @brief Verifica se está tentando reconectar
         * @return true se em processo de reconexão
         */
        bool IsReconnecting() const;
        
        /**
         * @brief Obtém status da reconexão
         * @return String com status da reconexão
         */
        std::string GetReconnectionStatus() const;
        
        /**
         * @brief Para sistema de reconexão
         */
        void StopReconnection();
        
        /**
         * @brief Força uma tentativa de reconexão imediata
         * @return true se reconectou com sucesso
         */
        bool ForceReconnect();
        
        /**
         * @brief Reseta contador de tentativas de reconexão
         */
        void ResetReconnectionAttempts();
        
        /**
         * @brief Configura parâmetros de reconexão a partir do config
         */
        void ConfigureReconnectionFromConfig();
    };

} // namespace Network

#endif // SOCKET_MANAGER_H
