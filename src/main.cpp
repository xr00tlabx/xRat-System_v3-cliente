// =============================================================================
// Main Application Entry Point - Ponto de Entrada da Aplicação
// =============================================================================
// Projeto: Main C++ Application
// Arquivo: main.cpp
// Descrição: Ponto de entrada principal da aplicação com arquitetura MVC
// Autor: Desenvolvedor
// Data: 11/08/2025
// =============================================================================

#include "../include/main.h"
#include "../include/controllers/MainController.h"
#include "../include/forms/MainForm.h"
#include "../include/utils/AppUtils.h"
#include "../include/utils/WindowMonitor.h"

// =============================================================================
// IMPLEMENTAÇÃO DA CLASSE APPLICATION
// =============================================================================

int Application::Run(HINSTANCE hInstance, int nCmdShow)
{
    AppUtils::DebugPrint("Application::Run - Iniciando aplicação\n");
    AppUtils::WriteLog("Aplicação iniciando", "INFO");

    // Inicializar todos os módulos
    if (!InitializeModules(hInstance))
    {
        AppUtils::ShowErrorMessage("Erro ao inicializar os módulos da aplicação!");
        return AppConstants::APP_EXIT_ERROR;
    }

    // Iniciar a aplicação principal
    if (!MainController::StartApplication(nCmdShow))
    {
        AppUtils::ShowErrorMessage("Erro ao iniciar a aplicação principal!");
        ShutdownModules();
        return AppConstants::APP_EXIT_ERROR;
    }

    AppUtils::WriteLog("Aplicação iniciada com sucesso", "INFO");

    // Executar o loop principal de mensagens
    int exitCode = MainController::RunMessageLoop();

    AppUtils::WriteLog("Loop de mensagens finalizado", "INFO");

    // Finalizar todos os módulos
    ShutdownModules();

    AppUtils::WriteLog("Aplicação finalizada", "INFO");
    AppUtils::DebugPrint("Application::Run - Aplicação finalizada\n");

    return exitCode;
}

bool Application::InitializeModules(HINSTANCE hInstance)
{
    AppUtils::DebugPrint("Application::InitializeModules - Inicializando módulos\n");

    try
    {
        // Inicializar o Controller Principal
        if (!MainController::Initialize(hInstance))
        {
            AppUtils::ShowErrorMessage("Erro ao inicializar o MainController!");
            return false;
        }

        AppUtils::DebugPrint("Application::InitializeModules - MainController inicializado\n");

        // O MainForm será inicializado pelo Controller quando necessário

        // Iniciar thread de monitoramento de janelas
        WindowMonitor::Start();
        AppUtils::DebugPrint("Application::InitializeModules - WindowMonitor iniciado\n");

        AppUtils::WriteLog("Todos os módulos inicializados com sucesso", "INFO");
        return true;
    }
    catch (...)
    {
        AppUtils::ShowErrorMessage("Exceção durante a inicialização dos módulos!");
        return false;
    }
}

void Application::ShutdownModules()
{
    AppUtils::DebugPrint("Application::ShutdownModules - Finalizando módulos\n");

    try
    {
        // Parar thread de monitoramento primeiro
        WindowMonitor::Stop();
        AppUtils::DebugPrint("Application::ShutdownModules - WindowMonitor finalizado\n");

        // Finalizar o Form
        MainForm::Cleanup();
        AppUtils::DebugPrint("Application::ShutdownModules - MainForm finalizado\n");

        // Finalizar o Controller
        MainController::Shutdown();
        AppUtils::DebugPrint("Application::ShutdownModules - MainController finalizado\n");

        AppUtils::WriteLog("Todos os módulos finalizados com sucesso", "INFO");
    }
    catch (...)
    {
        AppUtils::DebugPrint("Exceção durante a finalização dos módulos\n");
    }
}

// =============================================================================
// FUNÇÃO PRINCIPAL DO WINDOWS
// =============================================================================

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /* hPrevInstance */, LPSTR /* lpCmdLine */, int nCmdShow)
{
// Inicializar sistema de debug
#ifdef _DEBUG
    AppUtils::DebugPrint("=== MAIN APPLICATION DEBUG MODE ===\n");
    AppUtils::DebugPrint("WinMain: Iniciando em modo DEBUG\n");
#endif

    AppUtils::WriteLog("=== SESSÃO INICIADA ===", "SYSTEM");
    AppUtils::WriteLog("WinMain: Ponto de entrada da aplicação", "INFO");

    // Executar a aplicação usando a classe Application
    int exitCode = Application::Run(hInstance, nCmdShow);

    // Log de finalização
    AppUtils::WriteLog("WinMain: Aplicação finalizada com código: " + std::to_string(exitCode), "INFO");
    AppUtils::WriteLog("=== SESSÃO FINALIZADA ===", "SYSTEM");

#ifdef _DEBUG
    AppUtils::DebugPrint("WinMain: Aplicação finalizada\n");
    AppUtils::DebugPrint("=== MAIN APPLICATION DEBUG END ===\n");
#endif

    return exitCode;
}
