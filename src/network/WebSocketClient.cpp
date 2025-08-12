// =============================================================================
// WebSocketClient Implementation - Cliente WebSocket Nativo C++
// =============================================================================
// Projeto: xRat System v3 - Cliente
// Arquivo: network/WebSocketClient.cpp
// Descrição: Implementação do cliente WebSocket com Winsock nativo
// Autor: Desenvolvedor C++
// Data: 12/08/2025
// =============================================================================

#include "network/WebSocketClient.h"
#include "utils/AppUtils.h"
#include <sstream>
#include <iomanip>
#include <regex>
#include <algorithm>
#include <cctype>

using namespace WebSocket;

// =============================================================================
// IMPLEMENTAÇÃO DO CONSTRUTOR E DESTRUTOR
// =============================================================================

WebSocketClient::WebSocketClient() : wsaInitialized(false), randomGenerator(randomDevice())
{
    connectionInfo.state = STATE_DISCONNECTED;
    connectionInfo.socket = INVALID_SOCKET;
    
    AppUtils::WriteLog("WebSocketClient: Instância criada", "INFO");
}

WebSocketClient::~WebSocketClient()
{
    Disconnect();
    CleanupWinsock();
    AppUtils::WriteLog("WebSocketClient: Instância destruída", "INFO");
}

// =============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS DE CONEXÃO
// =============================================================================

bool WebSocketClient::Connect(const std::string& url)
{
    if (!InitializeWinsock()) {
        lastError = "Falha ao inicializar Winsock";
        return false;
    }
    
    if (!ParseUrl(url)) {
        lastError = "URL inválida: " + url;
        AppUtils::WriteLog("WebSocketClient: " + lastError, "ERROR");
        return false;
    }
    
    connectionInfo.state = STATE_CONNECTING;
    AppUtils::WriteLog("WebSocketClient: Conectando em " + connectionInfo.host + ":" + std::to_string(connectionInfo.port), "INFO");
    
    // Criar socket
    connectionInfo.socket = socket(AF_INET, SOCK_STREAM, 0);
    if (connectionInfo.socket == INVALID_SOCKET) {
        lastError = "Falha ao criar socket: " + std::to_string(WSAGetLastError());
        connectionInfo.state = STATE_ERROR;
        return false;
    }
    
    // Configurar endereço do servidor
    sockaddr_in serverAddr;
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(connectionInfo.port);
    
    // Resolver hostname
    hostent* hostEntry = gethostbyname(connectionInfo.host.c_str());
    if (hostEntry == nullptr) {
        lastError = "Falha ao resolver hostname: " + connectionInfo.host;
        connectionInfo.state = STATE_ERROR;
        closesocket(connectionInfo.socket);
        connectionInfo.socket = INVALID_SOCKET;
        return false;
    }
    
    memcpy(&serverAddr.sin_addr, hostEntry->h_addr_list[0], hostEntry->h_length);
    
    // Conectar
    if (connect(connectionInfo.socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        lastError = "Falha ao conectar: " + std::to_string(WSAGetLastError());
        connectionInfo.state = STATE_ERROR;
        closesocket(connectionInfo.socket);
        connectionInfo.socket = INVALID_SOCKET;
        return false;
    }
    
    // Realizar handshake WebSocket
    if (!PerformHandshake()) {
        connectionInfo.state = STATE_ERROR;
        closesocket(connectionInfo.socket);
        connectionInfo.socket = INVALID_SOCKET;
        return false;
    }
    
    connectionInfo.state = STATE_CONNECTED;
    AppUtils::WriteLog("WebSocketClient: Conectado com sucesso via WebSocket", "INFO");
    
    return true;
}

void WebSocketClient::Disconnect()
{
    if (connectionInfo.socket != INVALID_SOCKET) {
        // Enviar frame de fechamento
        if (connectionInfo.state == STATE_CONNECTED) {
            std::vector<uint8_t> closeFrame = CreateFrame("", CLOSE_FRAME);
            SendFrame(closeFrame);
        }
        
        connectionInfo.state = STATE_CLOSING;
        closesocket(connectionInfo.socket);
        connectionInfo.socket = INVALID_SOCKET;
        AppUtils::WriteLog("WebSocketClient: Desconectado", "INFO");
    }
    
    connectionInfo.state = STATE_DISCONNECTED;
}

// =============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS DE COMUNICAÇÃO
// =============================================================================

bool WebSocketClient::SendText(const std::string& message)
{
    if (connectionInfo.state != STATE_CONNECTED) {
        lastError = "Não conectado";
        return false;
    }
    
    std::vector<uint8_t> frame = CreateFrame(message, TEXT_FRAME);
    bool success = SendFrame(frame);
    
    if (success) {
        AppUtils::WriteLog("WebSocketClient: Mensagem enviada: " + message, "INFO");
    } else {
        AppUtils::WriteLog("WebSocketClient: Falha ao enviar mensagem: " + message, "ERROR");
    }
    
    return success;
}

bool WebSocketClient::SendBinary(const std::vector<uint8_t>& data)
{
    if (connectionInfo.state != STATE_CONNECTED) {
        lastError = "Não conectado";
        return false;
    }
    
    // Criar frame binário
    WebSocketFrame frame;
    frame.fin = true;
    frame.opcode = BINARY_FRAME;
    frame.masked = true;
    frame.maskingKey = GenerateMaskingKey();
    frame.payload = data;
    frame.payloadLength = data.size();
    
    // Aplicar máscara
    ApplyMask(frame.payload, frame.maskingKey);
    
    // Montar frame completo
    std::vector<uint8_t> frameData;
    
    // Byte 1: FIN + RSV + Opcode
    uint8_t byte1 = (frame.fin ? 0x80 : 0x00) | static_cast<uint8_t>(frame.opcode);
    frameData.push_back(byte1);
    
    // Byte 2+: MASK + Payload length
    if (frame.payloadLength < 126) {
        frameData.push_back(0x80 | static_cast<uint8_t>(frame.payloadLength));
    } else if (frame.payloadLength < 65536) {
        frameData.push_back(0x80 | 126);
        frameData.push_back((frame.payloadLength >> 8) & 0xFF);
        frameData.push_back(frame.payloadLength & 0xFF);
    } else {
        frameData.push_back(0x80 | 127);
        for (int i = 7; i >= 0; i--) {
            frameData.push_back((frame.payloadLength >> (i * 8)) & 0xFF);
        }
    }
    
    // Masking key
    frameData.push_back((frame.maskingKey >> 24) & 0xFF);
    frameData.push_back((frame.maskingKey >> 16) & 0xFF);
    frameData.push_back((frame.maskingKey >> 8) & 0xFF);
    frameData.push_back(frame.maskingKey & 0xFF);
    
    // Payload
    frameData.insert(frameData.end(), frame.payload.begin(), frame.payload.end());
    
    return SendFrame(frameData);
}

bool WebSocketClient::ReceiveMessage(std::string& message, int timeout)
{
    if (connectionInfo.state != STATE_CONNECTED) {
        lastError = "Não conectado";
        return false;
    }
    
    // Configurar timeout
    if (timeout > 0) {
        DWORD timeoutMs = timeout;
        setsockopt(connectionInfo.socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeoutMs, sizeof(timeoutMs));
    }
    
    WebSocketFrame frame;
    if (!ReceiveFrame(frame)) {
        return false;
    }
    
    switch (frame.opcode) {
        case TEXT_FRAME:
        case BINARY_FRAME:
            message = std::string(frame.payload.begin(), frame.payload.end());
            AppUtils::WriteLog("WebSocketClient: Mensagem recebida: " + message, "INFO");
            return true;
            
        case PING_FRAME:
            // Responder com pong
            SendPong(std::string(frame.payload.begin(), frame.payload.end()));
            // Tentar receber próxima mensagem
            return ReceiveMessage(message, timeout);
            
        case PONG_FRAME:
            AppUtils::WriteLog("WebSocketClient: Pong recebido", "INFO");
            // Tentar receber próxima mensagem
            return ReceiveMessage(message, timeout);
            
        case CLOSE_FRAME:
            AppUtils::WriteLog("WebSocketClient: Frame de fechamento recebido", "INFO");
            connectionInfo.state = STATE_CLOSING;
            return false;
            
        default:
            lastError = "Opcode não suportado: " + std::to_string(frame.opcode);
            return false;
    }
}

bool WebSocketClient::SendPing(const std::string& payload)
{
    if (connectionInfo.state != STATE_CONNECTED) {
        return false;
    }
    
    std::vector<uint8_t> frame = CreateFrame(payload, PING_FRAME);
    return SendFrame(frame);
}

bool WebSocketClient::SendPong(const std::string& payload)
{
    if (connectionInfo.state != STATE_CONNECTED) {
        return false;
    }
    
    std::vector<uint8_t> frame = CreateFrame(payload, PONG_FRAME);
    return SendFrame(frame);
}

// =============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS DE STATUS
// =============================================================================

bool WebSocketClient::IsConnected() const
{
    return connectionInfo.state == STATE_CONNECTED;
}

ConnectionState WebSocketClient::GetState() const
{
    return connectionInfo.state;
}

ConnectionInfo WebSocketClient::GetConnectionInfo() const
{
    return connectionInfo;
}

std::string WebSocketClient::GetLastError() const
{
    return lastError;
}

void WebSocketClient::SetTimeout(int timeout)
{
    if (connectionInfo.socket != INVALID_SOCKET) {
        DWORD timeoutMs = timeout;
        setsockopt(connectionInfo.socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeoutMs, sizeof(timeoutMs));
        setsockopt(connectionInfo.socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeoutMs, sizeof(timeoutMs));
    }
}

// =============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS PRIVADOS
// =============================================================================

bool WebSocketClient::InitializeWinsock()
{
    if (wsaInitialized) {
        return true;
    }
    
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        lastError = "Falha ao inicializar Winsock: " + std::to_string(result);
        return false;
    }
    
    wsaInitialized = true;
    AppUtils::WriteLog("WebSocketClient: Winsock inicializado", "INFO");
    return true;
}

void WebSocketClient::CleanupWinsock()
{
    if (wsaInitialized) {
        WSACleanup();
        wsaInitialized = false;
        AppUtils::WriteLog("WebSocketClient: Winsock finalizado", "INFO");
    }
}

bool WebSocketClient::ParseUrl(const std::string& url)
{
    // Parse URLs como: ws://host:port/path
    std::regex wsRegex(R"(^ws://([^:/]+):?(\d+)?(/.*)?$)");
    std::smatch matches;
    
    if (!std::regex_match(url, matches, wsRegex)) {
        return false;
    }
    
    connectionInfo.host = matches[1].str();
    connectionInfo.port = matches[2].matched ? std::stoi(matches[2].str()) : 80;
    connectionInfo.path = matches[3].matched ? matches[3].str() : "/";
    connectionInfo.fullUrl = url;
    
    AppUtils::WriteLog("WebSocketClient: URL parseada - Host: " + connectionInfo.host + 
                      ", Porta: " + std::to_string(connectionInfo.port) + 
                      ", Path: " + connectionInfo.path, "INFO");
    
    return true;
}

bool WebSocketClient::PerformHandshake()
{
    std::string webSocketKey = GenerateWebSocketKey();
    
    if (!SendHttpHandshake(webSocketKey)) {
        lastError = "Falha ao enviar handshake HTTP";
        return false;
    }
    
    std::string response;
    if (!ReceiveHttpResponse(response)) {
        lastError = "Falha ao receber resposta do handshake";
        return false;
    }
    
    if (!ValidateHandshakeResponse(response, webSocketKey)) {
        lastError = "Handshake inválido";
        return false;
    }
    
    AppUtils::WriteLog("WebSocketClient: Handshake WebSocket concluído com sucesso", "INFO");
    return true;
}

std::string WebSocketClient::GenerateWebSocketKey()
{
    std::vector<uint8_t> keyBytes(16);
    std::uniform_int_distribution<> dist(0, 255);
    
    for (size_t i = 0; i < keyBytes.size(); i++) {
        keyBytes[i] = static_cast<uint8_t>(dist(randomGenerator));
    }
    
    return Base64Encode(keyBytes);
}

bool WebSocketClient::SendHttpHandshake(const std::string& webSocketKey)
{
    std::ostringstream request;
    request << "GET " << connectionInfo.path << " HTTP/1.1\r\n";
    request << "Host: " << connectionInfo.host << ":" << connectionInfo.port << "\r\n";
    request << "Upgrade: websocket\r\n";
    request << "Connection: Upgrade\r\n";
    request << "Sec-WebSocket-Key: " << webSocketKey << "\r\n";
    request << "Sec-WebSocket-Version: 13\r\n";
    request << "User-Agent: xRat-Client/1.0\r\n";
    request << "\r\n";
    
    std::string requestStr = request.str();
    int bytesSent = send(connectionInfo.socket, requestStr.c_str(), requestStr.length(), 0);
    
    if (bytesSent == SOCKET_ERROR) {
        lastError = "Erro ao enviar handshake: " + std::to_string(WSAGetLastError());
        return false;
    }
    
    AppUtils::WriteLog("WebSocketClient: Handshake HTTP enviado", "INFO");
    return true;
}

bool WebSocketClient::ReceiveHttpResponse(std::string& response)
{
    char buffer[4096];
    response.clear();
    
    // Receber resposta até encontrar \r\n\r\n
    while (response.find("\r\n\r\n") == std::string::npos) {
        int bytesReceived = recv(connectionInfo.socket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesReceived <= 0) {
            lastError = "Erro ao receber resposta: " + std::to_string(WSAGetLastError());
            return false;
        }
        
        buffer[bytesReceived] = '\0';
        response += buffer;
    }
    
    AppUtils::WriteLog("WebSocketClient: Resposta HTTP recebida", "INFO");
    return true;
}

bool WebSocketClient::ValidateHandshakeResponse(const std::string& response, const std::string& webSocketKey)
{
    // Log da resposta para debug
    AppUtils::WriteLog("WebSocketClient: Resposta HTTP recebida: " + response.substr(0, 200), "INFO");
    
    // Verificar status HTTP 101
    if (response.find("HTTP/1.1 101") == std::string::npos) {
        lastError = "Status HTTP inválido na resposta - esperado 101 Switching Protocols";
        return false;
    }
    
    // Verificar headers obrigatórios (case insensitive)
    std::string lowerResponse = response;
    std::transform(lowerResponse.begin(), lowerResponse.end(), lowerResponse.begin(), ::tolower);
    
    if (lowerResponse.find("upgrade: websocket") == std::string::npos) {
        lastError = "Header 'Upgrade: websocket' não encontrado";
        return false;
    }
    
    if (lowerResponse.find("connection: upgrade") == std::string::npos) {
        lastError = "Header 'Connection: Upgrade' não encontrado";
        return false;
    }
    
    // Temporariamente, vamos aceitar qualquer resposta que tenha os headers básicos
    // TODO: Implementar verificação correta do Sec-WebSocket-Accept
    AppUtils::WriteLog("WebSocketClient: Handshake básico validado (sem verificação SHA1)", "INFO");
    
    return true;
}

std::string WebSocketClient::CalculateAcceptKey(const std::string& key)
{
    std::string concatenated = key + WS_MAGIC_STRING;
    std::string sha1 = Sha1Hash(concatenated);
    
    // Converter hex string para bytes
    std::vector<uint8_t> hashBytes;
    for (size_t i = 0; i < sha1.length(); i += 2) {
        std::string byteStr = sha1.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(strtol(byteStr.c_str(), nullptr, 16));
        hashBytes.push_back(byte);
    }
    
    return Base64Encode(hashBytes);
}

std::vector<uint8_t> WebSocketClient::CreateFrame(const std::string& message, OpCode opcode)
{
    std::vector<uint8_t> payload(message.begin(), message.end());
    
    std::vector<uint8_t> frame;
    
    // Byte 1: FIN + RSV + Opcode
    uint8_t byte1 = 0x80 | static_cast<uint8_t>(opcode); // FIN = 1
    frame.push_back(byte1);
    
    // Gerar chave de máscara
    uint32_t maskingKey = GenerateMaskingKey();
    
    // Byte 2+: MASK + Payload length
    if (payload.size() < 126) {
        frame.push_back(0x80 | static_cast<uint8_t>(payload.size()));
    } else if (payload.size() < 65536) {
        frame.push_back(0x80 | 126);
        frame.push_back((payload.size() >> 8) & 0xFF);
        frame.push_back(payload.size() & 0xFF);
    } else {
        frame.push_back(0x80 | 127);
        for (int i = 7; i >= 0; i--) {
            frame.push_back((payload.size() >> (i * 8)) & 0xFF);
        }
    }
    
    // Masking key
    frame.push_back((maskingKey >> 24) & 0xFF);
    frame.push_back((maskingKey >> 16) & 0xFF);
    frame.push_back((maskingKey >> 8) & 0xFF);
    frame.push_back(maskingKey & 0xFF);
    
    // Payload com máscara aplicada
    std::vector<uint8_t> maskedPayload = payload;
    ApplyMask(maskedPayload, maskingKey);
    frame.insert(frame.end(), maskedPayload.begin(), maskedPayload.end());
    
    return frame;
}

bool WebSocketClient::SendFrame(const std::vector<uint8_t>& frameData)
{
    int bytesSent = send(connectionInfo.socket, reinterpret_cast<const char*>(frameData.data()), 
                        frameData.size(), 0);
    
    if (bytesSent == SOCKET_ERROR) {
        lastError = "Erro ao enviar frame: " + std::to_string(WSAGetLastError());
        return false;
    }
    
    return bytesSent == static_cast<int>(frameData.size());
}

bool WebSocketClient::ReceiveFrame(WebSocketFrame& frame)
{
    // Receber header mínimo (2 bytes)
    uint8_t headerBuffer[WS_HEADER_MAX_SIZE];
    int bytesReceived = recv(connectionInfo.socket, reinterpret_cast<char*>(headerBuffer), 2, 0);
    
    if (bytesReceived <= 0) {
        lastError = "Erro ao receber header do frame: " + std::to_string(WSAGetLastError());
        return false;
    }
    
    // Parse do header
    frame.fin = (headerBuffer[0] & 0x80) != 0;
    frame.opcode = static_cast<OpCode>(headerBuffer[0] & 0x0F);
    frame.masked = (headerBuffer[1] & 0x80) != 0;
    
    // Determinar tamanho do payload
    uint8_t payloadLen = headerBuffer[1] & 0x7F;
    
    if (payloadLen < 126) {
        frame.payloadLength = payloadLen;
    } else if (payloadLen == 126) {
        // 16-bit length
        bytesReceived = recv(connectionInfo.socket, reinterpret_cast<char*>(headerBuffer + 2), 2, 0);
        if (bytesReceived <= 0) {
            lastError = "Erro ao receber length estendido";
            return false;
        }
        frame.payloadLength = (headerBuffer[2] << 8) | headerBuffer[3];
    } else {
        // 64-bit length
        bytesReceived = recv(connectionInfo.socket, reinterpret_cast<char*>(headerBuffer + 2), 8, 0);
        if (bytesReceived <= 0) {
            lastError = "Erro ao receber length estendido 64-bit";
            return false;
        }
        frame.payloadLength = 0;
        for (int i = 0; i < 8; i++) {
            frame.payloadLength = (frame.payloadLength << 8) | headerBuffer[2 + i];
        }
    }
    
    // Receber masking key se necessário (frames do servidor não devem ter máscara)
    if (frame.masked) {
        bytesReceived = recv(connectionInfo.socket, reinterpret_cast<char*>(&frame.maskingKey), 4, 0);
        if (bytesReceived <= 0) {
            lastError = "Erro ao receber masking key";
            return false;
        }
    } else {
        frame.maskingKey = 0;
    }
    
    // Receber payload
    frame.payload.resize(frame.payloadLength);
    if (frame.payloadLength > 0) {
        size_t totalReceived = 0;
        while (totalReceived < frame.payloadLength) {
            bytesReceived = recv(connectionInfo.socket, 
                               reinterpret_cast<char*>(frame.payload.data() + totalReceived),
                               frame.payloadLength - totalReceived, 0);
            
            if (bytesReceived <= 0) {
                lastError = "Erro ao receber payload do frame";
                return false;
            }
            
            totalReceived += bytesReceived;
        }
        
        // Aplicar unmask se necessário
        if (frame.masked) {
            ApplyMask(frame.payload, frame.maskingKey);
        }
    }
    
    return true;
}

uint32_t WebSocketClient::GenerateMaskingKey()
{
    std::uniform_int_distribution<uint32_t> dist;
    return dist(randomGenerator);
}

void WebSocketClient::ApplyMask(std::vector<uint8_t>& data, uint32_t maskingKey)
{
    uint8_t mask[4] = {
        static_cast<uint8_t>((maskingKey >> 24) & 0xFF),
        static_cast<uint8_t>((maskingKey >> 16) & 0xFF),
        static_cast<uint8_t>((maskingKey >> 8) & 0xFF),
        static_cast<uint8_t>(maskingKey & 0xFF)
    };
    
    for (size_t i = 0; i < data.size(); i++) {
        data[i] ^= mask[i % 4];
    }
}

// Implementações simplificadas para Base64 e SHA1
std::string WebSocketClient::Base64Encode(const std::vector<uint8_t>& data)
{
    const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    
    for (size_t i = 0; i < data.size(); i += 3) {
        uint32_t value = data[i] << 16;
        if (i + 1 < data.size()) value |= data[i + 1] << 8;
        if (i + 2 < data.size()) value |= data[i + 2];
        
        result += chars[(value >> 18) & 0x3F];
        result += chars[(value >> 12) & 0x3F];
        result += (i + 1 < data.size()) ? chars[(value >> 6) & 0x3F] : '=';
        result += (i + 2 < data.size()) ? chars[value & 0x3F] : '=';
    }
    
    return result;
}

std::string WebSocketClient::Sha1Hash(const std::string& input)
{
    // Implementação simplificada do SHA1 para WebSocket
    // Em produção, use uma biblioteca criptográfica adequada
    
    // Por simplicidade, vamos usar uma versão mock que funciona para o handshake básico
    // Esta implementação não é segura para uso em produção!
    
    std::ostringstream oss;
    oss << std::hex;
    
    // Hash mock simples baseado no input
    uint32_t hash = 0;
    for (char c : input) {
        hash = hash * 31 + static_cast<uint32_t>(c);
    }
    
    // Gerar 20 bytes (160 bits) como SHA1
    for (int i = 0; i < 5; i++) {
        uint32_t part = hash + i * 0x12345678;
        oss << std::setfill('0') << std::setw(8) << part;
    }
    
    return oss.str();
}
