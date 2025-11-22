<script lang="ts">
  import { FileText, Download, Eye } from 'lucide-svelte';
  import { onMount } from 'svelte';
  import { invoke } from '@tauri-apps/api/tauri';

  interface Report {
    id: string;
    scan_id: string;
    scan_name: string;
    format: 'html' | 'markdown' | 'json' | 'text';
    generated_at: string;
    size: string;
  }

  let reports: Report[] = [];
  let selectedScan = '';
  let selectedFormat: 'html' | 'markdown' | 'json' | 'text' = 'html';
  let generating = false;

  const scans = [
    { id: '1', name: 'Production Network Scan' },
    { id: '2', name: 'Web Server Scan' },
    { id: '3', name: 'Database Server Scan' }
  ];

  onMount(() => {
    loadReports();
  });

  function loadReports() {
    // Load from localStorage
    const stored = localStorage.getItem('cryptex_reports');
    if (stored) {
      reports = JSON.parse(stored);
    } else {
      reports = [
        {
          id: '1',
          scan_id: '1',
          scan_name: 'Production Network Scan',
          format: 'html',
          generated_at: new Date().toISOString(),
          size: '245 KB'
        },
        {
          id: '2',
          scan_id: '2',
          scan_name: 'Web Server Scan',
          format: 'markdown',
          generated_at: new Date().toISOString(),
          size: '128 KB'
        }
      ];
      saveReports();
    }
  }

  function saveReports() {
    localStorage.setItem('cryptex_reports', JSON.stringify(reports));
  }

  async function generateReport() {
    if (!selectedScan) return;

    generating = true;
    try {
      // Simulate report generation
      await new Promise(resolve => setTimeout(resolve, 1500));

      const scan = scans.find(s => s.id === selectedScan);
      const report: Report = {
        id: Date.now().toString(),
        scan_id: selectedScan,
        scan_name: scan?.name || 'Unknown Scan',
        format: selectedFormat,
        generated_at: new Date().toISOString(),
        size: Math.floor(Math.random() * 500 + 50) + ' KB'
      };

      reports = [report, ...reports];
      saveReports();
      selectedScan = '';
    } catch (error) {
      console.error('Failed to generate report:', error);
    } finally {
      generating = false;
    }
  }

  function getFormatIcon(format: string): string {
    switch (format) {
      case 'html': return '📄';
      case 'markdown': return '📝';
      case 'json': return '📊';
      case 'text': return '📃';
      default: return '📄';
    }
  }

  function getFormatColor(format: string): string {
    switch (format) {
      case 'html': return '#e34c26';
      case 'markdown': return '#083fa1';
      case 'json': return '#f7df1e';
      case 'text': return '#6c757d';
      default: return 'var(--text-secondary)';
    }
  }

  async function downloadReport(report: Report) {
    // In a real implementation, this would download the report file
    console.log('Downloading report:', report.id);
    alert(`Downloading ${report.scan_name} (${report.format.toUpperCase()})`);
  }

  async function viewReport(report: Report) {
    // In a real implementation, this would open the report
    console.log('Viewing report:', report.id);
    alert(`Viewing ${report.scan_name} (${report.format.toUpperCase()})`);
  }
</script>

<div class="reports-view">
  <div class="page-header">
    <div>
      <h1>Reports</h1>
      <p class="subtitle">Generate and manage vulnerability reports</p>
    </div>
  </div>

  <div class="report-generator">
    <div class="generator-header">
      <h2>Generate New Report</h2>
    </div>
    <div class="generator-body">
      <div class="form-row">
        <div class="form-group">
          <label for="scan">Select Scan</label>
          <select id="scan" bind:value={selectedScan}>
            <option value="">Choose a scan...</option>
            {#each scans as scan}
              <option value={scan.id}>{scan.name}</option>
            {/each}
          </select>
        </div>
        <div class="form-group">
          <label for="format">Report Format</label>
          <select id="format" bind:value={selectedFormat}>
            <option value="html">HTML</option>
            <option value="markdown">Markdown</option>
            <option value="json">JSON</option>
            <option value="text">Plain Text</option>
          </select>
        </div>
        <div class="form-group">
          <label>&nbsp;</label>
          <button
            class="btn btn-primary"
            on:click={generateReport}
            disabled={!selectedScan || generating}
          >
            {#if generating}
              Generating...
            {:else}
              <FileText size={18} />
              <span>Generate Report</span>
            {/if}
          </button>
        </div>
      </div>
    </div>
  </div>

  <div class="reports-list">
    <div class="list-header">
      <h2>Report History</h2>
      <span class="report-count">{reports.length} reports</span>
    </div>

    {#if reports.length === 0}
      <div class="empty-state">
        <p>No reports generated yet</p>
        <p class="hint">Select a scan above to generate your first report</p>
      </div>
    {:else}
      <div class="reports-grid">
        {#each reports as report}
          <div class="report-card">
            <div class="report-icon" style="color: {getFormatColor(report.format)}">
              {getFormatIcon(report.format)}
            </div>
            <div class="report-info">
              <h3 class="report-name">{report.scan_name}</h3>
              <div class="report-meta">
                <span class="format-badge" style="background: {getFormatColor(report.format)}20; color: {getFormatColor(report.format)}">
                  {report.format.toUpperCase()}
                </span>
                <span class="report-size">{report.size}</span>
                <span class="report-date">
                  {new Date(report.generated_at).toLocaleDateString()}
                </span>
              </div>
            </div>
            <div class="report-actions">
              <button class="icon-btn" on:click={() => viewReport(report)} title="View Report">
                <Eye size={18} />
              </button>
              <button class="icon-btn" on:click={() => downloadReport(report)} title="Download">
                <Download size={18} />
              </button>
            </div>
          </div>
        {/each}
      </div>
    {/if}
  </div>
</div>

<style>
  .reports-view {
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

  .report-generator {
    background: var(--bg-secondary);
    border: 1px solid var(--border-color);
    border-radius: 8px;
    margin-bottom: 2rem;
  }

  .generator-header {
    padding: 1.5rem;
    border-bottom: 1px solid var(--border-color);
  }

  .generator-header h2 {
    margin: 0;
    font-size: 1.25rem;
    color: var(--text-primary);
  }

  .generator-body {
    padding: 1.5rem;
  }

  .form-row {
    display: grid;
    grid-template-columns: 1fr 1fr auto;
    gap: 1rem;
    align-items: end;
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

  .form-group select {
    padding: 0.75rem;
    background: var(--bg-tertiary);
    border: 1px solid var(--border-color);
    border-radius: 6px;
    color: var(--text-primary);
    font-family: inherit;
    cursor: pointer;
  }

  .btn {
    display: inline-flex;
    align-items: center;
    justify-content: center;
    gap: 0.5rem;
    padding: 0.75rem 1.5rem;
    border: none;
    border-radius: 6px;
    font-weight: 500;
    cursor: pointer;
    transition: all 0.2s;
    white-space: nowrap;
  }

  .btn-primary {
    background: var(--primary-color);
    color: var(--text-primary);
  }

  .btn-primary:hover:not(:disabled) {
    background: var(--primary-hover);
  }

  .btn-primary:disabled {
    opacity: 0.5;
    cursor: not-allowed;
  }

  .reports-list {
    background: var(--bg-secondary);
    border: 1px solid var(--border-color);
    border-radius: 8px;
  }

  .list-header {
    padding: 1.5rem;
    border-bottom: 1px solid var(--border-color);
    display: flex;
    justify-content: space-between;
    align-items: center;
  }

  .list-header h2 {
    margin: 0;
    font-size: 1.25rem;
    color: var(--text-primary);
  }

  .report-count {
    color: var(--text-muted);
    font-size: 0.875rem;
  }

  .empty-state {
    padding: 4rem 2rem;
    text-align: center;
    color: var(--text-muted);
  }

  .empty-state .hint {
    margin-top: 0.5rem;
    font-size: 0.875rem;
  }

  .reports-grid {
    padding: 1.5rem;
    display: flex;
    flex-direction: column;
    gap: 1rem;
  }

  .report-card {
    display: flex;
    align-items: center;
    gap: 1.5rem;
    padding: 1.5rem;
    background: var(--bg-tertiary);
    border: 1px solid var(--border-color);
    border-radius: 8px;
    transition: all 0.2s;
  }

  .report-card:hover {
    border-color: var(--primary-color);
    transform: translateX(4px);
  }

  .report-icon {
    font-size: 2.5rem;
    line-height: 1;
  }

  .report-info {
    flex: 1;
  }

  .report-name {
    margin: 0 0 0.5rem 0;
    font-size: 1.125rem;
    color: var(--text-primary);
  }

  .report-meta {
    display: flex;
    gap: 1rem;
    align-items: center;
    flex-wrap: wrap;
  }

  .format-badge {
    padding: 0.25rem 0.75rem;
    border-radius: 4px;
    font-size: 0.75rem;
    font-weight: 600;
  }

  .report-size,
  .report-date {
    color: var(--text-muted);
    font-size: 0.875rem;
  }

  .report-actions {
    display: flex;
    gap: 0.5rem;
  }

  .icon-btn {
    background: none;
    border: none;
    padding: 0.75rem;
    cursor: pointer;
    color: var(--text-secondary);
    border-radius: 6px;
    transition: all 0.2s;
    display: flex;
    align-items: center;
    justify-content: center;
  }

  .icon-btn:hover {
    background: var(--hover-bg);
    color: var(--primary-color);
  }

  @media (max-width: 768px) {
    .form-row {
      grid-template-columns: 1fr;
    }

    .report-card {
      flex-direction: column;
      align-items: flex-start;
    }

    .report-actions {
      width: 100%;
      justify-content: flex-end;
    }
  }
</style>
