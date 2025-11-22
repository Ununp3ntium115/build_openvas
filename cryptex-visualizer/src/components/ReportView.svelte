<script lang="ts">
  import { createQuery } from '@tanstack/svelte-query';
  import { CryptexAPI } from '@services/api';
  import { FileText, Code, FileJson } from 'lucide-svelte';

  export let scanId: string;
  export let format: 'json' | 'html' | 'markdown' = 'html';

  const api = new CryptexAPI();

  $: reportQuery = createQuery({
    queryKey: ['report', scanId, format],
    queryFn: () => api.generateReport(scanId, format),
  });

  $: report = $reportQuery.data;
</script>

<div class="report-view">
  <div class="report-header">
    <div class="format-info">
      {#if format === 'html'}
        <FileText size={20} />
        <span>HTML Report</span>
      {:else if format === 'json'}
        <FileJson size={20} />
        <span>JSON Report</span>
      {:else}
        <Code size={20} />
        <span>Markdown Report</span>
      {/if}
    </div>
  </div>

  {#if $reportQuery.isLoading}
    <div class="loading">
      <div class="spinner"></div>
      <p>Generating {format.toUpperCase()} report...</p>
    </div>
  {:else if $reportQuery.isError}
    <div class="error">
      <p>Failed to generate report: {$reportQuery.error.message}</p>
    </div>
  {:else if report}
    <div class="report-content">
      {#if format === 'html'}
        <div class="html-report">
          {@html report}
        </div>
      {:else if format === 'json'}
        <pre class="json-report"><code>{JSON.stringify(JSON.parse(report), null, 2)}</code></pre>
      {:else}
        <div class="markdown-report">
          <pre>{report}</pre>
        </div>
      {/if}
    </div>
  {/if}
</div>

<style>
  .report-view {
    background: var(--bg-secondary);
    border: 1px solid var(--border);
    border-radius: 8px;
    overflow: hidden;
  }

  .report-header {
    padding: 1rem 1.5rem;
    background: var(--bg-tertiary);
    border-bottom: 1px solid var(--border);
  }

  .format-info {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    color: var(--text-primary);
    font-weight: 600;
  }

  .loading, .error {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    padding: 3rem;
    gap: 1rem;
  }

  .loading {
    color: var(--text-secondary);
  }

  .error {
    color: var(--critical);
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

  .report-content {
    padding: 1.5rem;
    max-height: 70vh;
    overflow-y: auto;
  }

  .html-report {
    background: white;
    padding: 2rem;
    border-radius: 4px;
    color: #000;
  }

  .html-report :global(h1) {
    color: #000;
    font-size: 2rem;
    margin-bottom: 1rem;
  }

  .html-report :global(h2) {
    color: #333;
    font-size: 1.5rem;
    margin-top: 2rem;
    margin-bottom: 1rem;
  }

  .html-report :global(h3) {
    color: #666;
    font-size: 1.25rem;
    margin-top: 1.5rem;
    margin-bottom: 0.5rem;
  }

  .html-report :global(table) {
    width: 100%;
    border-collapse: collapse;
    margin: 1rem 0;
  }

  .html-report :global(th),
  .html-report :global(td) {
    padding: 0.75rem;
    border: 1px solid #ddd;
    text-align: left;
  }

  .html-report :global(th) {
    background: #f5f5f5;
    font-weight: 600;
  }

  .json-report {
    background: var(--bg-primary);
    padding: 1.5rem;
    border-radius: 4px;
    overflow-x: auto;
  }

  .json-report code {
    color: var(--text-primary);
    font-family: 'Fira Code', 'Consolas', monospace;
    font-size: 0.875rem;
    line-height: 1.6;
  }

  .markdown-report {
    background: var(--bg-primary);
    padding: 1.5rem;
    border-radius: 4px;
  }

  .markdown-report pre {
    color: var(--text-primary);
    font-family: 'Fira Code', 'Consolas', monospace;
    font-size: 0.875rem;
    line-height: 1.6;
    white-space: pre-wrap;
    word-wrap: break-word;
  }
</style>
