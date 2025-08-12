// =============================================================================
// WindowMonitor Header - Monitor de Janela Ativa
// =============================================================================
// Projeto: Main C++ Application
// Arquivo: utils/WindowMonitor.h
// Descrição: Thread em background para registrar título e handle da janela ativa
// Autor: Desenvolvedor
// Data: 11/08/2025
// =============================================================================

#ifndef WINDOW_MONITOR_H
#define WINDOW_MONITOR_H

#include <windows.h>

namespace WindowMonitor
{
    /**
     * @brief Inicia a thread de monitoramento da janela ativa (intervalo de 2s)
     */
    void Start();

    /**
     * @brief Solicita parada e aguarda a finalização da thread
     */
    void Stop();

    /**
     * @brief Indica se a thread está em execução
     */
    bool IsRunning();
}

#endif // WINDOW_MONITOR_H
