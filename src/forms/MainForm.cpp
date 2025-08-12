// =============================================================================
// MainForm Implementation - Implementação do Formulário Principal
// =============================================================================
// Projeto: Main C++ Application
// Arquivo: forms/MainForm.cpp
// Descrição: Implementação da interface principal
// Autor: Desenvolvedor
// Data: 11/08/2025
// =============================================================================

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include "../../include/forms/MainForm.h"
#include "../../include/controllers/MainController.h"
#include "../../include/utils/AppUtils.h"
#include <string>

// =============================================================================
// VARIÁVEIS ESTÁTICAS
// =============================================================================

std::unique_ptr<FormControls> MainForm::controls = nullptr;
MainController *MainForm::controller = nullptr;
HINSTANCE MainForm::hInstance = nullptr;
bool MainForm::isInitialized = false;

// =============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS ESTÁTICOS PÚBLICOS
// =============================================================================

bool MainForm::Initialize(HINSTANCE hInst, MainController *ctrl)
{
    try
    {
        hInstance = hInst;
        controller = ctrl;
        controls.reset(new FormControls());

        AppUtils::DebugPrint("MainForm::Initialize - Formulário inicializado\n");
        AppUtils::WriteLog("Formulário inicializado", "INFO");

        return true;
    }
    catch (...)
    {
        AppUtils::ShowErrorMessage("Erro ao inicializar o formulário!");
        return false;
    }
}

bool MainForm::CreateMainWindow(int nCmdShow)
{
    if (!controls || !hInstance)
    {
        AppUtils::ShowErrorMessage("Formulário não foi inicializado!");
        return false;
    }

    // Registrar a classe da janela
    if (!RegisterWindowClass())
    {
        return false;
    }

    // Criar a janela principal
    controls->hMainWindow = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"MainApplicationWindow",
        L"Main Application - Arquitetura MVC",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        FormConstants::WINDOW_WIDTH, FormConstants::WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL);

    if (!controls->hMainWindow)
    {
        AppUtils::ShowErrorMessage("Erro ao criar a janela principal!");
        return false;
    }

    // Centralizar a janela
    CenterWindow();

    // Criar controles
    if (!CreateControls())
    {
        return false;
    }

    // Configurar fontes
    SetupFonts();

    // Aplicar tema visual
    ApplyVisualTheme();

    // Exibir a janela
    ShowWindow(controls->hMainWindow, nCmdShow);
    UpdateWindow(controls->hMainWindow);

    isInitialized = true;
    AppUtils::DebugPrint("MainForm::CreateMainWindow - Janela criada com sucesso\n");

    return true;
}

bool MainForm::CreateControls()
{
    if (!controls || !controls->hMainWindow)
    {
        return false;
    }

    HWND hwnd = controls->hMainWindow;
    const int margin = FormConstants::ControlPos::MARGIN;
    const int buttonWidth = FormConstants::ControlPos::BUTTON_WIDTH;
    const int buttonHeight = FormConstants::ControlPos::BUTTON_HEIGHT;
    const int spacing = FormConstants::ControlPos::SPACING;

    // Calcular posições
    int currentY = margin;

    // Label de título
    controls->hLabelTitle = CreateWindowEx(
        0, L"STATIC",
        L"Main Application - Arquitetura MVC",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        margin, currentY, FormConstants::WINDOW_WIDTH - (margin * 2), 30,
        hwnd, (HMENU)FormConstants::ID_LABEL_TEXT,
        hInstance, NULL);

    currentY += 40;

    // Label de status
    controls->hLabelStatus = CreateWindowEx(
        0, L"STATIC",
        L"Pronto para uso. Clique em um botão para começar!",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        margin, currentY, FormConstants::WINDOW_WIDTH - (margin * 2), 25,
        hwnd, (HMENU)(FormConstants::ID_LABEL_TEXT + 10),
        hInstance, NULL);

    currentY += 40;

    // Primeira linha de botões
    int buttonY = currentY;
    int button1X = margin;
    int button2X = margin + buttonWidth + spacing;

    // Botão "Mostrar Informações"
    controls->hButtonHello = CreateWindowEx(
        0, L"BUTTON",
        L"Mostrar Informações",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        button1X, buttonY, buttonWidth + 30, buttonHeight,
        hwnd, (HMENU)FormConstants::ID_BUTTON_HELLO,
        hInstance, NULL);

    // Botão "Sobre"
    controls->hButtonAbout = CreateWindowEx(
        0, L"BUTTON",
        L"Sobre",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        button2X + 30, buttonY, buttonWidth, buttonHeight,
        hwnd, (HMENU)FormConstants::ID_BUTTON_ABOUT,
        hInstance, NULL);

    currentY += buttonHeight + spacing;

    // Label de status da conexão
    controls->hLabelConnectionStatus = CreateWindowEx(
        0, L"STATIC",
        L"Status da Conexão: Verificando...",
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        margin, currentY, FormConstants::WINDOW_WIDTH - (margin * 2), 25,
        hwnd, (HMENU)FormConstants::ID_LABEL_CONNECTION_STATUS,
        hInstance, NULL);

    currentY += 30;

    // Terceira linha de botões - Controles de reconexão
    buttonY = currentY;

    // Botão "Forçar Reconexão"
    controls->hButtonReconnect = CreateWindowEx(
        0, L"BUTTON",
        L"Forçar Reconexão",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        button1X, buttonY, buttonWidth + 30, buttonHeight,
        hwnd, (HMENU)FormConstants::ID_BUTTON_RECONNECT,
        hInstance, NULL);

    // Botão "Parar Reconexão"
    controls->hButtonStopReconnect = CreateWindowEx(
        0, L"BUTTON",
        L"Parar Reconexão",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        button2X + 30, buttonY, buttonWidth, buttonHeight,
        hwnd, (HMENU)FormConstants::ID_BUTTON_STOP_RECONNECT,
        hInstance, NULL);

    currentY += buttonHeight + spacing;

    // Segunda linha de botões
    buttonY = currentY;

    // Botão "Configurações"
    controls->hButtonConfig = CreateWindowEx(
        0, L"BUTTON",
        L"Configurações",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        button1X, buttonY, buttonWidth + 30, buttonHeight,
        hwnd, (HMENU)FormConstants::ID_BUTTON_CONFIG,
        hInstance, NULL);

    // Botão "Sair"
    controls->hButtonExit = CreateWindowEx(
        0, L"BUTTON",
        L"Sair",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        button2X + 30, buttonY, buttonWidth, buttonHeight,
        hwnd, (HMENU)FormConstants::ID_BUTTON_EXIT,
        hInstance, NULL);

    // Verificar se todos os controles foram criados
    if (!controls->hLabelTitle || !controls->hLabelStatus ||
        !controls->hButtonHello || !controls->hButtonAbout ||
        !controls->hButtonConfig || !controls->hButtonExit ||
        !controls->hLabelConnectionStatus || !controls->hButtonReconnect ||
        !controls->hButtonStopReconnect)
    {
        AppUtils::ShowErrorMessage("Erro ao criar os controles da interface!");
        return false;
    }

    AppUtils::DebugPrint("MainForm::CreateControls - Controles criados com sucesso\n");
    return true;
}

void MainForm::SetupFonts()
{
    if (!controls)
        return;

    // Fonte principal
    controls->hMainFont = CreateFont(
        16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        VARIABLE_PITCH, TEXT("Segoe UI"));

    // Fonte do título
    controls->hTitleFont = CreateFont(
        18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        VARIABLE_PITCH, TEXT("Segoe UI"));

    // Aplicar fontes aos controles
    if (controls->hMainFont)
    {
        SendMessage(controls->hLabelStatus, WM_SETFONT, (WPARAM)controls->hMainFont, TRUE);
        SendMessage(controls->hButtonHello, WM_SETFONT, (WPARAM)controls->hMainFont, TRUE);
        SendMessage(controls->hButtonAbout, WM_SETFONT, (WPARAM)controls->hMainFont, TRUE);
        SendMessage(controls->hButtonConfig, WM_SETFONT, (WPARAM)controls->hMainFont, TRUE);
        SendMessage(controls->hButtonExit, WM_SETFONT, (WPARAM)controls->hMainFont, TRUE);
    }

    if (controls->hTitleFont)
    {
        SendMessage(controls->hLabelTitle, WM_SETFONT, (WPARAM)controls->hTitleFont, TRUE);
    }

    AppUtils::DebugPrint("MainForm::SetupFonts - Fontes configuradas\n");
}

void MainForm::UpdateLayout()
{
    if (!controls || !controls->hMainWindow)
        return;

    RECT rect;
    GetClientRect(controls->hMainWindow, &rect);

    // Recalcular posições se necessário
    // Esta função pode ser expandida para layouts dinâmicos

    InvalidateRect(controls->hMainWindow, NULL, TRUE);
}

void MainForm::Cleanup()
{
    if (controls)
    {
        // Limpar fontes
        if (controls->hMainFont)
        {
            DeleteObject(controls->hMainFont);
        }
        if (controls->hTitleFont)
        {
            DeleteObject(controls->hTitleFont);
        }

        controls.reset();
    }

    isInitialized = false;
    controller = nullptr;
    hInstance = nullptr;

    AppUtils::DebugPrint("MainForm::Cleanup - Limpeza concluída\n");
}

// =============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS DE ACESSO
// =============================================================================

HWND MainForm::GetMainWindow()
{
    return controls ? controls->hMainWindow : nullptr;
}

bool MainForm::IsInitialized()
{
    return isInitialized && controls != nullptr;
}

// =============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS DE CONTROLE DE INTERFACE
// =============================================================================

void MainForm::UpdateStatusText(const char *text)
{
    if (controls && controls->hLabelStatus)
    {
        // Converter texto para wide string
        int len = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
        wchar_t* wtext = new wchar_t[len];
        MultiByteToWideChar(CP_UTF8, 0, text, -1, wtext, len);
        
        SetWindowText(controls->hLabelStatus, wtext);
        AppUtils::DebugPrint(("Status atualizado: " + std::string(text) + "\n").c_str());
        
        delete[] wtext;
    }
}

void MainForm::UpdateConnectionStatus(const std::string& connectionInfo)
{
    if (controls && controls->hLabelConnectionStatus)
    {
        std::string statusText = "Status da Conexão: " + connectionInfo;
        
        // Converter texto para wide string
        int len = MultiByteToWideChar(CP_UTF8, 0, statusText.c_str(), -1, NULL, 0);
        wchar_t* wtext = new wchar_t[len];
        MultiByteToWideChar(CP_UTF8, 0, statusText.c_str(), -1, wtext, len);
        
        SetWindowText(controls->hLabelConnectionStatus, wtext);
        AppUtils::DebugPrint(("Status da conexão atualizado: " + statusText + "\n").c_str());
        
        delete[] wtext;
    }
}

void MainForm::UpdateReconnectionControls(bool isConnected, bool isReconnecting)
{
    if (!controls || !controls->hButtonReconnect || !controls->hButtonStopReconnect)
        return;
    
    // Habilitar botão "Forçar Reconexão" apenas quando não está conectado e não está tentando reconectar
    EnableWindow(controls->hButtonReconnect, !isConnected && !isReconnecting);
    
    // Habilitar botão "Parar Reconexão" apenas quando está tentando reconectar
    EnableWindow(controls->hButtonStopReconnect, isReconnecting);
    
    AppUtils::DebugPrint("Controles de reconexão atualizados\n");
}

void MainForm::SetControlsEnabled(bool enabled)
{
    if (!controls)
        return;

    EnableWindow(controls->hButtonHello, enabled);
    EnableWindow(controls->hButtonAbout, enabled);
    EnableWindow(controls->hButtonConfig, enabled);
    EnableWindow(controls->hButtonExit, enabled);

    AppUtils::DebugPrint(enabled ? "Controles habilitados\n" : "Controles desabilitados\n");
}

void MainForm::SetVisible(bool visible)
{
    if (controls && controls->hMainWindow)
    {
        ShowWindow(controls->hMainWindow, visible ? SW_SHOW : SW_HIDE);
    }
}

void MainForm::CenterWindow()
{
    if (controls && controls->hMainWindow)
    {
        AppUtils::CenterWindow(controls->hMainWindow,
                               FormConstants::WINDOW_WIDTH,
                               FormConstants::WINDOW_HEIGHT);
    }
}

// =============================================================================
// IMPLEMENTAÇÃO DO CALLBACK DA JANELA
// =============================================================================

LRESULT CALLBACK MainForm::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        int controlId = LOWORD(wParam);
        int notificationCode = HIWORD(wParam);
        ProcessControlCommand(controlId, notificationCode);
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        DrawCustomBackground(hdc, ps.rcPaint);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_CLOSE:
    {
        // Chamar o controller para verificar se pode fechar
        if (MainController::OnWindowClosing())
        {
            DestroyWindow(hwnd);
        }
        return 0;
    }

    case WM_DESTROY:
    {
        AppUtils::DebugPrint("MainForm::WindowProc - WM_DESTROY recebido\n");
        
        // Parar timer de atualização de status
        KillTimer(hwnd, 1);
        
        PostQuitMessage(0);
        return 0;
    }

    case WM_SIZE:
    {
        // Atualizar layout quando a janela for redimensionada
        UpdateLayout();
        return 0;
    }

    case WM_TIMER:
    {
        // Timer para atualização periódica do status da conexão
        if (wParam == 1) // Timer ID 1 = status update
        {
            MainController::UpdateConnectionStatusUI();
        }
        return 0;
    }

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

// =============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS PRIVADOS
// =============================================================================

bool MainForm::RegisterWindowClass()
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MainApplicationWindow";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        AppUtils::ShowErrorMessage("Erro ao registrar a classe da janela!");
        return false;
    }

    return true;
}

void MainForm::CalculateControlPositions()
{
    // Esta função pode ser usada para layouts mais complexos
    // Por enquanto, as posições são calculadas diretamente em CreateControls()
}

void MainForm::ApplyVisualTheme()
{
    if (!controls || !controls->hMainWindow)
        return;

    // Aplicar temas visuais modernos se disponível
    // Por enquanto, usar o tema padrão do sistema
}

void MainForm::ProcessControlCommand(int controlId, int notificationCode)
{
    // Verificar se é um clique de botão
    if (notificationCode != BN_CLICKED)
        return;

    switch (controlId)
    {
    case FormConstants::ID_BUTTON_HELLO:
        MainController::OnButtonHelloClicked();
        break;

    case FormConstants::ID_BUTTON_ABOUT:
        MainController::OnButtonAboutClicked();
        break;

    case FormConstants::ID_BUTTON_CONFIG:
        MainController::OnButtonConfigClicked();
        break;

    case FormConstants::ID_BUTTON_EXIT:
        MainController::OnButtonExitClicked();
        break;

    case FormConstants::ID_BUTTON_RECONNECT:
        OnButtonReconnectClicked();
        break;

    case FormConstants::ID_BUTTON_STOP_RECONNECT:
        OnButtonStopReconnectClicked();
        break;

    default:
        AppUtils::DebugPrint(("Comando não reconhecido: " + std::to_string(controlId) + "\n").c_str());
        break;
    }
}

// =============================================================================
// IMPLEMENTAÇÃO DOS HANDLERS DE BOTÕES DE RECONEXÃO
// =============================================================================

void MainForm::OnButtonReconnectClicked()
{
    AppUtils::WriteLog("MainForm: Botão 'Forçar Reconexão' clicado", "INFO");
    
    // Atualizar status
    UpdateStatusText("Forçando reconexão...");
    
    // Chamar método do controller
    bool success = MainController::ForceReconnect();
    
    if (success) {
        UpdateStatusText("Reconexão forçada bem-sucedida!");
    } else {
        UpdateStatusText("Falha na reconexão forçada.");
    }
    
    // Atualizar informações de conexão
    std::string connectionInfo = MainController::GetSocketConnectionInfo();
    UpdateConnectionStatus(connectionInfo);
    
    // Atualizar controles
    UpdateReconnectionControls(
        MainController::IsSocketConnected(),
        MainController::IsReconnecting()
    );
}

void MainForm::OnButtonStopReconnectClicked()
{
    AppUtils::WriteLog("MainForm: Botão 'Parar Reconexão' clicado", "INFO");
    
    // Atualizar status
    UpdateStatusText("Parando sistema de reconexão...");
    
    // Chamar método do controller
    MainController::StopReconnection();
    
    UpdateStatusText("Sistema de reconexão parado.");
    
    // Atualizar informações de conexão
    std::string connectionInfo = MainController::GetSocketConnectionInfo();
    UpdateConnectionStatus(connectionInfo);
    
    // Atualizar controles
    UpdateReconnectionControls(
        MainController::IsSocketConnected(),
        MainController::IsReconnecting()
    );
}

void MainForm::DrawCustomBackground(HDC hdc, const RECT &rect)
{
    // Criar gradiente de fundo
    RECT clientRect;
    GetClientRect(controls->hMainWindow, &clientRect);

    // Cor de fundo principal
    HBRUSH hBrush = CreateSolidBrush(RGB(248, 250, 252));
    FillRect(hdc, &clientRect, hBrush);
    DeleteObject(hBrush);

    // Borda decorativa
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(59, 130, 246));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    // Desenhar borda com cantos arredondados (simulado)
    Rectangle(hdc, 5, 5, clientRect.right - 5, clientRect.bottom - 5);

    // Linha separadora após o título
    MoveToEx(hdc, 20, 80, NULL);
    LineTo(hdc, clientRect.right - 20, 80);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}
