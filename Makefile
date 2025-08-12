# =============================================================================
# Makefile para Main Application C++
# =============================================================================
# Projeto: Main C++ Application
# Descrição: Sistema de build profissional para aplicação Windows
# Autor: Desenvolvedor
# Data: 11/08/2025
# =============================================================================

# =============================================================================
# CONFIGURAÇÕES DO PROJETO
# =============================================================================

PROJECT_NAME = main
VERSION = 1.0.1
TARGET_NAME = $(PROJECT_NAME).exe

# Diretórios
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin
OBJ_DIR = obj
DOCS_DIR = docs

# =============================================================================
# CONFIGURAÇÕES DO COMPILADOR
# =============================================================================

# Compilador (detecta automaticamente)
CXX := $(shell which g++ 2>/dev/null || which cl 2>/dev/null || echo g++)
IS_MSVC := $(findstring cl,$(CXX))

# Flags comuns
CPPSTD = c++11
OPTIMIZATION = -O2
WARNINGS = -Wall -Wextra

# Flags específicas por compilador
ifdef IS_MSVC
    # Visual Studio / MSVC
    CXXFLAGS = /std:$(CPPSTD) /W4 $(OPTIMIZATION) /I$(INCLUDE_DIR)
    LDFLAGS = /Fe:$(BIN_DIR)/$(TARGET_NAME)
    LIBS = gdi32.lib user32.lib kernel32.lib
    OBJ_EXT = obj
    MKDIR_CMD = if not exist "$1" mkdir "$1"
    RM_CMD = del /f /q
    RMDIR_CMD = rmdir /s /q
else
    # GCC / MinGW
    CXXFLAGS = -std=$(CPPSTD) $(WARNINGS) $(OPTIMIZATION) -I$(INCLUDE_DIR)
    LDFLAGS = -o $(BIN_DIR)/$(TARGET_NAME)
    LIBS = -lgdi32 -luser32 -lkernel32
    OBJ_EXT = o
    MKDIR_CMD = mkdir -p $1
    RM_CMD = rm -f
    RMDIR_CMD = rm -rf
endif

# Modo debug/release
ifdef DEBUG
    ifdef IS_MSVC
        CXXFLAGS += /Zi /Od /D_DEBUG
        LDFLAGS += /DEBUG
    else
        CXXFLAGS += -g -O0 -D_DEBUG
    endif
else
    ifdef IS_MSVC
        CXXFLAGS += /DNDEBUG
    else
        CXXFLAGS += -DNDEBUG
    endif
endif

# =============================================================================
# ARQUIVOS DO PROJETO
# =============================================================================

# Encontrar todos os arquivos .cpp no diretório src (recursivo)
SOURCES := $(shell find $(SRC_DIR) -name "*.cpp" 2>/dev/null || where /r $(SRC_DIR) *.cpp 2>nul)
# Normalizar objetos para Windows/Unix
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.$(OBJ_EXT),$(filter %.cpp,$(SOURCES)))
# Incluir cabeçalhos principais e de subpastas
HEADERS := $(wildcard $(INCLUDE_DIR)/*.h) $(wildcard $(INCLUDE_DIR)/*/*.h)

# =============================================================================
# TARGETS PRINCIPAIS
# =============================================================================

.PHONY: all clean run debug release install help info dirs

# Target padrão
all: dirs $(BIN_DIR)/$(TARGET_NAME)

# Criar diretórios necessários
dirs:
	$(call MKDIR_CMD,$(BUILD_DIR))
	$(call MKDIR_CMD,$(BIN_DIR))
	$(call MKDIR_CMD,$(OBJ_DIR))
	$(call MKDIR_CMD,$(OBJ_DIR)/utils)
	$(call MKDIR_CMD,$(OBJ_DIR)/forms)
	$(call MKDIR_CMD,$(OBJ_DIR)/controllers)

# Target principal - construir o executável
$(BIN_DIR)/$(TARGET_NAME): $(OBJECTS)
	@echo [LINK] Linkando $(TARGET_NAME)...
ifdef IS_MSVC
	$(CXX) $(OBJECTS) $(LDFLAGS) $(LIBS)
else
	$(CXX) $(OBJECTS) $(LDFLAGS) $(LIBS)
endif
	@echo [INFO] Build concluído: $(BIN_DIR)/$(TARGET_NAME)

# Compilar arquivos objeto
$(OBJ_DIR)/%.$(OBJ_EXT): $(SRC_DIR)/%.cpp $(HEADERS)
	@echo [CC] Compilando $<...
ifdef IS_MSVC
	$(CXX) $(CXXFLAGS) /c $< /Fo:$@
else
	$(CXX) $(CXXFLAGS) -c $< -o $@
endif

# =============================================================================
# TARGETS DE BUILD
# =============================================================================

# Build otimizado (release)
release:
	@echo [INFO] Compilando versão Release...
	$(MAKE) all

# Build com debug
debug:
	@echo [INFO] Compilando versão Debug...
	$(MAKE) all DEBUG=1

# Build rápido (sem otimizações)
fast: OPTIMIZATION = -O0
fast: all

# =============================================================================
# TARGETS DE EXECUÇÃO E TESTE
# =============================================================================

# Executar o programa
run: $(BIN_DIR)/$(TARGET_NAME)
	@echo [RUN] Executando $(TARGET_NAME)...
	@cd $(BIN_DIR) && ./$(TARGET_NAME)

# Executar em modo debug
run-debug: debug
	@echo [RUN-DEBUG] Executando versão debug...
	@cd $(BIN_DIR) && ./$(TARGET_NAME)

# =============================================================================
# TARGETS DE LIMPEZA
# =============================================================================

# Limpeza completa
clean:
	@echo [CLEAN] Removendo arquivos temporários...
	$(RM_CMD) $(OBJ_DIR)/*.*
	$(RM_CMD) $(BIN_DIR)/$(TARGET_NAME)
	@echo [INFO] Limpeza concluída.

# Limpeza total (incluindo diretórios)
distclean: clean
	@echo [DISTCLEAN] Removendo diretórios de build...
	$(RMDIR_CMD) $(BUILD_DIR)
	$(RMDIR_CMD) $(BIN_DIR)
	$(RMDIR_CMD) $(OBJ_DIR)

# =============================================================================
# TARGETS DE UTILIDADE
# =============================================================================

# Instalar (copiar para system32 - requer admin)
install: $(BIN_DIR)/$(TARGET_NAME)
	@echo [INSTALL] Instalando $(TARGET_NAME)...
	copy $(BIN_DIR)\$(TARGET_NAME) C:\Windows\System32\
	@echo [INFO] Instalação concluída.

# Informações do projeto
info:
	@echo ===============================================
	@echo   Informações do Projeto
	@echo ===============================================
	@echo Projeto:      $(PROJECT_NAME)
	@echo Versão:       $(VERSION)
	@echo Target:       $(TARGET_NAME)
	@echo Compilador:   $(CXX)
	@echo Fontes:       $(SOURCES)
	@echo Objetos:      $(OBJECTS)
	@echo Cabeçalhos:   $(HEADERS)
	@echo Flags:        $(CXXFLAGS)
	@echo Libs:         $(LIBS)
	@echo ===============================================

# Ajuda
help:
	@echo ===============================================
	@echo   Sistema de Build - Main Application
	@echo ===============================================
	@echo Targets disponíveis:
	@echo.
	@echo   all          - Compilar o projeto (padrão)
	@echo   debug        - Compilar versão debug
	@echo   release      - Compilar versão release
	@echo   fast         - Compilar sem otimização
	@echo.
	@echo   run          - Executar o programa
	@echo   run-debug    - Executar versão debug
	@echo.
	@echo   clean        - Limpar arquivos temporários
	@echo   distclean    - Limpeza completa
	@echo.
	@echo   install      - Instalar no sistema
	@echo   info         - Mostrar informações
	@echo   help         - Mostrar esta ajuda
	@echo.
	@echo Exemplos:
	@echo   make                 # Compilar
	@echo   make debug           # Compilar debug
	@echo   make run             # Compilar e executar
	@echo   make clean all       # Limpar e recompilar
	@echo ===============================================

# =============================================================================
# CONFIGURAÇÕES AVANÇADAS
# =============================================================================

# Não deletar arquivos intermediários
.PRECIOUS: $(OBJ_DIR)/%.$(OBJ_EXT)

# Targets que não são arquivos
.PHONY: all clean run debug release install help info dirs fast run-debug distclean
