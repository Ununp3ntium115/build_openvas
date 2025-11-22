<script lang="ts">
  import { Link } from 'svelte-routing';
  import { Shield, Activity, FileText, Settings } from 'lucide-svelte';

  export let mode: 'standalone' | 'embedded' = 'standalone';

  // Only show header in standalone mode
  $: showHeader = mode === 'standalone';
</script>

{#if showHeader}
  <header class="cryptex-header">
    <div class="header-content">
      <div class="logo">
        <Shield size={32} />
        <div class="logo-text">
          <h1>CRYPTEX</h1>
          <span class="tagline">The Visualizer</span>
        </div>
      </div>

      <nav class="nav">
        <Link to="/" class="nav-link">
          <Activity size={18} />
          <span>Dashboard</span>
        </Link>
        <Link to="/scans" class="nav-link">
          <FileText size={18} />
          <span>Scans</span>
        </Link>
        <Link to="/settings" class="nav-link">
          <Settings size={18} />
          <span>Settings</span>
        </Link>
      </nav>

      <div class="header-actions">
        <div class="status-indicator">
          <span class="status-dot online"></span>
          <span class="status-text">Connected</span>
        </div>
      </div>
    </div>
  </header>
{/if}

<style>
  .cryptex-header {
    background: var(--bg-secondary);
    border-bottom: 2px solid var(--border);
    position: sticky;
    top: 0;
    z-index: 100;
  }

  .header-content {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: 1rem 2rem;
    max-width: 1600px;
    margin: 0 auto;
  }

  .logo {
    display: flex;
    align-items: center;
    gap: 1rem;
    color: var(--accent);
  }

  .logo-text h1 {
    margin: 0;
    font-size: 1.5rem;
    font-weight: 700;
    color: var(--text-primary);
    letter-spacing: 2px;
  }

  .tagline {
    font-size: 0.75rem;
    color: var(--text-secondary);
    text-transform: uppercase;
    letter-spacing: 1px;
  }

  .nav {
    display: flex;
    gap: 1rem;
  }

  .nav-link {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    padding: 0.75rem 1.25rem;
    background: transparent;
    border: 1px solid transparent;
    border-radius: 6px;
    color: var(--text-secondary);
    text-decoration: none;
    transition: all 0.2s;
    font-weight: 500;
  }

  .nav-link:hover {
    color: var(--text-primary);
    background: var(--bg-tertiary);
    border-color: var(--border);
  }

  .nav-link:global(.active) {
    color: var(--accent);
    background: var(--bg-tertiary);
    border-color: var(--accent);
  }

  .header-actions {
    display: flex;
    align-items: center;
    gap: 1rem;
  }

  .status-indicator {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    padding: 0.5rem 1rem;
    background: var(--bg-tertiary);
    border: 1px solid var(--border);
    border-radius: 6px;
  }

  .status-dot {
    width: 8px;
    height: 8px;
    border-radius: 50%;
    animation: pulse 2s infinite;
  }

  .status-dot.online {
    background: var(--success);
  }

  .status-text {
    font-size: 0.875rem;
    color: var(--text-secondary);
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
