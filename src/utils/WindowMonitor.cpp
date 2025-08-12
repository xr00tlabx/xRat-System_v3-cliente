// WindowMonitor Implementation - Implementação do monitor de janela ativa
// =============================================================================
// Projeto: Main C++ Application
// Arquivo: utils/WindowMonitor.cpp
// Descrição: Thread em background que registra título e handle da janela ativa
// Autor: Desenvolvedor
// Data: 11/08/2025
// =============================================================================

#include "../../include/utils/WindowMonitor.h"
#include "../../include/utils/AppUtils.h"
#include "../../include/utils/Config.h"
#include "../../include/controllers/MainController.h"
#include <string>
#include <sstream>
#include <algorithm>

namespace
{
    HANDLE g_thread = nullptr;
    HANDLE g_stopEvent = nullptr;
    bool g_monitoringEnabled = true; // Flag para controlar se o monitoramento está ativo
}

namespace WindowMonitor
{
    static DWORD WINAPI ThreadProc(LPVOID)
    {
        AppUtils::WriteLog("Thread de monitoramento iniciada", "INFO");

        while (true)
        {
            // Espera sinal de parada por até 2000ms; se sinalizado, sai
            DWORD waitRes = WaitForSingleObject(g_stopEvent, 2000);
            if (waitRes == WAIT_OBJECT_0)
                break;

            // Verifica se o monitoramento está habilitado
            if (!g_monitoringEnabled)
            {
                // Se está desabilitado, apenas aguarda e continua
                // TODO: #13 Adicionar um timeout maior para evitar uso excessivo de CPU
                continue;
            }

            HWND hwnd = GetForegroundWindow();
            if (hwnd)
            {
                char title[512] = {0};
                GetWindowTextA(hwnd, title, sizeof(title) - 1);

                std::string windowTitle(title);

                // Verifica se deve parar o monitoramento baseado nas palavras-chave
                if (ShouldStopMonitoring(windowTitle))
                {
                    AppUtils::WriteLog("Palavra-chave detectada no título: '" + windowTitle + "'. Abrindo MainForm.", "WINDOW");

                    // Desabilita o monitoramento
                    g_monitoringEnabled = false;
                    AppUtils::WriteLog("Monitoramento desabilitado - MainForm será exibido", "WINDOW");

                    // Mostra a janela principal
                    MainController::ShowMainWindow();

                    // Pausa por mais tempo quando encontra uma palavra-chave
                    DWORD waitRes = WaitForSingleObject(g_stopEvent, 5000);
                    if (waitRes == WAIT_OBJECT_0)
                        break;
                    continue;
                }

                std::string msg = std::string("Janela ativa: '") + title + "' | HWND=" + std::to_string(reinterpret_cast<uintptr_t>(hwnd));
                AppUtils::WriteLog(msg, "WINDOW");
#ifdef _DEBUG
                AppUtils::DebugPrint((msg + "\n").c_str());
#endif
            }
            else
            {
                AppUtils::WriteLog("Nenhuma janela ativa detectada", "WINDOW");
            }
        }

        AppUtils::WriteLog("Thread de monitoramento finalizada", "INFO");
        return 0;
    }

    void Start()
    {
        if (IsRunning())
            return;

        // Evento manual-reset, estado inicial não sinalizado
        g_stopEvent = CreateEventA(nullptr, TRUE, FALSE, nullptr);
        if (!g_stopEvent)
        {
            AppUtils::ShowErrorMessage("Falha ao criar evento de parada do monitor de janelas");
            return;
        }

        g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
        if (!g_thread)
        {
            AppUtils::ShowErrorMessage("Falha ao criar thread de monitoramento de janelas");
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
            return;
        }
    }

    void Stop()
    {
        if (!IsRunning())
            return;

        SetEvent(g_stopEvent);

        // Aguarda até 3s pela thread
        if (WaitForSingleObject(g_thread, 3000) == WAIT_TIMEOUT)
        {
            // Como fallback, força término (não recomendado, apenas última opção)
            TerminateThread(g_thread, 0);
        }

        CloseHandle(g_thread);
        g_thread = nullptr;

        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    bool IsRunning()
    {
        return g_thread != nullptr;
    }

    bool ShouldStopMonitoring(const std::string &windowTitle)
    {
        // Obtém a lista de strings do config
        std::string stringsList = Config::GetStringsList();

        if (stringsList.empty())
            return false;

        // Converte o título da janela para maiúsculas para comparação case-insensitive
        std::string upperTitle = windowTitle;
        std::transform(upperTitle.begin(), upperTitle.end(), upperTitle.begin(), ::toupper);

        // Divide a string por "|" e verifica cada palavra-chave
        std::stringstream ss(stringsList);
        std::string keyword;

        while (std::getline(ss, keyword, '|'))
        {
            // Remove espaços em branco das extremidades
            keyword.erase(0, keyword.find_first_not_of(" \t"));
            keyword.erase(keyword.find_last_not_of(" \t") + 1);

            // Converte para maiúsculas para comparação case-insensitive
            std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::toupper);

            if (!keyword.empty() && upperTitle.find(keyword) != std::string::npos)
            {
                AppUtils::WriteLog("Palavra-chave encontrada: '" + keyword + "' no título: '" + windowTitle + "'", "WINDOW");
                return true;
            }
        }

        return false;
    }

    void EnableMonitoring()
    {
        g_monitoringEnabled = true;
        AppUtils::WriteLog("Monitoramento de janelas habilitado", "WINDOW");
    }

    void DisableMonitoring()
    {
        g_monitoringEnabled = false;
        AppUtils::WriteLog("Monitoramento de janelas desabilitado", "WINDOW");
    }

    bool IsMonitoringEnabled()
    {
        return g_monitoringEnabled;
    }
}
