<script lang="ts">
  import { invoke } from '@tauri-apps/api/tauri';
  import { Menu, Activity, Settings, HelpCircle, Minimize2, Maximize2, X } from 'lucide-svelte';
  import { appWindow } from '@tauri-apps/api/window';

  export let apiStatus = false;

  let isMaximized = false;

  async function minimize() {
    await appWindow.minimize();
  }

  async function toggleMaximize() {
    if (isMaximized) {
      await appWindow.unmaximize();
    } else {
      await appWindow.maximize();
    }
    isMaximized = !isMaximized;
  }

  async function close() {
    await appWindow.close();
  }

  async function openNodeRed() {
    await invoke('open_node_red');
  }

  async function connectToPyro() {
    await invoke('connect_to_pyro');
  }
</script>

<header class="top-bar">
  <div class="left-section">
    <button class="icon-btn" on:click={() => dispatch('toggleSidebar')}>
      <Menu size={20} />
    </button>

    <div class="logo">
      <svg width="28" height="28" viewBox="0 0 64 64" fill="none">
        <defs>
          <linearGradient id="fireGradient" x1="0%" y1="0%" x2="100%" y2="100%">
            <stop offset="0%" stop-color="#ff4500"/>
            <stop offset="50%" stop-color="#ff6b35"/>
            <stop offset="100%" stop-color="#ffa500"/>
          </linearGradient>
        </defs>
        <path d="M32 4L8 14V28C8 42 16 54 32 60C48 54 56 42 56 28V14L32 4Z"
              fill="url(#fireGradient)"
              stroke="var(--primary-hover)"
              stroke-width="2"/>
        <path d="M32 20C28 24 28 28 32 32C36 28 36 24 32 20Z" fill="var(--accent-fire)" opacity="0.8"/>
      </svg>
      <span class="logo-text">openFireVulnerability</span>
    </div>

    <div class="status-indicator">
      <span class="status-dot" class:online={apiStatus}></span>
      <span class="status-text">{apiStatus ? 'Connected' : 'Disconnected'}</span>
    </div>
  </div>

  <div class="center-section">
    <h1 class="page-title">openFireVulnerability Security Platform</h1>
  </div>

  <div class="right-section">
    <button class="action-btn" on:click={openNodeRed} title="Open Node-RED">
      <Activity size={18} />
      <span>Node-RED</span>
    </button>

    <button class="action-btn" on:click={connectToPyro} title="Connect to PYRO">
      <Activity size={18} />
      <span>PYRO</span>
    </button>

    <button class="icon-btn" title="Settings">
      <Settings size={20} />
    </button>

    <button class="icon-btn" title="Help">
      <HelpCircle size={20} />
    </button>

    <div class="divider"></div>

    <button class="window-btn" on:click={minimize}>
      <Minimize2 size={16} />
    </button>

    <button class="window-btn" on:click={toggleMaximize}>
      <Maximize2 size={16} />
    </button>

    <button class="window-btn close" on:click={close}>
      <X size={16} />
    </button>
  </div>
</header>

<style>
  .top-bar {
    display: flex;
    align-items: center;
    justify-content: space-between;
    height: 50px;
    background: var(--bg-secondary);
    border-bottom: 1px solid var(--border-color);
    padding: 0 1rem;
    user-select: none;
    -webkit-app-region: drag;
  }

  .left-section, .right-section {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    -webkit-app-region: no-drag;
  }

  .center-section {
    flex: 1;
    text-align: center;
  }

  .page-title {
    margin: 0;
    font-size: 0.95rem;
    font-weight: 600;
    background: var(--gradient-fire);
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
    background-clip: text;
    letter-spacing: 0.5px;
  }

  .logo {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    margin-left: 0.5rem;
  }

  .logo-text {
    font-size: 1.1rem;
    font-weight: 700;
    background: var(--gradient-fire);
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
    background-clip: text;
    letter-spacing: 0.5px;
  }

  .status-indicator {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    padding: 0.25rem 0.75rem;
    background: var(--bg-tertiary);
    border-radius: 4px;
    font-size: 0.875rem;
  }

  .status-dot {
    width: 8px;
    height: 8px;
    border-radius: 50%;
    background: var(--error-color);
    animation: pulse 2s infinite;
  }

  .status-dot.online {
    background: var(--primary-color);
    box-shadow: 0 0 8px rgba(255, 107, 53, 0.5);
  }

  .status-text {
    color: var(--text-secondary);
    font-size: 0.875rem;
  }

  .icon-btn, .action-btn, .window-btn {
    background: transparent;
    border: none;
    color: var(--text-secondary);
    cursor: pointer;
    padding: 0.5rem;
    border-radius: 4px;
    display: flex;
    align-items: center;
    gap: 0.5rem;
    transition: all 0.2s;
  }

  .icon-btn:hover, .action-btn:hover {
    background: var(--bg-hover);
    color: var(--text-primary);
  }

  .action-btn {
    padding: 0.5rem 0.75rem;
    font-size: 0.875rem;
  }

  .action-btn:hover {
    color: var(--primary-color);
    box-shadow: var(--shadow-fire);
  }

  .window-btn {
    padding: 0.5rem 0.75rem;
  }

  .window-btn:hover {
    background: var(--bg-hover);
  }

  .window-btn.close:hover {
    background: var(--error-color);
    color: white;
  }

  .divider {
    width: 1px;
    height: 24px;
    background: var(--border-color);
    margin: 0 0.5rem;
  }

  @keyframes pulse {
    0%, 100% {
      opacity: 1;
    }
    50% {
      opacity: 0.5;
    }
  }
</style>
