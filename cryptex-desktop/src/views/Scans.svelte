<script lang="ts">
  import { invoke } from '@tauri-apps/api/tauri';
  import { Play, Pause, Trash2, Eye, MoreVertical } from 'lucide-svelte';

  let scans = [
    {
      id: '1',
      name: 'Production Network Scan',
      target: '192.168.1.0/24',
      status: 'Completed',
      progress: 100,
      vulns: { critical: 5, high: 12, medium: 24, low: 18 },
      date: '2024-01-20 14:30',
    },
    {
      id: '2',
      name: 'Web Servers Assessment',
      target: 'web-servers.local',
      status: 'Running',
      progress: 65,
      vulns: { critical: 2, high: 8, medium: 15, low: 10 },
      date: '2024-01-20 16:00',
    },
    {
      id: '3',
      name: 'Database Servers',
      target: '10.0.1.0/24',
      status: 'Queued',
      progress: 0,
      vulns: { critical: 0, high: 0, medium: 0, low: 0 },
      date: '2024-01-20 18:00',
    },
  ];

  async function startScan(target: string) {
    try {
      const result = await invoke('start_scan', { target });
      console.log('Scan started:', result);
    } catch (e) {
      console.error('Failed to start scan:', e);
    }
  }

  function getStatusColor(status: string) {
    const colors = {
      'Completed': 'var(--gsa-green)',
      'Running': 'var(--info)',
      'Queued': 'var(--warning)',
      'Failed': 'var(--error)'
    };
    return colors[status] || 'var(--text-secondary)';
  }
</script>

<div class="scans-view">
  <div class="page-header">
    <div>
      <h1>Scans</h1>
      <p class="subtitle">Manage vulnerability scans</p>
    </div>
    <button class="btn btn-primary">
      <Play size={18} />
      <span>New Scan</span>
    </button>
  </div>

  <div class="scans-table">
    <table>
      <thead>
        <tr>
          <th>Scan Name</th>
          <th>Target</th>
          <th>Status</th>
          <th>Progress</th>
          <th>Vulnerabilities</th>
          <th>Date</th>
          <th>Actions</th>
        </tr>
      </thead>
      <tbody>
        {#each scans as scan}
          <tr>
            <td class="scan-name">{scan.name}</td>
            <td class="target">{scan.target}</td>
            <td>
              <span class="status-badge" style="color: {getStatusColor(scan.status)}">
                {scan.status}
              </span>
            </td>
            <td>
              <div class="progress-bar">
                <div class="progress-fill" style="width: {scan.progress}%"></div>
                <span class="progress-text">{scan.progress}%</span>
              </div>
            </td>
            <td>
              <div class="vulns-summary">
                {#if scan.vulns.critical > 0}
                  <span class="vuln-badge critical">{scan.vulns.critical}</span>
                {/if}
                {#if scan.vulns.high > 0}
                  <span class="vuln-badge high">{scan.vulns.high}</span>
                {/if}
                {#if scan.vulns.medium > 0}
                  <span class="vuln-badge medium">{scan.vulns.medium}</span>
                {/if}
                {#if scan.vulns.low > 0}
                  <span class="vuln-badge low">{scan.vulns.low}</span>
                {/if}
              </div>
            </td>
            <td class="date">{scan.date}</td>
            <td>
              <div class="actions">
                <button class="icon-btn" title="View Results">
                  <Eye size={16} />
                </button>
                <button class="icon-btn" title="Stop Scan">
                  <Pause size={16} />
                </button>
                <button class="icon-btn" title="Delete">
                  <Trash2 size={16} />
                </button>
                <button class="icon-btn" title="More">
                  <MoreVertical size={16} />
                </button>
              </div>
            </td>
          </tr>
        {/each}
      </tbody>
    </table>
  </div>
</div>

<style>
  .scans-view {
    max-width: 1600px;
    margin: 0 auto;
  }

  .page-header {
    display: flex;
    align-items: center;
    justify-content: space-between;
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

  .btn {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    padding: 0.75rem 1.5rem;
    border: none;
    border-radius: 4px;
    font-size: 0.875rem;
    font-weight: 600;
    cursor: pointer;
    transition: all 0.2s;
  }

  .btn-primary {
    background: var(--gsa-green);
    color: var(--bg-primary);
  }

  .btn-primary:hover {
    background: var(--gsa-green-dark);
  }

  .scans-table {
    background: var(--bg-secondary);
    border: 1px solid var(--border-color);
    border-radius: 8px;
    overflow: hidden;
  }

  table {
    width: 100%;
    border-collapse: collapse;
  }

  thead {
    background: var(--bg-tertiary);
  }

  th {
    padding: 1rem 1.5rem;
    text-align: left;
    color: var(--text-secondary);
    font-weight: 600;
    font-size: 0.875rem;
    text-transform: uppercase;
    letter-spacing: 0.5px;
    border-bottom: 1px solid var(--border-color);
  }

  td {
    padding: 1rem 1.5rem;
    border-bottom: 1px solid var(--border-color);
    color: var(--text-primary);
  }

  tr:hover {
    background: var(--bg-hover);
  }

  .scan-name {
    font-weight: 600;
    color: var(--gsa-green);
  }

  .target {
    font-family: monospace;
    color: var(--text-secondary);
  }

  .status-badge {
    display: inline-block;
    padding: 0.25rem 0.75rem;
    border-radius: 12px;
    font-size: 0.75rem;
    font-weight: 600;
    background: rgba(255, 255, 255, 0.1);
  }

  .progress-bar {
    position: relative;
    width: 120px;
    height: 20px;
    background: var(--bg-tertiary);
    border-radius: 10px;
    overflow: hidden;
  }

  .progress-fill {
    position: absolute;
    top: 0;
    left: 0;
    height: 100%;
    background: var(--gsa-green);
    transition: width 0.3s ease;
  }

  .progress-text {
    position: absolute;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%);
    font-size: 0.75rem;
    font-weight: 600;
    color: var(--text-primary);
  }

  .vulns-summary {
    display: flex;
    gap: 0.5rem;
  }

  .vuln-badge {
    display: inline-block;
    padding: 0.25rem 0.5rem;
    border-radius: 4px;
    font-size: 0.75rem;
    font-weight: 600;
    min-width: 24px;
    text-align: center;
  }

  .vuln-badge.critical {
    background: var(--critical);
    color: white;
  }

  .vuln-badge.high {
    background: var(--high);
    color: white;
  }

  .vuln-badge.medium {
    background: var(--medium);
    color: var(--bg-primary);
  }

  .vuln-badge.low {
    background: var(--low);
    color: var(--bg-primary);
  }

  .date {
    font-size: 0.875rem;
    color: var(--text-secondary);
  }

  .actions {
    display: flex;
    gap: 0.5rem;
  }

  .icon-btn {
    background: transparent;
    border: none;
    color: var(--text-secondary);
    cursor: pointer;
    padding: 0.25rem;
    border-radius: 4px;
    transition: all 0.2s;
  }

  .icon-btn:hover {
    background: var(--bg-hover);
    color: var(--gsa-green);
  }
</style>
