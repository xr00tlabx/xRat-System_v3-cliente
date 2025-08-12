// =============================================================================
// MainForm Header - Formulário Principal
// =============================================================================
// Projeto: Main C++ Application
// Arquivo: forms/MainForm.h
// Descrição: Classe responsável pela interface principal
// Autor: Desenvolvedor
// Data: 11/08/2025
// =============================================================================

#ifndef MAIN_FORM_H
#define MAIN_FORM_H

#include <windows.h>
#include <memory>

// Forward declarations
class MainController;

// =============================================================================
// CONSTANTES DA INTERFACE
// =============================================================================

namespace FormConstants
{
    // IDs dos controles
    const int ID_BUTTON_HELLO = 1001;
    const int ID_BUTTON_EXIT = 1002;
    const int ID_LABEL_TEXT = 1003;
    const int ID_BUTTON_ABOUT = 1004;
    const int ID_BUTTON_CONFIG = 1005;

    // Dimensões da janela
    const int WINDOW_WIDTH = 500;
    const int WINDOW_HEIGHT = 400;

    // Posições dos controles
    namespace ControlPos
    {
        const int MARGIN = 20;
        const int BUTTON_WIDTH = 120;
        const int BUTTON_HEIGHT = 35;
        const int LABEL_HEIGHT = 25;
        const int SPACING = 15;
    }
}

// =============================================================================
// ESTRUTURA DE DADOS DOS CONTROLES
// =============================================================================

struct FormControls
{
    HWND hMainWindow;
    HWND hLabelTitle;
    HWND hLabelStatus;
    HWND hButtonHello;
    HWND hButtonAbout;
    HWND hButtonConfig;
    HWND hButtonExit;

    HFONT hMainFont;
    HFONT hTitleFont;

    FormControls() : hMainWindow(nullptr), hLabelTitle(nullptr), hLabelStatus(nullptr),
                     hButtonHello(nullptr), hButtonAbout(nullptr), hButtonConfig(nullptr),
                     hButtonExit(nullptr), hMainFont(nullptr), hTitleFont(nullptr) {}
};

// =============================================================================
// CLASSE MAINFORM
// =============================================================================

class MainForm
{
private:
    static std::unique_ptr<FormControls> controls;
    static MainController *controller;
    static HINSTANCE hInstance;
    static bool isInitialized;

public:
    // =============================================================================
    // MÉTODOS ESTÁTICOS PÚBLICOS
    // =============================================================================

    /**
     * @brief Inicializa o formulário
     * @param hInst Instância da aplicação
     * @param ctrl Ponteiro para o controller
     * @return true se inicializado com sucesso
     */
    static bool Initialize(HINSTANCE hInst, MainController *ctrl);

    /**
     * @brief Cria a janela principal
     * @param nCmdShow Modo de exibição da janela
     * @return true se criada com sucesso
     */
    static bool CreateMainWindow(int nCmdShow);

    /**
     * @brief Cria todos os controles da interface
     * @return true se criados com sucesso
     */
    static bool CreateControls();

    /**
     * @brief Configura as fontes dos controles
     */
    static void SetupFonts();

    /**
     * @brief Atualiza o layout dos controles
     */
    static void UpdateLayout();

    /**
     * @brief Limpa recursos do formulário
     */
    static void Cleanup();

    // =============================================================================
    // MÉTODOS DE ACESSO
    // =============================================================================

    /**
     * @brief Obtém o handle da janela principal
     * @return Handle da janela principal
     */
    static HWND GetMainWindow();

    /**
     * @brief Verifica se o formulário foi inicializado
     * @return true se inicializado
     */
    static bool IsInitialized();

    // =============================================================================
    // MÉTODOS DE CONTROLE DE INTERFACE
    // =============================================================================

    /**
     * @brief Atualiza o texto do label de status
     * @param text Novo texto
     */
    static void UpdateStatusText(const wchar_t *text);

    /**
     * @brief Habilita/desabilita controles
     * @param enabled Estado dos controles
     */
    static void SetControlsEnabled(bool enabled);

    /**
     * @brief Mostra/oculta a janela
     * @param visible Estado de visibilidade
     */
    static void SetVisible(bool visible);

    /**
     * @brief Centraliza a janela na tela
     */
    static void CenterWindow();

    // =============================================================================
    // CALLBACK DA JANELA
    // =============================================================================

    /**
     * @brief Procedimento de janela (Window Procedure)
     * @param hwnd Handle da janela
     * @param uMsg Mensagem
     * @param wParam Parâmetro W
     * @param lParam Parâmetro L
     * @return Resultado do processamento da mensagem
     */
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    // =============================================================================
    // MÉTODOS PRIVADOS
    // =============================================================================

    /**
     * @brief Registra a classe da janela
     * @return true se registrada com sucesso
     */
    static bool RegisterWindowClass();

    /**
     * @brief Calcula posições dos controles
     */
    static void CalculateControlPositions();

    /**
     * @brief Aplica tema visual
     */
    static void ApplyVisualTheme();

    /**
     * @brief Processa comando de controle
     * @param controlId ID do controle
     * @param notificationCode Código de notificação
     */
    static void ProcessControlCommand(int controlId, int notificationCode);

    /**
     * @brief Desenha o fundo personalizado
     * @param hdc Device context
     * @param rect Retângulo da área cliente
     */
    static void DrawCustomBackground(HDC hdc, const RECT &rect);
};

#endif // MAIN_FORM_H
