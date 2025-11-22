<script lang="ts">
  import { createQuery } from '@tanstack/svelte-query';
  import { CryptexAPI } from '@services/api';
  import VulnerabilityList from './VulnerabilityList.svelte';
  import ReportView from './ReportView.svelte';
  import { ChevronLeft, Download, AlertTriangle } from 'lucide-svelte';

  export let scanId: string;

  const api = new CryptexAPI();

  const scanQuery = createQuery({
    queryKey: ['scan', scanId],
    queryFn: () => api.getScan(scanId),
    refetchInterval: (data) => data?.status === 'running' ? 3000 : false,
  });

  const resultsQuery = createQuery({
    queryKey: ['scan-results', scanId],
    queryFn: () => api.getScanResults(scanId),
    enabled: $scanQuery.data?.status === 'completed',
  });

  let activeTab: 'vulnerabilities' | 'report' = 'vulnerabilities';
  let reportFormat: 'json' | 'html' | 'markdown' = 'html';

  async function downloadReport(format: 'json' | 'html' | 'markdown') {
    const report = await api.generateReport(scanId, format);
    const blob = new Blob([report], {
      type: format === 'json' ? 'application/json' :
            format === 'html' ? 'text/html' : 'text/markdown'
    });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `cryptex-scan-${scanId}.${format}`;
    a.click();
    URL.revokeObjectURL(url);
  }

  $: scan = $scanQuery.data;
  $: results = $resultsQuery.data || [];
  $: isRunning = scan?.status === 'running';
</script>

<div class="scan-view">
  <div class="scan-header">
    <button class="back-btn" on:click={() => window.history.back()}>
      <ChevronLeft size={20} />
      Back to Dashboard
    </button>

    <div class="scan-info">
      <h1>Scan: {scanId.slice(0, 8)}</h1>
      <span class="status" class:running={isRunning} class:completed={scan?.status === 'completed'}>
        {scan?.status || 'Loading...'}
      </span>
    </div>
  </div>

  {#if $scanQuery.isLoading}
    <div class="loading">Loading scan details...</div>
  {:else if $scanQuery.isError}
    <div class="error">
      <AlertTriangle size={24} />
      <p>Failed to load scan: {$scanQuery.error.message}</p>
    </div>
  {:else if scan}
    <div class="scan-stats">
      <div class="stat">
        <span class="label">Target</span>
        <span class="value">{scan.target}</span>
      </div>
      <div class="stat">
        <span class="label">Total Vulnerabilities</span>
        <span class="value">{scan.total_vulnerabilities}</span>
      </div>
      <div class="stat critical">
        <span class="label">Critical</span>
        <span class="value">{scan.critical}</span>
      </div>
      <div class="stat high">
        <span class="label">High</span>
        <span class="value">{scan.high}</span>
      </div>
      <div class="stat medium">
        <span class="label">Medium</span>
        <span class="value">{scan.medium}</span>
      </div>
      <div class="stat low">
        <span class="label">Low</span>
        <span class="value">{scan.low}</span>
      </div>
    </div>

    {#if scan.status === 'completed'}
      <div class="tabs">
        <button
          class="tab"
          class:active={activeTab === 'vulnerabilities'}
          on:click={() => activeTab = 'vulnerabilities'}
        >
          Vulnerabilities
        </button>
        <button
          class="tab"
          class:active={activeTab === 'report'}
          on:click={() => activeTab = 'report'}
        >
          Report
        </button>

        <div class="tab-actions">
          <select bind:value={reportFormat}>
            <option value="html">HTML</option>
            <option value="json">JSON</option>
            <option value="markdown">Markdown</option>
          </select>
          <button class="download-btn" on:click={() => downloadReport(reportFormat)}>
            <Download size={16} />
            Download Report
          </button>
        </div>
      </div>

      <div class="tab-content">
        {#if activeTab === 'vulnerabilities'}
          <VulnerabilityList vulnerabilities={results} />
        {:else}
          <ReportView {scanId} format={reportFormat} />
        {/if}
      </div>
    {:else if isRunning}
      <div class="running-indicator">
        <div class="spinner"></div>
        <p>Scan in progress... This page will auto-update.</p>
      </div>
    {/if}
  {/if}
</div>

<style>
  .scan-view {
    padding: 2rem;
  }

  .scan-header {
    margin-bottom: 2rem;
  }

  .back-btn {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    padding: 0.5rem 1rem;
    background: var(--bg-secondary);
    border: 1px solid var(--border);
    border-radius: 6px;
    color: var(--text-primary);
    cursor: pointer;
    transition: all 0.2s;
    margin-bottom: 1rem;
  }

  .back-btn:hover {
    background: var(--bg-tertiary);
    border-color: var(--accent);
  }

  .scan-info {
    display: flex;
    align-items: center;
    gap: 1rem;
  }

  .scan-info h1 {
    font-size: 1.5rem;
    color: var(--text-primary);
    margin: 0;
  }

  .status {
    padding: 0.25rem 0.75rem;
    border-radius: 4px;
    font-size: 0.875rem;
    text-transform: uppercase;
    font-weight: 600;
  }

  .status.running {
    background: var(--warning);
    color: var(--bg-primary);
  }

  .status.completed {
    background: var(--success);
    color: var(--bg-primary);
  }

  .scan-stats {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
    gap: 1rem;
    margin-bottom: 2rem;
  }

  .stat {
    background: var(--bg-secondary);
    border: 1px solid var(--border);
    border-radius: 8px;
    padding: 1rem;
  }

  .stat.critical {
    border-color: var(--critical);
  }

  .stat.high {
    border-color: var(--high);
  }

  .stat.medium {
    border-color: var(--medium);
  }

  .stat.low {
    border-color: var(--low);
  }

  .stat .label {
    display: block;
    font-size: 0.875rem;
    color: var(--text-secondary);
    margin-bottom: 0.5rem;
  }

  .stat .value {
    display: block;
    font-size: 1.5rem;
    font-weight: 600;
    color: var(--text-primary);
  }

  .tabs {
    display: flex;
    gap: 0.5rem;
    border-bottom: 2px solid var(--border);
    margin-bottom: 2rem;
    position: relative;
  }

  .tab {
    padding: 0.75rem 1.5rem;
    background: transparent;
    border: none;
    color: var(--text-secondary);
    cursor: pointer;
    border-bottom: 2px solid transparent;
    margin-bottom: -2px;
    transition: all 0.2s;
  }

  .tab:hover {
    color: var(--text-primary);
  }

  .tab.active {
    color: var(--accent);
    border-bottom-color: var(--accent);
  }

  .tab-actions {
    margin-left: auto;
    display: flex;
    gap: 0.5rem;
    align-items: center;
  }

  .tab-actions select {
    padding: 0.5rem;
    background: var(--bg-secondary);
    border: 1px solid var(--border);
    border-radius: 4px;
    color: var(--text-primary);
  }

  .download-btn {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    padding: 0.5rem 1rem;
    background: var(--accent);
    border: none;
    border-radius: 4px;
    color: var(--bg-primary);
    cursor: pointer;
    transition: all 0.2s;
  }

  .download-btn:hover {
    opacity: 0.9;
  }

  .loading, .error {
    text-align: center;
    padding: 3rem;
    color: var(--text-secondary);
  }

  .error {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 1rem;
    color: var(--critical);
  }

  .running-indicator {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 1rem;
    padding: 3rem;
  }

  .spinner {
    width: 40px;
    height: 40px;
    border: 3px solid var(--border);
    border-top-color: var(--accent);
    border-radius: 50%;
    animation: spin 1s linear infinite;
  }

  @keyframes spin {
    to { transform: rotate(360deg); }
  }
</style>
