<script lang="ts">
  import { onMount } from 'svelte';
  import { invoke } from '@tauri-apps/api/tauri';
  import {
    Shield,
    AlertTriangle,
    Activity,
    TrendingUp,
    Database,
    Clock,
    Target,
    FileText
  } from 'lucide-svelte';

  let stats = {
    totalScans: 0,
    totalVulnerabilities: 0,
    critical: 0,
    high: 0,
    medium: 0,
    low: 0,
    kevCount: 0
  };

  let recentScans = [];
  let loading = true;

  onMount(async () => {
    await loadDashboardData();
  });

  async function loadDashboardData() {
    try {
      const archiveStats = await invoke('get_archive_stats');
      stats = {
        totalScans: archiveStats.total_scans || 0,
        totalVulnerabilities: archiveStats.total_vulnerabilities || 0,
        critical: archiveStats.critical || 0,
        high: archiveStats.high || 0,
        medium: archiveStats.medium || 0,
        low: archiveStats.low || 0,
        kevCount: archiveStats.kev_count || 0
      };
      loading = false;
    } catch (e) {
      console.error('Failed to load dashboard data:', e);
      loading = false;
    }
  }

  async function startQuickScan() {
    // Open scan dialog
    console.log('Start quick scan');
  }
</script>

<div class="dashboard">
  <div class="page-header">
    <h1>Dashboard</h1>
    <p class="subtitle">Vulnerability Assessment Overview</p>
  </div>

  <!-- Statistics Cards -->
  <div class="stats-grid">
    <div class="stat-card primary">
      <div class="stat-icon">
        <Shield size={32} />
      </div>
      <div class="stat-content">
        <h3 class="stat-value">{stats.totalScans}</h3>
        <p class="stat-label">Total Scans</p>
      </div>
      <div class="stat-trend">
        <TrendingUp size={16} />
        <span>+12%</span>
      </div>
    </div>

    <div class="stat-card critical">
      <div class="stat-icon">
        <AlertTriangle size={32} />
      </div>
      <div class="stat-content">
        <h3 class="stat-value">{stats.critical}</h3>
        <p class="stat-label">Critical</p>
      </div>
      <div class="severity-bar">
        <div class="severity-fill critical" style="width: {stats.critical > 0 ? 100 : 0}%"></div>
      </div>
    </div>

    <div class="stat-card high">
      <div class="stat-icon">
        <AlertTriangle size={32} />
      </div>
      <div class="stat-content">
        <h3 class="stat-value">{stats.high}</h3>
        <p class="stat-label">High</p>
      </div>
      <div class="severity-bar">
        <div class="severity-fill high" style="width: {stats.high > 0 ? 80 : 0}%"></div>
      </div>
    </div>

    <div class="stat-card medium">
      <div class="stat-icon">
        <Activity size={32} />
      </div>
      <div class="stat-content">
        <h3 class="stat-value">{stats.medium}</h3>
        <p class="stat-label">Medium</p>
      </div>
      <div class="severity-bar">
        <div class="severity-fill medium" style="width: {stats.medium > 0 ? 60 : 0}%"></div>
      </div>
    </div>

    <div class="stat-card low">
      <div class="stat-icon">
        <Activity size={32} />
      </div>
      <div class="stat-content">
        <h3 class="stat-value">{stats.low}</h3>
        <p class="stat-label">Low</p>
      </div>
      <div class="severity-bar">
        <div class="severity-fill low" style="width: {stats.low > 0 ? 40 : 0}%"></div>
      </div>
    </div>

    <div class="stat-card warning">
      <div class="stat-icon">
        <Shield size={32} />
      </div>
      <div class="stat-content">
        <h3 class="stat-value">{stats.kevCount}</h3>
        <p class="stat-label">KEV Catalog</p>
      </div>
      <div class="stat-badge">CISA</div>
    </div>
  </div>

  <!-- Quick Actions Panel -->
  <div class="panels-row">
    <div class="panel">
      <div class="panel-header">
        <h2>Quick Scan</h2>
      </div>
      <div class="panel-content">
        <p class="panel-description">Start a quick vulnerability scan on a target</p>
        <div class="input-group">
          <input
            type="text"
            placeholder="Enter target IP or hostname"
            class="target-input"
          />
          <button class="btn btn-primary" on:click={startQuickScan}>
            <Target size={18} />
            <span>Start Scan</span>
          </button>
        </div>
      </div>
    </div>

    <div class="panel">
      <div class="panel-header">
        <h2>Recent Activity</h2>
      </div>
      <div class="panel-content">
        <div class="activity-list">
          <div class="activity-item">
            <Clock size={16} />
            <span class="activity-time">2 hours ago</span>
            <span class="activity-desc">Scan completed on 192.168.1.0/24</span>
          </div>
          <div class="activity-item">
            <Clock size={16} />
            <span class="activity-time">5 hours ago</span>
            <span class="activity-desc">Report generated: Q4-2024</span>
          </div>
          <div class="activity-item">
            <Clock size={16} />
            <span class="activity-time">1 day ago</span>
            <span class="activity-desc">New target added: Production Servers</span>
          </div>
        </div>
      </div>
    </div>
  </div>

  <!-- Vulnerability Distribution Chart -->
  <div class="panel chart-panel">
    <div class="panel-header">
      <h2>Vulnerability Distribution</h2>
      <div class="chart-legend">
        <div class="legend-item">
          <span class="legend-dot critical"></span>
          <span>Critical ({stats.critical})</span>
        </div>
        <div class="legend-item">
          <span class="legend-dot high"></span>
          <span>High ({stats.high})</span>
        </div>
        <div class="legend-item">
          <span class="legend-dot medium"></span>
          <span>Medium ({stats.medium})</span>
        </div>
        <div class="legend-item">
          <span class="legend-dot low"></span>
          <span>Low ({stats.low})</span>
        </div>
      </div>
    </div>
    <div class="panel-content">
      <div class="chart-container">
        <div class="donut-chart">
          <!-- SVG chart will go here -->
          <svg viewBox="0 0 200 200">
            <circle cx="100" cy="100" r="80" fill="none" stroke="var(--critical)" stroke-width="20"
                    stroke-dasharray="{stats.critical / stats.totalVulnerabilities * 502} 502" />
          </svg>
          <div class="chart-center">
            <div class="chart-value">{stats.totalVulnerabilities}</div>
            <div class="chart-label">Total Vulnerabilities</div>
          </div>
        </div>
      </div>
    </div>
  </div>

  <!-- System Status -->
  <div class="panel">
    <div class="panel-header">
      <h2>System Status</h2>
    </div>
    <div class="panel-content">
      <div class="status-grid">
        <div class="status-item">
          <Database size={20} />
          <span>Database</span>
          <span class="status-indicator online">Online</span>
        </div>
        <div class="status-item">
          <Activity size={20} />
          <span>API Server</span>
          <span class="status-indicator online">Running</span>
        </div>
        <div class="status-item">
          <FileText size={20} />
          <span>Node-RED</span>
          <span class="status-indicator online">Active</span>
        </div>
        <div class="status-item">
          <Shield size={20} />
          <span>PYRO Platform</span>
          <span class="status-indicator online">Connected</span>
        </div>
      </div>
    </div>
  </div>
</div>

<style>
  .dashboard {
    max-width: 1600px;
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
    font-size: 1rem;
  }

  .stats-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(240px, 1fr));
    gap: 1.5rem;
    margin-bottom: 2rem;
  }

  .stat-card {
    background: var(--bg-secondary);
    border: 1px solid var(--border-color);
    border-radius: 8px;
    padding: 1.5rem;
    position: relative;
    overflow: hidden;
  }

  .stat-card::before {
    content: '';
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    height: 3px;
  }

  .stat-card.primary::before { background: var(--gradient-fire); }
  .stat-card.critical::before { background: var(--critical-color); }
  .stat-card.high::before { background: var(--high-color); }
  .stat-card.medium::before { background: var(--medium-color); }
  .stat-card.low::before { background: var(--low-color); }
  .stat-card.warning::before { background: var(--warning-color); }

  .stat-icon {
    color: var(--text-muted);
    margin-bottom: 1rem;
  }

  .stat-content h3 {
    margin: 0;
    font-size: 2rem;
    font-weight: 700;
    color: var(--text-primary);
  }

  .stat-content p {
    margin: 0.25rem 0 0 0;
    font-size: 0.875rem;
    color: var(--text-secondary);
    text-transform: uppercase;
    letter-spacing: 0.5px;
  }

  .stat-trend {
    display: flex;
    align-items: center;
    gap: 0.25rem;
    margin-top: 0.5rem;
    font-size: 0.875rem;
    color: var(--primary-color);
  }

  .severity-bar {
    height: 4px;
    background: var(--bg-tertiary);
    border-radius: 2px;
    margin-top: 1rem;
    overflow: hidden;
  }

  .severity-fill {
    height: 100%;
    transition: width 0.5s ease;
  }

  .severity-fill.critical { background: var(--critical-color); }
  .severity-fill.high { background: var(--high-color); }
  .severity-fill.medium { background: var(--medium-color); }
  .severity-fill.low { background: var(--low-color); }

  .stat-badge {
    position: absolute;
    top: 1rem;
    right: 1rem;
    background: var(--warning-color);
    color: var(--text-inverse);
    font-size: 0.75rem;
    padding: 0.25rem 0.5rem;
    border-radius: 4px;
    font-weight: 700;
  }

  .panels-row {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(400px, 1fr));
    gap: 1.5rem;
    margin-bottom: 2rem;
  }

  .panel {
    background: var(--bg-secondary);
    border: 1px solid var(--border-color);
    border-radius: 8px;
    overflow: hidden;
  }

  .panel-header {
    padding: 1rem 1.5rem;
    border-bottom: 1px solid var(--border-color);
    display: flex;
    align-items: center;
    justify-content: space-between;
  }

  .panel-header h2 {
    margin: 0;
    font-size: 1.125rem;
    color: var(--text-primary);
  }

  .panel-content {
    padding: 1.5rem;
  }

  .panel-description {
    margin: 0 0 1rem 0;
    color: var(--text-secondary);
    font-size: 0.875rem;
  }

  .input-group {
    display: flex;
    gap: 0.75rem;
  }

  .target-input {
    flex: 1;
    padding: 0.75rem;
    background: var(--bg-tertiary);
    border: 1px solid var(--border-color);
    border-radius: 4px;
    color: var(--text-primary);
    font-size: 0.875rem;
  }

  .target-input:focus {
    outline: none;
    border-color: var(--primary-color);
    box-shadow: var(--shadow-fire);
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
    transition: all var(--transition-normal);
  }

  .btn-primary {
    background: var(--gradient-fire);
    color: var(--text-inverse);
    box-shadow: var(--shadow-fire);
  }

  .btn-primary:hover {
    background: var(--gradient-ember);
    box-shadow: var(--shadow-md);
    transform: translateY(-1px);
  }

  .activity-list {
    display: flex;
    flex-direction: column;
    gap: 0.75rem;
  }

  .activity-item {
    display: flex;
    align-items: center;
    gap: 0.75rem;
    padding: 0.75rem;
    background: var(--bg-tertiary);
    border-radius: 4px;
    font-size: 0.875rem;
  }

  .activity-time {
    color: var(--text-muted);
    min-width: 80px;
  }

  .activity-desc {
    color: var(--text-secondary);
  }

  .chart-legend {
    display: flex;
    gap: 1rem;
  }

  .legend-item {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    font-size: 0.875rem;
    color: var(--text-secondary);
  }

  .legend-dot {
    width: 12px;
    height: 12px;
    border-radius: 50%;
  }

  .legend-dot.critical { background: var(--critical-color); }
  .legend-dot.high { background: var(--high-color); }
  .legend-dot.medium { background: var(--medium-color); }
  .legend-dot.low { background: var(--low-color); }

  .chart-container {
    display: flex;
    justify-content: center;
    padding: 2rem 0;
  }

  .donut-chart {
    position: relative;
    width: 200px;
    height: 200px;
  }

  .chart-center {
    position: absolute;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%);
    text-align: center;
  }

  .chart-value {
    font-size: 2.5rem;
    font-weight: 700;
    background: var(--gradient-fire);
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
    background-clip: text;
  }

  .chart-label {
    font-size: 0.875rem;
    color: var(--text-muted);
  }

  .status-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
    gap: 1rem;
  }

  .status-item {
    display: flex;
    align-items: center;
    gap: 0.75rem;
    padding: 1rem;
    background: var(--bg-tertiary);
    border-radius: 4px;
    color: var(--text-secondary);
    transition: all var(--transition-fast);
  }

  .status-item:hover {
    background: var(--bg-hover);
    box-shadow: var(--shadow-sm);
  }

  .status-item span:nth-child(2) {
    flex: 1;
  }

  .status-indicator {
    font-size: 0.75rem;
    padding: 0.25rem 0.75rem;
    border-radius: 12px;
    font-weight: 600;
  }

  .status-indicator.online {
    background: rgba(255, 107, 53, 0.15);
    color: var(--primary-color);
  }
</style>
