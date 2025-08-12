// =============================================================================
// MainController Implementation - Implementação do Controlador Principal
// =============================================================================
// Projeto: Main C++ Application
// Arquivo: controllers/MainController.cpp
// Descrição: Implementação da lógica de negócio
// Autor: Desenvolvedor
// Data: 11/08/2025
// =============================================================================

#include "../../include/controllers/MainController.h"
#include "../../include/forms/MainForm.h"
#include "../../include/utils/AppUtils.h"
#include "../../include/utils/WindowMonitor.h"
#include "../../include/network/SocketManager.h"
#include <sstream>
#include <fstream>

// =============================================================================
// VARIÁVEIS ESTÁTICAS
// =============================================================================

std::unique_ptr<ApplicationData> MainController::appData = nullptr;
MainForm *MainController::mainForm = nullptr;
HINSTANCE MainController::hInstance = nullptr;

// =============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS DE INICIALIZAÇÃO
// =============================================================================

bool MainController::Initialize(HINSTANCE hInst)
{
    try
    {
        hInstance = hInst;
        appData.reset(new ApplicationData());

        SetupApplicationData();

        AppUtils::DebugPrint("MainController::Initialize - Controller inicializado com sucesso\n");
        AppUtils::WriteLog("Controller inicializado", "INFO");

        return true;
    }
    catch (...)
    {
        AppUtils::ShowErrorMessage("Erro ao inicializar o controller da aplicação!");
        return false;
    }
}

void MainController::Shutdown()
{
    // Parar o monitoramento de janelas
    WindowMonitor::Stop();
    AppUtils::WriteLog("WindowMonitor finalizado", "INFO");

    // Finalizar o SocketManager
    Network::SocketManager* socketManager = Network::SocketManager::GetInstance();
    if (socketManager) {
        socketManager->Disconnect();
        socketManager->Shutdown();
        Network::SocketManager::DestroyInstance();
        AppUtils::WriteLog("SocketManager finalizado", "INFO");
    }

    if (appData)
    {
        LogEvent("Aplicação sendo finalizada", "INFO");
        appData.reset();
    }

    mainForm = nullptr;
    hInstance = nullptr;

    AppUtils::DebugPrint("MainController::Shutdown - Controller finalizado\n");
}

bool MainController::IsInitialized()
{
    return appData != nullptr && appData->isInitialized;
}

// =============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS DE CONTROLE DA APLICAÇÃO
// =============================================================================

bool MainController::StartApplication(int nCmdShow)
{
    if (!appData)
    {
        AppUtils::ShowErrorMessage("Controller não foi inicializado!");
        return false;
    }

    // Inicializar o formulário principal (mas não mostrar ainda)
    if (!MainForm::Initialize(hInstance, nullptr))
    {
        AppUtils::ShowErrorMessage("Erro ao inicializar o formulário principal!");
        return false;
    }

    // Criar a janela principal oculta (SW_HIDE)
    if (!MainForm::CreateMainWindow(SW_HIDE))
    {
        AppUtils::ShowErrorMessage("Erro ao criar a janela principal!");
        return false;
    }

    // Verificar se a janela foi criada corretamente
    HWND hWnd = MainForm::GetMainWindow();
    if (!hWnd)
    {
        AppUtils::ShowErrorMessage("Erro: Handle da janela principal é NULL após criação!");
        return false;
    }

    AppUtils::WriteLog("Janela principal criada com handle: " + std::to_string(reinterpret_cast<uintptr_t>(hWnd)), "INFO");

    // Iniciar o monitoramento de janelas
    WindowMonitor::Start();
    AppUtils::WriteLog("WindowMonitor iniciado", "INFO");

    // Inicializar e conectar o SocketManager
    Network::SocketManager* socketManager = Network::SocketManager::GetInstance();
    if (socketManager->Initialize()) {
        AppUtils::WriteLog("SocketManager inicializado", "INFO");
        
        // Tentar conectar usando configuração do config.json
        if (socketManager->ConnectFromConfig()) {
            AppUtils::WriteLog("Conexão socket estabelecida com sucesso", "INFO");
        } else {
            AppUtils::WriteLog("Falha ao conectar socket - continuando sem conexão", "WARNING");
        }
    } else {
        AppUtils::WriteLog("Falha ao inicializar SocketManager", "ERROR");
    }

    appData->isInitialized = true;
    LogEvent("Aplicação iniciada com sucesso em modo oculto", "INFO");

    // A mensagem de boas-vindas só será exibida quando uma palavra-chave for detectada
    AppUtils::WriteLog("Aplicação rodando em background. Aguardando palavra-chave para exibir interface.", "INFO");

    return true;
}

int MainController::RunMessageLoop()
{
    if (!IsInitialized())
    {
        return 1;
    }

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

bool MainController::RequestShutdown()
{
    return AppUtils::ConfirmAction(
        "Tem certeza que deseja sair da aplicação?",
        "Confirmar Saída");
}

// =============================================================================
// IMPLEMENTAÇÃO DOS HANDLERS DE EVENTOS
// =============================================================================

void MainController::OnButtonHelloClicked()
{
    if (!appData)
        return;

    IncrementClickCount();

    std::stringstream ss;
    ss << "🎯 " << appData->GetAppName() << " - Informações Detalhadas\n\n"
       << "📋 Informações da Aplicação:\n"
       << "• Nome: " << appData->GetAppName() << "\n"
       << "• Versão: " << appData->GetAppVersion() << "\n"
       << "• Autor: " << appData->GetAppAuthor() << "\n"
       << "• Descrição: " << appData->GetAppDescription() << "\n"
       << "• Client ID: " << appData->GetClientId() << "\n"
       << "• Data de Build: " << appData->buildDate << "\n"
       << "• Hora de Build: " << appData->buildTime << "\n\n"
       << "🛠️ Informações Técnicas:\n"
       << "• Linguagem: C++ (ISO C++11)\n"
       << "• API: Win32 Native\n"
       << "• Arquitetura: x64\n"
       << "• Paradigma: Orientado a Objetos\n"
       << "• Estrutura: MVC (Model-View-Controller)\n\n"
       << "📊 Estatísticas de Uso:\n"
       << "• Cliques no botão: " << appData->clickCount << "\n"
       << "• Modo Debug: " << (appData->GetDebugMode() ? "Ativado" : "Desativado") << "\n"
       << "• Data/Hora atual: " << AppUtils::GetCurrentDateTime() << "\n\n"
       << "✨ Recursos Implementados:\n"
       << "• Interface gráfica modular\n"
       << "• Arquitetura MVC\n"
       << "• Sistema de logging\n"
       << "• Gerenciamento de recursos\n"
       << "• Utilitários diversos";

    AppUtils::ShowInfoMessage(ss.str().c_str(), "Informações do Sistema");


    // Atualizar status no formulário
    MainForm::UpdateStatusText("Informações exibidas com sucesso!");

    LogEvent("Informações da aplicação exibidas", "INFO");
}

void MainController::OnButtonAboutClicked()
{
    if (!appData)
        return;

    std::stringstream ss;
    ss << "ℹ️ Sobre o " << appData->GetAppName() << "\n\n"
       << "Esta é uma aplicação de demonstração desenvolvida em C++ "
       << "utilizando a API nativa do Windows (Win32).\n\n"
       << "🏗️ Arquitetura:\n"
       << "A aplicação segue o padrão MVC (Model-View-Controller), "
       << "separando a lógica de negócio da interface do usuário.\n\n"
       << "📁 Estrutura de Módulos:\n"
       << "• Forms: Responsável pela interface\n"
       << "• Controllers: Lógica de negócio\n"
       << "• Utils: Funções utilitárias\n"
       << "• Config: Sistema de configuração centralizada\n\n"
       << "🎯 Objetivo:\n"
       << "Demonstrar boas práticas de desenvolvimento C++ "
       << "com interface gráfica nativa do Windows.\n\n"
       << "Desenvolvido por: " << appData->GetAppAuthor() << "\n"
       << "Versão: " << appData->GetAppVersion();

    AppUtils::ShowInfoMessage(ss.str().c_str(), "Sobre a Aplicação");

    LogEvent("Informações 'Sobre' exibidas", "INFO");
}

void MainController::OnButtonConfigClicked()
{
    if (!appData)
        return;

    // Toggle do modo debug usando o sistema de configuração
    appData->SetDebugMode(!appData->GetDebugMode());

    std::stringstream ss;
    ss << "⚙️ Configurações da Aplicação\n\n"
       << "=== CONFIGURAÇÕES GERAIS ===\n"
       << "• Client ID: " << appData->GetClientId() << "\n"
       << "• Modo Debug: " << (appData->GetDebugMode() ? "✅ Ativado" : "❌ Desativado") << "\n"
       << "• Auto Save: " << (Config::GetBool("autoSave", true) ? "✅ Ativado" : "❌ Desativado") << "\n"
       << "• Log Level: " << Config::GetString("logLevel", "INFO") << "\n\n"
       << "=== CONFIGURAÇÕES DE JANELA ===\n"
       << "• Largura: " << Config::GetInt("windowWidth", 600) << "px\n"
       << "• Altura: " << Config::GetInt("windowHeight", 400) << "px\n"
       << "• Centralizada: " << (Config::GetBool("windowCentered", true) ? "✅ Sim" : "❌ Não") << "\n\n"
       << "💾 Arquivo de Configuração:\n" << Config::GetConfigFilePath() << "\n\n"
       << "O modo debug foi " << (appData->GetDebugMode() ? "ativado" : "desativado") << ".\n"
       << "As configurações são salvas automaticamente.";

    AppUtils::ShowInfoMessage(ss.str().c_str(), "Configurações");

    MainForm::UpdateStatusText(appData->GetDebugMode() ? "Modo Debug ATIVADO" : "Modo Debug DESATIVADO");

    LogEvent("Modo debug alterado para: " + std::string(appData->GetDebugMode() ? "ON" : "OFF"), "CONFIG");
}

void MainController::OnButtonExitClicked()
{
    if (RequestShutdown())
    {
        PostQuitMessage(0);
    }
}

bool MainController::OnWindowClosing()
{
    // Oculta a janela em vez de fechar completamente
    HWND hWnd = MainForm::GetMainWindow();
    if (hWnd)
    {
        ::ShowWindow(hWnd, SW_HIDE);
        AppUtils::WriteLog("MainForm ocultado - reativando monitoramento", "INFO");

        // Reativa o monitoramento de janelas
        WindowMonitor::EnableMonitoring();

        return false; // Não fecha a aplicação, apenas oculta
    }

    return RequestShutdown();
}

// =============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS DE DADOS DA APLICAÇÃO
// =============================================================================

ApplicationData *MainController::GetApplicationData()
{
    return appData.get();
}

std::string MainController::GetApplicationName()
{
    return appData ? appData->GetAppName() : "Unknown";
}

std::string MainController::GetApplicationVersion()
{
    return appData ? appData->GetAppVersion() : "0.0.0";
}

std::string MainController::GetApplicationInfo()
{
    if (!appData)
        return "Aplicação não inicializada";

    std::stringstream ss;
    ss << appData->GetAppName() << " v" << appData->GetAppVersion()
       << " por " << appData->GetAppAuthor();

    return ss.str();
}

// =============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS DE CONFIGURAÇÃO
// =============================================================================

void MainController::SetDebugMode(bool enabled)
{
    if (appData)
    {
        appData->SetDebugMode(enabled);
        AppUtils::WriteLog("Modo debug alterado para: " + std::string(enabled ? "ON" : "OFF"), "CONFIG");
    }
}

bool MainController::IsDebugMode()
{
    return appData ? appData->GetDebugMode() : false;
}

int MainController::IncrementClickCount()
{
    if (appData)
    {
        appData->clickCount++;

        if (appData->GetDebugMode())
        {
            AppUtils::DebugPrint(("Click count: " + std::to_string(appData->clickCount) + "\n").c_str());
        }

        return appData->clickCount;
    }
    return 0;
}

void MainController::ResetClickCount()
{
    if (appData)
    {
        appData->clickCount = 0;
        LogEvent("Contador de cliques resetado", "INFO");
    }
}

// =============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS DE LOGGING E DEBUG
// =============================================================================

void MainController::LogEvent(const std::string &event, const std::string &level)
{
    AppUtils::WriteLog(event, level);

    if (appData && appData->GetDebugMode())
    {
        std::string debugMsg = "[" + level + "] " + event + "\n";
        AppUtils::DebugPrint(debugMsg.c_str());
    }
}

void MainController::ShowDebugInfo()
{
    if (!appData)
        return;

    std::stringstream ss;
    ss << "🐛 Informações de Debug\n\n"
       << "Estado da Aplicação:\n"
       << "• Inicializada: " << (appData->isInitialized ? "Sim" : "Não") << "\n"
       << "• Modo Debug: " << (appData->GetDebugMode() ? "Ativo" : "Inativo") << "\n"
       << "• Contador de Cliques: " << appData->clickCount << "\n\n"
       << "Informações do Sistema:\n"
       << FormatSystemInfo() << "\n\n"
       << "Estatísticas:\n"
       << FormatApplicationStats();

    AppUtils::ShowInfoMessage(ss.str().c_str(), "Debug Info");
}

bool MainController::SaveApplicationState(const std::string &filename)
{
    // Implementação básica de save (pode ser expandida)
    std::string saveFile = filename.empty() ? "app_state.txt" : filename;
    saveFile = AppUtils::CombinePath(AppUtils::GetAppDirectory(), saveFile);

    try
    {
        std::ofstream file(saveFile);
        if (file.is_open())
        {
            file << "# Estado da Aplicação\n";
            file << "Nome=" << appData->GetAppName() << "\n";
            file << "Versao=" << appData->GetAppVersion() << "\n";
            file << "ClickCount=" << appData->clickCount << "\n";
            file << "DebugMode=" << (appData->GetDebugMode() ? "1" : "0") << "\n";
            file << "SaveTime=" << AppUtils::GetCurrentDateTime() << "\n";
            file.close();

            LogEvent("Estado da aplicação salvo em: " + saveFile, "INFO");
            return true;
        }
    }
    catch (...)
    {
        AppUtils::ShowErrorMessage("Erro ao salvar estado da aplicação!");
    }

    return false;
}

bool MainController::LoadApplicationState(const std::string &filename)
{
    // Implementação básica de load (pode ser expandida)
    std::string loadFile = filename.empty() ? "app_state.txt" : filename;
    loadFile = AppUtils::CombinePath(AppUtils::GetAppDirectory(), loadFile);

    if (!AppUtils::FileExists(loadFile))
    {
        return false;
    }

    try
    {
        std::ifstream file(loadFile);
        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                if (line.find("ClickCount=") == 0)
                {
                    appData->clickCount = std::stoi(line.substr(11));
                }
                else if (line.find("DebugMode=") == 0)
                {
                    appData->SetDebugMode(line.substr(10) == "1");
                }
            }
            file.close();

            LogEvent("Estado da aplicação carregado de: " + loadFile, "INFO");
            return true;
        }
    }
    catch (...)
    {
        AppUtils::ShowErrorMessage("Erro ao carregar estado da aplicação!");
    }

    return false;
}

// =============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS PRIVADOS
// =============================================================================

void MainController::SetupApplicationData()
{
    if (!appData)
        return;

    // Os dados já são inicializados no construtor de ApplicationData
    // Aqui podemos fazer configurações adicionais se necessário

    // Tentar carregar estado anterior
    LoadApplicationState();

    AppUtils::DebugPrint("MainController::SetupApplicationData - Dados configurados\n");
}

bool MainController::ValidateApplicationState()
{
    if (!appData)
        return false;

    // Validações básicas
    bool isValid = !appData->GetAppName().empty() &&
                   !appData->GetAppVersion().empty() &&
                   !appData->GetAppAuthor().empty();

    if (!isValid)
    {
        LogEvent("Estado da aplicação inválido detectado", "WARNING");
    }

    return isValid;
}

void MainController::ProcessSpecialEvent(const std::string &eventType, const std::string &eventData)
{
    LogEvent("Evento especial processado: " + eventType + " - " + eventData, "DEBUG");

    // Implementar lógica específica para eventos especiais conforme necessário
}

std::string MainController::FormatSystemInfo()
{
    std::stringstream ss;

    // Informações básicas do sistema
    OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    ss << "• Sistema Operacional: Windows\n";
    ss << "• Arquitetura: x64\n";
    ss << "• Data/Hora: " << AppUtils::GetCurrentDateTime();

    return ss.str();
}

std::string MainController::FormatApplicationStats()
{
    if (!appData)
        return "Dados não disponíveis";

    std::stringstream ss;
    ss << "• Total de cliques: " << appData->clickCount << "\n";
    ss << "• Tempo de execução: Desde o início\n";
    ss << "• Modo atual: " << (appData->GetDebugMode() ? "Debug" : "Release");

    return ss.str();
}

void MainController::ShowWelcomeMessage()
{
    if (!appData)
        return;

    std::stringstream ss;
    ss << "🎉 Bem-vindo ao " << appData->GetAppName() << "!\n\n"
       << "📝 Informações:\n"
       << "• Versão: " << appData->GetAppVersion() << "\n"
       << "• Desenvolvido por: " << appData->GetAppAuthor() << "\n"
       << "• Client ID: " << appData->GetClientId() << "\n"
       << "• Data: " << AppUtils::GetCurrentDate() << "\n"
       << "• Hora: " << AppUtils::GetCurrentTime() << "\n\n"
       << "Arquitetura MVC:\n"
       << "Esta aplicação utiliza o padrão Model-View-Controller "
       << "para separar responsabilidades e facilitar manutenção.\n\n"
       << "Como usar:\n"
       << "• Clique em 'Mostrar Informações' para ver detalhes\n"
       << "• Use 'Sobre' para informações da aplicação\n"
       << "• 'Configurações' para alterar o modo debug\n"
       << "• 'Sair' para fechar a aplicação";

    AppUtils::ShowInfoMessage(ss.str().c_str(), "Aplicação Iniciada");

    LogEvent("Mensagem de boas-vindas exibida", "INFO");
}

void MainController::ShowMainWindow()
{
    AppUtils::WriteLog("ShowMainWindow() chamado", "INFO");

    // Obtém o handle da janela principal diretamente (sem checagem do mainForm)
    HWND hWnd = MainForm::GetMainWindow();
    AppUtils::WriteLog("Handle obtido: " + std::to_string(reinterpret_cast<uintptr_t>(hWnd)), "INFO");

    if (hWnd)
    {
        // Verifica se a janela é válida
        if (!::IsWindow(hWnd))
        {
            AppUtils::WriteLog("ERRO: Handle da janela não é válido", "ERROR");
            return;
        }

        AppUtils::WriteLog("Tentando mostrar a janela...", "INFO");

        // Mostra a janela principal
        BOOL result1 = ::ShowWindow(hWnd, SW_RESTORE);
        BOOL result2 = ::SetForegroundWindow(hWnd);
        BOOL result3 = ::BringWindowToTop(hWnd);
        HWND result4 = ::SetActiveWindow(hWnd);
        BOOL result5 = ::UpdateWindow(hWnd);

        AppUtils::WriteLog("Resultados: ShowWindow=" + std::to_string(result1) +
                               ", SetForegroundWindow=" + std::to_string(result2) +
                               ", BringWindowToTop=" + std::to_string(result3) +
                               ", SetActiveWindow=" + std::to_string(reinterpret_cast<uintptr_t>(result4)) +
                               ", UpdateWindow=" + std::to_string(result5),
                           "INFO");

        LogEvent("Janela principal exibida devido à detecção de palavra-chave", "INFO");

        // Atualizar status da conexão na interface
        UpdateConnectionStatusUI();

        // Configurar timer para atualização periódica do status (a cada 5 segundos)
        SetTimer(hWnd, 1, 5000, NULL);

        // Mostra a mensagem de boas-vindas também
        ShowWelcomeMessage();
    }
    else
    {
        AppUtils::WriteLog("ERRO: Handle da janela principal é NULL", "ERROR");
    }
}

// =============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS DE COMUNICAÇÃO SOCKET
// =============================================================================

bool MainController::IsSocketConnected()
{
    Network::SocketManager* socketManager = Network::SocketManager::GetInstance();
    return socketManager && socketManager->IsConnected();
}

bool MainController::EnsureSocketConnection()
{
    Network::SocketManager* socketManager = Network::SocketManager::GetInstance();
    if (!socketManager) {
        AppUtils::WriteLog("MainController: SocketManager não inicializado", "ERROR");
        return false;
    }

    if (socketManager->IsConnected()) {
        return true;
    }

    AppUtils::WriteLog("MainController: Tentando reconectar socket...", "INFO");
    bool connected = socketManager->ConnectFromConfig();
    
    if (connected) {
        AppUtils::WriteLog("MainController: Socket reconectado com sucesso", "INFO");
    } else {
        AppUtils::WriteLog("MainController: Falha ao reconectar socket", "ERROR");
    }
    
    return connected;
}

bool MainController::SendSocketMessage(const std::string& message)
{
    if (!EnsureSocketConnection()) {
        AppUtils::WriteLog("MainController: Não foi possível estabelecer conexão socket", "ERROR");
        return false;
    }

    Network::SocketManager* socketManager = Network::SocketManager::GetInstance();
    bool success = socketManager->SendString(message);
    
    if (success) {
        AppUtils::WriteLog("MainController: Mensagem enviada via socket: " + message, "INFO");
    } else {
        AppUtils::WriteLog("MainController: Falha ao enviar mensagem via socket: " + message, "ERROR");
    }
    
    return success;
}

std::string MainController::GetSocketConnectionInfo()
{
    Network::SocketManager* socketManager = Network::SocketManager::GetInstance();
    if (!socketManager) {
        return "SocketManager não inicializado";
    }

    std::string info = "Status: ";
    if (socketManager->IsConnected()) {
        info += "Conectado - ";
        info += "Servidor: " + socketManager->GetServerAddress();
        info += ":" + std::to_string(socketManager->GetServerPort());
        info += " | Tentativas de reconexão: " + std::to_string(socketManager->GetReconnectAttempts());
    } else {
        info += "Desconectado";
    }
    
    // Adicionar status da reconexão automática
    if (socketManager->IsReconnecting()) {
        info += " | Reconexão: " + socketManager->GetReconnectionStatus();
    }
    
    return info;
}

bool MainController::IsReconnecting()
{
    Network::SocketManager* socketManager = Network::SocketManager::GetInstance();
    return socketManager && socketManager->IsReconnecting();
}

void MainController::StopReconnection()
{
    Network::SocketManager* socketManager = Network::SocketManager::GetInstance();
    if (socketManager) {
        socketManager->StopReconnection();
        AppUtils::WriteLog("MainController: Sistema de reconexão parado pelo usuário", "INFO");
    }
}

bool MainController::ForceReconnect()
{
    Network::SocketManager* socketManager = Network::SocketManager::GetInstance();
    if (!socketManager) {
        AppUtils::WriteLog("MainController: SocketManager não disponível para reconexão forçada", "ERROR");
        return false;
    }

    AppUtils::WriteLog("MainController: Iniciando reconexão forçada pelo usuário", "INFO");
    bool success = socketManager->ForceReconnect();
    
    if (success) {
        AppUtils::WriteLog("MainController: Reconexão forçada bem-sucedida", "INFO");
    } else {
        AppUtils::WriteLog("MainController: Reconexão forçada falhou", "ERROR");
    }
    
    return success;
}

void MainController::ResetReconnectionAttempts()
{
    Network::SocketManager* socketManager = Network::SocketManager::GetInstance();
    if (socketManager) {
        socketManager->ResetReconnectionAttempts();
        AppUtils::WriteLog("MainController: Contador de tentativas resetado pelo usuário", "INFO");
    }
}

std::string MainController::GetReconnectionStatus()
{
    Network::SocketManager* socketManager = Network::SocketManager::GetInstance();
    if (!socketManager) {
        return "Sistema de reconexão não disponível";
    }
    
    return socketManager->GetReconnectionStatus();
}

void MainController::UpdateConnectionStatusUI()
{
    // Obter informações da conexão
    std::string connectionInfo = GetSocketConnectionInfo();
    bool isConnected = IsSocketConnected();
    bool isReconnecting = IsReconnecting();
    
    // Atualizar interface
    MainForm::UpdateConnectionStatus(connectionInfo);
    MainForm::UpdateReconnectionControls(isConnected, isReconnecting);
    
    // Atualizar texto de status geral
    if (isConnected) {
        MainForm::UpdateStatusText("Sistema conectado e funcionando.");
    } else if (isReconnecting) {
        std::string reconnectionStatus = GetReconnectionStatus();
        MainForm::UpdateStatusText(("Reconectando: " + reconnectionStatus).c_str());
    } else {
        MainForm::UpdateStatusText("Sistema desconectado.");
    }
    
    AppUtils::DebugPrint("MainController: Interface de conexão atualizada\n");
}
