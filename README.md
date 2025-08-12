# xRat System v3 - Cliente WebSocket

## Descrição
Aplicação Windows nativa em C++ com arquitetura MVC para monitoramento e controle de sistema. Implementa cliente WebSocket nativo para comunicação em tempo real com servidor remoto.

## Características Principais
- **Arquitetura**: MVC (Model-View-Controller)
- **Linguagem**: C++11 com Win32 API
- **Comunicação**: Cliente WebSocket nativo (RFC 6455)
- **Interface**: Win32 API pura sem dependências externas
- **Build**: MinGW/GCC ou Visual Studio/MSVC
- **Threading**: Thread dedicada para recebimento de mensagens WebSocket
- **Monitoramento**: Thread em background registra janelas ativas a cada 2 segundos

## 🚀 Funcionalidades WebSocket
- **Protocolo WebSocket RFC 6455** - Implementação completa nativa em C++
- **Handshake automático** - Conexão automática na inicialização
- **Comunicação bidirecional** - Envio e recebimento de mensagens em tempo real
- **Thread de recebimento** - Processamento de mensagens sem bloquear interface
- **Formato JSON** - Troca de mensagens estruturadas
- **Configuração flexível** - URL do servidor configurável via JSON

## Estrutura do Projeto
```
├── src/                    # Código-fonte
│   ├── controllers/        # Lógica de negócio
│   ├── forms/             # Interface gráfica Win32
│   ├── network/           # Cliente WebSocket nativo
│   │   ├── WebSocketClient.cpp  # Implementação protocolo WebSocket
│   │   └── SocketManager.cpp    # Gerenciamento de conexão
│   ├── utils/             # Utilitários, monitoramento e configuração
│   └── main.cpp           # Ponto de entrada
├── include/               # Headers correspondentes
│   └── network/           # Headers do módulo WebSocket
├── bin/                   # Executáveis gerados
├── obj/                   # Arquivos objeto
├── config.json            # Configuração (incluindo URL WebSocket)
├── Makefile              # Build principal
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

## ⚙️ Configuração

### Arquivo config.json
```json
{
    "api_url": "ws://192.168.10.101:8080/cli/ws",
    "client_id": "CLIENT_WIN_001",
    "debug_mode": true,
    "log_level": "INFO"
}
```

### Parâmetros WebSocket
- **api_url**: URL do servidor WebSocket (ws:// ou wss://)
- **client_id**: Identificador único do cliente
- **debug_mode**: Ativa logs detalhados de debug
- **log_level**: Nível de logging (DEBUG, INFO, WARNING, ERROR)

## 🌟 Funcionalidades
- **Cliente WebSocket nativo** - Comunicação em tempo real com servidor
- **Thread de recebimento** - Processamento de mensagens sem bloquear UI
- **Sistema de ping/pong** - Manutenção automática da conexão
- **Formato JSON** - Troca de mensagens estruturadas
- Interface gráfica com botões: Informações, Sobre, Configurações, Sair
- Sistema de logging completo (`bin/application.log`)
- Thread de monitoramento de janelas ativas em background
- Modo debug/release configurável
- Arquitetura modular para fácil manutenção

## 📡 Comunicação WebSocket

### Mensagens Suportadas
- **welcome** - Mensagem de boas-vindas do servidor
- **server_ping** - Teste de conectividade (responde com client_pong)
- **client_pong** - Resposta ao ping do servidor
- **echo** - Eco de mensagens enviadas pelo cliente

### Exemplo de Mensagem
```json
{
    "type": "server_ping",
    "message": "Teste de comunicação do servidor",
    "timestamp": "2025-08-12T04:44:09.539Z",
    "clientId": 1
}
```

## Requisitos
- Windows 7 ou superior
- MinGW/GCC ou Visual Studio/MSVC
- Win32 API e Winsock2 (incluídos no Windows)

## 📊 Logs e Monitoramento
O sistema registra automaticamente:
- **Eventos da aplicação** - Inicialização, conexões, erros
- **Comunicação WebSocket** - Mensagens enviadas e recebidas
- **Títulos e handles de janelas ativas** (intervalo de 2s)
- **Debug info detalhado** em modo de desenvolvimento
- **Threads** - Status das threads de conexão e recebimento

### Exemplo de Log
```
[Tue Aug 12 01:44:09 2025] [INFO] WebSocketClient: Mensagem recebida: {"type":"server_ping"...}
[Tue Aug 12 01:44:09 2025] [INFO] 📤 Respondendo ao ping do servidor
[Tue Aug 12 01:44:09 2025] [INFO] WebSocketClient: Mensagem enviada: {"type":"client_pong"...}
```

Verifique `bin/application.log` para acompanhar toda a atividade em tempo real.

## 🏗️ Arquitetura Técnica

### Módulos Principais
- **WebSocketClient** - Implementação completa do protocolo WebSocket
- **SocketManager** - Gerenciamento de conexão e threads
- **MainController** - Lógica de negócio e coordenação
- **MainForm** - Interface Win32 nativa
- **AppUtils** - Sistema de logging e utilitários
- **Config** - Sistema de configuração JSON
- **WindowMonitor** - Monitoramento de janelas em background

### Threading
- **Thread Principal** - Interface gráfica Win32
- **Thread de Conexão** - Estabelecimento de conexão WebSocket
- **Thread de Recebimento** - Processamento contínuo de mensagens
- **Thread de Monitoramento** - Janelas ativas (2s intervals)

## 🚀 Como Usar

1. **Configure o servidor WebSocket** no arquivo `config.json`
2. **Compile** usando `.\build_mvc.bat` ou `mingw32-make`
3. **Execute** o `bin/main.exe`
4. **Monitore** logs em `bin/application.log`
5. **Interaja** com a interface gráfica

### Testando a Comunicação
O cliente automaticamente:
- Conecta ao servidor WebSocket configurado
- Responde aos pings do servidor
- Registra toda comunicação nos logs
- Mantém a conexão ativa

## 📁 Exemplos de Configuração

### Servidor Local
```json
{
    "api_url": "ws://localhost:8080/cli/ws",
    "client_id": "CLIENT_LOCAL_001"
}
```

### Servidor Remoto
```json
{
    "api_url": "ws://192.168.1.100:8080/cli/ws",
    "client_id": "CLIENT_REMOTE_001"
}
```

## 🤝 Contribuição
Para contribuir com o projeto:
1. Mantenha a arquitetura MVC
2. Use o sistema de logging `AppUtils::WriteLog`
3. Teste com MinGW e MSVC
4. Documente mudanças no código

## 📄 Licença
Este projeto é desenvolvido para fins educacionais e de pesquisa.

---
**xRat System v3** - Cliente WebSocket Nativo C++ | Arquitetura MVC | Win32 API
- **Data**: Agosto 2025
- **Arquitetura**: MVC com separação clara de responsabilidades
- **Padrões**: C++11, logging estruturado, gerenciamento de recursos
