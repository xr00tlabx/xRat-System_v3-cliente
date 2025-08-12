// =============================================================================
// AppUtils Implementation - Implementação dos Utilitários
// =============================================================================
// Projeto: Main C++ Application
// Arquivo: utils/AppUtils.cpp
// Descrição: Implementação das funções utilitárias
// Autor: Desenvolvedor
// Data: 11/08/2025
// =============================================================================

#include "../../include/utils/AppUtils.h"
#include <ctime>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iomanip>

namespace AppUtils
{
    // =============================================================================
    // IMPLEMENTAÇÃO DE FUNÇÕES DE JANELA
    // =============================================================================

    void CenterWindow(HWND hwnd, int width, int height)
    {
        int screenWidth, screenHeight;
        GetScreenDimensions(screenWidth, screenHeight);

        int x = (screenWidth - width) / 2;
        int y = (screenHeight - height) / 2;

        // Garantir que a janela não fique fora da tela
        if (x < 0)
            x = 0;
        if (y < 0)
            y = 0;

        SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }

    void GetScreenDimensions(int &screenWidth, int &screenHeight)
    {
        screenWidth = GetSystemMetrics(SM_CXSCREEN);
        screenHeight = GetSystemMetrics(SM_CYSCREEN);
    }

    // =============================================================================
    // IMPLEMENTAÇÃO DE FUNÇÕES DE DATA/HORA
    // =============================================================================

    std::string GetCurrentDateTime()
    {
        time_t now = time(0);
        char *dt = ctime(&now);
        std::string result(dt);

        // Remove o \n do final
        if (!result.empty() && result.back() == '\n')
        {
            result.pop_back();
        }

        return result;
    }

    std::string GetCurrentDate()
    {
        time_t now = time(0);
        struct tm *timeinfo = localtime(&now);

        char buffer[80];
        strftime(buffer, sizeof(buffer), "%d/%m/%Y", timeinfo);

        return std::string(buffer);
    }

    std::string GetCurrentTime()
    {
        time_t now = time(0);
        struct tm *timeinfo = localtime(&now);

        char buffer[80];
        strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);

        return std::string(buffer);
    }

    // =============================================================================
    // IMPLEMENTAÇÃO DE FUNÇÕES DE MENSAGEM
    // =============================================================================

    void ShowErrorMessage(const char *message, const char *title)
    {
        // Converter para Unicode
        std::wstring wmessage = StringToWString(std::string(message));
        std::wstring wtitle = title ? StringToWString(std::string(title)) : L"Erro";
        MessageBoxW(NULL, wmessage.c_str(), wtitle.c_str(), MB_OK | MB_ICONERROR);
    }

    void ShowInfoMessage(const char *message, const char *title)
    {
        // Converter para Unicode
        std::wstring wmessage = StringToWString(std::string(message));
        std::wstring wtitle = title ? StringToWString(std::string(title)) : L"Informação";
        MessageBoxW(NULL, wmessage.c_str(), wtitle.c_str(), MB_OK | MB_ICONINFORMATION);
    }

    void ShowWarningMessage(const char *message, const char *title)
    {
        // Converter para Unicode
        std::wstring wmessage = StringToWString(std::string(message));
        std::wstring wtitle = title ? StringToWString(std::string(title)) : L"Aviso";
        MessageBoxW(NULL, wmessage.c_str(), wtitle.c_str(), MB_OK | MB_ICONWARNING);
    }

    bool ConfirmAction(const char *message, const char *title)
    {
        // Converter para Unicode
        std::wstring wmessage = StringToWString(std::string(message));
        std::wstring wtitle = title ? StringToWString(std::string(title)) : L"Confirmação";
        int result = MessageBoxW(NULL, wmessage.c_str(), wtitle.c_str(), MB_YESNO | MB_ICONQUESTION);
        return (result == IDYES);
    }

    // =============================================================================
    // IMPLEMENTAÇÃO DE FUNÇÕES DE STRING
    // =============================================================================

    std::wstring StringToWString(const std::string &str)
    {
        if (str.empty())
            return std::wstring();

        int size = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        std::wstring result(size, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &result[0], size);

        return result;
    }

    std::string WStringToString(const std::wstring &wstr)
    {
        if (wstr.empty())
            return std::string();

        int size = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string result(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &result[0], size, NULL, NULL);

        return result;
    }

    std::string Trim(const std::string &str)
    {
        std::string result = str;

        // Remove espaços do início
        result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch)
                                                  { return !std::isspace(ch); }));

        // Remove espaços do fim
        result.erase(std::find_if(result.rbegin(), result.rend(), [](unsigned char ch)
                                  { return !std::isspace(ch); })
                         .base(),
                     result.end());

        return result;
    }

    // =============================================================================
    // IMPLEMENTAÇÃO DE FUNÇÕES DE ARQUIVO
    // =============================================================================

    bool FileExists(const std::string &filename)
    {
        DWORD attrib = GetFileAttributesA(filename.c_str());
        return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
    }

    std::string GetAppDirectory()
    {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);

        std::string path(buffer);
        size_t pos = path.find_last_of("\\/");

        if (pos != std::string::npos)
        {
            return path.substr(0, pos + 1);
        }

        return ".\\";
    }

    std::string CombinePath(const std::string &path1, const std::string &path2)
    {
        std::string result = path1;

        // Garantir que path1 termine com separador
        if (!result.empty() && result.back() != '\\' && result.back() != '/')
        {
            result += "\\";
        }

        // Remover separador do início de path2 se existir
        std::string path2_clean = path2;
        if (!path2_clean.empty() && (path2_clean.front() == '\\' || path2_clean.front() == '/'))
        {
            path2_clean = path2_clean.substr(1);
        }

        return result + path2_clean;
    }

    // =============================================================================
    // IMPLEMENTAÇÃO DE FUNÇÕES DE DEBUG
    // =============================================================================

    void DebugPrint(const char *message)
    {
#ifdef _DEBUG
        OutputDebugStringA(message);
#endif
        // Em release, não fazer nada para otimização
        (void)message;
    }

    void WriteLog(const std::string &message, const std::string &logType)
    {
        try
        {
            std::string logFileName = CombinePath(GetAppDirectory(), "application.log");
            std::ofstream logFile(logFileName, std::ios::app);

            if (logFile.is_open())
            {
                logFile << "[" << GetCurrentDateTime() << "] "
                        << "[" << logType << "] "
                        << message << std::endl;
                logFile.close();
            }
        }
        catch (...)
        {
            // Se falhar ao escrever no log, não fazer nada para evitar loops
        }
    }

    // =============================================================================
    // IMPLEMENTAÇÃO DE FUNÇÕES DE INFORMAÇÕES DO SISTEMA
    // =============================================================================

    std::string GetMemoryInfo()
    {
        try
        {
            MEMORYSTATUSEX memInfo;
            memInfo.dwLength = sizeof(MEMORYSTATUSEX);
            
            if (GlobalMemoryStatusEx(&memInfo))
            {
                // Converter bytes para MB
                DWORDLONG totalPhysMB = memInfo.ullTotalPhys / (1024 * 1024);
                DWORDLONG availPhysMB = memInfo.ullAvailPhys / (1024 * 1024);
                DWORDLONG usedPhysMB = totalPhysMB - availPhysMB;
                DWORDLONG totalVirtualMB = memInfo.ullTotalVirtual / (1024 * 1024);
                DWORDLONG availVirtualMB = memInfo.ullAvailVirtual / (1024 * 1024);
                
                std::ostringstream oss;
                oss << "{"
                    << "\"total_mb\":" << totalPhysMB << ","
                    << "\"used_mb\":" << usedPhysMB << ","
                    << "\"available_mb\":" << availPhysMB << ","
                    << "\"usage_percent\":" << memInfo.dwMemoryLoad << ","
                    << "\"virtual_total_mb\":" << totalVirtualMB << ","
                    << "\"virtual_available_mb\":" << availVirtualMB
                    << "}";
                
                return oss.str();
            }
        }
        catch (...)
        {
            WriteLog("Erro ao obter informações de memória", "ERROR");
        }
        
        return "{\"error\":\"Falha ao obter informações de memória\"}";
    }

    std::string GetCpuInfo()
    {
        try
        {
            // Obter informações básicas do processador
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            
            // Obter informações de performance (uso de CPU)
            static FILETIME prevSysIdle, prevSysKernel, prevSysUser;
            FILETIME sysIdle, sysKernel, sysUser;
            
            if (GetSystemTimes(&sysIdle, &sysKernel, &sysUser))
            {
                // Calcular diferenças para obter percentual de uso
                ULARGE_INTEGER idle, kernel, user;
                ULARGE_INTEGER prevIdle, prevKernel, prevUser;
                
                idle.LowPart = sysIdle.dwLowDateTime;
                idle.HighPart = sysIdle.dwHighDateTime;
                kernel.LowPart = sysKernel.dwLowDateTime;
                kernel.HighPart = sysKernel.dwHighDateTime;
                user.LowPart = sysUser.dwLowDateTime;
                user.HighPart = sysUser.dwHighDateTime;
                
                prevIdle.LowPart = prevSysIdle.dwLowDateTime;
                prevIdle.HighPart = prevSysIdle.dwHighDateTime;
                prevKernel.LowPart = prevSysKernel.dwLowDateTime;
                prevKernel.HighPart = prevSysKernel.dwHighDateTime;
                prevUser.LowPart = prevSysUser.dwLowDateTime;
                prevUser.HighPart = prevSysUser.dwHighDateTime;
                
                // Salvar valores atuais para próxima medição
                prevSysIdle = sysIdle;
                prevSysKernel = sysKernel;
                prevSysUser = sysUser;
                
                // Calcular diferenças
                ULONGLONG idleDiff = idle.QuadPart - prevIdle.QuadPart;
                ULONGLONG kernelDiff = kernel.QuadPart - prevKernel.QuadPart;
                ULONGLONG userDiff = user.QuadPart - prevUser.QuadPart;
                ULONGLONG totalDiff = kernelDiff + userDiff;
                
                double cpuUsage = 0.0;
                if (totalDiff > 0)
                {
                    cpuUsage = ((double)(totalDiff - idleDiff) / totalDiff) * 100.0;
                }
                
                std::ostringstream oss;
                oss << "{"
                    << "\"cores\":" << sysInfo.dwNumberOfProcessors << ","
                    << "\"architecture\":\"" << (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? "x64" : "x86") << "\","
                    << "\"usage_percent\":" << std::fixed << std::setprecision(2) << cpuUsage
                    << "}";
                
                return oss.str();
            }
        }
        catch (...)
        {
            WriteLog("Erro ao obter informações de CPU", "ERROR");
        }
        
        return "{\"error\":\"Falha ao obter informações de CPU\"}";
    }

    std::string GetDiskInfo()
    {
        try
        {
            // Obter informações do disco C: (sistema)
            ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
            
            if (GetDiskFreeSpaceEx("C:\\", &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes))
            {
                // Converter bytes para GB
                double totalGB = (double)totalNumberOfBytes.QuadPart / (1024.0 * 1024.0 * 1024.0);
                double freeGB = (double)totalNumberOfFreeBytes.QuadPart / (1024.0 * 1024.0 * 1024.0);
                double usedGB = totalGB - freeGB;
                double usagePercent = (usedGB / totalGB) * 100.0;
                
                std::ostringstream oss;
                oss << "{"
                    << "\"drive\":\"C:\","
                    << "\"total_gb\":" << std::fixed << std::setprecision(2) << totalGB << ","
                    << "\"used_gb\":" << std::fixed << std::setprecision(2) << usedGB << ","
                    << "\"free_gb\":" << std::fixed << std::setprecision(2) << freeGB << ","
                    << "\"usage_percent\":" << std::fixed << std::setprecision(2) << usagePercent
                    << "}";
                
                return oss.str();
            }
        }
        catch (...)
        {
            WriteLog("Erro ao obter informações de disco", "ERROR");
        }
        
        return "{\"error\":\"Falha ao obter informações de disco\"}";
    }

    std::string GetSystemInfo()
    {
        try
        {
            std::string memInfo = GetMemoryInfo();
            std::string cpuInfo = GetCpuInfo();
            std::string diskInfo = GetDiskInfo();
            
            std::ostringstream oss;
            oss << "{"
                << "\"timestamp\":\"" << GetCurrentDateTime() << "\","
                << "\"memory\":" << memInfo << ","
                << "\"cpu\":" << cpuInfo << ","
                << "\"disk\":" << diskInfo
                << "}";
            
            return oss.str();
        }
        catch (...)
        {
            WriteLog("Erro ao obter informações do sistema", "ERROR");
        }
        
        return "{\"error\":\"Falha ao obter informações do sistema\"}";
    }
}
