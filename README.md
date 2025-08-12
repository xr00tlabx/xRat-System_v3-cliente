# xRat System v3 - Cliente CharSP AutoIT

## Descrição
Aplicação Windows nativa em C++ com arquitetura MVC para monitoramento e controle de sistema. Utiliza Win32 API pura sem dependências externas.

## Características
- **Arquitetura**: MVC (Model-View-Controller)
- **Linguagem**: C++11
- **Interface**: Win32 API nativa
- **Build**: MinGW/GCC ou Visual Studio
- **Monitoramento**: Thread em background registra janelas ativas a cada 2 segundos

## Estrutura do Projeto
```
├── src/                    # Código-fonte
│   ├── controllers/        # Lógica de negócio
│   ├── forms/             # Interface gráfica Win32
│   ├── utils/             # Utilitários, monitoramento e configuração
│   └── main.cpp           # Ponto de entrada
├── include/               # Headers correspondentes
├── examples/              # Exemplos de uso do sistema
├── bin/                   # Executáveis gerados
├── obj/                   # Arquivos objeto
├── docs/                  # Documentação
├── config.json            # Arquivo de configuração centralizada
├── Makefile              # Build principal
├── Makefile.mvc          # Build modular MVC
└── build_mvc.bat         # Script de build Windows
```

## Como Compilar

### Opção 1: Script Batch (Recomendado)
```cmd
.\build_mvc.bat
```

### Opção 2: Make
```cmd
# MinGW/MSYS2
mingw32-make

# Com targets específicos
mingw32-make debug
mingw32-make release
mingw32-make run
```

## Funcionalidades
- Interface gráfica com botões: Informações, Sobre, Configurações, Sair
- Sistema de logging completo (`bin/application.log`)
- Thread de monitoramento de janelas ativas em background
- Modo debug/release configurável
- Arquitetura modular para fácil manutenção
- **Sistema de configuração centralizada** com arquivo JSON
- **Client ID configurável** e outras configurações personalizáveis

## Requisitos
- Windows 7 ou superior
- MinGW/GCC ou Visual Studio
- Win32 API (incluída no Windows)

## Logs e Monitoramento
O sistema registra automaticamente:
- Eventos da aplicação
- Títulos e handles de janelas ativas (intervalo de 2s)
- Debug info em modo de desenvolvimento

Verifique `bin/application.log` para acompanhar a atividade.

## Sistema de Configuração

A aplicação agora possui um sistema de configuração centralizada baseado em JSON:

### Arquivo de Configuração (`config.json`)
```json
{
  "clientId": "CLIENT_DEFAULT_001",
  "appName": "Main Application",
  "appVersion": "1.0.0", 
  "debugMode": false,
  "autoSave": true,
  "windowWidth": 600,
  "windowHeight": 400
}
```

### Como Usar no Código
```cpp
// Ler configurações
std::string clientId = Config::GetClientId();
bool debugMode = Config::GetDebugMode();

// Modificar configurações
Config::SetClientId("NEW_CLIENT_123");
Config::SetDebugMode(true);
```

### Configurações Disponíveis
- **clientId**: Identificador único do cliente
- **debugMode**: Modo debug ativo/inativo
- **appName, appVersion, appAuthor**: Metadados da aplicação  
- **windowWidth, windowHeight**: Dimensões da janela
- **autoSave, logLevel**: Configurações de comportamento

Consulte `CONFIG_SYSTEM.md` para documentação completa e `examples/` para exemplos de uso.

## Desenvolvido em
- **Data**: Agosto 2025
- **Arquitetura**: MVC com separação clara de responsabilidades
- **Padrões**: C++11, logging estruturado, gerenciamento de recursos
