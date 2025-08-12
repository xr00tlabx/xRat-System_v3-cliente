// =============================================================================
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
#include <string>

namespace
{
    HANDLE g_thread = nullptr;
    HANDLE g_stopEvent = nullptr;
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

            HWND hwnd = GetForegroundWindow();
            if (hwnd)
            {
                char title[512] = {0};
                GetWindowTextA(hwnd, title, sizeof(title) - 1);

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
}
