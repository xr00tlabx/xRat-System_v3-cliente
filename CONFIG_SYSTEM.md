# Sistema de Configuração Centralizada

## Visão Geral

O sistema de configuração centralizada foi implementado para atender ao requisito de centralizar todas as configurações da aplicação em um arquivo de configuração, permitindo que outras partes do código possam utilizá-las e alterá-las.

## Arquivos Implementados

### `include/utils/Config.h`
- **Namespace Config**: Sistema centralizado de configuração
- **Tipos de dados**: Suporte a string, int, bool e double
- **Funções principais**: Initialize(), LoadFromFile(), SaveToFile()
- **Acessors**: GetString(), GetInt(), GetBool(), GetDouble()
- **Mutators**: SetString(), SetInt(), SetBool(), SetDouble()
- **Configurações específicas**: GetClientId(), SetClientId(), GetDebugMode(), SetDebugMode()

### `src/utils/Config.cpp`
- **Parser JSON simples**: Implementação própria sem dependências externas
- **Gerenciamento automático**: Carregamento e salvamento automático das configurações
- **Valores padrão**: Sistema de fallback para configurações não encontradas
- **Logging integrado**: Registra alterações de configuração no sistema de log

### `config.json`
Arquivo de configuração padrão com as seguintes configurações iniciais:

```json
{
  "clientId": "CLIENT_DEFAULT_001",
  "appName": "Main Application", 
  "appVersion": "1.0.0",
  "appAuthor": "Desenvolvedor C++",
  "appDescription": "Aplicação Windows desenvolvida em C++",
  "debugMode": false,
  "autoSave": true,
  "logLevel": "INFO",
  "windowWidth": 600,
  "windowHeight": 400,
  "windowCentered": true
}
```

## Integração com o Código Existente

### ApplicationData (MainController.h)
- **Antes**: Valores hardcoded no construtor
- **Depois**: Métodos que delegam para o sistema Config
- **Exemplo**: `GetAppName()` → `Config::GetString("appName")`

### MainController.cpp
- **Inicialização**: Config::Initialize() é chamado no startup da aplicação
- **Uso**: Todas as referências a campos hardcoded foram atualizadas
- **Configurações**: Botão de configuração agora mostra todas as configurações ativas

### main.cpp
- **Inicialização**: Config é inicializado antes dos outros módulos
- **Finalização**: Config é finalizado após os outros módulos para manter ordem

## Como Usar

### Lendo Configurações
```cpp
// Ler configurações básicas
std::string clientId = Config::GetClientId();
bool debugMode = Config::GetDebugMode();
int windowWidth = Config::GetInt("windowWidth", 800);

// Ler com valores padrão
std::string logLevel = Config::GetString("logLevel", "INFO");
```

### Modificando Configurações
```cpp
// Modificar e salvar automaticamente
Config::SetClientId("NEW_CLIENT_123");
Config::SetDebugMode(true);
Config::SetInt("windowWidth", 1024, true); // true = salvar no arquivo
```

### Verificando Existência
```cpp
if (Config::HasConfig("customSetting")) {
    std::string value = Config::GetString("customSetting");
}
```

## Características Técnicas

### Formato JSON Simples
- Parser próprio sem dependências externas
- Suporte a strings (com escape), números, booleans
- Estrutura plana (sem objetos aninhados para simplicidade)

### Gerenciamento de Arquivos
- Carregamento automático na inicialização
- Salvamento automático nas alterações (configurável)
- Criação automática de arquivo padrão se não existir

### Tratamento de Erros
- Validação de formato JSON
- Fallback para valores padrão
- Logging de erros de E/O
- Tratamento de exceções

### Integração com Logging
- Registra inicialização e finalização do sistema
- Registra alterações de configuração importantes
- Mensagens de debug em modo de desenvolvimento

## Benefícios

1. **Centralização**: Todas as configurações em um local
2. **Persistência**: Configurações salvas automaticamente
3. **Flexibilidade**: Fácil adição de novas configurações
4. **Manutenibilidade**: Código mais limpo e organizudo
5. **Configurabilidade**: Usuários podem editar o arquivo JSON
6. **Extensibilidade**: Sistema preparado para crescer

## Configurações Disponíveis

### Configurações da Aplicação
- `clientId`: Identificador único do cliente (conforme solicitado)
- `appName`: Nome da aplicação
- `appVersion`: Versão da aplicação
- `appAuthor`: Autor da aplicação
- `appDescription`: Descrição da aplicação

### Configurações de Comportamento
- `debugMode`: Modo de debug ativo/inativo
- `autoSave`: Salvamento automático de configurações
- `logLevel`: Nível de logging (INFO, DEBUG, ERROR, etc.)

### Configurações de Interface
- `windowWidth`: Largura da janela principal
- `windowHeight`: Altura da janela principal  
- `windowCentered`: Se a janela deve ser centralizada

## Exemplo de Uso no Código

O código foi atualizado para usar o sistema Config em vez de valores hardcoded:

**Antes:**
```cpp
struct ApplicationData {
    std::string appName = "Main Application";
    bool isDebugMode = false;
    // ...
};
```

**Depois:**
```cpp
struct ApplicationData {
    std::string GetAppName() const { return Config::GetString("appName", "Main Application"); }
    bool GetDebugMode() const { return Config::GetDebugMode(); }
    void SetDebugMode(bool enabled) { Config::SetDebugMode(enabled); }
    // ...
};
```

Este sistema atende completamente ao requisito da issue de centralizar todas as configurações, incluindo o clientId solicitado, e permite que outras partes do código utilizem e alterem as configurações conforme necessário.