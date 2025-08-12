@echo off
echo =========================================
echo   Main Application - Build Script
echo   Arquitetura MVC (Model-View-Controller)
echo =========================================
echo.

REM Verificar se estamos no diretório correto
if not exist "src" (
    echo [ERRO] Diretório 'src' não encontrado!
    echo Certifique-se de estar no diretório raiz do projeto.
    pause
    exit /b 1
)

REM Verificar se o g++ está disponível
where g++ >nul 2>&1
if %ERRORLEVEL% == 0 (
    echo [INFO] Compilador GCC/MinGW encontrado.
    goto :compile_gcc
)

REM Verificar se o Visual Studio está disponível
where cl >nul 2>&1
if %ERRORLEVEL% == 0 (
    echo [INFO] Compilador Visual Studio encontrado.
    goto :compile_msvc
)

echo [ERRO] Nenhum compilador C++ encontrado!
echo.
echo Para compilar este projeto, você precisa de:
echo 1. MinGW/GCC - https://www.mingw-w64.org/
echo 2. Visual Studio - https://visualstudio.microsoft.com/
echo.
pause
exit /b 1

:compile_gcc
echo.
echo [BUILD] Compilando com GCC/MinGW...
echo [BUILD] Usando arquitetura MVC modular
echo.

REM Criar diretórios se não existirem
if not exist "obj" mkdir "obj"
if not exist "obj\utils" mkdir "obj\utils"
if not exist "obj\forms" mkdir "obj\forms"
if not exist "obj\controllers" mkdir "obj\controllers"
if not exist "bin" mkdir "bin"

echo [1/7] Compilando módulo Utils...
g++ -std=c++11 -Wall -O2 -Iinclude -c src/utils/AppUtils.cpp -o obj/utils/AppUtils.o
if %ERRORLEVEL% neq 0 (
    echo [ERRO] Falha ao compilar módulo Utils!
    pause
    exit /b 1
)

echo [2/7] Compilando módulo Config...
g++ -std=c++11 -Wall -O2 -Iinclude -c src/utils/Config.cpp -o obj/utils/Config.o
if %ERRORLEVEL% neq 0 (
    echo [ERRO] Falha ao compilar módulo Config!
    pause
    exit /b 1
)

echo [3/7] Compilando módulo WindowMonitor...
g++ -std=c++11 -Wall -O2 -Iinclude -c src/utils/WindowMonitor.cpp -o obj/utils/WindowMonitor.o
if %ERRORLEVEL% neq 0 (
    echo [ERRO] Falha ao compilar módulo WindowMonitor!
    pause
    exit /b 1
)

echo [4/7] Compilando módulo Controllers...
g++ -std=c++11 -Wall -O2 -Iinclude -c src/controllers/MainController.cpp -o obj/controllers/MainController.o
if %ERRORLEVEL% neq 0 (
    echo [ERRO] Falha ao compilar módulo Controllers!
    pause
    exit /b 1
)

echo [5/7] Compilando módulo Forms...
g++ -std=c++11 -Wall -O2 -Iinclude -c src/forms/MainForm.cpp -o obj/forms/MainForm.o
if %ERRORLEVEL% neq 0 (
    echo [ERRO] Falha ao compilar módulo Forms!
    pause
    exit /b 1
)

echo [6/7] Compilando módulo Main...
g++ -std=c++11 -Wall -O2 -Iinclude -c src/main.cpp -o obj/main.o
if %ERRORLEVEL% neq 0 (
    echo [ERRO] Falha ao compilar módulo Main!
    pause
    exit /b 1
)

echo [7/7] Linkando executável final...
g++ obj/main.o obj/utils/AppUtils.o obj/utils/Config.o obj/utils/WindowMonitor.o obj/controllers/MainController.o obj/forms/MainForm.o -o bin/main.exe -lgdi32 -luser32 -lkernel32
if %ERRORLEVEL% neq 0 (
    echo [ERRO] Falha ao linkar o executável!
    pause
    exit /b 1
)

goto :build_success

:compile_msvc
echo.
echo [BUILD] Compilando com Visual Studio...
echo [BUILD] Usando arquitetura MVC modular
echo.

REM Criar diretórios se não existirem
if not exist "obj" mkdir "obj"
if not exist "obj\utils" mkdir "obj\utils"
if not exist "obj\forms" mkdir "obj\forms"
if not exist "obj\controllers" mkdir "obj\controllers"
if not exist "bin" mkdir "bin"

echo [1/6] Compilando módulo Utils...
cl /std:c++11 /W3 /O2 /Iinclude /c src/utils/AppUtils.cpp /Fo:obj/utils/AppUtils.obj
if %ERRORLEVEL% neq 0 (
    echo [ERRO] Falha ao compilar módulo Utils!
    pause
    exit /b 1
)

echo [2/6] Compilando módulo Config...
cl /std:c++11 /W3 /O2 /Iinclude /c src/utils/Config.cpp /Fo:obj/utils/Config.obj
if %ERRORLEVEL% neq 0 (
    echo [ERRO] Falha ao compilar módulo Config!
    pause
    exit /b 1
)

echo [3/6] Compilando módulo WindowMonitor...
cl /std:c++11 /W3 /O2 /Iinclude /c src/utils/WindowMonitor.cpp /Fo:obj/utils/WindowMonitor.obj
if %ERRORLEVEL% neq 0 (
    echo [ERRO] Falha ao compilar módulo WindowMonitor!
    pause
    exit /b 1
)

echo [4/6] Compilando módulo Controllers...
cl /std:c++11 /W3 /O2 /Iinclude /c src/controllers/MainController.cpp /Fo:obj/controllers/MainController.obj
if %ERRORLEVEL% neq 0 (
    echo [ERRO] Falha ao compilar módulo Controllers!
    pause
    exit /b 1
)

echo [5/6] Compilando módulo Forms...
cl /std:c++11 /W3 /O2 /Iinclude /c src/forms/MainForm.cpp /Fo:obj/forms/MainForm.obj
if %ERRORLEVEL% neq 0 (
    echo [ERRO] Falha ao compilar módulo Forms!
    pause
    exit /b 1
)

echo [6/6] Compilando módulo Main...
cl /std:c++11 /W3 /O2 /Iinclude /c src/main.cpp /Fo:obj/main.obj
if %ERRORLEVEL% neq 0 (
    echo [ERRO] Falha ao compilar módulo Main!
    pause
    exit /b 1
)

echo [7/7] Linkando executável final...
link obj/main.obj obj/utils/AppUtils.obj obj/utils/Config.obj obj/utils/WindowMonitor.obj obj/controllers/MainController.obj obj/forms/MainForm.obj /OUT:bin/main.exe gdi32.lib user32.lib kernel32.lib
if %ERRORLEVEL% neq 0 (
    echo [ERRO] Falha ao linkar o executável!
    pause
    exit /b 1
)

:build_success
echo.
echo =========================================
echo           BUILD SUCCESSFUL!
echo =========================================
echo.
echo [SUCESSO] Programa compilado com sucesso!
echo [INFO] Executável criado: bin\main.exe
echo.
echo [INFO] Estrutura modular MVC implementada:
echo   ✓ Utils: Funções utilitárias
echo   ✓ Controllers: Lógica de negócio
echo   ✓ Forms: Interface gráfica
echo   ✓ Main: Ponto de entrada
echo.

REM Verificar se o executável foi criado
if exist "bin\main.exe" (
    echo [INFO] Tamanho do executável:
    for %%I in (bin\main.exe) do echo   %%~zI bytes
    echo.
    
    echo Deseja executar o programa agora? (S/N)
    set /p choice=
    if /i "%choice%"=="S" (
        echo.
        echo [RUN] Executando aplicação...
        echo =========================================
        start bin\main.exe
    )
) else (
    echo [ERRO] Executável não foi criado!
)

echo.
echo Pressione qualquer tecla para continuar...
pause >nul
goto :eof

:error
echo.
echo [ERRO] Falha na compilação!
echo Verifique os erros acima e tente novamente.
echo.
pause
exit /b 1
