// =============================================================================
// ReconnectionManager Header - Sistema de Reconexão Automática
// =============================================================================
// Projeto: xRat System v3 - Cliente
// Arquivo: network/ReconnectionManager.h
// Descrição: Gerenciador de reconexão automática com backoff exponencial
// Autor: Desenvolvedor C++
// Data: 12/08/2025
// =============================================================================

#ifndef RECONNECTION_MANAGER_H
#define RECONNECTION_MANAGER_H

#include <windows.h>
#include <string>

namespace Network
{
    // =============================================================================
    // ESTRUTURAS DE CONFIGURAÇÃO
    // =============================================================================

    struct ReconnectionConfig
    {
        bool enabled;              // Habilitar reconexão automática
        int maxAttempts;          // Número máximo de tentativas consecutivas
        int initialDelay;         // Delay inicial em milissegundos
        int maxDelay;            // Delay máximo em milissegundos
        double backoffFactor;    // Fator de multiplicação para backoff
        int timeout;             // Timeout para tentativas de conexão

        ReconnectionConfig() :
            enabled(true),
            maxAttempts(10),
            initialDelay(1000),     // 1 segundo
            maxDelay(60000),        // 60 segundos
            backoffFactor(2.0),
            timeout(5000)           // 5 segundos
        {}
    };

    // Estados do sistema de reconexão
    enum ReconnectionState
    {
        RECONNECTION_IDLE = 0,       // Não está tentando reconectar
        RECONNECTION_WAITING,        // Aguardando antes da próxima tentativa
        RECONNECTION_ATTEMPTING,     // Tentando reconectar
        RECONNECTION_FAILED,         // Falhou após max tentativas
        RECONNECTION_SUCCEEDED       // Reconectou com sucesso
    };

    // =============================================================================
    // CLASSE RECONNECTION MANAGER
    // =============================================================================

    class ReconnectionManager
    {
    private:
        ReconnectionConfig config;
        ReconnectionState state;
        int currentAttempts;
        DWORD lastAttemptTime;
        int currentDelay;
        bool shouldStop;
        
        HANDLE reconnectionThread;
        CRITICAL_SECTION criticalSection;
        
        // Callback para tentativa de conexão
        typedef bool (*ConnectionCallback)();
        ConnectionCallback connectionCallback;
        
        // Métodos privados
        static DWORD WINAPI ReconnectionThreadProc(LPVOID lpParam);
        void ReconnectionLoop();
        int CalculateNextDelay();
        void LogReconnectionAttempt();
        void LogReconnectionResult(bool success);
        
    public:
        // =============================================================================
        // MÉTODOS PÚBLICOS
        // =============================================================================

        /**
         * @brief Construtor
         */
        ReconnectionManager();

        /**
         * @brief Destrutor
         */
        ~ReconnectionManager();

        /**
         * @brief Inicializa o sistema de reconexão
         * @param callback Função callback para tentativas de conexão
         * @return true se inicializado com sucesso
         */
        bool Initialize(ConnectionCallback callback);

        /**
         * @brief Finaliza o sistema de reconexão
         */
        void Shutdown();

        /**
         * @brief Configura parâmetros de reconexão
         * @param newConfig Nova configuração
         */
        void ConfigureSettings(const ReconnectionConfig& newConfig);

        /**
         * @brief Inicia processo de reconexão
         * @return true se iniciado com sucesso
         */
        bool StartReconnection();

        /**
         * @brief Para processo de reconexão
         */
        void StopReconnection();

        /**
         * @brief Verifica se está tentando reconectar
         * @return true se em processo de reconexão
         */
        bool IsReconnecting() const;

        /**
         * @brief Obtém estado atual da reconexão
         * @return Estado da reconexão
         */
        ReconnectionState GetState() const;

        /**
         * @brief Obtém número de tentativas atuais
         * @return Número de tentativas
         */
        int GetCurrentAttempts() const;

        /**
         * @brief Obtém configuração atual
         * @return Configuração de reconexão
         */
        ReconnectionConfig GetConfig() const;

        /**
         * @brief Reseta contador de tentativas
         */
        void ResetAttempts();

        /**
         * @brief Força uma tentativa de reconexão imediata
         * @return true se reconectou com sucesso
         */
        bool ForceReconnect();

        /**
         * @brief Obtém tempo até próxima tentativa (em ms)
         * @return Tempo em milissegundos, 0 se não está aguardando
         */
        DWORD GetTimeUntilNextAttempt() const;

        /**
         * @brief Obtém string de status da reconexão
         * @return String descritiva do status
         */
        std::string GetStatusString() const;

        /**
         * @brief Verifica se atingiu máximo de tentativas
         * @return true se atingiu o máximo
         */
        bool HasReachedMaxAttempts() const;
    };

} // namespace Network

#endif // RECONNECTION_MANAGER_H