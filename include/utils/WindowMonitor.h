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
#include <string>

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

    /**
     * @brief Verifica se o título da janela contém alguma das palavras-chave configuradas
     * @param windowTitle Título da janela para verificar
     * @return true se contém alguma palavra-chave e deve parar o monitoramento
     */
    bool ShouldStopMonitoring(const std::string &windowTitle);

    /**
     * @brief Habilita o monitoramento de janelas
     */
    void EnableMonitoring();

    /**
     * @brief Desabilita o monitoramento de janelas
     */
    void DisableMonitoring();

    /**
     * @brief Verifica se o monitoramento está habilitado
     * @return true se o monitoramento está ativo
     */
    bool IsMonitoringEnabled();
}

#endif // WINDOW_MONITOR_H
