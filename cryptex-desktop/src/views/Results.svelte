<script lang="ts">
  import { Search, Filter, AlertTriangle, AlertCircle, Info } from 'lucide-svelte';
  import { onMount } from 'svelte';
  import { invoke } from '@tauri-apps/api/tauri';

  interface Result {
    cve_id: string;
    severity: 'critical' | 'high' | 'medium' | 'low';
    score: number;
    title: string;
    description: string;
    host: string;
    port: string;
    detected_at: string;
  }

  let results: Result[] = [];
  let filteredResults: Result[] = [];
  let searchQuery = '';
  let severityFilter = 'all';
  let loading = false;
  let selectedResult: Result | null = null;

  onMount(() => {
    loadResults();
  });

  async function loadResults() {
    loading = true;
    try {
      // Try to get results from API
      const response = await fetch('http://localhost:8080/api/v1/archive/stats');
      if (response.ok) {
        // For now, show some example results
        results = generateExampleResults();
      } else {
        results = generateExampleResults();
      }
    } catch (error) {
      console.error('Failed to load results:', error);
      results = generateExampleResults();
    } finally {
      loading = false;
      applyFilters();
    }
  }

  function generateExampleResults(): Result[] {
    return [
      {
        cve_id: 'CVE-2024-1234',
        severity: 'critical',
        score: 9.8,
        title: 'Remote Code Execution in Apache Server',
        description: 'A critical vulnerability allowing remote attackers to execute arbitrary code',
        host: '192.168.1.10',
        port: '80/tcp',
        detected_at: new Date().toISOString()
      },
      {
        cve_id: 'CVE-2024-5678',
        severity: 'high',
        score: 8.1,
        title: 'SQL Injection in Database Interface',
        description: 'SQL injection vulnerability in user authentication module',
        host: '192.168.1.11',
        port: '3306/tcp',
        detected_at: new Date().toISOString()
      },
      {
        cve_id: 'CVE-2024-9012',
        severity: 'medium',
        score: 5.3,
        title: 'Cross-Site Scripting (XSS) Vulnerability',
        description: 'Reflected XSS in search parameter',
        host: '192.168.1.12',
        port: '443/tcp',
        detected_at: new Date().toISOString()
      },
      {
        cve_id: 'CVE-2024-3456',
        severity: 'low',
        score: 3.1,
        title: 'Information Disclosure',
        description: 'Server version information exposed in HTTP headers',
        host: '192.168.1.13',
        port: '80/tcp',
        detected_at: new Date().toISOString()
      }
    ];
  }

  function applyFilters() {
    filteredResults = results.filter(result => {
      const matchesSearch = searchQuery === '' ||
        result.cve_id.toLowerCase().includes(searchQuery.toLowerCase()) ||
        result.title.toLowerCase().includes(searchQuery.toLowerCase()) ||
        result.host.includes(searchQuery);

      const matchesSeverity = severityFilter === 'all' || result.severity === severityFilter;

      return matchesSearch && matchesSeverity;
    });
  }

  $: searchQuery, severityFilter, applyFilters();

  function getSeverityColor(severity: string): string {
    switch (severity) {
      case 'critical': return 'var(--critical-color)';
      case 'high': return 'var(--high-color)';
      case 'medium': return 'var(--medium-color)';
      case 'low': return 'var(--low-color)';
      default: return 'var(--text-secondary)';
    }
  }

  function getSeverityIcon(severity: string) {
    switch (severity) {
      case 'critical':
      case 'high':
        return AlertTriangle;
      case 'medium':
        return AlertCircle;
      default:
        return Info;
    }
  }
</script>

<div class="results-view">
  <div class="page-header">
    <div>
      <h1>Results</h1>
      <p class="subtitle">Vulnerability scan results and findings</p>
    </div>
  </div>

  <div class="filters">
    <div class="search-box">
      <Search size={18} />
      <input
        type="text"
        placeholder="Search CVE ID, title, or host..."
        bind:value={searchQuery}
      />
    </div>

    <div class="severity-filters">
      <button
        class="filter-btn {severityFilter === 'all' ? 'active' : ''}"
        on:click={() => severityFilter = 'all'}
      >
        All ({results.length})
      </button>
      <button
        class="filter-btn {severityFilter === 'critical' ? 'active' : ''}"
        on:click={() => severityFilter = 'critical'}
      >
        <span class="severity-dot critical"></span>
        Critical ({results.filter(r => r.severity === 'critical').length})
      </button>
      <button
        class="filter-btn {severityFilter === 'high' ? 'active' : ''}"
        on:click={() => severityFilter = 'high'}
      >
        <span class="severity-dot high"></span>
        High ({results.filter(r => r.severity === 'high').length})
      </button>
      <button
        class="filter-btn {severityFilter === 'medium' ? 'active' : ''}"
        on:click={() => severityFilter = 'medium'}
      >
        <span class="severity-dot medium"></span>
        Medium ({results.filter(r => r.severity === 'medium').length})
      </button>
      <button
        class="filter-btn {severityFilter === 'low' ? 'active' : ''}"
        on:click={() => severityFilter = 'low'}
      >
        <span class="severity-dot low"></span>
        Low ({results.filter(r => r.severity === 'low').length})
      </button>
    </div>
  </div>

  <div class="results-container">
    {#if loading}
      <div class="loading">Loading results...</div>
    {:else if filteredResults.length === 0}
      <div class="empty-state">
        <p>No results found</p>
        {#if searchQuery || severityFilter !== 'all'}
          <button class="btn btn-secondary" on:click={() => { searchQuery = ''; severityFilter = 'all'; }}>
            Clear Filters
          </button>
        {/if}
      </div>
    {:else}
      <div class="results-grid">
        {#each filteredResults as result}
          <div class="result-card" on:click={() => selectedResult = result}>
            <div class="result-header">
              <div class="result-cve">
                <svelte:component this={getSeverityIcon(result.severity)} size={18} color={getSeverityColor(result.severity)} />
                <strong>{result.cve_id}</strong>
              </div>
              <div class="result-score" style="color: {getSeverityColor(result.severity)}">
                {result.score.toFixed(1)}
              </div>
            </div>
            <h3 class="result-title">{result.title}</h3>
            <p class="result-description">{result.description}</p>
            <div class="result-meta">
              <span class="meta-item">
                <strong>Host:</strong> {result.host}
              </span>
              <span class="meta-item">
                <strong>Port:</strong> {result.port}
              </span>
            </div>
            <div class="result-footer">
              <span class="severity-badge {result.severity}">
                {result.severity.toUpperCase()}
              </span>
              <span class="timestamp">
                {new Date(result.detected_at).toLocaleString()}
              </span>
            </div>
          </div>
        {/each}
      </div>
    {/if}
  </div>

  {#if selectedResult}
    <div class="modal-overlay" on:click={() => selectedResult = null}>
      <div class="modal" on:click|stopPropagation>
        <div class="modal-header">
          <h2>{selectedResult.cve_id}</h2>
          <span class="severity-badge {selectedResult.severity}">
            {selectedResult.severity.toUpperCase()}
          </span>
        </div>
        <div class="modal-body">
          <div class="detail-section">
            <h3>Title</h3>
            <p>{selectedResult.title}</p>
          </div>
          <div class="detail-section">
            <h3>Description</h3>
            <p>{selectedResult.description}</p>
          </div>
          <div class="detail-grid">
            <div class="detail-item">
              <strong>CVSS Score</strong>
              <span style="color: {getSeverityColor(selectedResult.severity)}">{selectedResult.score.toFixed(1)}</span>
            </div>
            <div class="detail-item">
              <strong>Severity</strong>
              <span>{selectedResult.severity}</span>
            </div>
            <div class="detail-item">
              <strong>Host</strong>
              <span>{selectedResult.host}</span>
            </div>
            <div class="detail-item">
              <strong>Port</strong>
              <span>{selectedResult.port}</span>
            </div>
            <div class="detail-item">
              <strong>Detected</strong>
              <span>{new Date(selectedResult.detected_at).toLocaleString()}</span>
            </div>
          </div>
        </div>
        <div class="modal-footer">
          <button class="btn btn-secondary" on:click={() => selectedResult = null}>Close</button>
        </div>
      </div>
    </div>
  {/if}
</div>

<style>
  .results-view {
    padding: 2rem;
    max-width: 1400px;
    margin: 0 auto;
  }

  .page-header {
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

  .filters {
    margin-bottom: 2rem;
  }

  .search-box {
    display: flex;
    align-items: center;
    gap: 0.75rem;
    padding: 0.75rem 1rem;
    background: var(--bg-secondary);
    border: 1px solid var(--border-color);
    border-radius: 8px;
    margin-bottom: 1rem;
  }

  .search-box input {
    flex: 1;
    background: none;
    border: none;
    color: var(--text-primary);
    font-size: 1rem;
    outline: none;
  }

  .severity-filters {
    display: flex;
    gap: 0.5rem;
    flex-wrap: wrap;
  }

  .filter-btn {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    padding: 0.5rem 1rem;
    background: var(--bg-secondary);
    border: 1px solid var(--border-color);
    border-radius: 6px;
    color: var(--text-secondary);
    cursor: pointer;
    transition: all 0.2s;
  }

  .filter-btn:hover {
    background: var(--hover-bg);
    border-color: var(--primary-color);
  }

  .filter-btn.active {
    background: var(--primary-color);
    color: var(--text-primary);
    border-color: var(--primary-color);
  }

  .severity-dot {
    width: 8px;
    height: 8px;
    border-radius: 50%;
  }

  .severity-dot.critical { background: var(--critical-color); }
  .severity-dot.high { background: var(--high-color); }
  .severity-dot.medium { background: var(--medium-color); }
  .severity-dot.low { background: var(--low-color); }

  .results-container {
    min-height: 400px;
  }

  .loading, .empty-state {
    padding: 4rem 2rem;
    text-align: center;
    color: var(--text-muted);
  }

  .results-grid {
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(350px, 1fr));
    gap: 1.5rem;
  }

  .result-card {
    background: var(--bg-secondary);
    border: 1px solid var(--border-color);
    border-radius: 8px;
    padding: 1.5rem;
    cursor: pointer;
    transition: all 0.2s;
  }

  .result-card:hover {
    border-color: var(--primary-color);
    transform: translateY(-2px);
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.3);
  }

  .result-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 1rem;
  }

  .result-cve {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    color: var(--text-primary);
  }

  .result-score {
    font-size: 1.5rem;
    font-weight: 700;
  }

  .result-title {
    margin: 0 0 0.75rem 0;
    font-size: 1.125rem;
    color: var(--text-primary);
  }

  .result-description {
    margin: 0 0 1rem 0;
    color: var(--text-secondary);
    font-size: 0.875rem;
    line-height: 1.5;
    display: -webkit-box;
    -webkit-line-clamp: 2;
    -webkit-box-orient: vertical;
    overflow: hidden;
  }

  .result-meta {
    display: flex;
    gap: 1rem;
    margin-bottom: 1rem;
    font-size: 0.875rem;
  }

  .meta-item {
    color: var(--text-muted);
  }

  .meta-item strong {
    color: var(--text-secondary);
  }

  .result-footer {
    display: flex;
    justify-content: space-between;
    align-items: center;
  }

  .severity-badge {
    padding: 0.25rem 0.75rem;
    border-radius: 4px;
    font-size: 0.75rem;
    font-weight: 600;
  }

  .severity-badge.critical {
    background: rgba(220, 38, 38, 0.2);
    color: var(--critical-color);
  }

  .severity-badge.high {
    background: rgba(251, 146, 60, 0.2);
    color: var(--high-color);
  }

  .severity-badge.medium {
    background: rgba(250, 204, 21, 0.2);
    color: var(--medium-color);
  }

  .severity-badge.low {
    background: rgba(74, 222, 128, 0.2);
    color: var(--low-color);
  }

  .timestamp {
    color: var(--text-muted);
    font-size: 0.75rem;
  }

  /* Modal styles */
  .modal-overlay {
    position: fixed;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: rgba(0, 0, 0, 0.7);
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 1000;
  }

  .modal {
    background: var(--bg-secondary);
    border-radius: 8px;
    width: 90%;
    max-width: 700px;
    max-height: 90vh;
    overflow-y: auto;
    border: 1px solid var(--border-color);
  }

  .modal-header {
    padding: 1.5rem;
    border-bottom: 1px solid var(--border-color);
    display: flex;
    justify-content: space-between;
    align-items: center;
  }

  .modal-header h2 {
    margin: 0;
    font-size: 1.5rem;
    color: var(--text-primary);
  }

  .modal-body {
    padding: 1.5rem;
  }

  .modal-footer {
    padding: 1.5rem;
    border-top: 1px solid var(--border-color);
    display: flex;
    justify-content: flex-end;
  }

  .detail-section {
    margin-bottom: 1.5rem;
  }

  .detail-section h3 {
    margin: 0 0 0.5rem 0;
    color: var(--text-secondary);
    font-size: 0.875rem;
    text-transform: uppercase;
  }

  .detail-section p {
    margin: 0;
    color: var(--text-primary);
    line-height: 1.6;
  }

  .detail-grid {
    display: grid;
    grid-template-columns: repeat(2, 1fr);
    gap: 1rem;
  }

  .detail-item {
    display: flex;
    flex-direction: column;
    gap: 0.25rem;
  }

  .detail-item strong {
    color: var(--text-secondary);
    font-size: 0.875rem;
    text-transform: uppercase;
  }

  .detail-item span {
    color: var(--text-primary);
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

  .btn-secondary {
    background: var(--bg-tertiary);
    color: var(--text-secondary);
  }

  .btn-secondary:hover {
    background: var(--hover-bg);
  }
</style>
