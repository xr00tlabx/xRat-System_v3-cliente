# xRat WebSocket Server

Servidor WebSocket simples para testar conexão com o cliente C++ do xRat System v3.

## Configuração

### Dependências
- Node.js >= 14.0.0
- npm ou yarn

### Instalação
```bash
cd servidor
npm install
```

### Execução

#### Servidor
```bash
# Modo normal
npm start

# Modo desenvolvimento (auto-restart)
npm run dev
```

#### Cliente de Teste
```bash
npm test
```

## Endpoints

### HTTP
- `GET /` - Informações do servidor e estatísticas
- `GET /status` - Status simplificado do servidor

### WebSocket
- `ws://localhost:8080/cli/ws` - Endpoint principal para clientes

## Configuração do Cliente C++

No arquivo `config.json` do cliente C++, use:
```json
{
  "apiUrl": "ws://192.168.10.101:8080/cli/ws"
}
```

## Tipos de Mensagem Suportados

### Cliente → Servidor
- `ping` - Teste de conectividade
- `window_info` - Informações de janela ativa
- `system_info` - Informações do sistema
- `broadcast` - Mensagem para retransmitir

### Servidor → Cliente
- `welcome` - Mensagem de boas-vindas
- `pong` - Resposta ao ping
- `ping` - Ping do servidor
- `echo` - Eco de mensagens
- `broadcast` - Mensagem retransmitida
- `error` - Erro no processamento

## Logs

O servidor registra:
- Conexões e desconexões
- Mensagens recebidas e enviadas
- Erros de processamento
- Estatísticas em tempo real

## Desenvolvimento

### Estrutura
```
servidor/
├── package.json      # Configuração do projeto
├── server.js         # Servidor principal
├── test-client.js    # Cliente de teste
└── README.md         # Esta documentação
```

### Porta e Host
- **Porta**: 8080
- **Host**: 0.0.0.0 (aceita de qualquer IP)
- **Endpoint**: /cli/ws

### Modificações
Para alterar porta ou outras configurações, edite as constantes no início do `server.js`:
```javascript
const PORT = 8080;
const HOST = '0.0.0.0';
```
