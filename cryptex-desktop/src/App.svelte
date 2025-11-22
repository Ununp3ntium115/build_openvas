<script lang="ts">
  /**
   * openFireVulnerability Desktop Application
   * Professional fire-themed vulnerability assessment platform
   * Stack: Tauri + Rust (redb) + Svelte + Node-RED
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

<div class="app fire-theme">
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
    /* openFireVulnerability color scheme - Fire-themed professional palette */
    /* Primary brand colors - Fire theme */
    --primary-color: #ff6b35;        /* Flame Orange - primary brand color */
    --primary-hover: #ff8c5a;        /* Lighter flame for hover states */
    --primary-dark: #e55a2b;         /* Darker flame for pressed states */

    /* Secondary and accent colors */
    --secondary-color: #ffa500;      /* Ember Orange - secondary accent */
    --secondary-hover: #ffb733;      /* Lighter ember */
    --accent-fire: #ff4500;          /* Pure fire red-orange */
    --accent-ember: #d4621e;         /* Deep ember */

    /* Dark professional background - security tool aesthetic */
    --bg-primary: #0f0f0f;           /* Near black - deepest background */
    --bg-secondary: #1a1a1a;         /* Charcoal - primary surfaces */
    --bg-tertiary: #242424;          /* Lighter charcoal - elevated surfaces */
    --bg-hover: #2e2e2e;             /* Hover state background */
    --bg-active: #383838;            /* Active/pressed state */

    /* Borders and dividers */
    --border-color: #333333;         /* Subtle borders */
    --border-light: #444444;         /* Lighter borders for emphasis */
    --border-focus: #ff6b35;         /* Fire orange for focused elements */

    /* Text colors */
    --text-primary: #f5f5f5;         /* High contrast white-ish */
    --text-secondary: #cccccc;       /* Medium gray for secondary text */
    --text-muted: #999999;           /* Muted gray for hints/labels */
    --text-inverse: #0f0f0f;         /* Dark text for light backgrounds */

    /* Severity colors - fire gradient */
    --critical-color: #dc2626;       /* Critical red - immediate danger */
    --high-color: #fb923c;           /* High orange - urgent */
    --medium-color: #facc15;         /* Medium yellow - warning */
    --low-color: #4ade80;            /* Low green - minimal risk */
    --info-color: #60a5fa;           /* Info blue - neutral information */

    /* Legacy severity color names for compatibility */
    --critical: #dc2626;
    --high: #fb923c;
    --medium: #facc15;
    --low: #4ade80;
    --log: #60a5fa;

    /* Status colors */
    --success-color: #22c55e;        /* Success green */
    --warning-color: #f59e0b;        /* Warning amber */
    --error-color: #ef4444;          /* Error red */
    --running-color: #3b82f6;        /* Running blue */

    /* Legacy status color names for compatibility */
    --success: #22c55e;
    --warning: #f59e0b;
    --error: #ef4444;
    --info: #60a5fa;

    /* Gradients - fire-themed */
    --gradient-fire: linear-gradient(135deg, #ff4500 0%, #ff6b35 50%, #ffa500 100%);
    --gradient-ember: linear-gradient(135deg, #d4621e 0%, #e55a2b 100%);
    --gradient-dark: linear-gradient(180deg, #1a1a1a 0%, #0f0f0f 100%);

    /* Shadows - enhanced depth */
    --shadow-sm: 0 1px 3px rgba(0, 0, 0, 0.5);
    --shadow: 0 2px 8px rgba(0, 0, 0, 0.4);
    --shadow-md: 0 4px 12px rgba(0, 0, 0, 0.5);
    --shadow-lg: 0 8px 24px rgba(0, 0, 0, 0.6);
    --shadow-fire: 0 0 20px rgba(255, 107, 53, 0.15);  /* Subtle fire glow */

    /* Transitions */
    --transition-fast: 150ms cubic-bezier(0.4, 0, 0.2, 1);
    --transition-normal: 250ms cubic-bezier(0.4, 0, 0.2, 1);
    --transition-slow: 350ms cubic-bezier(0.4, 0, 0.2, 1);
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
