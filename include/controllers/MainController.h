// =============================================================================
// MainController Header - Controlador Principal
// =============================================================================
// Projeto: Main C++ Application
// Arquivo: controllers/MainController.h
// Descrição: Classe responsável pela lógica de negócio
// Autor: Desenvolvedor
// Data: 11/08/2025
// =============================================================================

#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include <windows.h>
#include <string>
#include <memory>
#include "../utils/Config.h"

// Forward declarations
class MainForm;

// =============================================================================
// ESTRUTURA DE DADOS DA APLICAÇÃO
// =============================================================================

struct ApplicationData
{
    std::string buildDate;
    std::string buildTime;
    bool isInitialized;
    int clickCount;

    ApplicationData() : buildDate(__DATE__),
                        buildTime(__TIME__),
                        isInitialized(false),
                        clickCount(0) {}
                        
    // Métodos para acessar configurações via Config system
    std::string GetAppName() const { return Config::GetString("appName", "Main Application"); }
    std::string GetAppVersion() const { return Config::GetString("appVersion", "1.0.0"); }
    std::string GetAppAuthor() const { return Config::GetString("appAuthor", "Desenvolvedor C++"); }
    std::string GetAppDescription() const { return Config::GetString("appDescription", "Aplicação Windows desenvolvida em C++"); }
    std::string GetClientId() const { return Config::GetClientId(); }
    bool GetDebugMode() const { return Config::GetDebugMode(); }
    
    void SetDebugMode(bool enabled) { Config::SetDebugMode(enabled); }
    void SetClientId(const std::string& clientId) { Config::SetClientId(clientId); }
};

// =============================================================================
// CLASSE MAINCONTROLLER
// =============================================================================

class MainController
{
private:
    static std::unique_ptr<ApplicationData> appData;
    static MainForm *mainForm;
    static HINSTANCE hInstance;

public:
    // =============================================================================
    // MÉTODOS DE INICIALIZAÇÃO
    // =============================================================================

    /**
     * @brief Inicializa o controller
     * @param hInst Instância da aplicação
     * @return true se inicializado com sucesso
     */
    static bool Initialize(HINSTANCE hInst);

    /**
     * @brief Finaliza o controller e limpa recursos
     */
    static void Shutdown();

    /**
     * @brief Verifica se o controller foi inicializado
     * @return true se inicializado
     */
    static bool IsInitialized();

    // =============================================================================
    // MÉTODOS DE CONTROLE DA APLICAÇÃO
    // =============================================================================

    /**
     * @brief Inicia a aplicação
     * @param nCmdShow Modo de exibição da janela
     * @return true se iniciada com sucesso
     */
    static bool StartApplication(int nCmdShow);

    /**
     * @brief Executa o loop principal da aplicação
     * @return Código de saída da aplicação
     */
    static int RunMessageLoop();

    /**
     * @brief Solicita fechamento da aplicação
     * @return true se pode fechar, false caso contrário
     */
    static bool RequestShutdown();

    /**
     * @brief Exibe mensagem de boas-vindas
     */
    static void ShowWelcomeMessage();

    /**
     * @brief Mostra a janela principal (quando palavra-chave é detectada)
     */
    static void ShowMainWindow();

    // =============================================================================
    // HANDLERS DE EVENTOS (CHAMADOS PELO FORM)
    // =============================================================================

    /**
     * @brief Manipula clique no botão Hello
     */
    static void OnButtonHelloClicked();

    /**
     * @brief Manipula clique no botão About
     */
    static void OnButtonAboutClicked();

    /**
     * @brief Manipula clique no botão Config
     */
    static void OnButtonConfigClicked();

    /**
     * @brief Manipula clique no botão Exit
     */
    static void OnButtonExitClicked();

    /**
     * @brief Manipula tentativa de fechamento da janela
     * @return true se pode fechar, false caso contrário
     */
    static bool OnWindowClosing();

    // =============================================================================
    // MÉTODOS DE DADOS DA APLICAÇÃO
    // =============================================================================

    /**
     * @brief Obtém dados da aplicação
     * @return Ponteiro para os dados da aplicação
     */
    static ApplicationData *GetApplicationData();

    /**
     * @brief Obtém nome da aplicação
     * @return Nome da aplicação
     */
    static std::string GetApplicationName();

    /**
     * @brief Obtém versão da aplicação
     * @return Versão da aplicação
     */
    static std::string GetApplicationVersion();

    /**
     * @brief Obtém informações completas da aplicação
     * @return String com informações formatadas
     */
    static std::string GetApplicationInfo();

    // =============================================================================
    // MÉTODOS DE CONFIGURAÇÃO
    // =============================================================================

    /**
     * @brief Habilita/desabilita modo debug
     * @param enabled Estado do modo debug
     */
    static void SetDebugMode(bool enabled);

    /**
     * @brief Verifica se está em modo debug
     * @return true se em modo debug
     */
    static bool IsDebugMode();

    /**
     * @brief Incrementa contador de cliques
     * @return Novo valor do contador
     */
    static int IncrementClickCount();

    /**
     * @brief Reseta contador de cliques
     */
    static void ResetClickCount();

    // =============================================================================
    // MÉTODOS DE COMUNICAÇÃO SOCKET
    // =============================================================================

    /**
     * @brief Obtém status da conexão socket
     * @return true se conectado
     */
    static bool IsSocketConnected();

    /**
     * @brief Reconecta socket se necessário
     * @return true se conectado/reconectado com sucesso
     */
    static bool EnsureSocketConnection();

    /**
     * @brief Envia mensagem via socket
     * @param message Mensagem a ser enviada
     * @return true se enviado com sucesso
     */
    static bool SendSocketMessage(const std::string& message);

    /**
     * @brief Obtém informações da conexão socket
     * @return String com informações da conexão
     */
    static std::string GetSocketConnectionInfo();

    /**
     * @brief Verifica se está tentando reconectar
     * @return true se em processo de reconexão
     */
    static bool IsReconnecting();

    /**
     * @brief Para sistema de reconexão automática
     */
    static void StopReconnection();

    /**
     * @brief Força uma tentativa de reconexão imediata
     * @return true se reconectou com sucesso
     */
    static bool ForceReconnect();

    /**
     * @brief Reseta contador de tentativas de reconexão
     */
    static void ResetReconnectionAttempts();

    /**
     * @brief Obtém status detalhado da reconexão
     * @return String com status da reconexão
     */
    static std::string GetReconnectionStatus();

    /**
     * @brief Atualiza status da conexão na interface do usuário
     */
    static void UpdateConnectionStatusUI();

    // =============================================================================
    // MÉTODOS DE LOGGING E DEBUG
    // =============================================================================

    /**
     * @brief Registra evento no log
     * @param event Descrição do evento
     * @param level Nível do log (INFO, WARNING, ERROR)
     */
    static void LogEvent(const std::string &event, const std::string &level = "INFO");

    /**
     * @brief Exibe informações de debug
     */
    static void ShowDebugInfo();

    /**
     * @brief Salva dados da aplicação em arquivo
     * @param filename Nome do arquivo (opcional)
     * @return true se salvou com sucesso
     */
    static bool SaveApplicationState(const std::string &filename = "");

    /**
     * @brief Carrega dados da aplicação de arquivo
     * @param filename Nome do arquivo (opcional)
     * @return true se carregou com sucesso
     */
    static bool LoadApplicationState(const std::string &filename = "");

private:
    // =============================================================================
    // MÉTODOS PRIVADOS
    // =============================================================================

    /**
     * @brief Configura dados iniciais da aplicação
     */
    static void SetupApplicationData();

    /**
     * @brief Valida estado da aplicação
     * @return true se estado é válido
     */
    static bool ValidateApplicationState();

    /**
     * @brief Processa eventos especiais
     * @param eventType Tipo do evento
     * @param eventData Dados do evento
     */
    static void ProcessSpecialEvent(const std::string &eventType, const std::string &eventData);

    /**
     * @brief Formata string de informações do sistema
     * @return String formatada com informações do sistema
     */
    static std::string FormatSystemInfo();

    /**
     * @brief Formata string de estatísticas da aplicação
     * @return String formatada com estatísticas
     */
    static std::string FormatApplicationStats();
};

#endif // MAIN_CONTROLLER_H
