// =============================================================================
// Config Usage Examples - Exemplos de Uso do Sistema de Configuração
// =============================================================================
// Este arquivo demonstra como outras partes do código podem usar o sistema Config

#include "../include/utils/Config.h"
#include <iostream>

// Exemplo de função que lê configurações
void ExampleReadConfigurations()
{
    // Ler o Client ID (configuração principal solicitada na issue)
    std::string clientId = Config::GetClientId();
    std::cout << "Client ID atual: " << clientId << std::endl;
    
    // Ler outras configurações com valores padrão
    std::string appName = Config::GetString("appName", "Aplicação Desconhecida");
    bool debugMode = Config::GetDebugMode();
    int windowWidth = Config::GetInt("windowWidth", 800);
    bool autoSave = Config::GetBool("autoSave", true);
    
    std::cout << "Aplicação: " << appName << std::endl;
    std::cout << "Debug Mode: " << (debugMode ? "Ativo" : "Inativo") << std::endl;
    std::cout << "Largura da Janela: " << windowWidth << "px" << std::endl;
    std::cout << "Auto Save: " << (autoSave ? "Ativo" : "Inativo") << std::endl;
}

// Exemplo de função que modifica configurações
void ExampleModifyConfigurations()
{
    // Alterar o Client ID (conforme solicitado na issue)
    Config::SetClientId("CLIENT_MODIFIED_002");
    
    // Alterar modo debug
    Config::SetDebugMode(true);
    
    // Alterar configurações de janela
    Config::SetInt("windowWidth", 1024);
    Config::SetInt("windowHeight", 768);
    
    // Adicionar nova configuração personalizada
    Config::SetString("customFeature", "enabled");
    Config::SetDouble("customThreshold", 0.75);
    
    std::cout << "Configurações alteradas e salvas automaticamente!" << std::endl;
}

// Exemplo de função que verifica e gerencia configurações
void ExampleManageConfigurations()
{
    // Verificar se uma configuração existe
    if (Config::HasConfig("customFeature")) {
        std::cout << "Configuração customFeature existe: " 
                  << Config::GetString("customFeature") << std::endl;
    }
    
    // Listar informações do sistema de configuração
    std::cout << "Sistema Config inicializado: " 
              << (Config::IsInitialized() ? "Sim" : "Não") << std::endl;
    std::cout << "Arquivo de configuração: " 
              << Config::GetConfigFilePath() << std::endl;
    
    // Exemplo de configuração condicional
    if (!Config::HasConfig("firstRun")) {
        Config::SetBool("firstRun", false);
        std::cout << "Primeira execução detectada, configurações iniciais aplicadas." << std::endl;
    }
}

// Exemplo de uso em uma classe de negócio
class BusinessLogic 
{
private:
    std::string clientId;
    bool debugEnabled;
    
public:
    BusinessLogic() {
        // Inicializar com configurações do sistema
        RefreshFromConfig();
    }
    
    void RefreshFromConfig() {
        clientId = Config::GetClientId();
        debugEnabled = Config::GetDebugMode();
    }
    
    void ProcessData() {
        if (debugEnabled) {
            std::cout << "DEBUG: Processando dados para cliente " << clientId << std::endl;
        }
        
        // Lógica de negócio aqui...
        
        // Atualizar estatísticas nas configurações
        int processCount = Config::GetInt("processCount", 0);
        Config::SetInt("processCount", processCount + 1);
    }
    
    void UpdateClientId(const std::string& newClientId) {
        clientId = newClientId;
        Config::SetClientId(newClientId); // Persistir mudança
        std::cout << "Client ID atualizado para: " << newClientId << std::endl;
    }
};

// Exemplo de uso em configurações específicas de módulos
namespace ModuleExample 
{
    void InitializeModule() {
        // Cada módulo pode ter suas próprias configurações
        bool moduleEnabled = Config::GetBool("moduleExample.enabled", true);
        std::string moduleMode = Config::GetString("moduleExample.mode", "normal");
        double moduleTimeout = Config::GetDouble("moduleExample.timeout", 30.0);
        
        if (moduleEnabled) {
            std::cout << "Módulo inicializado em modo: " << moduleMode 
                      << " com timeout de " << moduleTimeout << "s" << std::endl;
        }
    }
    
    void ConfigureModule(const std::string& mode, double timeout) {
        Config::SetString("moduleExample.mode", mode);
        Config::SetDouble("moduleExample.timeout", timeout);
        Config::SetBool("moduleExample.enabled", true);
        
        std::cout << "Módulo configurado: " << mode << ", timeout: " << timeout << "s" << std::endl;
    }
}

// =============================================================================
// RESUMO DOS EXEMPLOS:
// =============================================================================
// 
// 1. ExampleReadConfigurations(): Como ler configurações com valores padrão
// 2. ExampleModifyConfigurations(): Como alterar e salvar configurações
// 3. ExampleManageConfigurations(): Como verificar e gerenciar configurações
// 4. BusinessLogic: Como usar em classes de negócio 
// 5. ModuleExample: Como usar para configurações específicas de módulos
//
// O sistema permite que qualquer parte do código:
// - Leia configurações centralizadas
// - Modifique configurações (que são salvas automaticamente)
// - Adicione novas configurações conforme necessário
// - Use valores padrão quando configurações não existem
// - Organize configurações por namespaces (ex: "module.setting")
//
// Isso atende completamente ao requisito da issue de centralizar todas as
// configurações e permitir que outras partes do código as utilizem e alterem.
// =============================================================================