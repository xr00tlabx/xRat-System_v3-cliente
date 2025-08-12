# Changelog

Todas as mudanças notáveis neste projeto serão documentadas neste arquivo.

O formato é baseado em [Keep a Changelog](https://keepachangelog.com/pt-BR/1.0.0/),
e este projeto adere ao [Versionamento Semântico](https://semver.org/lang/pt-BR/).

## [1.0.3] - 2025-08-12

### Adicionado
- **FEATURE**: Sistema de informações do sistema no ping/pong
  - Cliente agora envia informações de memória, CPU e disco quando responde ao ping do servidor
  - Funções implementadas: `GetMemoryInfo()`, `GetCpuInfo()`, `GetDiskInfo()`, `GetSystemInfo()`
  - Resposta JSON estruturada com informações detalhadas do sistema
  - Logs melhorados para indicar envio de informações do sistema
  - Documentação completa em `FEATURE_SYSTEM_INFO.md`

### Melhorado
- SocketManager agora inclui informações do sistema na resposta de pong
- AppUtils expandido com funções de coleta de informações de hardware
- Melhor tratamento de erros e logging para operações de sistema

### Técnico
- Uso de APIs nativas do Windows: `GlobalMemoryStatusEx`, `GetSystemInfo`, `GetSystemTimes`, `GetDiskFreeSpaceEx`
- Formato JSON estruturado para fácil parsing no servidor
- Implementação eficiente com tratamento de erros robusto

## [1.0.2] - 2025-08-11

### Corrigido
- **CRÍTICO**: Corrigido erro de linkagem "undefined reference to Config::*" que impedia a compilação
- Adicionado módulo Config.cpp aos scripts de build (build_mvc.bat)
- Atualizados workflows GitHub Actions para incluir compilação do módulo Config
- Corrigida numeração dos passos de build (1/7 a 7/7)
- Incluído obj/utils/Config.o no processo de linkagem para MinGW e MSVC

### Melhorado
- Build agora completa em ~6 segundos sem erros
- Todos os módulos (AppUtils, Config, WindowMonitor, Controllers, Forms, Main) sendo compilados corretamente
- Sistema de configuração totalmente funcional

## [1.0.1] - 2025-08-12

### Corrigido
- Corrigido falha no workflow de build do GitHub Actions
- Melhorada detecção de caminho do MinGW nos workflows para buscar múltiplos caminhos dinamicamente
- Corrigidos problemas de build do workflow: caracteres Unicode, setup do MinGW, warnings do MSVC
- Corrigidos problemas no workflow de build relacionados a configuração do ambiente

### Alterado
- Atualizada configuração do GitHub Actions para maior robustez
- Melhorada estabilidade dos workflows de CI/CD

## [1.0.0] - 2025-08-11

### Adicionado
- Configuração completa de GitHub Actions para CI/CD
- Workflows automáticos para builds MinGW e MSVC
- Workflow de release com binários otimizados
- Testes de qualidade de código
- Templates estruturados para Issues (bug report e feature request)
- Template para Pull Requests com checklist completo
- Configuração de links para documentação e suporte
- Sistema de build profissional com Makefile
- Arquitetura MVC completa para aplicação Windows
- Interface gráfica com Win32 API nativa
- Sistema de monitoramento de janelas em background
- Sistema de logging estruturado
- Documentação expandida com guias de workflow

### Características Iniciais
- Aplicação Windows nativa em C++11
- Interface: Win32 API pura sem dependências externas
- Arquitetura: MVC (Model-View-Controller)
- Build: MinGW/GCC e Visual Studio
- Monitoramento: Thread em background registra janelas ativas a cada 2 segundos
- Logs automáticos em `bin/application.log`
- Funcionalidades: Informações, Sobre, Configurações, Sair
