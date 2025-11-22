<script lang="ts">
  /**
   * CRYPTEX Visualizer - Main Application Component
   *
   * Supports both standalone and embedded modes
   */

  import { Router, Route } from 'svelte-routing';
  import { QueryClient, QueryClientProvider } from '@tanstack/svelte-query';
  import { cryptexStore } from './stores/cryptex';

  import Dashboard from './components/Dashboard.svelte';
  import ScanView from './components/ScanView.svelte';
  import VulnerabilityList from './components/VulnerabilityList.svelte';
  import ReportView from './components/ReportView.svelte';
  import Header from './components/Header.svelte';

  // Props
  export let mode: 'standalone' | 'embedded' = 'standalone';
  export let apiUrl: string = 'http://localhost:8080';

  // Initialize QueryClient for data fetching
  const queryClient = new QueryClient({
    defaultOptions: {
      queries: {
        staleTime: 5000,
        refetchOnWindowFocus: false,
      },
    },
  });

  // Initialize CRYPTEX store
  $: cryptexStore.setConfig({ mode, apiUrl });
</script>

<QueryClientProvider client={queryClient}>
  <div class="cryptex-app" class:embedded={mode === 'embedded'}>
    {#if mode === 'standalone'}
      <Header />
    {/if}

    <main class="cryptex-main">
      <Router>
        <Route path="/" component={Dashboard} />
        <Route path="/scans/:id" component={ScanView} />
        <Route path="/vulnerabilities" component={VulnerabilityList} />
        <Route path="/reports/:id" component={ReportView} />
      </Router>
    </main>
  </div>
</QueryClientProvider>

<style>
  .cryptex-app {
    min-height: 100vh;
    background: var(--bg-primary);
    color: var(--text-primary);
  }

  .cryptex-app.embedded {
    min-height: auto;
    border-radius: 0.5rem;
    overflow: hidden;
  }

  .cryptex-main {
    container-type: inline-size;
    padding: 1rem;
  }

  /* CSS variables are defined in app.css */
</style>
