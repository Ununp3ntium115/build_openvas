<script lang="ts">
  import { Save, Server, Database, Bell, Shield } from 'lucide-svelte';
  import { onMount } from 'svelte';

  interface Config {
    apiUrl: string;
    apiPort: number;
    dbPath: string;
    autoStartServer: boolean;
    notificationsEnabled: boolean;
    scanTimeout: number;
    maxConcurrentScans: number;
    reportRetentionDays: number;
  }

  let config: Config = {
    apiUrl: 'http://localhost',
    apiPort: 8080,
    dbPath: '$HOME/.local/share/CRYPTEX/cryptex.db',
    autoStartServer: true,
    notificationsEnabled: true,
    scanTimeout: 3600,
    maxConcurrentScans: 5,
    reportRetentionDays: 90
  };

  let saved = false;

  onMount(() => {
    loadConfig();
  });

  function loadConfig() {
    const stored = localStorage.getItem('cryptex_config');
    if (stored) {
      config = JSON.parse(stored);
    }
  }

  function saveConfig() {
    localStorage.setItem('cryptex_config', JSON.stringify(config));
    saved = true;
    setTimeout(() => saved = false, 3000);
  }

  function resetToDefaults() {
    if (confirm('Reset all settings to default values?')) {
      config = {
        apiUrl: 'http://localhost',
        apiPort: 8080,
        dbPath: '$HOME/.local/share/CRYPTEX/cryptex.db',
        autoStartServer: true,
        notificationsEnabled: true,
        scanTimeout: 3600,
        maxConcurrentScans: 5,
        reportRetentionDays: 90
      };
      saveConfig();
    }
  }
</script>

<div class="configuration-view">
  <div class="page-header">
    <div>
      <h1>Configuration</h1>
      <p class="subtitle">System configuration and settings</p>
    </div>
    <div class="header-actions">
      <button class="btn btn-secondary" on:click={resetToDefaults}>
        Reset to Defaults
      </button>
      <button class="btn btn-primary" on:click={saveConfig}>
        <Save size={18} />
        <span>Save Settings</span>
      </button>
    </div>
  </div>

  {#if saved}
    <div class="save-notification">
      Settings saved successfully!
    </div>
  {/if}

  <div class="config-sections">
    <!-- API Server Configuration -->
    <div class="config-section">
      <div class="section-header">
        <Server size={24} />
        <div>
          <h2>API Server</h2>
          <p>Configure the CRYPTEX REST API server</p>
        </div>
      </div>
      <div class="section-body">
        <div class="form-group">
          <label for="apiUrl">API Base URL</label>
          <input
            type="text"
            id="apiUrl"
            bind:value={config.apiUrl}
            placeholder="http://localhost"
          />
          <small>Base URL for the CRYPTEX API server</small>
        </div>
        <div class="form-group">
          <label for="apiPort">API Port</label>
          <input
            type="number"
            id="apiPort"
            bind:value={config.apiPort}
            min="1"
            max="65535"
          />
          <small>Port number for the API server (1-65535)</small>
        </div>
        <div class="form-group">
          <label class="checkbox-label">
            <input type="checkbox" bind:checked={config.autoStartServer} />
            <span>Auto-start API server on launch</span>
          </label>
          <small>Automatically start the embedded API server when the application starts</small>
        </div>
      </div>
    </div>

    <!-- Database Configuration -->
    <div class="config-section">
      <div class="section-header">
        <Database size={24} />
        <div>
          <h2>Database</h2>
          <p>Configure the redb database location</p>
        </div>
      </div>
      <div class="section-body">
        <div class="form-group">
          <label for="dbPath">Database Path</label>
          <input
            type="text"
            id="dbPath"
            bind:value={config.dbPath}
            placeholder="$HOME/.local/share/CRYPTEX/cryptex.db"
          />
          <small>Full path to the CRYPTEX database file. Use $HOME for user home directory.</small>
        </div>
      </div>
    </div>

    <!-- Scan Configuration -->
    <div class="config-section">
      <div class="section-header">
        <Shield size={24} />
        <div>
          <h2>Scan Settings</h2>
          <p>Configure scan behavior and limits</p>
        </div>
      </div>
      <div class="section-body">
        <div class="form-group">
          <label for="scanTimeout">Scan Timeout (seconds)</label>
          <input
            type="number"
            id="scanTimeout"
            bind:value={config.scanTimeout}
            min="60"
            max="86400"
          />
          <small>Maximum time allowed for a single scan (60-86400 seconds)</small>
        </div>
        <div class="form-group">
          <label for="maxConcurrentScans">Max Concurrent Scans</label>
          <input
            type="number"
            id="maxConcurrentScans"
            bind:value={config.maxConcurrentScans}
            min="1"
            max="20"
          />
          <small>Maximum number of scans that can run simultaneously (1-20)</small>
        </div>
        <div class="form-group">
          <label for="reportRetention">Report Retention (days)</label>
          <input
            type="number"
            id="reportRetention"
            bind:value={config.reportRetentionDays}
            min="1"
            max="365"
          />
          <small>How long to keep scan reports before auto-deletion (1-365 days)</small>
        </div>
      </div>
    </div>

    <!-- Notifications -->
    <div class="config-section">
      <div class="section-header">
        <Bell size={24} />
        <div>
          <h2>Notifications</h2>
          <p>Configure notification preferences</p>
        </div>
      </div>
      <div class="section-body">
        <div class="form-group">
          <label class="checkbox-label">
            <input type="checkbox" bind:checked={config.notificationsEnabled} />
            <span>Enable desktop notifications</span>
          </label>
          <small>Show desktop notifications for scan completion and critical vulnerabilities</small>
        </div>
      </div>
    </div>
  </div>

  <div class="config-info">
    <h3>Current Configuration</h3>
    <div class="info-grid">
      <div class="info-item">
        <strong>Full API URL</strong>
        <code>{config.apiUrl}:{config.apiPort}</code>
      </div>
      <div class="info-item">
        <strong>Database Location</strong>
        <code>{config.dbPath}</code>
      </div>
      <div class="info-item">
        <strong>Auto-start Server</strong>
        <code>{config.autoStartServer ? 'Enabled' : 'Disabled'}</code>
      </div>
      <div class="info-item">
        <strong>Notifications</strong>
        <code>{config.notificationsEnabled ? 'Enabled' : 'Disabled'}</code>
      </div>
    </div>
  </div>
</div>

<style>
  .configuration-view {
    padding: 2rem;
    max-width: 1000px;
    margin: 0 auto;
  }

  .page-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 2rem;
  }

  .page-header h1 {
    margin: 0;
    font-size: 2rem;
    color: var(--text-primary);
  }

  .subtitle {
    margin: 0.5rem 0 0 0;
    color: var(--text-muted);
  }

  .header-actions {
    display: flex;
    gap: 1rem;
  }

  .btn {
    display: inline-flex;
    align-items: center;
    gap: 0.5rem;
    padding: 0.75rem 1.5rem;
    border: none;
    border-radius: 6px;
    font-weight: 500;
    cursor: pointer;
    transition: all 0.2s;
  }

  .btn-primary {
    background: var(--primary-color);
    color: var(--text-primary);
  }

  .btn-primary:hover {
    background: var(--primary-hover);
  }

  .btn-secondary {
    background: var(--bg-tertiary);
    color: var(--text-secondary);
    border: 1px solid var(--border-color);
  }

  .btn-secondary:hover {
    background: var(--hover-bg);
  }

  .save-notification {
    background: var(--primary-color);
    color: var(--text-primary);
    padding: 1rem;
    border-radius: 6px;
    margin-bottom: 2rem;
    text-align: center;
    font-weight: 500;
    animation: slideIn 0.3s ease-out;
  }

  @keyframes slideIn {
    from {
      opacity: 0;
      transform: translateY(-10px);
    }
    to {
      opacity: 1;
      transform: translateY(0);
    }
  }

  .config-sections {
    display: flex;
    flex-direction: column;
    gap: 2rem;
    margin-bottom: 2rem;
  }

  .config-section {
    background: var(--bg-secondary);
    border: 1px solid var(--border-color);
    border-radius: 8px;
    overflow: hidden;
  }

  .section-header {
    padding: 1.5rem;
    border-bottom: 1px solid var(--border-color);
    display: flex;
    gap: 1rem;
    align-items: flex-start;
    color: var(--primary-color);
  }

  .section-header h2 {
    margin: 0;
    font-size: 1.25rem;
    color: var(--text-primary);
  }

  .section-header p {
    margin: 0.25rem 0 0 0;
    color: var(--text-muted);
    font-size: 0.875rem;
  }

  .section-body {
    padding: 1.5rem;
    display: flex;
    flex-direction: column;
    gap: 1.5rem;
  }

  .form-group {
    display: flex;
    flex-direction: column;
    gap: 0.5rem;
  }

  .form-group label {
    color: var(--text-secondary);
    font-weight: 500;
    font-size: 0.875rem;
  }

  .form-group input[type="text"],
  .form-group input[type="number"] {
    padding: 0.75rem;
    background: var(--bg-tertiary);
    border: 1px solid var(--border-color);
    border-radius: 6px;
    color: var(--text-primary);
    font-family: inherit;
    font-size: 1rem;
  }

  .form-group input[type="text"]:focus,
  .form-group input[type="number"]:focus {
    outline: none;
    border-color: var(--primary-color);
  }

  .checkbox-label {
    display: flex;
    align-items: center;
    gap: 0.75rem;
    cursor: pointer;
  }

  .checkbox-label input[type="checkbox"] {
    width: 1.25rem;
    height: 1.25rem;
    cursor: pointer;
  }

  .checkbox-label span {
    color: var(--text-primary);
    font-weight: 500;
  }

  .form-group small {
    color: var(--text-muted);
    font-size: 0.875rem;
  }

  .config-info {
    background: var(--bg-secondary);
    border: 1px solid var(--border-color);
    border-radius: 8px;
    padding: 1.5rem;
  }

  .config-info h3 {
    margin: 0 0 1rem 0;
    color: var(--text-primary);
    font-size: 1.125rem;
  }

  .info-grid {
    display: grid;
    grid-template-columns: repeat(2, 1fr);
    gap: 1rem;
  }

  .info-item {
    display: flex;
    flex-direction: column;
    gap: 0.25rem;
  }

  .info-item strong {
    color: var(--text-secondary);
    font-size: 0.875rem;
  }

  .info-item code {
    background: var(--bg-tertiary);
    padding: 0.5rem;
    border-radius: 4px;
    color: var(--primary-color);
    font-size: 0.875rem;
  }

  @media (max-width: 768px) {
    .page-header {
      flex-direction: column;
      align-items: flex-start;
      gap: 1rem;
    }

    .header-actions {
      width: 100%;
    }

    .btn {
      flex: 1;
    }

    .info-grid {
      grid-template-columns: 1fr;
    }
  }
</style>
