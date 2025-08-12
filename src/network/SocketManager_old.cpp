#include "network/SocketManager.h"
#include "utils/AppUtils.h"
#include "utils/Config.h"
#include <sstream>
#include <iostream>

using namespace Network;

// Static member initialization
SocketManager* SocketManager::instance = nullptr;

SocketManager::SocketManager() {
    // Initialize connection data
    ZeroMemory(&connectionData, sizeof(ConnectionData));
    connectionData.webSocketClient = nullptr;
    connectionData.state = SOCKET_DISCONNECTED;
    connectionData.autoReconnect = true;
    connectionData.shouldStop = false;
    connectionData.connectionThread = NULL;
    connectionData.reconnectAttempts = 0;
    
    // Initialize critical section
    InitializeCriticalSection(&criticalSection);
    wsaInitialized = false;
    
    AppUtils::WriteLog("SocketManager: Instância criada", "INFO");
}

SocketManager::~SocketManager() {
    Shutdown();
    
    // Limpar WebSocket client
    if (connectionData.webSocketClient) {
        delete connectionData.webSocketClient;
        connectionData.webSocketClient = nullptr;
    }
    
    DeleteCriticalSection(&criticalSection);
    AppUtils::WriteLog("SocketManager: Instância destruída", "INFO");
}

SocketManager* SocketManager::GetInstance() {
    if (instance == nullptr) {
        instance = new SocketManager();
    }
    return instance;
}

void SocketManager::DestroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

bool SocketManager::Initialize() {
    EnterCriticalSection(&criticalSection);
    
    bool result = InitializeWinsock();
    if (result) {
        AppUtils::WriteLog("SocketManager: Winsock inicializado com sucesso", "INFO");
    } else {
        AppUtils::WriteLog("SocketManager: Falha ao inicializar Winsock", "ERROR");
    }
    
    LeaveCriticalSection(&criticalSection);
    return result;
}

bool SocketManager::InitializeWinsock() {
    if (wsaInitialized) {
        return true;
    }
    
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    
    if (result != 0) {
        std::stringstream ss;
        ss << "WSAStartup falhou com erro: " << result;
        AppUtils::WriteLog(ss.str(), "ERROR");
        return false;
    }
    
    wsaInitialized = true;
    return true;
}

void SocketManager::CleanupWinsock() {
    if (wsaInitialized) {
        WSACleanup();
        wsaInitialized = false;
        AppUtils::WriteLog("SocketManager: Winsock finalizado", "INFO");
    }
}

bool SocketManager::Connect(const std::string& serverAddress, int port) {
    EnterCriticalSection(&criticalSection);
    
    // Store connection parameters
    connectionData.serverAddress = serverAddress;
    connectionData.serverPort = port;
    connectionData.state = SOCKET_CONNECTING;
    connectionData.lastConnectionAttempt = GetTickCount();
    
    std::stringstream ss;
    ss << "SocketManager: Tentando conectar em " << serverAddress << ":" << port;
    AppUtils::WriteLog(ss.str(), "INFO");
    
    bool result = AttemptConnection();
    
    LeaveCriticalSection(&criticalSection);
    return result;
}

bool SocketManager::AttemptConnection() {
    // Create socket
    connectionData.socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connectionData.socketHandle == INVALID_SOCKET) {
        HandleConnectionError("Falha ao criar socket");
        return false;
    }
    
    // Set up server address structure
    sockaddr_in serverAddr;
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(connectionData.serverPort);
    
    // Convert IP address
    int result = inet_pton(AF_INET, connectionData.serverAddress.c_str(), &serverAddr.sin_addr);
    if (result != 1) {
        HandleConnectionError("Endereço IP inválido");
        closesocket(connectionData.socketHandle);
        connectionData.socketHandle = INVALID_SOCKET;
        return false;
    }
    
    // Attempt connection
    result = connect(connectionData.socketHandle, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        HandleConnectionError("Falha na conexão com o servidor");
        closesocket(connectionData.socketHandle);
        connectionData.socketHandle = INVALID_SOCKET;
        return false;
    }
    
    connectionData.state = SOCKET_CONNECTED;
    connectionData.reconnectAttempts = 0;
    
    std::stringstream ss;
    ss << "SocketManager: Conectado com sucesso em " << connectionData.serverAddress 
       << ":" << connectionData.serverPort;
    AppUtils::WriteLog(ss.str(), "INFO");
    
    return true;
}

bool SocketManager::Disconnect() {
    EnterCriticalSection(&criticalSection);
    
    bool result = true;
    
    if (connectionData.socketHandle != INVALID_SOCKET) {
        int shutdownResult = shutdown(connectionData.socketHandle, SD_BOTH);
        if (shutdownResult == SOCKET_ERROR) {
            AppUtils::WriteLog("SocketManager: Aviso - Falha no shutdown do socket", "WARNING");
        }
        
        int closeResult = closesocket(connectionData.socketHandle);
        if (closeResult == SOCKET_ERROR) {
            AppUtils::WriteLog("SocketManager: Erro ao fechar socket", "ERROR");
            result = false;
        } else {
            AppUtils::WriteLog("SocketManager: Socket fechado com sucesso", "INFO");
        }
        
        connectionData.socketHandle = INVALID_SOCKET;
    }
    
    connectionData.state = SOCKET_DISCONNECTED;
    
    LeaveCriticalSection(&criticalSection);
    return result;
}

void SocketManager::Shutdown() {
    EnterCriticalSection(&criticalSection);
    
    connectionData.shouldStop = true;
    
    // Wait for connection thread to finish
    if (connectionData.connectionThread != NULL) {
        LeaveCriticalSection(&criticalSection);
        WaitForSingleObject(connectionData.connectionThread, 5000); // Wait up to 5 seconds
        EnterCriticalSection(&criticalSection);
        CloseHandle(connectionData.connectionThread);
        connectionData.connectionThread = NULL;
    }
    
    Disconnect();
    CleanupWinsock();
    
    AppUtils::WriteLog("SocketManager: Sistema de sockets encerrado", "INFO");
    
    LeaveCriticalSection(&criticalSection);
}

bool SocketManager::SendData(const char* data, int length) {
    if (!IsConnected()) {
        AppUtils::WriteLog("SocketManager: Tentativa de envio com socket desconectado", "WARNING");
        return false;
    }
    
    int bytesSent = send(connectionData.socketHandle, data, length, 0);
    if (bytesSent == SOCKET_ERROR) {
        HandleConnectionError("Falha ao enviar dados");
        return false;
    }
    
    if (bytesSent != length) {
        std::stringstream ss;
        ss << "SocketManager: Envio parcial - esperado: " << length << ", enviado: " << bytesSent;
        AppUtils::WriteLog(ss.str(), "WARNING");
        return false;
    }
    
    std::stringstream ss;
    ss << "SocketManager: " << bytesSent << " bytes enviados com sucesso";
    AppUtils::WriteLog(ss.str(), "DEBUG");
    
    return true;
}

bool SocketManager::SendString(const std::string& message) {
    if (!IsConnected()) {
        AppUtils::WriteLog("SocketManager: Tentativa de envio com WebSocket desconectado", "WARNING");
        return false;
    }
    
    bool success = connectionData.webSocketClient->SendText(message);
    
    if (success) {
        AppUtils::WriteLog("SocketManager: Mensagem WebSocket enviada: " + message, "INFO");
    } else {
        std::string error = connectionData.webSocketClient->GetLastError();
        AppUtils::WriteLog("SocketManager: Falha ao enviar via WebSocket: " + error, "ERROR");
        connectionData.state = SOCKET_ERROR_STATE;
    }
    
    return success;
}

// =============================================================================
// NOVOS MÉTODOS WEBSOCKET ADICIONADOS
// =============================================================================

bool SocketManager::Disconnect() {
    EnterCriticalSection(&criticalSection);
    
    if (connectionData.webSocketClient) {
        connectionData.webSocketClient->Disconnect();
        AppUtils::WriteLog("SocketManager: WebSocket desconectado", "INFO");
    }
    
    connectionData.state = SOCKET_DISCONNECTED;
    
    LeaveCriticalSection(&criticalSection);
    return true;
}

void SocketManager::Shutdown() {
    AppUtils::WriteLog("SocketManager: Iniciando shutdown", "INFO");
    
    // Stop any running threads
    connectionData.shouldStop = true;
    
    // Disconnect WebSocket
    Disconnect();
    
    // Clean up WebSocket client
    if (connectionData.webSocketClient) {
        delete connectionData.webSocketClient;
        connectionData.webSocketClient = nullptr;
    }
    
    // Clean up Winsock
    CleanupWinsock();
    
    AppUtils::WriteLog("SocketManager: Shutdown completo", "INFO");
}

int SocketManager::ReceiveData(char* buffer, int bufferSize) {
    if (!IsConnected()) {
        AppUtils::WriteLog("SocketManager: Tentativa de recebimento com socket desconectado", "WARNING");
        return -1;
    }
    
    int bytesReceived = recv(connectionData.socketHandle, buffer, bufferSize - 1, 0);
    
    if (bytesReceived == SOCKET_ERROR) {
        HandleConnectionError("Falha ao receber dados");
        return -1;
    }
    
    if (bytesReceived == 0) {
        AppUtils::WriteLog("SocketManager: Conexão fechada pelo servidor", "INFO");
        connectionData.state = SOCKET_DISCONNECTED;
        return 0;
    }
    
    buffer[bytesReceived] = '\0'; // Null terminate
    
    std::stringstream ss;
    ss << "SocketManager: " << bytesReceived << " bytes recebidos";
    AppUtils::WriteLog(ss.str(), "DEBUG");
    
    return bytesReceived;
}

std::string SocketManager::ReceiveString() {
    char buffer[4096];
    int bytesReceived = ReceiveData(buffer, sizeof(buffer));
    
    if (bytesReceived > 0) {
        return std::string(buffer, bytesReceived);
    }
    
    return "";
}

SocketState SocketManager::GetConnectionState() const {
    return connectionData.state;
}

bool SocketManager::IsConnected() const {
    if (!connectionData.webSocketClient) {
        return false;
    }
    return connectionData.webSocketClient->IsConnected() && connectionData.state == SOCKET_CONNECTED;
}

void SocketManager::SetAutoReconnect(bool enable) {
    EnterCriticalSection(&criticalSection);
    connectionData.autoReconnect = enable;
    
    std::stringstream ss;
    ss << "SocketManager: Auto-reconexão " << (enable ? "ativada" : "desativada");
    AppUtils::WriteLog(ss.str(), "INFO");
    
    LeaveCriticalSection(&criticalSection);
}

bool SocketManager::StartReconnectProcess() {
    if (connectionData.state == SOCKET_CONNECTED) {
        return true;
    }
    
    connectionData.state = SOCKET_RECONNECTING;
    connectionData.reconnectAttempts++;
    
    std::stringstream ss;
    ss << "SocketManager: Iniciando tentativa de reconexão #" << connectionData.reconnectAttempts;
    AppUtils::WriteLog(ss.str(), "INFO");
    
    return AttemptConnection();
}

std::string SocketManager::GetServerAddress() const {
    return connectionData.serverAddress;
}

int SocketManager::GetServerPort() const {
    return connectionData.serverPort;
}

int SocketManager::GetReconnectAttempts() const {
    return connectionData.reconnectAttempts;
}

DWORD SocketManager::GetLastConnectionTime() const {
    return connectionData.lastConnectionAttempt;
}

std::string SocketManager::GetLastSocketError() const {
    int errorCode = WSAGetLastError();
    std::stringstream ss;
    ss << "Código de erro Winsock: " << errorCode;
    return ss.str();
}

void SocketManager::HandleConnectionError(const std::string& errorMsg) {
    connectionData.state = SOCKET_ERROR_STATE;
    
    std::stringstream ss;
    ss << "SocketManager: " << errorMsg << " - " << GetLastSocketError();
    AppUtils::WriteLog(ss.str(), "ERROR");
}

void SocketManager::LogConnectionStatus(const std::string& status) {
    std::stringstream ss;
    ss << "SocketManager Status: " << status 
       << " | Estado: " << connectionData.state
       << " | Servidor: " << connectionData.serverAddress 
       << ":" << connectionData.serverPort
       << " | Tentativas: " << connectionData.reconnectAttempts;
    
    AppUtils::WriteLog(ss.str(), "SYSTEM");
}

bool SocketManager::ConnectFromConfig() {
    std::string apiUrl = Config::GetApiUrl();
    if (apiUrl.empty()) {
        AppUtils::WriteLog("SocketManager: URL da API não encontrada no config", "ERROR");
        return false;
    }
    
    AppUtils::WriteLog("SocketManager: Conectando via WebSocket usando config - " + apiUrl, "INFO");
    
    // Criar cliente WebSocket se não existir
    if (!connectionData.webSocketClient) {
        connectionData.webSocketClient = new WebSocket::WebSocketClient();
    }
    
    // Conectar usando WebSocket
    bool connected = connectionData.webSocketClient->Connect(apiUrl);
    
    if (connected) {
        connectionData.state = SOCKET_CONNECTED;
        connectionData.reconnectAttempts = 0;
        AppUtils::WriteLog("SocketManager: Conectado via WebSocket com sucesso", "INFO");
    } else {
        connectionData.state = SOCKET_ERROR_STATE;
        connectionData.reconnectAttempts++;
        std::string error = connectionData.webSocketClient->GetLastError();
        AppUtils::WriteLog("SocketManager: Falha na conexão WebSocket: " + error, "ERROR");
    }
    
    return connected;
}

bool SocketManager::ParseWebSocketUrl(const std::string& url, std::string& host, int& port) {
    // Parser para URLs do tipo: ws://192.168.10.101:8856/cli/ws
    
    // Remove o protocolo ws://
    std::string urlCopy = url;
    if (urlCopy.substr(0, 5) == "ws://") {
        urlCopy = urlCopy.substr(5);
    } else if (urlCopy.substr(0, 6) == "wss://") {
        urlCopy = urlCopy.substr(6);
    }
    
    // Encontra a posição dos dois pontos para separar host e porta
    size_t colonPos = urlCopy.find(':');
    if (colonPos == std::string::npos) {
        AppUtils::WriteLog("SocketManager: URL inválida - sem porta: " + url, "ERROR");
        return false;
    }
    
    // Extrai o host
    host = urlCopy.substr(0, colonPos);
    
    // Extrai a porta (até encontrar '/' ou fim da string)
    std::string portStr;
    size_t slashPos = urlCopy.find('/', colonPos);
    if (slashPos != std::string::npos) {
        portStr = urlCopy.substr(colonPos + 1, slashPos - colonPos - 1);
    } else {
        portStr = urlCopy.substr(colonPos + 1);
    }
    
    // Converte porta para inteiro
    try {
        port = std::stoi(portStr);
        if (port <= 0 || port > 65535) {
            AppUtils::WriteLog("SocketManager: Porta inválida: " + portStr, "ERROR");
            return false;
        }
    } catch (const std::exception&) {
        AppUtils::WriteLog("SocketManager: Erro ao converter porta: " + portStr, "ERROR");
        return false;
    }
    
    AppUtils::WriteLog("SocketManager: URL parseada - Host: " + host + ", Porta: " + std::to_string(port), "INFO");
    return true;
}
