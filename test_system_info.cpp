// =============================================================================
// Teste de Sistema de Informações - Exemplo de Uso
// =============================================================================
// Projeto: xRat System v3 - Cliente
// Arquivo: test_system_info.cpp
// Descrição: Exemplo de como usar as funções de informações do sistema
// Autor: Desenvolvedor C++
// Data: 12/08/2025
// =============================================================================

#include <iostream>
#include <string>
#include "include/utils/AppUtils.h"

int main()
{
    std::cout << "=== TESTE DE INFORMAÇÕES DO SISTEMA ===" << std::endl;
    std::cout << std::endl;

    // Teste individual das funções
    std::cout << "1. Informações de Memória:" << std::endl;
    std::string memInfo = AppUtils::GetMemoryInfo();
    std::cout << memInfo << std::endl;
    std::cout << std::endl;

    std::cout << "2. Informações de CPU:" << std::endl;
    std::string cpuInfo = AppUtils::GetCpuInfo();
    std::cout << cpuInfo << std::endl;
    std::cout << std::endl;

    std::cout << "3. Informações de Disco:" << std::endl;
    std::string diskInfo = AppUtils::GetDiskInfo();
    std::cout << diskInfo << std::endl;
    std::cout << std::endl;

    std::cout << "4. Informações Completas do Sistema:" << std::endl;
    std::string systemInfo = AppUtils::GetSystemInfo();
    std::cout << systemInfo << std::endl;
    std::cout << std::endl;

    std::cout << "=== SIMULAÇÃO DE RESPOSTA PING/PONG ===" << std::endl;
    
    // Simular resposta de ping como seria enviada pelo SocketManager
    std::string timestamp = std::to_string(GetTickCount());
    std::string pongResponse = R"({"type":"pong","timestamp":")" + timestamp + R"(","system_info":)" + systemInfo + R"(})";
    
    std::cout << "Resposta de Pong que seria enviada ao servidor:" << std::endl;
    std::cout << pongResponse << std::endl;
    std::cout << std::endl;

    std::cout << "=== TESTE CONCLUÍDO ===" << std::endl;
    
    return 0;
}

/*
Para compilar este teste:

1. MinGW/GCC:
g++ -std=c++11 -I. test_system_info.cpp src/utils/AppUtils.cpp -o test_system_info.exe -lws2_32

2. MSVC:
cl /std:c++11 /I. test_system_info.cpp src/utils/AppUtils.cpp /link ws2_32.lib

Exemplo de saída esperada:

=== TESTE DE INFORMAÇÕES DO SISTEMA ===

1. Informações de Memória:
{"total_mb":8192,"used_mb":4096,"available_mb":4096,"usage_percent":50,"virtual_total_mb":16384,"virtual_available_mb":12288}

2. Informações de CPU:
{"cores":8,"architecture":"x64","usage_percent":25.75}

3. Informações de Disco:
{"drive":"C:","total_gb":500.00,"used_gb":250.00,"free_gb":250.00,"usage_percent":50.00}

4. Informações Completas do Sistema:
{"timestamp":"12/08/2025 15:30:45","memory":{"total_mb":8192,"used_mb":4096,"available_mb":4096,"usage_percent":50,"virtual_total_mb":16384,"virtual_available_mb":12288},"cpu":{"cores":8,"architecture":"x64","usage_percent":25.75},"disk":{"drive":"C:","total_gb":500.00,"used_gb":250.00,"free_gb":250.00,"usage_percent":50.00}}

=== SIMULAÇÃO DE RESPOSTA PING/PONG ===
Resposta de Pong que seria enviada ao servidor:
{"type":"pong","timestamp":"1691847600000","system_info":{"timestamp":"12/08/2025 15:30:45","memory":{"total_mb":8192,"used_mb":4096,"available_mb":4096,"usage_percent":50,"virtual_total_mb":16384,"virtual_available_mb":12288},"cpu":{"cores":8,"architecture":"x64","usage_percent":25.75},"disk":{"drive":"C:","total_gb":500.00,"used_gb":250.00,"free_gb":250.00,"usage_percent":50.00}}}

=== TESTE CONCLUÍDO ===
*/
