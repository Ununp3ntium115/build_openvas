<script lang="ts">
  /**
   * CRYPTEX Desktop Application
   * Professional OpenVAS-style vulnerability assessment UI
   */

  import { onMount } from 'svelte';
  import { invoke } from '@tauri-apps/api/tauri';
  import { Router, Route } from 'svelte-routing';

  import Sidebar from './components/Sidebar.svelte';
  import TopBar from './components/TopBar.svelte';
  import Dashboard from './views/Dashboard.svelte';
  import Scans from './views/Scans.svelte';
  import Targets from './views/Targets.svelte';
  import Results from './views/Results.svelte';
  import Reports from './views/Reports.svelte';
  import Configuration from './views/Configuration.svelte';

  let apiStatus = false;
  let sidebarCollapsed = false;

  onMount(async () => {
    // Check if API server is running
    try {
      apiStatus = await invoke('get_api_status');
      if (!apiStatus) {
        // Auto-start if not running
        await invoke('start_api_server');
        apiStatus = true;
      }
    } catch (e) {
      console.error('Failed to check API status:', e);
    }
  });

  function toggleSidebar() {
    sidebarCollapsed = !sidebarCollapsed;
  }
</script>

<div class="app greenbone-theme">
  <TopBar bind:apiStatus on:toggleSidebar={toggleSidebar} />

  <div class="main-container">
    <Sidebar collapsed={sidebarCollapsed} />

    <main class="content" class:expanded={sidebarCollapsed}>
      <Router>
        <Route path="/" component={Dashboard} />
        <Route path="/scans" component={Scans} />
        <Route path="/targets" component={Targets} />
        <Route path="/results" component={Results} />
        <Route path="/results/:id" component={Results} />
        <Route path="/reports" component={Reports} />
        <Route path="/configuration" component={Configuration} />
      </Router>
    </main>
  </div>
</div>

<style>
  :global(:root) {
    /* Greenbone/OpenVAS color scheme */
    --gsa-green: #66c430;
    --gsa-green-dark: #5ab028;
    --gsa-green-light: #7ed13f;

    --bg-primary: #1a1d1e;
    --bg-secondary: #242729;
    --bg-tertiary: #2e3235;
    --bg-hover: #363a3d;

    --border-color: #3a3f42;
    --border-light: #4a4f52;

    --text-primary: #e8e8e8;
    --text-secondary: #b8b8b8;
    --text-muted: #888888;

    --critical: #d94f4f;
    --high: #f59842;
    --medium: #f5c842;
    --low: #7ed13f;
    --log: #4a9eff;

    --success: #66c430;
    --warning: #f59842;
    --error: #d94f4f;
    --info: #4a9eff;

    --shadow: 0 2px 8px rgba(0, 0, 0, 0.3);
    --shadow-lg: 0 4px 16px rgba(0, 0, 0, 0.4);
  }

  :global(body) {
    margin: 0;
    padding: 0;
    font-family: 'Segoe UI', 'Roboto', 'Oxygen', 'Ubuntu', sans-serif;
    background: var(--bg-primary);
    color: var(--text-primary);
    overflow: hidden;
  }

  .app {
    width: 100vw;
    height: 100vh;
    display: flex;
    flex-direction: column;
    overflow: hidden;
  }

  .main-container {
    display: flex;
    flex: 1;
    overflow: hidden;
  }

  .content {
    flex: 1;
    overflow-y: auto;
    background: var(--bg-primary);
    padding: 1.5rem;
    transition: margin-left 0.3s ease;
  }

  .content.expanded {
    margin-left: 0;
  }

  /* Scrollbar styling */
  :global(::-webkit-scrollbar) {
    width: 12px;
    height: 12px;
  }

  :global(::-webkit-scrollbar-track) {
    background: var(--bg-secondary);
  }

  :global(::-webkit-scrollbar-thumb) {
    background: var(--border-color);
    border-radius: 6px;
  }

  :global(::-webkit-scrollbar-thumb:hover) {
    background: var(--border-light);
  }
</style>
