# Feature: Sistema de Informações do Sistema no Ping

## Descrição
Esta feature implementa o envio automático de informações do sistema (memória, CPU e disco) quando o cliente responde a um ping do servidor.

## Funcionalidades Implementadas

### 1. Coleta de Informações de Memória
- **Memória Total**: Total de RAM física disponível no sistema
- **Memória Usada**: Quantidade de RAM atualmente em uso
- **Memória Disponível**: Quantidade de RAM livre
- **Percentual de Uso**: Percentual de uso da memória física
- **Memória Virtual**: Informações sobre memória virtual total e disponível

### 2. Coleta de Informações de CPU
- **Número de Cores**: Quantidade de processadores/cores disponíveis
- **Arquitetura**: Arquitetura do processador (x64/x86)
- **Percentual de Uso**: Percentual de uso atual da CPU

### 3. Coleta de Informações de Disco
- **Drive**: Drive principal do sistema (C:)
- **Espaço Total**: Espaço total do disco em GB
- **Espaço Usado**: Espaço usado no disco em GB
- **Espaço Livre**: Espaço livre disponível em GB
- **Percentual de Uso**: Percentual de uso do disco

## Formato da Resposta JSON

Quando o servidor envia um ping (`{"type":"server_ping"}`), o cliente agora responde com:

```json
{
  "type": "pong",
  "timestamp": "1691847600000",
  "system_info": {
    "timestamp": "12/08/2025 15:30:45",
    "memory": {
      "total_mb": 8192,
      "used_mb": 4096,
      "available_mb": 4096,
      "usage_percent": 50,
      "virtual_total_mb": 16384,
      "virtual_available_mb": 12288
    },
    "cpu": {
      "cores": 8,
      "architecture": "x64",
      "usage_percent": 25.75
    },
    "disk": {
      "drive": "C:",
      "total_gb": 500.00,
      "used_gb": 250.00,
      "free_gb": 250.00,
      "usage_percent": 50.00
    }
  }
}
```

## Arquivos Modificados

### 1. `include/utils/AppUtils.h`
- Adicionadas declarações das funções de coleta de informações do sistema:
  - `GetMemoryInfo()`
  - `GetCpuInfo()`
  - `GetDiskInfo()`
  - `GetSystemInfo()`

### 2. `src/utils/AppUtils.cpp`
- Implementadas as funções de coleta de informações do sistema
- Utiliza APIs nativas do Windows para obter informações precisas:
  - `GlobalMemoryStatusEx()` para memória
  - `GetSystemInfo()` e `GetSystemTimes()` para CPU
  - `GetDiskFreeSpaceEx()` para disco

### 3. `src/network/SocketManager.cpp`
- Modificada a função `ProcessReceivedMessage()` para incluir informações do sistema na resposta de pong
- Melhoria nos logs para indicar que informações do sistema estão sendo enviadas

## APIs do Windows Utilizadas

### Memória
- `GlobalMemoryStatusEx()`: Obtém informações detalhadas sobre o uso de memória

### CPU
- `GetSystemInfo()`: Obtém informações básicas do sistema/processador
- `GetSystemTimes()`: Obtém tempos de sistema para calcular uso de CPU

### Disco
- `GetDiskFreeSpaceEx()`: Obtém informações de espaço em disco

## Vantagens da Implementação

1. **Performance**: Uso de APIs nativas do Windows para máxima eficiência
2. **Precisão**: Informações precisas e atualizadas em tempo real
3. **Compatibilidade**: Funciona em todas as versões do Windows
4. **Formato JSON**: Fácil parsing no servidor
5. **Logging**: Registra todas as operações para debug
6. **Tratamento de Erros**: Retorna mensagens de erro em caso de falha

## Como Testar

1. **Compile o projeto**: Execute `build_mvc.bat`
2. **Execute o cliente**: `bin/main.exe`
3. **Configure o servidor**: Para enviar pings periódicos
4. **Verifique os logs**: Arquivo `bin/application.log` mostrará as operações
5. **Analise as respostas**: O servidor receberá as informações do sistema

## Exemplo de Log

```
[12/08/2025 15:30:45] [INFO] 📨 Mensagem recebida: {"type":"server_ping"}
[12/08/2025 15:30:45] [INFO] 📤 Respondendo ao ping do servidor com informações do sistema
[12/08/2025 15:30:45] [INFO] ✅ Resposta de pong enviada com informações do sistema
```

## Considerações de Segurança

- As informações são coletadas apenas quando solicitado pelo servidor
- Não há acesso a informações sensíveis ou privadas
- Apenas estatísticas básicas de hardware são compartilhadas
- Todas as operações são registradas nos logs

## Expansões Futuras

Esta implementação pode ser facilmente expandida para incluir:
- Informações de rede
- Lista de processos ativos
- Temperatura do hardware (se disponível)
- Informações de usuário logado
- Status de antivírus
