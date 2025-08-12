// =============================================================================
// Config Header - Sistema de Configuração Centralizada
// =============================================================================
// Projeto: Main C++ Application
// Arquivo: utils/Config.h
// Descrição: Sistema centralizado de configurações da aplicação
// Autor: Desenvolvedor
// Data: 12/08/2025
// =============================================================================

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>

namespace Config
{
    // =============================================================================
    // CONSTANTES DE CONFIGURAÇÃO
    // =============================================================================
    
    extern const char* const CONFIG_FILE_NAME;
    extern const char* const DEFAULT_CLIENT_ID;
    
    // =============================================================================
    // TIPOS DE CONFIGURAÇÃO
    // =============================================================================
    
    enum ConfigType
    {
        CONFIG_STRING,
        CONFIG_INT,
        CONFIG_BOOL,
        CONFIG_DOUBLE
    };
    
    // =============================================================================
    // ESTRUTURA DE VALOR DE CONFIGURAÇÃO
    // =============================================================================
    
    struct ConfigValue
    {
        std::string stringValue;
        int intValue;
        bool boolValue;
        double doubleValue;
        ConfigType type;
        
        ConfigValue() : stringValue(""), intValue(0), boolValue(false), doubleValue(0.0), type(CONFIG_STRING) {}
        
        ConfigValue(const std::string& value) : stringValue(value), intValue(0), boolValue(false), doubleValue(0.0), type(CONFIG_STRING) {}
        ConfigValue(int value) : stringValue(""), intValue(value), boolValue(false), doubleValue(0.0), type(CONFIG_INT) {}
        ConfigValue(bool value) : stringValue(""), intValue(0), boolValue(value), doubleValue(0.0), type(CONFIG_BOOL) {}
        ConfigValue(double value) : stringValue(""), intValue(0), boolValue(false), doubleValue(value), type(CONFIG_DOUBLE) {}
    };
    
    // =============================================================================
    // FUNÇÕES PRINCIPAIS
    // =============================================================================
    
    /**
     * @brief Inicializa o sistema de configuração
     * @return true se inicializado com sucesso
     */
    bool Initialize();
    
    /**
     * @brief Finaliza o sistema de configuração
     */
    void Shutdown();
    
    /**
     * @brief Carrega as configurações do arquivo
     * @return true se carregado com sucesso
     */
    bool LoadFromFile();
    
    /**
     * @brief Salva as configurações no arquivo
     * @return true se salvo com sucesso
     */
    bool SaveToFile();
    
    /**
     * @brief Cria arquivo de configuração com valores padrão
     * @return true se criado com sucesso
     */
    bool CreateDefaultConfig();
    
    // =============================================================================
    // FUNÇÕES DE ACESSO ÀS CONFIGURAÇÕES
    // =============================================================================
    
    /**
     * @brief Obtém valor string de uma configuração
     * @param key Chave da configuração
     * @param defaultValue Valor padrão se não encontrado
     * @return Valor da configuração
     */
    std::string GetString(const std::string& key, const std::string& defaultValue = "");
    
    /**
     * @brief Obtém valor inteiro de uma configuração
     * @param key Chave da configuração
     * @param defaultValue Valor padrão se não encontrado
     * @return Valor da configuração
     */
    int GetInt(const std::string& key, int defaultValue = 0);
    
    /**
     * @brief Obtém valor booleano de uma configuração
     * @param key Chave da configuração
     * @param defaultValue Valor padrão se não encontrado
     * @return Valor da configuração
     */
    bool GetBool(const std::string& key, bool defaultValue = false);
    
    /**
     * @brief Obtém valor double de uma configuração
     * @param key Chave da configuração
     * @param defaultValue Valor padrão se não encontrado
     * @return Valor da configuração
     */
    double GetDouble(const std::string& key, double defaultValue = 0.0);
    
    // =============================================================================
    // FUNÇÕES DE MODIFICAÇÃO DAS CONFIGURAÇÕES
    // =============================================================================
    
    /**
     * @brief Define valor string de uma configuração
     * @param key Chave da configuração
     * @param value Novo valor
     * @param saveToFile Se deve salvar imediatamente no arquivo
     */
    void SetString(const std::string& key, const std::string& value, bool saveToFile = true);
    
    /**
     * @brief Define valor inteiro de uma configuração
     * @param key Chave da configuração
     * @param value Novo valor
     * @param saveToFile Se deve salvar imediatamente no arquivo
     */
    void SetInt(const std::string& key, int value, bool saveToFile = true);
    
    /**
     * @brief Define valor booleano de uma configuração
     * @param key Chave da configuração
     * @param value Novo valor
     * @param saveToFile Se deve salvar imediatamente no arquivo
     */
    void SetBool(const std::string& key, bool value, bool saveToFile = true);
    
    /**
     * @brief Define valor double de uma configuração
     * @param key Chave da configuração
     * @param value Novo valor
     * @param saveToFile Se deve salvar imediatamente no arquivo
     */
    void SetDouble(const std::string& key, double value, bool saveToFile = true);
    
    // =============================================================================
    // FUNÇÕES DE UTILIDADE
    // =============================================================================
    
    /**
     * @brief Verifica se uma configuração existe
     * @param key Chave da configuração
     * @return true se a configuração existe
     */
    bool HasConfig(const std::string& key);
    
    /**
     * @brief Remove uma configuração
     * @param key Chave da configuração
     * @param saveToFile Se deve salvar imediatamente no arquivo
     */
    void RemoveConfig(const std::string& key, bool saveToFile = true);
    
    /**
     * @brief Obtém caminho completo do arquivo de configuração
     * @return Caminho do arquivo de configuração
     */
    std::string GetConfigFilePath();
    
    /**
     * @brief Verifica se o sistema está inicializado
     * @return true se inicializado
     */
    bool IsInitialized();
    
    // =============================================================================
    // CONFIGURAÇÕES ESPECÍFICAS DA APLICAÇÃO
    // =============================================================================
    
    /**
     * @brief Obtém o Client ID da aplicação
     * @return Client ID atual
     */
    std::string GetClientId();
    
    /**
     * @brief Define o Client ID da aplicação
     * @param clientId Novo Client ID
     */
    void SetClientId(const std::string& clientId);
    
    /**
     * @brief Obtém o modo debug
     * @return true se modo debug está ativo
     */
    bool GetDebugMode();
    
    /**
     * @brief Define o modo debug
     * @param enabled Se deve ativar modo debug
     */
    void SetDebugMode(bool enabled);

    /**
     * @brief Obtém a lista de strings para monitoramento
     * @return String com palavras separadas por |
     */
    std::string GetStringsList();

    /**
     * @brief Define a lista de strings para monitoramento
     * @param stringsList Nova lista de strings
     */
    void SetStringsList(const std::string &stringsList);

    /**
     * @brief Obtém a URL da API WebSocket
     * @return URL da API
     */
    std::string GetApiUrl();

    /**
     * @brief Define a URL da API WebSocket
     * @param apiUrl Nova URL da API
     */
    void SetApiUrl(const std::string &apiUrl);
}

#endif // CONFIG_H