// =============================================================================
// WebSocketClient Header - Cliente WebSocket Nativo C++
// =============================================================================
// Projeto: xRat System v3 - Cliente
// Arquivo: network/WebSocketClient.h
// Descrição: Cliente WebSocket implementado com Winsock nativo
// Autor: Desenvolvedor C++
// Data: 12/08/2025
// =============================================================================

#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <random>

#pragma comment(lib, "ws2_32.lib")

namespace WebSocket
{
    // =============================================================================
    // ENUMS E CONSTANTES
    // =============================================================================
    
    enum ConnectionState
    {
        STATE_DISCONNECTED = 0,
        STATE_CONNECTING,
        STATE_CONNECTED,
        STATE_CLOSING,
        STATE_ERROR
    };
    
    enum OpCode
    {
        CONTINUATION = 0x0,
        TEXT_FRAME = 0x1,
        BINARY_FRAME = 0x2,
        CLOSE_FRAME = 0x8,
        PING_FRAME = 0x9,
        PONG_FRAME = 0xA
    };
    
    // Constantes WebSocket
    const char* const WS_MAGIC_STRING = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    const int WS_HEADER_MIN_SIZE = 2;
    const int WS_HEADER_MAX_SIZE = 14;
    const int DEFAULT_BUFFER_SIZE = 4096;
    
    // =============================================================================
    // ESTRUTURAS DE DADOS
    // =============================================================================
    
    struct WebSocketFrame
    {
        bool fin;
        OpCode opcode;
        bool masked;
        uint64_t payloadLength;
        uint32_t maskingKey;
        std::vector<uint8_t> payload;
        
        WebSocketFrame() : fin(true), opcode(TEXT_FRAME), masked(true), 
                          payloadLength(0), maskingKey(0) {}
    };
    
    struct ConnectionInfo
    {
        std::string host;
        int port;
        std::string path;
        std::string fullUrl;
        ConnectionState state;
        SOCKET socket;
        
        ConnectionInfo() : host(""), port(0), path(""), fullUrl(""), 
                          state(STATE_DISCONNECTED), socket(INVALID_SOCKET) {}
    };
    
    // =============================================================================
    // CLASSE WEBSOCKET CLIENT
    // =============================================================================
    
    class WebSocketClient
    {
    private:
        ConnectionInfo connectionInfo;
        bool wsaInitialized;
        std::string lastError;
        std::random_device randomDevice;
        std::mt19937 randomGenerator;
        
        // Métodos privados para WebSocket
        bool InitializeWinsock();
        void CleanupWinsock();
        bool PerformHandshake();
        std::string GenerateWebSocketKey();
        std::string CalculateAcceptKey(const std::string& key);
        bool SendHttpHandshake(const std::string& webSocketKey);
        bool ReceiveHttpResponse(std::string& response);
        bool ValidateHandshakeResponse(const std::string& response, const std::string& expectedKey);
        
        // Métodos para frames WebSocket
        std::vector<uint8_t> CreateFrame(const std::string& message, OpCode opcode = TEXT_FRAME);
        bool SendFrame(const std::vector<uint8_t>& frameData);
        bool ReceiveFrame(WebSocketFrame& frame);
        uint32_t GenerateMaskingKey();
        void ApplyMask(std::vector<uint8_t>& data, uint32_t maskingKey);
        
        // Utilitários
        bool ParseUrl(const std::string& url);
        std::string Base64Encode(const std::vector<uint8_t>& data);
        std::string Sha1Hash(const std::string& input);
        
    public:
        // =============================================================================
        // MÉTODOS PÚBLICOS
        // =============================================================================
        
        /**
         * @brief Construtor
         */
        WebSocketClient();
        
        /**
         * @brief Destrutor
         */
        ~WebSocketClient();
        
        /**
         * @brief Conecta ao servidor WebSocket
         * @param url URL WebSocket (ws://host:port/path)
         * @return true se conectado com sucesso
         */
        bool Connect(const std::string& url);
        
        /**
         * @brief Desconecta do servidor
         */
        void Disconnect();
        
        /**
         * @brief Envia mensagem de texto
         * @param message Mensagem a ser enviada
         * @return true se enviado com sucesso
         */
        bool SendText(const std::string& message);
        
        /**
         * @brief Envia dados binários
         * @param data Dados binários
         * @return true se enviado com sucesso
         */
        bool SendBinary(const std::vector<uint8_t>& data);
        
        /**
         * @brief Recebe mensagem (bloqueante)
         * @param message Buffer para receber a mensagem
         * @param timeout Timeout em milissegundos (0 = sem timeout)
         * @return true se recebido com sucesso
         */
        bool ReceiveMessage(std::string& message, int timeout = 5000);
        
        /**
         * @brief Envia ping
         * @param payload Payload do ping (opcional)
         * @return true se enviado com sucesso
         */
        bool SendPing(const std::string& payload = "");
        
        /**
         * @brief Envia pong
         * @param payload Payload do pong
         * @return true se enviado com sucesso
         */
        bool SendPong(const std::string& payload);
        
        /**
         * @brief Verifica se está conectado
         * @return true se conectado
         */
        bool IsConnected() const;
        
        /**
         * @brief Obtém estado da conexão
         * @return Estado atual da conexão
         */
        ConnectionState GetState() const;
        
        /**
         * @brief Obtém informações da conexão
         * @return Estrutura com informações da conexão
         */
        ConnectionInfo GetConnectionInfo() const;
        
        /**
         * @brief Obtém último erro
         * @return String com a descrição do último erro
         */
        std::string GetLastError() const;
        
        /**
         * @brief Define timeout para operações de socket
         * @param timeout Timeout em milissegundos
         */
        void SetTimeout(int timeout);
    };
}

#endif // WEBSOCKET_CLIENT_H
