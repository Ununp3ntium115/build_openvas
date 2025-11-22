<script lang="ts">
  import { link } from 'svelte-routing';
  import {
    LayoutDashboard,
    Scan,
    Target,
    FileText,
    ClipboardList,
    Settings,
    Database,
    Activity
  } from 'lucide-svelte';

  export let collapsed = false;

  const menuItems = [
    { path: '/', icon: LayoutDashboard, label: 'Dashboard', badge: null },
    { path: '/scans', icon: Scan, label: 'Scans', badge: null },
    { path: '/targets', icon: Target, label: 'Targets', badge: null },
    { path: '/results', icon: FileText, label: 'Results', badge: '245' },
    { path: '/reports', icon: ClipboardList, label: 'Reports', badge: null },
    { path: '/configuration', icon: Settings, label: 'Configuration', badge: null },
  ];

  const quickActions = [
    { icon: Scan, label: 'New Scan', action: 'newScan' },
    { icon: Target, label: 'New Target', action: 'newTarget' },
    { icon: ClipboardList, label: 'Generate Report', action: 'generateReport' },
  ];
</script>

<aside class="sidebar" class:collapsed>
  <nav class="nav-menu">
    {#each menuItems as item}
      <a href={item.path} use:link class="nav-item" title={item.label}>
        <svelte:component this={item.icon} size={20} />
        {#if !collapsed}
          <span class="nav-label">{item.label}</span>
        {/if}
        {#if item.badge && !collapsed}
          <span class="badge">{item.badge}</span>
        {/if}
      </a>
    {/each}
  </nav>

  {#if !collapsed}
    <div class="quick-actions">
      <h3 class="section-title">Quick Actions</h3>
      {#each quickActions as action}
        <button class="quick-action-btn">
          <svelte:component this={action.icon} size={18} />
          <span>{action.label}</span>
        </button>
      {/each}
    </div>

    <div class="status-panel">
      <h3 class="section-title">System Status</h3>
      <div class="status-item">
        <Database size={16} />
        <span>Database</span>
        <span class="status-badge online">Online</span>
      </div>
      <div class="status-item">
        <Activity size={16} />
        <span>API Server</span>
        <span class="status-badge online">Running</span>
      </div>
    </div>
  {/if}
</aside>

<style>
  .sidebar {
    width: 260px;
    background: var(--bg-secondary);
    border-right: 1px solid var(--border-color);
    display: flex;
    flex-direction: column;
    transition: width 0.3s ease;
    overflow-y: auto;
  }

  .sidebar.collapsed {
    width: 60px;
  }

  .nav-menu {
    padding: 1rem 0;
  }

  .nav-item {
    display: flex;
    align-items: center;
    gap: 0.75rem;
    padding: 0.75rem 1rem;
    color: var(--text-secondary);
    text-decoration: none;
    transition: all 0.2s;
    position: relative;
  }

  .collapsed .nav-item {
    justify-content: center;
    padding: 0.75rem;
  }

  .nav-item:hover {
    background: var(--bg-hover);
    color: var(--text-primary);
  }

  .nav-item:global(.active) {
    background: var(--bg-tertiary);
    color: var(--primary-color);
    border-left: 3px solid var(--primary-color);
    box-shadow: var(--shadow-fire);
  }

  .nav-item:global(.active)::before {
    content: '';
    position: absolute;
    left: 0;
    top: 0;
    bottom: 0;
    width: 3px;
    background: var(--gradient-fire);
  }

  .nav-label {
    flex: 1;
    font-size: 0.875rem;
    font-weight: 500;
  }

  .badge {
    background: var(--error);
    color: white;
    font-size: 0.75rem;
    padding: 0.125rem 0.5rem;
    border-radius: 10px;
    font-weight: 600;
  }

  .quick-actions {
    padding: 1rem;
    border-top: 1px solid var(--border-color);
  }

  .section-title {
    font-size: 0.75rem;
    text-transform: uppercase;
    color: var(--text-muted);
    margin: 0 0 0.75rem 0;
    font-weight: 600;
    letter-spacing: 0.5px;
  }

  .quick-action-btn {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    width: 100%;
    padding: 0.5rem 0.75rem;
    background: var(--bg-tertiary);
    border: 1px solid var(--border-color);
    border-radius: 4px;
    color: var(--text-secondary);
    cursor: pointer;
    transition: all 0.2s;
    margin-bottom: 0.5rem;
    font-size: 0.875rem;
  }

  .quick-action-btn:hover {
    background: var(--bg-hover);
    border-color: var(--primary-color);
    color: var(--primary-color);
    box-shadow: var(--shadow-fire);
  }

  .status-panel {
    padding: 1rem;
    border-top: 1px solid var(--border-color);
    margin-top: auto;
  }

  .status-item {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    padding: 0.5rem 0;
    font-size: 0.875rem;
    color: var(--text-secondary);
  }

  .status-item span:nth-child(2) {
    flex: 1;
  }

  .status-badge {
    font-size: 0.75rem;
    padding: 0.125rem 0.5rem;
    border-radius: 10px;
    font-weight: 600;
  }

  .status-badge.online {
    background: rgba(255, 107, 53, 0.15);
    color: var(--primary-color);
  }
</style>
