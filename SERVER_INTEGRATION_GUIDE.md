# Instruções para o Servidor - Recebimento de Informações do Sistema

## Formato da Mensagem de Ping
Para solicitar informações do sistema, o servidor deve enviar:
```json
{
  "type": "server_ping",
  "message": "Teste de conectividade",
  "timestamp": "2025-08-12T05:53:52.234Z",
  "clientId": 2
}
```

## Formato da Resposta do Cliente
O cliente responderá com:
```json
{
  "type": "pong",
  "timestamp": "2682875",
  "system_info": {
    "timestamp": "Tue Aug 12 02:53:52 2025",
    "memory": {
      "total_mb": 16000,
      "used_mb": 12527,
      "available_mb": 3473,
      "usage_percent": 78,
      "virtual_total_mb": 134217727,
      "virtual_available_mb": 134213521
    },
    "cpu": {
      "cores": 20,
      "architecture": "x64",
      "usage_percent": 7.75
    },
    "disk": {
      "drive": "C:",
      "total_gb": 475.96,
      "used_gb": 445.33,
      "free_gb": 30.63,
      "usage_percent": 93.57
    }
  }
}
```

## Estrutura de Dados

### Informações de Memória
- `total_mb`: Total de RAM física em MB
- `used_mb`: RAM usada em MB
- `available_mb`: RAM disponível em MB
- `usage_percent`: Percentual de uso da RAM (0-100)
- `virtual_total_mb`: Total de memória virtual em MB
- `virtual_available_mb`: Memória virtual disponível em MB

### Informações de CPU
- `cores`: Número de cores/processadores
- `architecture`: Arquitetura do processador ("x64" ou "x86")
- `usage_percent`: Percentual de uso atual da CPU (0-100)

### Informações de Disco
- `drive`: Letra do drive (geralmente "C:")
- `total_gb`: Espaço total do disco em GB
- `used_gb`: Espaço usado em GB
- `free_gb`: Espaço livre em GB
- `usage_percent`: Percentual de uso do disco (0-100)

## Exemplo de Código JavaScript (Node.js) para o Servidor

```javascript
// Processar resposta de pong com informações do sistema
function handlePongMessage(clientId, message) {
  try {
    const data = JSON.parse(message);
    
    if (data.type === 'pong' && data.system_info) {
      const info = data.system_info;
      
      console.log(`Cliente ${clientId} - Informações do Sistema:`);
      console.log(`Timestamp: ${info.timestamp}`);
      
      // Informações de Memória
      console.log(`Memória: ${info.memory.used_mb}MB / ${info.memory.total_mb}MB (${info.memory.usage_percent}%)`);
      
      // Informações de CPU
      console.log(`CPU: ${info.cpu.cores} cores (${info.cpu.architecture}) - ${info.cpu.usage_percent}% uso`);
      
      // Informações de Disco
      console.log(`Disco ${info.disk.drive} ${info.disk.used_gb}GB / ${info.disk.total_gb}GB (${info.disk.usage_percent}% uso)`);
      
      // Salvar no banco de dados
      saveSystemInfo(clientId, info);
      
      // Verificar alertas (exemplo: alta utilização)
      checkSystemAlerts(clientId, info);
    }
  } catch (error) {
    console.error('Erro ao processar informações do sistema:', error);
  }
}

// Exemplo de verificação de alertas
function checkSystemAlerts(clientId, info) {
  const alerts = [];
  
  // Alerta de memória alta (>90%)
  if (info.memory.usage_percent > 90) {
    alerts.push(`Memória crítica: ${info.memory.usage_percent}%`);
  }
  
  // Alerta de CPU alta (>80%)
  if (info.cpu.usage_percent > 80) {
    alerts.push(`CPU alta: ${info.cpu.usage_percent}%`);
  }
  
  // Alerta de disco quase cheio (>95%)
  if (info.disk.usage_percent > 95) {
    alerts.push(`Disco quase cheio: ${info.disk.usage_percent}%`);
  }
  
  if (alerts.length > 0) {
    console.warn(`⚠️ Alertas do Cliente ${clientId}:`, alerts);
    // Enviar notificação ou executar ação automática
  }
}
```

## Frequência Recomendada
- **Monitoramento normal**: A cada 30-60 segundos
- **Monitoramento intensivo**: A cada 10-15 segundos
- **Modo econômico**: A cada 2-5 minutos

## Considerações de Performance
- As informações são coletadas em tempo real
- O impacto na performance do cliente é mínimo
- Recomenda-se não fazer mais de 1 ping por segundo por cliente
- O tamanho da mensagem é aproximadamente 500-800 bytes

## Casos de Uso
1. **Monitoramento de Infraestrutura**: Acompanhar saúde dos sistemas
2. **Alertas Automáticos**: Detectar problemas antes que afetem usuários
3. **Relatórios de Capacidade**: Planejamento de recursos
4. **Diagnóstico Remoto**: Identificar gargalos de performance
5. **Análise de Tendências**: Acompanhar uso ao longo do tempo
