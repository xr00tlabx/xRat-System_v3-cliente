# Changelog

Todas as mudanças notáveis neste projeto serão documentadas neste arquivo.

O formato é baseado em [Keep a Changelog](https://keepachangelog.com/pt-BR/1.0.0/),
e este projeto adere ao [Versionamento Semântico](https://semver.org/lang/pt-BR/).

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
