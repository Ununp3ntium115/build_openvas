# FRONTEND ARCHITECTURE: The Visualizer
**Project**: CRYPTEX - Cross-Platform Rust Yielding Penetration Testing Execution eXperience
**Tech Stack**: Svelte 4/5 + TypeScript + Electron (optional)
**Philosophy**: Reactive, Accessible, Liberating

## Overview

The Visualizer is the frontend component of CRYPTEX, providing a modern, reactive user interface for security assessment operations. It embraces Svelte's compile-time optimization and TypeScript's type safety to create a lightweight, fast, and maintainable application.

---

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                      The Visualizer (Frontend)                   │
│                                                                  │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │              Svelte Application Layer                       │ │
│  │                                                             │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐    │ │
│  │  │   Routes     │  │  Components  │  │    Stores    │    │ │
│  │  │   (Pages)    │  │   (Reusable) │  │   (State)    │    │ │
│  │  └──────────────┘  └──────────────┘  └──────────────┘    │ │
│  └────────────────────────────────────────────────────────────┘ │
│                              │                                   │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │              Service Layer (TypeScript)                     │ │
│  │                                                             │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐    │ │
│  │  │  API Client  │  │  WebSocket   │  │  LocalStorage│    │ │
│  │  │  (REST)      │  │  Client      │  │  Manager     │    │ │
│  │  └──────────────┘  └──────────────┘  └──────────────┘    │ │
│  └────────────────────────────────────────────────────────────┘ │
│                              │                                   │
└──────────────────────────────┼───────────────────────────────────┘
                               │
                               │ HTTP/WS
                               ▼
                    ┌──────────────────────┐
                    │   The Interface      │
                    │   (Rust API Server)  │
                    └──────────────────────┘
```

---

## Technology Stack

### **Core Technologies**

| Technology | Purpose | Version |
|-----------|---------|---------|
| **Svelte** | Reactive UI framework | 4.x / 5.x |
| **TypeScript** | Type-safe JavaScript | 5.x |
| **Vite** | Build tool & dev server | 5.x |
| **TailwindCSS** | Utility-first styling | 3.x |
| **Chart.js** | Data visualization | 4.x |
| **Electron** (optional) | Desktop app wrapper | 28.x |

### **Supporting Libraries**

- **svelte-routing**: Client-side routing
- **svelte-i18n**: Internationalization
- **@tanstack/svelte-query**: Data fetching & caching
- **date-fns**: Date manipulation
- **zod**: Runtime type validation
- **lucide-svelte**: Icon library

---

## Project Structure

```
cryptex-visualizer/
├── package.json
├── vite.config.ts
├── tsconfig.json
├── tailwind.config.js
├── electron/                   # Electron wrapper (optional)
│   ├── main.ts
│   └── preload.ts
├── public/                     # Static assets
│   ├── favicon.ico
│   └── cryptex-logo.svg
├── src/
│   ├── main.ts                 # Application entry point
│   ├── App.svelte              # Root component
│   ├── routes/                 # Page components
│   │   ├── Dashboard.svelte
│   │   ├── Infiltrations.svelte
│   │   ├── Weaknesses.svelte
│   │   ├── Reports.svelte
│   │   └── Settings.svelte
│   ├── components/             # Reusable components
│   │   ├── common/
│   │   │   ├── Button.svelte
│   │   │   ├── Card.svelte
│   │   │   ├── Modal.svelte
│   │   │   └── Table.svelte
│   │   ├── infiltration/
│   │   │   ├── InfiltrationForm.svelte
│   │   │   ├── InfiltrationList.svelte
│   │   │   └── InfiltrationProgress.svelte
│   │   ├── weakness/
│   │   │   ├── WeaknessCard.svelte
│   │   │   ├── WeaknessDetails.svelte
│   │   │   └── SeverityBadge.svelte
│   │   └── charts/
│   │       ├── SeverityChart.svelte
│   │       ├── TrendChart.svelte
│   │       └── RiskMatrix.svelte
│   ├── stores/                 # State management
│   │   ├── commune.ts          # Server state
│   │   ├── infiltrations.ts    # Scan jobs state
│   │   ├── weaknesses.ts       # Vulnerabilities state
│   │   ├── ui.ts               # UI state (modals, notifications)
│   │   └── auth.ts             # Authentication state
│   ├── services/               # API & business logic
│   │   ├── api/
│   │   │   ├── client.ts       # HTTP client
│   │   │   ├── infiltrator.ts  # Scan API
│   │   │   ├── assessor.ts     # Vulnerability API
│   │   │   └── propagandist.ts # Report API
│   │   ├── websocket.ts        # WebSocket client
│   │   └── storage.ts          # LocalStorage wrapper
│   ├── types/                  # TypeScript types
│   │   ├── index.ts
│   │   ├── infiltration.ts
│   │   ├── weakness.ts
│   │   └── api.ts
│   ├── utils/                  # Utility functions
│   │   ├── format.ts
│   │   ├── validation.ts
│   │   └── cryptex.ts          # Cryptex dictionary client-side
│   └── styles/
│       ├── global.css
│       └── theme.css
└── tests/
    ├── unit/
    └── integration/
```

---

## Core Implementation

### **1. Main Application Entry**

```typescript
// src/main.ts

import App from './App.svelte';
import './styles/global.css';

// Initialize the Visualizer
const app = new App({
  target: document.getElementById('app')!,
});

export default app;
```

### **2. Root Component**

```svelte
<!-- src/App.svelte -->

<script lang="ts">
  import { Router, Route } from 'svelte-routing';
  import { onMount } from 'svelte';

  import Dashboard from './routes/Dashboard.svelte';
  import Infiltrations from './routes/Infiltrations.svelte';
  import Weaknesses from './routes/Weaknesses.svelte';
  import Reports from './routes/Reports.svelte';
  import Settings from './routes/Settings.svelte';

  import Navbar from './components/common/Navbar.svelte';
  import NotificationSystem from './components/common/NotificationSystem.svelte';

  import { initializeCommune } from './stores/commune';
  import { initializeWebSocket } from './services/websocket';

  // The Awakening - Initialize application
  onMount(async () => {
    await initializeCommune();
    initializeWebSocket();
  });
</script>

<Router>
  <div class="app">
    <Navbar />

    <main class="container">
      <Route path="/" component={Dashboard} />
      <Route path="/infiltrations" component={Infiltrations} />
      <Route path="/weaknesses" component={Weaknesses} />
      <Route path="/reports" component={Reports} />
      <Route path="/settings" component={Settings} />
    </main>

    <NotificationSystem />
  </div>
</Router>

<style>
  .app {
    display: flex;
    flex-direction: column;
    min-height: 100vh;
  }

  .container {
    flex: 1;
    padding: 2rem;
    max-width: 1400px;
    margin: 0 auto;
    width: 100%;
  }
</style>
```

---

### **3. State Management with Stores**

```typescript
// src/stores/infiltrations.ts

import { writable, derived } from 'svelte/store';
import type { InfiltrationJob } from '../types/infiltration';
import { infiltratorAPI } from '../services/api/infiltrator';

// Store for all infiltration jobs
export const infiltrations = writable<InfiltrationJob[]>([]);

// Store for active infiltration
export const activeInfiltration = writable<string | null>(null);

// Derived store: active infiltrations only
export const activeInfiltrations = derived(
  infiltrations,
  ($infiltrations) => $infiltrations.filter(
    (job) => job.status === 'running' || job.status === 'queued'
  )
);

// Actions
export const infiltrationActions = {
  // The Call to Action - Start new infiltration
  async the_call_to_action(
    targets: string[],
    scanType: string
  ): Promise<void> {
    try {
      const job = await infiltratorAPI.the_infiltration(targets, {
        scan_type: scanType,
        options: { intensity: 7 },
      });

      infiltrations.update((jobs) => [...jobs, job]);
      activeInfiltration.set(job.id);
    } catch (error) {
      console.error('Infiltration initiation failed:', error);
      throw error;
    }
  },

  // The Observation - Update infiltration progress
  async the_observation(jobId: string): Promise<void> {
    try {
      const progress = await infiltratorAPI.the_observation(jobId);

      infiltrations.update((jobs) =>
        jobs.map((job) =>
          job.id === jobId
            ? { ...job, progress: progress.progress_percent, status: progress.status }
            : job
        )
      );
    } catch (error) {
      console.error('Observation failed:', error);
      throw error;
    }
  },

  // The Revelation - Get infiltration results
  async the_revelation(jobId: string): Promise<void> {
    try {
      const results = await infiltratorAPI.the_revelation(jobId);

      infiltrations.update((jobs) =>
        jobs.map((job) =>
          job.id === jobId
            ? { ...job, status: 'completed', results }
            : job
        )
      );
    } catch (error) {
      console.error('Revelation failed:', error);
      throw error;
    }
  },
};
```

---

### **4. API Service Layer**

```typescript
// src/services/api/client.ts

import axios, { AxiosInstance } from 'axios';
import type { CryptexError } from '../../types/api';

class APIClient {
  private seeker: AxiosInstance;
  private gateway: string;

  constructor() {
    this.gateway = import.meta.env.VITE_API_URL || 'http://localhost:8080';

    this.seeker = axios.create({
      baseURL: this.gateway,
      timeout: 30000,
      headers: {
        'Content-Type': 'application/json',
      },
    });

    // Request interceptor
    this.seeker.interceptors.request.use(
      (config) => {
        // Add authentication token if available
        const token = localStorage.getItem('cryptex_sigil');
        if (token) {
          config.headers.Authorization = `Bearer ${token}`;
        }
        return config;
      },
      (error) => Promise.reject(error)
    );

    // Response interceptor
    this.seeker.interceptors.response.use(
      (response) => response,
      (error) => {
        const cryptexError: CryptexError = {
          code: error.response?.status || 500,
          message: error.response?.data?.message || 'Unknown error',
          details: error.response?.data?.details,
        };
        return Promise.reject(cryptexError);
      }
    );
  }

  get client(): AxiosInstance {
    return this.seeker;
  }
}

export const apiClient = new APIClient().client;
```

```typescript
// src/services/api/infiltrator.ts

import { apiClient } from './client';
import type {
  InfiltrationJob,
  InfiltrationProgress,
  InfiltrationResults,
  ScanConfig,
} from '../../types/infiltration';

export const infiltratorAPI = {
  // The Infiltration - Start scan
  async the_infiltration(
    targets: string[],
    config: ScanConfig
  ): Promise<InfiltrationJob> {
    const response = await apiClient.post('/api/v1/scans/infiltration', {
      targets,
      scan_type: config.scan_type,
      options: config.options,
    });
    return response.data;
  },

  // The Observation - Get progress
  async the_observation(jobId: string): Promise<InfiltrationProgress> {
    const response = await apiClient.get(
      `/api/v1/scans/infiltration/${jobId}/progress`
    );
    return response.data;
  },

  // The Revelation - Get results
  async the_revelation(
    jobId: string,
    filters?: any
  ): Promise<InfiltrationResults> {
    const response = await apiClient.get(
      `/api/v1/scans/infiltration/${jobId}/results`,
      { params: filters }
    );
    return response.data;
  },

  // The Gathering - List all infiltrations
  async the_gathering(): Promise<InfiltrationJob[]> {
    const response = await apiClient.get('/api/v1/scans/infiltration');
    return response.data.jobs;
  },
};
```

---

### **5. WebSocket Integration**

```typescript
// src/services/websocket.ts

import { infiltrations } from '../stores/infiltrations';
import { weaknesses } from '../stores/weaknesses';
import { notifications } from '../stores/ui';

class CryptexWebSocket {
  private ws: WebSocket | null = null;
  private gateway: string;
  private reconnectAttempts = 0;
  private maxReconnectAttempts = 5;

  constructor() {
    this.gateway = import.meta.env.VITE_WS_URL || 'ws://localhost:9090';
  }

  // The Connection - Establish WebSocket
  connect(): void {
    try {
      this.ws = new WebSocket(this.gateway);

      this.ws.onopen = () => {
        console.log('WebSocket connected to the commune');
        this.reconnectAttempts = 0;

        // Subscribe to real-time updates
        this.send({
          type: 'subscribe',
          channels: ['infiltrations', 'weaknesses', 'metrics'],
        });
      };

      this.ws.onmessage = (event) => {
        this.handleMessage(JSON.parse(event.data));
      };

      this.ws.onerror = (error) => {
        console.error('WebSocket error:', error);
      };

      this.ws.onclose = () => {
        console.log('WebSocket disconnected');
        this.attemptReconnect();
      };
    } catch (error) {
      console.error('Failed to establish WebSocket connection:', error);
      this.attemptReconnect();
    }
  }

  private handleMessage(message: any): void {
    switch (message.type) {
      case 'infiltration_update':
        this.handleInfiltrationUpdate(message.data);
        break;
      case 'weakness_discovered':
        this.handleWeaknessDiscovered(message.data);
        break;
      case 'metric_update':
        this.handleMetricUpdate(message.data);
        break;
      default:
        console.warn('Unknown message type:', message.type);
    }
  }

  private handleInfiltrationUpdate(data: any): void {
    infiltrations.update((jobs) =>
      jobs.map((job) =>
        job.id === data.job_id
          ? { ...job, progress: data.progress, status: data.status }
          : job
      )
    );
  }

  private handleWeaknessDiscovered(data: any): void {
    weaknesses.update((list) => [...list, data.weakness]);

    // Show notification
    if (data.weakness.severity === 'critical') {
      notifications.add({
        type: 'warning',
        title: 'Critical Weakness Discovered',
        message: `CVE-${data.weakness.cve_id}: ${data.weakness.name}`,
      });
    }
  }

  private handleMetricUpdate(data: any): void {
    // Update metrics store
    console.log('Metrics updated:', data);
  }

  private attemptReconnect(): void {
    if (this.reconnectAttempts >= this.maxReconnectAttempts) {
      console.error('Max reconnection attempts reached');
      return;
    }

    const delay = Math.min(1000 * 2 ** this.reconnectAttempts, 30000);
    this.reconnectAttempts++;

    setTimeout(() => {
      console.log(`Reconnecting... (attempt ${this.reconnectAttempts})`);
      this.connect();
    }, delay);
  }

  private send(data: any): void {
    if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      this.ws.send(JSON.stringify(data));
    }
  }

  disconnect(): void {
    if (this.ws) {
      this.ws.close();
      this.ws = null;
    }
  }
}

export const cryptexWS = new CryptexWebSocket();

export function initializeWebSocket(): void {
  cryptexWS.connect();
}
```

---

### **6. Component Example: Infiltration Form**

```svelte
<!-- src/components/infiltration/InfiltrationForm.svelte -->

<script lang="ts">
  import { infiltrationActions } from '../../stores/infiltrations';
  import Button from '../common/Button.svelte';
  import { notifications } from '../../stores/ui';

  let targets = '';
  let scanType: 'quick' | 'full' | 'deep' = 'full';
  let loading = false;

  async function handleSubmit() {
    if (!targets.trim()) {
      notifications.add({
        type: 'error',
        title: 'Invalid Input',
        message: 'Please enter at least one target',
      });
      return;
    }

    const targetList = targets
      .split('\n')
      .map((t) => t.trim())
      .filter((t) => t.length > 0);

    loading = true;

    try {
      await infiltrationActions.the_call_to_action(targetList, scanType);

      notifications.add({
        type: 'success',
        title: 'Infiltration Initiated',
        message: `Scan queued for ${targetList.length} target(s)`,
      });

      // Reset form
      targets = '';
    } catch (error) {
      notifications.add({
        type: 'error',
        title: 'Infiltration Failed',
        message: error.message,
      });
    } finally {
      loading = false;
    }
  }
</script>

<div class="infiltration-form">
  <h2>Initiate Infiltration</h2>

  <form on:submit|preventDefault={handleSubmit}>
    <div class="form-group">
      <label for="targets">
        Targets (one per line)
      </label>
      <textarea
        id="targets"
        bind:value={targets}
        placeholder="192.168.1.0/24&#10;example.com&#10;10.0.0.1"
        rows="5"
        required
      />
    </div>

    <div class="form-group">
      <label for="scan-type">Scan Type</label>
      <select id="scan-type" bind:value={scanType}>
        <option value="quick">Quick Scan</option>
        <option value="full">Full Scan</option>
        <option value="deep">Deep Scan</option>
      </select>
    </div>

    <Button
      type="submit"
      variant="primary"
      {loading}
      disabled={loading}
    >
      {loading ? 'Initiating...' : 'Begin Infiltration'}
    </Button>
  </form>
</div>

<style>
  .infiltration-form {
    background: var(--card-bg);
    padding: 1.5rem;
    border-radius: 8px;
    box-shadow: var(--shadow-sm);
  }

  h2 {
    margin-bottom: 1.5rem;
    color: var(--text-primary);
  }

  .form-group {
    margin-bottom: 1rem;
  }

  label {
    display: block;
    margin-bottom: 0.5rem;
    font-weight: 500;
    color: var(--text-secondary);
  }

  textarea,
  select {
    width: 100%;
    padding: 0.75rem;
    border: 1px solid var(--border-color);
    border-radius: 4px;
    font-family: monospace;
    background: var(--input-bg);
    color: var(--text-primary);
  }

  textarea {
    resize: vertical;
  }
</style>
```

---

## Electron Integration (Optional)

### **Configuration**

```typescript
// electron/main.ts

import { app, BrowserWindow } from 'electron';
import path from 'path';

function createWindow() {
  const mainWindow = new BrowserWindow({
    width: 1200,
    height: 800,
    title: 'CRYPTEX - Security Assessment Platform',
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      contextIsolation: true,
      nodeIntegration: false,
    },
  });

  if (process.env.NODE_ENV === 'development') {
    mainWindow.loadURL('http://localhost:5173');
    mainWindow.webContents.openDevTools();
  } else {
    mainWindow.loadFile(path.join(__dirname, '../dist/index.html'));
  }
}

app.whenReady().then(createWindow);

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});
```

---

## Build Configuration

```typescript
// vite.config.ts

import { defineConfig } from 'vite';
import { svelte } from '@sveltejs/vite-plugin-svelte';

export default defineConfig({
  plugins: [svelte()],
  build: {
    outDir: 'dist',
    sourcemap: true,
  },
  server: {
    port: 5173,
    proxy: {
      '/api': {
        target: 'http://localhost:8080',
        changeOrigin: true,
      },
    },
  },
});
```

---

## Deployment

### **Development**
```bash
npm run dev
```

### **Production Build**
```bash
npm run build
```

### **Electron Build**
```bash
npm run electron:build
```

---

## Conclusion

The Visualizer provides a modern, reactive interface that complements the Rust backend. It follows anarchist principles of transparency and user empowerment, presenting security data in an accessible and actionable format.

---

*"The interface liberates, the visualizer empowers."*
*- Anarchist Rust Collective, 2025*
