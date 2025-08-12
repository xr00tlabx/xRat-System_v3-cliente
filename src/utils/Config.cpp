// =============================================================================
// Config Implementation - Implementação do Sistema de Configuração
// =============================================================================
// Projeto: Main C++ Application
// Arquivo: utils/Config.cpp
// Descrição: Implementação do sistema centralizado de configurações
// Autor: Desenvolvedor
// Data: 12/08/2025
// =============================================================================

#include "../../include/utils/Config.h"
#include "../../include/utils/AppUtils.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace Config
{
    // =============================================================================
    // VARIÁVEIS GLOBAIS PRIVADAS
    // =============================================================================
    
    static std::map<std::string, ConfigValue> configMap;
    static bool isInitialized = false;
    static std::string configFilePath;
    
    // =============================================================================
    // CONSTANTES PÚBLICAS
    // =============================================================================
    
    const char* const CONFIG_FILE_NAME = "config.json";
    const char* const DEFAULT_CLIENT_ID = "CLIENT_DEFAULT_001";
    
    // =============================================================================
    // FUNÇÕES AUXILIARES PRIVADAS
    // =============================================================================
    
    static std::string TrimString(const std::string& str)
    {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    static std::string EscapeJsonString(const std::string& str)
    {
        std::string result;
        for (char c : str)
        {
            switch (c)
            {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
    
    static std::string UnescapeJsonString(const std::string& str)
    {
        std::string result;
        for (size_t i = 0; i < str.length(); i++)
        {
            if (str[i] == '\\' && i + 1 < str.length())
            {
                switch (str[i + 1])
                {
                    case '"': result += '"'; i++; break;
                    case '\\': result += '\\'; i++; break;
                    case 'n': result += '\n'; i++; break;
                    case 'r': result += '\r'; i++; break;
                    case 't': result += '\t'; i++; break;
                    default: result += str[i]; break;
                }
            }
            else
            {
                result += str[i];
            }
        }
        return result;
    }
    
    static bool ParseJsonValue(const std::string& value, ConfigValue& configValue)
    {
        std::string trimmedValue = TrimString(value);
        
        if (trimmedValue.empty())
            return false;
        
        // String (entre aspas)
        if (trimmedValue.front() == '"' && trimmedValue.back() == '"')
        {
            std::string stringVal = trimmedValue.substr(1, trimmedValue.length() - 2);
            configValue = ConfigValue(UnescapeJsonString(stringVal));
            return true;
        }
        
        // Boolean
        if (trimmedValue == "true")
        {
            configValue = ConfigValue(true);
            return true;
        }
        if (trimmedValue == "false")
        {
            configValue = ConfigValue(false);
            return true;
        }
        
        // Number (int ou double)
        if (trimmedValue.find('.') != std::string::npos)
        {
            try
            {
                double doubleVal = std::stod(trimmedValue);
                configValue = ConfigValue(doubleVal);
                return true;
            }
            catch (...)
            {
                return false;
            }
        }
        else
        {
            try
            {
                int intVal = std::stoi(trimmedValue);
                configValue = ConfigValue(intVal);
                return true;
            }
            catch (...)
            {
                return false;
            }
        }
        
        return false;
    }
    
    static std::string FormatJsonValue(const ConfigValue& configValue)
    {
        switch (configValue.type)
        {
            case CONFIG_STRING:
                return "\"" + EscapeJsonString(configValue.stringValue) + "\"";
            case CONFIG_INT:
                return std::to_string(configValue.intValue);
            case CONFIG_BOOL:
                return configValue.boolValue ? "true" : "false";
            case CONFIG_DOUBLE:
                return std::to_string(configValue.doubleValue);
            default:
                return "\"\"";
        }
    }
    
    // =============================================================================
    // IMPLEMENTAÇÃO DAS FUNÇÕES PRINCIPAIS
    // =============================================================================
    
    bool Initialize()
    {
        if (isInitialized)
            return true;
        
        try
        {
            // Definir caminho do arquivo de configuração
            configFilePath = AppUtils::CombinePath(AppUtils::GetAppDirectory(), CONFIG_FILE_NAME);
            
            AppUtils::DebugPrint("Config::Initialize - Inicializando sistema de configuração\n");
            AppUtils::WriteLog("Inicializando sistema de configuração", "CONFIG");
            
            // Verificar se o arquivo existe
            if (!AppUtils::FileExists(configFilePath))
            {
                AppUtils::WriteLog("Arquivo de configuração não encontrado, criando arquivo padrão", "CONFIG");
                if (!CreateDefaultConfig())
                {
                    AppUtils::WriteLog("Erro ao criar arquivo de configuração padrão", "ERROR");
                    return false;
                }
            }
            
            // Carregar configurações do arquivo
            if (!LoadFromFile())
            {
                AppUtils::WriteLog("Erro ao carregar configurações do arquivo", "ERROR");
                return false;
            }
            
            isInitialized = true;
            AppUtils::WriteLog("Sistema de configuração inicializado com sucesso", "CONFIG");
            return true;
        }
        catch (...)
        {
            AppUtils::WriteLog("Exceção durante inicialização do sistema de configuração", "ERROR");
            return false;
        }
    }
    
    void Shutdown()
    {
        if (!isInitialized)
            return;
        
        try
        {
            AppUtils::DebugPrint("Config::Shutdown - Finalizando sistema de configuração\n");
            
            // Salvar configurações antes de finalizar
            SaveToFile();
            
            // Limpar dados
            configMap.clear();
            configFilePath.clear();
            isInitialized = false;
            
            AppUtils::WriteLog("Sistema de configuração finalizado", "CONFIG");
        }
        catch (...)
        {
            AppUtils::WriteLog("Exceção durante finalização do sistema de configuração", "ERROR");
        }
    }
    
    bool LoadFromFile()
    {
        try
        {
            std::ifstream file(configFilePath);
            if (!file.is_open())
            {
                AppUtils::WriteLog("Não foi possível abrir arquivo de configuração: " + configFilePath, "ERROR");
                return false;
            }
            
            std::string line;
            std::string content;
            
            // Ler todo o conteúdo do arquivo
            while (std::getline(file, line))
            {
                content += line + "\n";
            }
            file.close();
            
            // Parsing simples do JSON
            configMap.clear();
            
            // Remover espaços e quebras de linha desnecessárias
            content = TrimString(content);
            
            // Verificar se começa e termina com {}
            if (content.empty() || content.front() != '{' || content.back() != '}')
            {
                AppUtils::WriteLog("Formato JSON inválido no arquivo de configuração", "ERROR");
                return false;
            }
            
            // Remover as chaves externas
            content = content.substr(1, content.length() - 2);
            content = TrimString(content);
            
            if (content.empty())
            {
                AppUtils::WriteLog("Arquivo de configuração vazio, usando valores padrão", "CONFIG");
                return true;
            }
            
            // Dividir por vírgulas (parsing simples)
            std::istringstream ss(content);
            std::string pair;
            
            while (std::getline(ss, pair, ','))
            {
                pair = TrimString(pair);
                if (pair.empty()) continue;
                
                // Encontrar o separador ':'
                size_t colonPos = pair.find(':');
                if (colonPos == std::string::npos) continue;
                
                std::string key = TrimString(pair.substr(0, colonPos));
                std::string value = TrimString(pair.substr(colonPos + 1));
                
                // Remover aspas da chave se existirem
                if (key.front() == '"' && key.back() == '"')
                {
                    key = key.substr(1, key.length() - 2);
                }
                
                // Parsear o valor
                ConfigValue configValue;
                if (ParseJsonValue(value, configValue))
                {
                    configMap[key] = configValue;
                    AppUtils::DebugPrint(("Config carregado: " + key + "\n").c_str());
                }
            }
            
            AppUtils::WriteLog("Configurações carregadas com sucesso: " + std::to_string(configMap.size()) + " itens", "CONFIG");
            return true;
        }
        catch (...)
        {
            AppUtils::WriteLog("Exceção durante carregamento das configurações", "ERROR");
            return false;
        }
    }
    
    bool SaveToFile()
    {
        try
        {
            std::ofstream file(configFilePath);
            if (!file.is_open())
            {
                AppUtils::WriteLog("Não foi possível abrir arquivo para escrita: " + configFilePath, "ERROR");
                return false;
            }
            
            file << "{\n";
            
            bool first = true;
            for (const auto& pair : configMap)
            {
                if (!first)
                    file << ",\n";
                
                file << "  \"" << EscapeJsonString(pair.first) << "\": " << FormatJsonValue(pair.second);
                first = false;
            }
            
            file << "\n}\n";
            file.close();
            
            AppUtils::WriteLog("Configurações salvas com sucesso", "CONFIG");
            return true;
        }
        catch (...)
        {
            AppUtils::WriteLog("Exceção durante salvamento das configurações", "ERROR");
            return false;
        }
    }
    
    bool CreateDefaultConfig()
    {
        try
        {
            configMap.clear();
            
            // Configurações padrão da aplicação
            configMap["clientId"] = ConfigValue(std::string(DEFAULT_CLIENT_ID));
            configMap["appName"] = ConfigValue(std::string("Main Application"));
            configMap["appVersion"] = ConfigValue(std::string("1.0.0"));
            configMap["appAuthor"] = ConfigValue(std::string("Desenvolvedor C++"));
            configMap["appDescription"] = ConfigValue(std::string("Aplicação Windows desenvolvida em C++"));
            configMap["debugMode"] = ConfigValue(false);
            configMap["autoSave"] = ConfigValue(true);
            configMap["logLevel"] = ConfigValue(std::string("INFO"));
            configMap["windowWidth"] = ConfigValue(600);
            configMap["windowHeight"] = ConfigValue(400);
            configMap["windowCentered"] = ConfigValue(true);
            
            AppUtils::WriteLog("Configurações padrão criadas", "CONFIG");
            return SaveToFile();
        }
        catch (...)
        {
            AppUtils::WriteLog("Exceção durante criação das configurações padrão", "ERROR");
            return false;
        }
    }
    
    // =============================================================================
    // IMPLEMENTAÇÃO DAS FUNÇÕES DE ACESSO
    // =============================================================================
    
    std::string GetString(const std::string& key, const std::string& defaultValue)
    {
        auto it = configMap.find(key);
        if (it != configMap.end() && it->second.type == CONFIG_STRING)
        {
            return it->second.stringValue;
        }
        return defaultValue;
    }
    
    int GetInt(const std::string& key, int defaultValue)
    {
        auto it = configMap.find(key);
        if (it != configMap.end() && it->second.type == CONFIG_INT)
        {
            return it->second.intValue;
        }
        return defaultValue;
    }
    
    bool GetBool(const std::string& key, bool defaultValue)
    {
        auto it = configMap.find(key);
        if (it != configMap.end() && it->second.type == CONFIG_BOOL)
        {
            return it->second.boolValue;
        }
        return defaultValue;
    }
    
    double GetDouble(const std::string& key, double defaultValue)
    {
        auto it = configMap.find(key);
        if (it != configMap.end() && it->second.type == CONFIG_DOUBLE)
        {
            return it->second.doubleValue;
        }
        return defaultValue;
    }
    
    // =============================================================================
    // IMPLEMENTAÇÃO DAS FUNÇÕES DE MODIFICAÇÃO
    // =============================================================================
    
    void SetString(const std::string& key, const std::string& value, bool saveToFile)
    {
        configMap[key] = ConfigValue(value);
        if (saveToFile && isInitialized)
        {
            SaveToFile();
        }
    }
    
    void SetInt(const std::string& key, int value, bool saveToFile)
    {
        configMap[key] = ConfigValue(value);
        if (saveToFile && isInitialized)
        {
            SaveToFile();
        }
    }
    
    void SetBool(const std::string& key, bool value, bool saveToFile)
    {
        configMap[key] = ConfigValue(value);
        if (saveToFile && isInitialized)
        {
            SaveToFile();
        }
    }
    
    void SetDouble(const std::string& key, double value, bool saveToFile)
    {
        configMap[key] = ConfigValue(value);
        if (saveToFile && isInitialized)
        {
            SaveToFile();
        }
    }
    
    // =============================================================================
    // IMPLEMENTAÇÃO DAS FUNÇÕES DE UTILIDADE
    // =============================================================================
    
    bool HasConfig(const std::string& key)
    {
        return configMap.find(key) != configMap.end();
    }
    
    void RemoveConfig(const std::string& key, bool saveToFile)
    {
        configMap.erase(key);
        if (saveToFile && isInitialized)
        {
            SaveToFile();
        }
    }
    
    std::string GetConfigFilePath()
    {
        return configFilePath;
    }
    
    bool IsInitialized()
    {
        return isInitialized;
    }
    
    // =============================================================================
    // IMPLEMENTAÇÃO DAS CONFIGURAÇÕES ESPECÍFICAS
    // =============================================================================
    
    std::string GetClientId()
    {
        return GetString("clientId", DEFAULT_CLIENT_ID);
    }
    
    void SetClientId(const std::string& clientId)
    {
        SetString("clientId", clientId, true);
        AppUtils::WriteLog("Client ID atualizado: " + clientId, "CONFIG");
    }
    
    bool GetDebugMode()
    {
        return GetBool("debugMode", false);
    }
    
    void SetDebugMode(bool enabled)
    {
        SetBool("debugMode", enabled, true);
        AppUtils::WriteLog("Modo debug atualizado: " + std::string(enabled ? "ON" : "OFF"), "CONFIG");
    }
}