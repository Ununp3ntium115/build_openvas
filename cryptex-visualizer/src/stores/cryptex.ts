import { writable, derived } from 'svelte/store';

export interface CryptexConfig {
  mode: 'standalone' | 'embedded';
  apiUrl: string;
  wsUrl?: string;
}

export interface CryptexState {
  connected: boolean;
  lastSync: Date | null;
  error: string | null;
}

// Config store
function createConfigStore() {
  const { subscribe, set, update } = writable<CryptexConfig>({
    mode: 'standalone',
    apiUrl: 'http://localhost:8080',
    wsUrl: 'ws://localhost:8080/ws',
  });

  return {
    subscribe,
    set,
    update,
    setConfig: (config: Partial<CryptexConfig>) => {
      update(current => ({ ...current, ...config }));
    },
    setMode: (mode: 'standalone' | 'embedded') => {
      update(current => ({ ...current, mode }));
    },
    setApiUrl: (apiUrl: string) => {
      update(current => ({
        ...current,
        apiUrl,
        wsUrl: apiUrl.replace('http', 'ws') + '/ws',
      }));
    },
  };
}

// State store
function createStateStore() {
  const { subscribe, set, update } = writable<CryptexState>({
    connected: false,
    lastSync: null,
    error: null,
  });

  return {
    subscribe,
    set,
    update,
    setConnected: (connected: boolean) => {
      update(current => ({ ...current, connected }));
    },
    setError: (error: string | null) => {
      update(current => ({ ...current, error }));
    },
    setSynced: () => {
      update(current => ({ ...current, lastSync: new Date(), error: null }));
    },
  };
}

export const cryptexConfig = createConfigStore();
export const cryptexState = createStateStore();

// Derived store for connection status message
export const connectionStatus = derived(
  cryptexState,
  $state => {
    if ($state.error) return `Error: ${$state.error}`;
    if (!$state.connected) return 'Disconnected';
    if ($state.lastSync) {
      const seconds = Math.floor((Date.now() - $state.lastSync.getTime()) / 1000);
      if (seconds < 60) return 'Connected';
      if (seconds < 3600) return `Last sync ${Math.floor(seconds / 60)}m ago`;
      return `Last sync ${Math.floor(seconds / 3600)}h ago`;
    }
    return 'Connected';
  }
);

// Export a combined store for convenience
export const cryptexStore = {
  config: cryptexConfig,
  state: cryptexState,
  setConfig: cryptexConfig.setConfig,
  setMode: cryptexConfig.setMode,
  setApiUrl: cryptexConfig.setApiUrl,
  setConnected: cryptexState.setConnected,
  setError: cryptexState.setError,
  setSynced: cryptexState.setSynced,
};
