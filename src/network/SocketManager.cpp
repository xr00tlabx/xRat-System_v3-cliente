// =============================================================================
// SocketManager Implementation - Versão WebSocket
// =============================================================================
// Projeto: xRat System v3 - Cliente
// Arquivo: network/SocketManager.cpp
// Descrição: Gerenciador de conexão WebSocket
// Autor: Desenvolvedor C++
// Data: 12/08/2025
// =============================================================================

#include "network/SocketManager.h"
#include "network/ReconnectionManager.h"
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
    connectionData.receiveThread = NULL;
    connectionData.reconnectAttempts = 0;
    
    // Initialize critical section
    InitializeCriticalSection(&criticalSection);
    wsaInitialized = false;
    
    // Initialize reconnection manager
    reconnectionManager = new ReconnectionManager();
    
    AppUtils::WriteLog("SocketManager: Instância criada", "INFO");
}

SocketManager::~SocketManager() {
    Shutdown();
    
    // Limpar WebSocket client
    if (connectionData.webSocketClient) {
        delete connectionData.webSocketClient;
        connectionData.webSocketClient = nullptr;
    }
    
    // Limpar ReconnectionManager
    if (reconnectionManager) {
        delete reconnectionManager;
        reconnectionManager = nullptr;
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
    
    if (!InitializeWinsock()) {
        LeaveCriticalSection(&criticalSection);
        return false;
    }
    
    LeaveCriticalSection(&criticalSection);
    
    // Inicializar sistema de reconexão
    if (!InitializeReconnectionSystem()) {
        AppUtils::WriteLog("SocketManager: Aviso - Sistema de reconexão não inicializado", "WARNING");
    }
    
    AppUtils::WriteLog("SocketManager: Inicializado com sucesso", "INFO");
    return true;
}

bool SocketManager::InitializeWinsock() {
    if (wsaInitialized) {
        return true;
    }
    
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::stringstream ss;
        ss << "SocketManager: Falha ao inicializar Winsock: " << result;
        AppUtils::WriteLog(ss.str(), "ERROR");
        return false;
    }
    
    wsaInitialized = true;
    AppUtils::WriteLog("SocketManager: Winsock inicializado com sucesso", "INFO");
    return true;
}

void SocketManager::CleanupWinsock() {
    if (wsaInitialized) {
        WSACleanup();
        wsaInitialized = false;
        AppUtils::WriteLog("SocketManager: Winsock finalizado", "INFO");
    }
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
        
        // Iniciar thread de recebimento
        connectionData.receiveThread = CreateThread(
            NULL,
            0,
            ReceiveThreadProc,
            this,
            0,
            NULL
        );
        
        if (connectionData.receiveThread == NULL) {
            AppUtils::WriteLog("SocketManager: Falha ao criar thread de recebimento", "ERROR");
        } else {
            AppUtils::WriteLog("SocketManager: Thread de recebimento iniciada", "INFO");
        }
        
        AppUtils::WriteLog("SocketManager: Conectado via WebSocket com sucesso", "INFO");
        
        // Resetar tentativas de reconexão se conectou com sucesso
        if (reconnectionManager) {
            reconnectionManager->ResetAttempts();
        }
    } else {
        connectionData.state = SOCKET_ERROR_STATE;
        connectionData.reconnectAttempts++;
        std::string error = connectionData.webSocketClient->GetLastError();
        AppUtils::WriteLog("SocketManager: Falha na conexão WebSocket: " + error, "ERROR");
        
        // Iniciar reconexão automática se habilitada
        if (reconnectionManager && Config::GetReconnectionEnabled()) {
            AppUtils::WriteLog("SocketManager: Iniciando processo de reconexão automática", "INFO");
            reconnectionManager->StartReconnection();
        }
    }
    
    return connected;
}

bool SocketManager::Disconnect() {
    EnterCriticalSection(&criticalSection);
    
    // Parar thread de recebimento
    connectionData.shouldStop = true;
    
    if (connectionData.receiveThread != NULL) {
        LeaveCriticalSection(&criticalSection);
        WaitForSingleObject(connectionData.receiveThread, 5000); // Wait max 5 seconds
        CloseHandle(connectionData.receiveThread);
        EnterCriticalSection(&criticalSection);
        connectionData.receiveThread = NULL;
        AppUtils::WriteLog("SocketManager: Thread de recebimento parada", "INFO");
    }
    
    if (connectionData.webSocketClient) {
        connectionData.webSocketClient->Disconnect();
        AppUtils::WriteLog("SocketManager: WebSocket desconectado", "INFO");
    }
    
    connectionData.state = SOCKET_DISCONNECTED;
    connectionData.shouldStop = false; // Reset for next connection
    
    LeaveCriticalSection(&criticalSection);
    return true;
}

void SocketManager::Shutdown() {
    AppUtils::WriteLog("SocketManager: Iniciando shutdown", "INFO");
    
    // Stop any running threads
    connectionData.shouldStop = true;
    
    // Stop reconnection system
    if (reconnectionManager) {
        reconnectionManager->Shutdown();
    }
    
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

std::string SocketManager::ReceiveString() {
    if (!IsConnected()) {
        AppUtils::WriteLog("SocketManager: Tentativa de recebimento com WebSocket desconectado", "WARNING");
        return "";
    }
    
    std::string message;
    bool success = connectionData.webSocketClient->ReceiveMessage(message, 5000); // 5 sec timeout
    
    if (success) {
        AppUtils::WriteLog("SocketManager: Mensagem WebSocket recebida: " + message, "INFO");
        return message;
    } else {
        std::string error = connectionData.webSocketClient->GetLastError();
        AppUtils::WriteLog("SocketManager: Falha ao receber via WebSocket: " + error, "ERROR");
        connectionData.state = SOCKET_ERROR_STATE;
        return "";
    }
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
    
    AppUtils::WriteLog("SocketManager: Iniciando processo de reconexão", "INFO");
    return ConnectFromConfig();
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
    if (connectionData.webSocketClient) {
        return connectionData.webSocketClient->GetLastError();
    }
    return "WebSocket client não inicializado";
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

bool SocketManager::ParseWebSocketUrl(const std::string& url, std::string& host, int& port) {
    // Parse para URLs do tipo: ws://192.168.10.101:8080/cli/ws
    
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

// =============================================================================
// Receive Thread Implementation
// =============================================================================

DWORD WINAPI SocketManager::ReceiveThreadProc(LPVOID lpParam) {
    SocketManager* manager = static_cast<SocketManager*>(lpParam);
    AppUtils::WriteLog("SocketManager: Thread de recebimento iniciada", "INFO");
    
    std::string message;
    while (!manager->connectionData.shouldStop && 
           manager->connectionData.webSocketClient && 
           manager->connectionData.webSocketClient->IsConnected()) {
        
        // Tentar receber mensagem com timeout
        if (manager->connectionData.webSocketClient->ReceiveMessage(message, 1000)) {
            if (!message.empty()) {
                manager->ProcessReceivedMessage(message);
            }
        }
        
        // Pequena pausa para não sobrecarregar a CPU
        Sleep(10);
    }
    
    // Verificar se a thread foi finalizada por desconexão inesperada
    if (!manager->connectionData.shouldStop && 
        (!manager->connectionData.webSocketClient || !manager->connectionData.webSocketClient->IsConnected())) {
        
        AppUtils::WriteLog("SocketManager: Desconexão detectada na thread de recebimento", "WARNING");
        manager->connectionData.state = SOCKET_ERROR_STATE;
        
        // Iniciar reconexão automática se habilitada
        if (manager->reconnectionManager && Config::GetReconnectionEnabled()) {
            AppUtils::WriteLog("SocketManager: Iniciando reconexão automática devido à desconexão", "INFO");
            manager->reconnectionManager->StartReconnection();
        }
    }
    
    AppUtils::WriteLog("SocketManager: Thread de recebimento finalizada", "INFO");
    return 0;
}

void SocketManager::ProcessReceivedMessage(const std::string& message) {
    AppUtils::WriteLog("📨 Mensagem recebida: " + message, "INFO");
    
    // Aqui você pode implementar o processamento das mensagens
    // Por exemplo, parse JSON e responder baseado no tipo
    
    try {
        // Simples verificação se é uma mensagem de ping do servidor
        if (message.find("\"type\":\"server_ping\"") != std::string::npos) {
            AppUtils::WriteLog("📤 Respondendo ao ping do servidor", "INFO");
            
            // Responder com pong
            std::string pongResponse = R"({"type":"client_pong","timestamp":")" + 
                                     std::to_string(GetTickCount()) + R"("})";
            
            if (connectionData.webSocketClient) {
                connectionData.webSocketClient->SendText(pongResponse);
            }
        }
        else if (message.find("\"type\":\"welcome\"") != std::string::npos) {
            AppUtils::WriteLog("✅ Mensagem de boas-vindas recebida do servidor", "INFO");
        }
    } catch (const std::exception& e) {
        AppUtils::WriteLog("Erro ao processar mensagem: " + std::string(e.what()), "ERROR");
    }
}

// =============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS DE RECONEXÃO AUTOMÁTICA
// =============================================================================

bool SocketManager::InitializeReconnectionSystem()
{
    if (!reconnectionManager) {
        AppUtils::WriteLog("SocketManager: ReconnectionManager não inicializado", "ERROR");
        return false;
    }
    
    // Configurar a partir do config
    ConfigureReconnectionFromConfig();
    
    // Inicializar com callback
    bool success = reconnectionManager->Initialize(ConnectionCallback);
    
    if (success) {
        AppUtils::WriteLog("SocketManager: Sistema de reconexão inicializado com sucesso", "INFO");
    } else {
        AppUtils::WriteLog("SocketManager: Falha ao inicializar sistema de reconexão", "ERROR");
    }
    
    return success;
}

bool SocketManager::IsReconnecting() const
{
    if (!reconnectionManager) return false;
    return reconnectionManager->IsReconnecting();
}

std::string SocketManager::GetReconnectionStatus() const
{
    if (!reconnectionManager) return "Sistema de reconexão não disponível";
    return reconnectionManager->GetStatusString();
}

void SocketManager::StopReconnection()
{
    if (reconnectionManager) {
        reconnectionManager->StopReconnection();
        AppUtils::WriteLog("SocketManager: Sistema de reconexão parado", "INFO");
    }
}

bool SocketManager::ForceReconnect()
{
    if (!reconnectionManager) {
        AppUtils::WriteLog("SocketManager: ReconnectionManager não disponível para reconexão forçada", "ERROR");
        return false;
    }
    
    AppUtils::WriteLog("SocketManager: Iniciando reconexão forçada", "INFO");
    return reconnectionManager->ForceReconnect();
}

void SocketManager::ResetReconnectionAttempts()
{
    if (reconnectionManager) {
        reconnectionManager->ResetAttempts();
        AppUtils::WriteLog("SocketManager: Contador de tentativas de reconexão resetado", "INFO");
    }
}

void SocketManager::ConfigureReconnectionFromConfig()
{
    if (!reconnectionManager) return;
    
    ReconnectionConfig config;
    config.enabled = Config::GetReconnectionEnabled();
    config.maxAttempts = Config::GetReconnectionMaxAttempts();
    config.initialDelay = Config::GetReconnectionInitialDelay();
    config.maxDelay = Config::GetReconnectionMaxDelay();
    config.backoffFactor = Config::GetReconnectionBackoffFactor();
    config.timeout = Config::GetReconnectionTimeout();
    
    reconnectionManager->ConfigureSettings(config);
    AppUtils::WriteLog("SocketManager: Configuração de reconexão carregada do config", "INFO");
}

bool SocketManager::ConnectionCallback()
{
    // Obter instância do SocketManager
    SocketManager* manager = GetInstance();
    if (!manager) {
        AppUtils::WriteLog("SocketManager: Instância não disponível para callback", "ERROR");
        return false;
    }
    
    // Tentar conectar usando o método existente
    bool success = manager->ConnectFromConfig();
    
    if (success) {
        AppUtils::WriteLog("SocketManager: Callback de reconexão bem-sucedido", "INFO");
        // Resetar estado de erro se conectou
        manager->connectionData.state = SOCKET_CONNECTED;
    } else {
        AppUtils::WriteLog("SocketManager: Callback de reconexão falhou", "WARNING");
        manager->connectionData.state = SOCKET_ERROR_STATE;
    }
    
    return success;
}
