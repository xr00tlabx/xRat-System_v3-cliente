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
#include "utils/AppUtils.h"
#include "utils/Config.h"
#include <sstream>
#include <iostream>

using namespace Network;

// Static member initialization
SocketManager *SocketManager::instance = nullptr;

SocketManager::SocketManager()
{
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

SocketManager::~SocketManager()
{
    Shutdown();

    // Limpar WebSocket client
    if (connectionData.webSocketClient)
    {
        delete connectionData.webSocketClient;
        connectionData.webSocketClient = nullptr;
    }

    DeleteCriticalSection(&criticalSection);
    AppUtils::WriteLog("SocketManager: Instância destruída", "INFO");
}

SocketManager *SocketManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new SocketManager();
    }
    return instance;
}

void SocketManager::DestroyInstance()
{
    if (instance != nullptr)
    {
        delete instance;
        instance = nullptr;
    }
}

bool SocketManager::Initialize()
{
    EnterCriticalSection(&criticalSection);

    if (!InitializeWinsock())
    {
        LeaveCriticalSection(&criticalSection);
        return false;
    }

    LeaveCriticalSection(&criticalSection);
    AppUtils::WriteLog("SocketManager: Inicializado com sucesso", "INFO");
    return true;
}

bool SocketManager::InitializeWinsock()
{
    if (wsaInitialized)
    {
        return true;
    }

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::stringstream ss;
        ss << "SocketManager: Falha ao inicializar Winsock: " << result;
        AppUtils::WriteLog(ss.str(), "ERROR");
        return false;
    }

    wsaInitialized = true;
    AppUtils::WriteLog("SocketManager: Winsock inicializado com sucesso", "INFO");
    return true;
}

void SocketManager::CleanupWinsock()
{
    if (wsaInitialized)
    {
        WSACleanup();
        wsaInitialized = false;
        AppUtils::WriteLog("SocketManager: Winsock finalizado", "INFO");
    }
}

bool SocketManager::ConnectFromConfig()
{
    std::string apiUrl = Config::GetApiUrl();
    if (apiUrl.empty())
    {
        AppUtils::WriteLog("SocketManager: URL da API não encontrada no config", "ERROR");
        return false;
    }

    AppUtils::WriteLog("SocketManager: Conectando via WebSocket usando config - " + apiUrl, "INFO");

    // Criar cliente WebSocket se não existir
    if (!connectionData.webSocketClient)
    {
        connectionData.webSocketClient = new WebSocket::WebSocketClient();
    }

    // Conectar usando WebSocket
    bool connected = connectionData.webSocketClient->Connect(apiUrl);

    if (connected)
    {
        connectionData.state = SOCKET_CONNECTED;
        connectionData.reconnectAttempts = 0;
        AppUtils::WriteLog("SocketManager: Conectado via WebSocket com sucesso", "INFO");
    }
    else
    {
        connectionData.state = SOCKET_ERROR_STATE;
        connectionData.reconnectAttempts++;
        std::string error = connectionData.webSocketClient->GetLastError();
        AppUtils::WriteLog("SocketManager: Falha na conexão WebSocket: " + error, "ERROR");
    }

    return connected;
}

bool SocketManager::Disconnect()
{
    EnterCriticalSection(&criticalSection);

    if (connectionData.webSocketClient)
    {
        connectionData.webSocketClient->Disconnect();
        AppUtils::WriteLog("SocketManager: WebSocket desconectado", "INFO");
    }

    connectionData.state = SOCKET_DISCONNECTED;

    LeaveCriticalSection(&criticalSection);
    return true;
}

void SocketManager::Shutdown()
{
    AppUtils::WriteLog("SocketManager: Iniciando shutdown", "INFO");

    // Stop any running threads
    connectionData.shouldStop = true;

    // Disconnect WebSocket
    Disconnect();

    // Clean up WebSocket client
    if (connectionData.webSocketClient)
    {
        delete connectionData.webSocketClient;
        connectionData.webSocketClient = nullptr;
    }

    // Clean up Winsock
    CleanupWinsock();

    AppUtils::WriteLog("SocketManager: Shutdown completo", "INFO");
}

bool SocketManager::SendString(const std::string &message)
{
    if (!IsConnected())
    {
        AppUtils::WriteLog("SocketManager: Tentativa de envio com WebSocket desconectado", "WARNING");
        return false;
    }

    bool success = connectionData.webSocketClient->SendText(message);

    if (success)
    {
        AppUtils::WriteLog("SocketManager: Mensagem WebSocket enviada: " + message, "INFO");
    }
    else
    {
        std::string error = connectionData.webSocketClient->GetLastError();
        AppUtils::WriteLog("SocketManager: Falha ao enviar via WebSocket: " + error, "ERROR");
        connectionData.state = SOCKET_ERROR_STATE;
    }

    return success;
}

std::string SocketManager::ReceiveString()
{
    if (!IsConnected())
    {
        AppUtils::WriteLog("SocketManager: Tentativa de recebimento com WebSocket desconectado", "WARNING");
        return "";
    }

    std::string message;
    bool success = connectionData.webSocketClient->ReceiveMessage(message, 5000); // 5 sec timeout

    if (success)
    {
        AppUtils::WriteLog("SocketManager: Mensagem WebSocket recebida: " + message, "INFO");
        return message;
    }
    else
    {
        std::string error = connectionData.webSocketClient->GetLastError();
        AppUtils::WriteLog("SocketManager: Falha ao receber via WebSocket: " + error, "ERROR");
        connectionData.state = SOCKET_ERROR_STATE;
        return "";
    }
}

SocketState SocketManager::GetConnectionState() const
{
    return connectionData.state;
}

bool SocketManager::IsConnected() const
{
    if (!connectionData.webSocketClient)
    {
        return false;
    }
    return connectionData.webSocketClient->IsConnected() && connectionData.state == SOCKET_CONNECTED;
}

void SocketManager::SetAutoReconnect(bool enable)
{
    EnterCriticalSection(&criticalSection);
    connectionData.autoReconnect = enable;

    std::stringstream ss;
    ss << "SocketManager: Auto-reconexão " << (enable ? "ativada" : "desativada");
    AppUtils::WriteLog(ss.str(), "INFO");

    LeaveCriticalSection(&criticalSection);
}

bool SocketManager::StartReconnectProcess()
{
    if (connectionData.state == SOCKET_CONNECTED)
    {
        return true;
    }

    AppUtils::WriteLog("SocketManager: Iniciando processo de reconexão", "INFO");
    return ConnectFromConfig();
}

std::string SocketManager::GetServerAddress() const
{
    return connectionData.serverAddress;
}

int SocketManager::GetServerPort() const
{
    return connectionData.serverPort;
}

int SocketManager::GetReconnectAttempts() const
{
    return connectionData.reconnectAttempts;
}

DWORD SocketManager::GetLastConnectionTime() const
{
    return connectionData.lastConnectionAttempt;
}

std::string SocketManager::GetLastSocketError() const
{
    if (connectionData.webSocketClient)
    {
        return connectionData.webSocketClient->GetLastError();
    }
    return "WebSocket client não inicializado";
}

void SocketManager::LogConnectionStatus(const std::string &status)
{
    std::stringstream ss;
    ss << "SocketManager Status: " << status
       << " | Estado: " << connectionData.state
       << " | Servidor: " << connectionData.serverAddress
       << ":" << connectionData.serverPort
       << " | Tentativas: " << connectionData.reconnectAttempts;

    AppUtils::WriteLog(ss.str(), "SYSTEM");
}

bool SocketManager::ParseWebSocketUrl(const std::string &url, std::string &host, int &port)
{
    // Parse para URLs do tipo: ws://192.168.10.101:8080/cli/ws

    // Remove o protocolo ws://
    std::string urlCopy = url;
    if (urlCopy.substr(0, 5) == "ws://")
    {
        urlCopy = urlCopy.substr(5);
    }
    else if (urlCopy.substr(0, 6) == "wss://")
    {
        urlCopy = urlCopy.substr(6);
    }

    // Encontra a posição dos dois pontos para separar host e porta
    size_t colonPos = urlCopy.find(':');
    if (colonPos == std::string::npos)
    {
        AppUtils::WriteLog("SocketManager: URL inválida - sem porta: " + url, "ERROR");
        return false;
    }

    // Extrai o host
    host = urlCopy.substr(0, colonPos);

    // Extrai a porta (até encontrar '/' ou fim da string)
    std::string portStr;
    size_t slashPos = urlCopy.find('/', colonPos);
    if (slashPos != std::string::npos)
    {
        portStr = urlCopy.substr(colonPos + 1, slashPos - colonPos - 1);
    }
    else
    {
        portStr = urlCopy.substr(colonPos + 1);
    }

    // Converte porta para inteiro
    try
    {
        port = std::stoi(portStr);
        if (port <= 0 || port > 65535)
        {
            AppUtils::WriteLog("SocketManager: Porta inválida: " + portStr, "ERROR");
            return false;
        }
    }
    catch (const std::exception &)
    {
        AppUtils::WriteLog("SocketManager: Erro ao converter porta: " + portStr, "ERROR");
        return false;
    }

    AppUtils::WriteLog("SocketManager: URL parseada - Host: " + host + ", Porta: " + std::to_string(port), "INFO");
    return true;
}
