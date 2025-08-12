# xRat System v3 - Cliente - Workspace Copilot

Este workspace está configurado para o **xRat System v3 - Cliente**, uma aplicação nativa Windows C++ com arquitetura MVC.

## 🚀 Início Rápido para Copilot

### Comandos de Build Disponíveis
- **Ctrl+Shift+P** → `Tasks: Run Task` → `Build Debug (MinGW)`
- **Ctrl+Shift+P** → `Tasks: Run Task` → `Build Release (MinGW)`
- **Ctrl+Shift+P** → `Tasks: Run Task` → `Native Windows Build`

### Estrutura do Projeto
```
src/
├── controllers/    # Lógica de negócio (MVC)
├── forms/         # Interface Win32 GUI (MVC)
├── utils/         # Utilitários compartilhados
└── main.cpp       # Ponto de entrada

include/           # Headers (espelha src/)
bin/              # Executáveis gerados
obj/              # Arquivos objeto compilados
```

### Arquitetura MVC
- **Model**: Estruturas de dados em `ApplicationData` e utilities
- **View**: GUI Win32 em `src/forms/MainForm.cpp`
- **Controller**: Lógica de negócio em `src/controllers/MainController.cpp`

### Padrões de Código
- **Logging**: `AppUtils::WriteLog("mensagem", "LEVEL")`
- **Debug**: `AppUtils::DebugPrint("mensagem")`
- **UI**: Texto em português (PT-BR)
- **Erros**: `AppUtils::ShowErrorMessage()` para falhas críticas

### Funcionalidades Principais
- ✅ Monitoramento de janelas em background (a cada 2s)
- ✅ Interface GUI com botões: Informações, Sobre, Configurações, Sair
- ✅ Sistema de logs detalhado (`bin/application.log`)
- ✅ Arquitetura MVC completa

### Para Desenvolvedores
1. **Adicionar nova funcionalidade**: Seguir separação MVC
2. **Modificar UI**: Editar `src/forms/MainForm.cpp`
3. **Lógica de negócio**: Editar `src/controllers/MainController.cpp`
4. **Funções utilitárias**: Adicionar em `src/utils/AppUtils.cpp`

### Performance
- **Inicialização**: ~500ms
- **Uso de memória**: 2-4MB
- **Tamanho executável**: ~220KB (Release) / ~284KB (Debug)
