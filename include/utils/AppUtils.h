// =============================================================================
// Utils Header - Utilitários Gerais
// =============================================================================
// Projeto: Main C++ Application
// Arquivo: utils/AppUtils.h
// Descrição: Funções utilitárias para a aplicação
// Autor: Desenvolvedor
// Data: 11/08/2025
// =============================================================================

#ifndef APP_UTILS_H
#define APP_UTILS_H

#include <windows.h>
#include <string>

namespace AppUtils
{
    // =============================================================================
    // FUNÇÕES DE JANELA
    // =============================================================================

    /**
     * @brief Centraliza uma janela na tela
     * @param hwnd Handle da janela
     * @param width Largura da janela
     * @param height Altura da janela
     */
    void CenterWindow(HWND hwnd, int width, int height);

    /**
     * @brief Obtém as dimensões da tela
     * @param screenWidth Largura da tela (saída)
     * @param screenHeight Altura da tela (saída)
     */
    void GetScreenDimensions(int &screenWidth, int &screenHeight);

    // =============================================================================
    // FUNÇÕES DE DATA/HORA
    // =============================================================================

    /**
     * @brief Obtém a data e hora atual formatada
     * @return String com data/hora atual
     */
    std::string GetCurrentDateTime();

    /**
     * @brief Obtém apenas a data atual
     * @return String com data atual
     */
    std::string GetCurrentDate();

    /**
     * @brief Obtém apenas a hora atual
     * @return String com hora atual
     */
    std::string GetCurrentTime();

    // =============================================================================
    // FUNÇÕES DE MENSAGEM
    // =============================================================================

    /**
     * @brief Exibe uma mensagem de erro
     * @param message Mensagem a ser exibida
     * @param title Título da janela (opcional)
     */
    void ShowErrorMessage(const char *message, const char *title = "Erro");

    /**
     * @brief Exibe uma mensagem informativa
     * @param message Mensagem a ser exibida
     * @param title Título da janela (opcional)
     */
    void ShowInfoMessage(const char *message, const char *title = "Informação");

    /**
     * @brief Exibe uma mensagem de aviso
     * @param message Mensagem a ser exibida
     * @param title Título da janela (opcional)
     */
    void ShowWarningMessage(const char *message, const char *title = "Aviso");

    /**
     * @brief Solicita confirmação do usuário
     * @param message Mensagem de confirmação
     * @param title Título da janela (opcional)
     * @return true se o usuário confirmou, false caso contrário
     */
    bool ConfirmAction(const char *message, const char *title = "Confirmar");

    // =============================================================================
    // FUNÇÕES DE STRING
    // =============================================================================

    /**
     * @brief Converte string para wide string
     * @param str String de entrada
     * @return Wide string
     */
    std::wstring StringToWString(const std::string &str);

    /**
     * @brief Converte wide string para string
     * @param wstr Wide string de entrada
     * @return String normal
     */
    std::string WStringToString(const std::wstring &wstr);

    /**
     * @brief Remove espaços em branco no início e fim da string
     * @param str String a ser processada
     * @return String sem espaços nas extremidades
     */
    std::string Trim(const std::string &str);

    // =============================================================================
    // FUNÇÕES DE ARQUIVO
    // =============================================================================

    /**
     * @brief Verifica se um arquivo existe
     * @param filename Nome do arquivo
     * @return true se o arquivo existe, false caso contrário
     */
    bool FileExists(const std::string &filename);

    /**
     * @brief Obtém o diretório da aplicação
     * @return Caminho do diretório da aplicação
     */
    std::string GetAppDirectory();

    /**
     * @brief Combina caminhos de arquivo
     * @param path1 Primeiro caminho
     * @param path2 Segundo caminho
     * @return Caminho combinado
     */
    std::string CombinePath(const std::string &path1, const std::string &path2);

    // =============================================================================
    // FUNÇÕES DE DEBUG
    // =============================================================================

    /**
     * @brief Imprime mensagem de debug (apenas em modo debug)
     * @param message Mensagem de debug
     */
    void DebugPrint(const char *message);

    /**
     * @brief Salva log em arquivo
     * @param message Mensagem para o log
     * @param logType Tipo do log (INFO, ERROR, WARNING)
     */
    void WriteLog(const std::string &message, const std::string &logType = "INFO");
}

#endif // APP_UTILS_H
