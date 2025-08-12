// =============================================================================
// Main Application Header - Arquivo Principal
// =============================================================================
// Projeto: Main C++ Application
// Arquivo: main.h
// Descrição: Cabeçalho principal simplificado da aplicação
// Autor: Desenvolvedor
// Data: 11/08/2025
// =============================================================================

#ifndef MAIN_H
#define MAIN_H

#include <windows.h>

// =============================================================================
// INCLUDES DOS MÓDULOS
// =============================================================================

#include "utils/AppUtils.h"
#include "forms/MainForm.h"
#include "controllers/MainController.h"
#include "utils/WindowMonitor.h"

// =============================================================================
// CONFIGURAÇÕES DE DEBUG
// =============================================================================

#ifdef _DEBUG
#define DEBUG_PRINT(x) OutputDebugStringA(x)
#else
#define DEBUG_PRINT(x)
#endif

// =============================================================================
// CONSTANTES GLOBAIS DA APLICAÇÃO
// =============================================================================

namespace AppConstants
{
    const char *const APPLICATION_NAME = "Main Application";
    const char *const APPLICATION_VERSION = "1.0.0";
    const char *const WINDOW_CLASS_NAME = "MainApplicationWindow";

    const int APP_EXIT_SUCCESS = 0;
    const int APP_EXIT_ERROR = 1;
}

// =============================================================================
// CLASSE APPLICATION (PONTO DE ENTRADA)
// =============================================================================

class Application
{
public:
    /**
     * @brief Inicializa e executa a aplicação
     * @param hInstance Instância da aplicação
     * @param nCmdShow Modo de exibição da janela
     * @return Código de saída da aplicação
     */
    static int Run(HINSTANCE hInstance, int nCmdShow);

private:
    /**
     * @brief Inicializa todos os módulos da aplicação
     * @param hInstance Instância da aplicação
     * @return true se inicializado com sucesso
     */
    static bool InitializeModules(HINSTANCE hInstance);

    /**
     * @brief Finaliza todos os módulos da aplicação
     */
    static void ShutdownModules();
};

// =============================================================================
// DECLARAÇÃO DA FUNÇÃO PRINCIPAL
// =============================================================================

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

#endif // MAIN_H
