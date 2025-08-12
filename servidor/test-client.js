// =============================================================================
// Cliente de Teste para xRat WebSocket Server
// =============================================================================
// Descrição: Script para testar o servidor WebSocket
// Uso: node test-client.js
// =============================================================================

const WebSocket = require('ws');

// Configuração do cliente de teste
const SERVER_URL = 'ws://192.168.10.101:8080/cli/ws';
const CLIENT_NAME = 'TestClient_JS';

console.log('=============================================================================');
console.log('  xRat WebSocket - Cliente de Teste');
console.log('=============================================================================');
console.log(`[INFO] Conectando em: ${SERVER_URL}`);
console.log(`[INFO] Cliente: ${CLIENT_NAME}`);
console.log('=============================================================================');

// Criar conexão WebSocket
const ws = new WebSocket(SERVER_URL);

let messageCount = 0;
let connected = false;

// Event listener para abertura da conexão
ws.on('open', () => {
    console.log('[SUCESSO] Conectado ao servidor!');
    connected = true;

    // Enviar mensagem de identificação
    const identMessage = {
        type: 'identification',
        clientName: CLIENT_NAME,
        clientType: 'test_client',
        timestamp: new Date().toISOString()
    };

    ws.send(JSON.stringify(identMessage));
    messageCount++;

    console.log('[ENVIO] Mensagem de identificação enviada');

    // Simular envio de informações de janela
    setInterval(() => {
        if (connected) {
            const windowInfo = {
                type: 'window_info',
                windowTitle: 'Notepad - Untitled',
                processName: 'notepad.exe',
                windowHandle: '0x12345678',
                timestamp: new Date().toISOString()
            };

            ws.send(JSON.stringify(windowInfo));
            messageCount++;
            console.log(`[ENVIO] Informação de janela enviada (${messageCount})`);
        }
    }, 5000); // A cada 5 segundos

    // Simular ping
    setInterval(() => {
        if (connected) {
            const pingMessage = {
                type: 'ping',
                timestamp: new Date().toISOString()
            };

            ws.send(JSON.stringify(pingMessage));
            messageCount++;
            console.log(`[PING] Ping enviado (${messageCount})`);
        }
    }, 15000); // A cada 15 segundos
});

// Event listener para mensagens recebidas
ws.on('message', (data) => {
    try {
        const message = JSON.parse(data.toString());
        
        switch (message.type) {
            case 'welcome':
                console.log(`[RECEBIDO] Boas-vindas: ${message.message}`);
                console.log(`[INFO] ID do cliente: ${message.clientId}`);
                break;

            case 'pong':
                console.log(`[PONG] Recebido pong do servidor`);
                break;

            case 'ping':
                console.log(`[PING] Ping recebido do servidor`);
                // Responder com pong
                ws.send(JSON.stringify({
                    type: 'pong',
                    timestamp: new Date().toISOString(),
                    originalTimestamp: message.timestamp
                }));
                messageCount++;
                break;

            case 'broadcast':
                console.log(`[BROADCAST] ${message.message}`);
                break;

            case 'echo':
                console.log(`[ECHO] Servidor ecoou: ${JSON.stringify(message.originalMessage)}`);
                break;

            case 'error':
                console.error(`[ERRO] Servidor reportou erro: ${message.message}`);
                break;

            default:
                console.log(`[RECEBIDO] Mensagem: ${JSON.stringify(message)}`);
        }

    } catch (error) {
        console.error(`[ERRO] Erro ao processar mensagem: ${error.message}`);
        console.log(`[RAW] Dados brutos: ${data.toString()}`);
    }
});

// Event listener para erro
ws.on('error', (error) => {
    console.error(`[ERRO] Erro na conexão: ${error.message}`);
});

// Event listener para fechamento da conexão
ws.on('close', (code, reason) => {
    connected = false;
    console.log(`[DESCONEXÃO] Conexão fechada. Código: ${code}, Motivo: ${reason}`);
    console.log(`[INFO] Total de mensagens enviadas: ${messageCount}`);
});

// Tratamento de interrupção
process.on('SIGINT', () => {
    console.log('\n[INFO] Interrompendo cliente...');
    connected = false;
    ws.close(1000, 'Cliente sendo fechado');
    setTimeout(() => {
        process.exit(0);
    }, 1000);
});

console.log('[INFO] Cliente iniciado. Pressione Ctrl+C para sair.');
