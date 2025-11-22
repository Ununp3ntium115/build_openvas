import { cryptexConfig, cryptexState } from '@stores/cryptex';
import { scansStore } from '@stores/scans';
import { get } from 'svelte/store';

export interface WebSocketMessage {
  type: 'scan_started' | 'scan_progress' | 'scan_completed' | 'scan_failed' | 'vulnerability_found' | 'ping' | 'pong';
  data?: any;
  timestamp?: string;
}

export class CryptexWebSocket {
  private ws: WebSocket | null = null;
  private reconnectAttempts = 0;
  private maxReconnectAttempts = 5;
  private reconnectDelay = 1000;
  private heartbeatInterval: number | null = null;
  private messageHandlers: Map<string, Array<(data: any) => void>> = new Map();

  constructor() {
    this.connect();
  }

  private connect() {
    const config = get(cryptexConfig);
    const wsUrl = config.wsUrl || config.apiUrl.replace('http', 'ws') + '/ws';

    try {
      this.ws = new WebSocket(wsUrl);

      this.ws.onopen = () => {
        console.log('[CRYPTEX WebSocket] Connected');
        this.reconnectAttempts = 0;
        cryptexState.setConnected(true);
        cryptexState.setSynced();
        this.startHeartbeat();
      };

      this.ws.onmessage = (event) => {
        try {
          const message: WebSocketMessage = JSON.parse(event.data);
          this.handleMessage(message);
        } catch (error) {
          console.error('[CRYPTEX WebSocket] Failed to parse message:', error);
        }
      };

      this.ws.onerror = (error) => {
        console.error('[CRYPTEX WebSocket] Error:', error);
        cryptexState.setError('WebSocket connection error');
      };

      this.ws.onclose = () => {
        console.log('[CRYPTEX WebSocket] Disconnected');
        cryptexState.setConnected(false);
        this.stopHeartbeat();
        this.attemptReconnect();
      };
    } catch (error) {
      console.error('[CRYPTEX WebSocket] Connection failed:', error);
      cryptexState.setError('Failed to connect to WebSocket');
      this.attemptReconnect();
    }
  }

  private handleMessage(message: WebSocketMessage) {
    console.log('[CRYPTEX WebSocket] Received:', message.type, message.data);

    // Update last sync time
    cryptexState.setSynced();

    // Handle specific message types
    switch (message.type) {
      case 'scan_started':
        if (message.data?.scan) {
          scansStore.addScan(message.data.scan);
        }
        break;

      case 'scan_progress':
        if (message.data?.scan_id) {
          scansStore.updateScan(message.data.scan_id, {
            status: 'running',
            ...message.data.updates,
          });
        }
        break;

      case 'scan_completed':
        if (message.data?.scan_id) {
          scansStore.updateScan(message.data.scan_id, {
            status: 'completed',
            ...message.data.updates,
          });
        }
        break;

      case 'scan_failed':
        if (message.data?.scan_id) {
          scansStore.updateScan(message.data.scan_id, {
            status: 'failed',
            ...message.data.updates,
          });
        }
        break;

      case 'vulnerability_found':
        // Could be used to show real-time vulnerability discoveries
        console.log('[CRYPTEX] New vulnerability found:', message.data);
        break;

      case 'pong':
        // Heartbeat response
        break;

      default:
        console.warn('[CRYPTEX WebSocket] Unknown message type:', message.type);
    }

    // Call registered handlers
    const handlers = this.messageHandlers.get(message.type);
    if (handlers) {
      handlers.forEach(handler => handler(message.data));
    }

    // Call wildcard handlers
    const wildcardHandlers = this.messageHandlers.get('*');
    if (wildcardHandlers) {
      wildcardHandlers.forEach(handler => handler(message));
    }
  }

  private attemptReconnect() {
    if (this.reconnectAttempts >= this.maxReconnectAttempts) {
      console.error('[CRYPTEX WebSocket] Max reconnection attempts reached');
      cryptexState.setError('Failed to reconnect to WebSocket');
      return;
    }

    const delay = this.reconnectDelay * Math.pow(2, this.reconnectAttempts);
    console.log(`[CRYPTEX WebSocket] Reconnecting in ${delay}ms...`);

    setTimeout(() => {
      this.reconnectAttempts++;
      this.connect();
    }, delay);
  }

  private startHeartbeat() {
    this.heartbeatInterval = window.setInterval(() => {
      if (this.ws?.readyState === WebSocket.OPEN) {
        this.send({ type: 'ping' });
      }
    }, 30000); // Ping every 30 seconds
  }

  private stopHeartbeat() {
    if (this.heartbeatInterval !== null) {
      clearInterval(this.heartbeatInterval);
      this.heartbeatInterval = null;
    }
  }

  public send(message: WebSocketMessage) {
    if (this.ws?.readyState === WebSocket.OPEN) {
      this.ws.send(JSON.stringify(message));
    } else {
      console.warn('[CRYPTEX WebSocket] Cannot send message, not connected');
    }
  }

  public on(messageType: string, handler: (data: any) => void) {
    if (!this.messageHandlers.has(messageType)) {
      this.messageHandlers.set(messageType, []);
    }
    this.messageHandlers.get(messageType)!.push(handler);
  }

  public off(messageType: string, handler: (data: any) => void) {
    const handlers = this.messageHandlers.get(messageType);
    if (handlers) {
      const index = handlers.indexOf(handler);
      if (index > -1) {
        handlers.splice(index, 1);
      }
    }
  }

  public disconnect() {
    this.stopHeartbeat();
    if (this.ws) {
      this.ws.close();
      this.ws = null;
    }
  }

  public reconnect() {
    this.disconnect();
    this.reconnectAttempts = 0;
    this.connect();
  }

  public get isConnected(): boolean {
    return this.ws?.readyState === WebSocket.OPEN;
  }
}

// Singleton instance
let wsInstance: CryptexWebSocket | null = null;

export function getCryptexWebSocket(): CryptexWebSocket {
  if (!wsInstance) {
    wsInstance = new CryptexWebSocket();
  }
  return wsInstance;
}

export function disconnectCryptexWebSocket() {
  if (wsInstance) {
    wsInstance.disconnect();
    wsInstance = null;
  }
}
