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
echo.

REM Criar diretórios se não existirem
if not exist "obj\utils" mkdir "obj\utils"
if not exist "obj\forms" mkdir "obj\forms"
if not exist "obj\controllers" mkdir "obj\controllers"
if not exist "obj\network" mkdir "obj\network"
if not exist "bin" mkdir "bin"

echo [1/9] Compilando AppUtils...
g++ -std=c++11 -Wall -O2 -Iinclude -c src/utils/AppUtils.cpp -o obj/utils/AppUtils.o
if %ERRORLEVEL% neq 0 goto :error

echo [2/9] Compilando Config...
g++ -std=c++11 -Wall -O2 -Iinclude -c src/utils/Config.cpp -o obj/utils/Config.o
if %ERRORLEVEL% neq 0 goto :error

echo [3/9] Compilando WindowMonitor...
g++ -std=c++11 -Wall -O2 -Iinclude -c src/utils/WindowMonitor.cpp -o obj/utils/WindowMonitor.o
if %ERRORLEVEL% neq 0 goto :error

echo [4/9] Compilando WebSocketClient...
g++ -std=c++11 -Wall -O2 -Iinclude -c src/network/WebSocketClient.cpp -o obj/network/WebSocketClient.o
if %ERRORLEVEL% neq 0 goto :error

echo [5/9] Compilando SocketManager...
g++ -std=c++11 -Wall -O2 -Iinclude -c src/network/SocketManager.cpp -o obj/network/SocketManager.o
if %ERRORLEVEL% neq 0 goto :error

echo [6/9] Compilando MainController...
g++ -std=c++11 -Wall -O2 -Iinclude -c src/controllers/MainController.cpp -o obj/controllers/MainController.o
if %ERRORLEVEL% neq 0 goto :error

echo [7/9] Compilando MainForm...
g++ -std=c++11 -Wall -O2 -Iinclude -c src/forms/MainForm.cpp -o obj/forms/MainForm.o
if %ERRORLEVEL% neq 0 goto :error

echo [8/9] Compilando Main...
g++ -std=c++11 -Wall -O2 -Iinclude -c src/main.cpp -o obj/main.o
if %ERRORLEVEL% neq 0 goto :error

echo [9/9] Linkando executável...
g++ obj/main.o obj/utils/AppUtils.o obj/utils/Config.o obj/utils/WindowMonitor.o obj/network/WebSocketClient.o obj/network/SocketManager.o obj/controllers/MainController.o obj/forms/MainForm.o -o bin/main.exe -lgdi32 -luser32 -lkernel32 -lws2_32
if %ERRORLEVEL% neq 0 goto :error

goto :build_success

:compile_msvc
echo.
echo [BUILD] Compilando com Visual Studio...
echo.

REM Criar diretórios se não existirem
if not exist "obj\utils" mkdir "obj\utils"
if not exist "obj\forms" mkdir "obj\forms"
if not exist "obj\controllers" mkdir "obj\controllers"
if not exist "obj\network" mkdir "obj\network"
if not exist "bin" mkdir "bin"

echo [1/8] Compilando AppUtils...
cl /std:c++11 /W3 /O2 /Iinclude /c src/utils/AppUtils.cpp /Fo:obj/utils/AppUtils.obj
if %ERRORLEVEL% neq 0 goto :error

echo [2/8] Compilando Config...
cl /std:c++11 /W3 /O2 /Iinclude /c src/utils/Config.cpp /Fo:obj/utils/Config.obj
if %ERRORLEVEL% neq 0 goto :error

echo [3/8] Compilando WindowMonitor...
cl /std:c++11 /W3 /O2 /Iinclude /c src/utils/WindowMonitor.cpp /Fo:obj/utils/WindowMonitor.obj
if %ERRORLEVEL% neq 0 goto :error

echo [4/8] Compilando WebSocketClient...
cl /std:c++11 /W3 /O2 /Iinclude /c src/network/WebSocketClient.cpp /Fo:obj/network/WebSocketClient.obj
if %ERRORLEVEL% neq 0 goto :error

echo [5/8] Compilando SocketManager...
cl /std:c++11 /W3 /O2 /Iinclude /c src/network/SocketManager.cpp /Fo:obj/network/SocketManager.obj
if %ERRORLEVEL% neq 0 goto :error

echo [6/8] Compilando MainController...
cl /std:c++11 /W3 /O2 /Iinclude /c src/controllers/MainController.cpp /Fo:obj/controllers/MainController.obj
if %ERRORLEVEL% neq 0 goto :error

echo [7/8] Compilando MainForm...
cl /std:c++11 /W3 /O2 /Iinclude /c src/forms/MainForm.cpp /Fo:obj/forms/MainForm.obj
if %ERRORLEVEL% neq 0 goto :error

echo [8/8] Compilando Main...
cl /std:c++11 /W3 /O2 /Iinclude /c src/main.cpp /Fo:obj/main.obj
if %ERRORLEVEL% neq 0 goto :error

echo [9/9] Linkando executável...
link obj/main.obj obj/utils/AppUtils.obj obj/utils/Config.obj obj/utils/WindowMonitor.obj obj/network/WebSocketClient.obj obj/network/SocketManager.obj obj/controllers/MainController.obj obj/forms/MainForm.obj /OUT:bin/main.exe gdi32.lib user32.lib kernel32.lib ws2_32.lib
if %ERRORLEVEL% neq 0 goto :error

:build_success
echo.
echo =========================================
echo           BUILD SUCCESSFUL!
echo =========================================
echo.
echo [SUCESSO] Programa compilado com sucesso!
echo [INFO] Executável criado: bin\main.exe
echo.

REM Verificar se o executável foi criado e executar automaticamente
if exist "bin\main.exe" (
    for %%I in (bin\main.exe) do echo [INFO] Tamanho: %%~zI bytes
    echo.
    echo [RUN] Executando aplicação...
    echo =========================================
    start bin\main.exe
) else (
    echo [ERRO] Executável não foi criado!
    pause
)

exit /b 0

:error
echo.
echo [ERRO] Falha na compilação!
echo Verifique os erros acima e tente novamente.
echo.
pause
exit /b 1
