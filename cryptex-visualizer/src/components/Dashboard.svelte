<script lang="ts">
  /**
   * CRYPTEX Dashboard - Main Overview Component
   *
   * Shows summary statistics, recent scans, and top vulnerabilities
   */

  import { createQuery } from '@tanstack/svelte-query';
  import { Link } from 'svelte-routing';
  import { CryptexAPI } from '../services/api';
  import { AlertTriangle, Shield, TrendingUp, Activity } from 'lucide-svelte';

  const api = new CryptexAPI();

  // Fetch archive stats
  const statsQuery = createQuery({
    queryKey: ['stats'],
    queryFn: () => api.getArchiveStats(),
  });

  // Fetch recent scans
  const scansQuery = createQuery({
    queryKey: ['scans'],
    queryFn: () => api.listScans(),
  });
</script>

<div class="dashboard">
  <!-- Header -->
  <div class="dashboard-header">
    <h1>CRYPTEX Dashboard</h1>
    <p class="tagline">"No gods, no masters, no memory leaks."</p>
  </div>

  <!-- Stats Grid -->
  <div class="stats-grid">
    {#if $statsQuery.isSuccess}
      <div class="stat-card">
        <div class="stat-icon">
          <Activity />
        </div>
        <div class="stat-content">
          <div class="stat-value">{$statsQuery.data.total_scans}</div>
          <div class="stat-label">Total Scans</div>
        </div>
      </div>

      <div class="stat-card">
        <div class="stat-icon critical">
          <AlertTriangle />
        </div>
        <div class="stat-content">
          <div class="stat-value">{$statsQuery.data.total_vulnerabilities}</div>
          <div class="stat-label">Vulnerabilities</div>
        </div>
      </div>

      <div class="stat-card">
        <div class="stat-icon">
          <Shield />
        </div>
        <div class="stat-content">
          <div class="stat-value">{$statsQuery.data.total_results}</div>
          <div class="stat-label">Scan Results</div>
        </div>
      </div>

      <div class="stat-card">
        <div class="stat-icon">
          <TrendingUp />
        </div>
        <div class="stat-content">
          <div class="stat-value">Active</div>
          <div class="stat-label">System Status</div>
        </div>
      </div>
    {/if}
  </div>

  <!-- Recent Scans -->
  <div class="section">
    <h2>Recent Scans</h2>

    {#if $scansQuery.isLoading}
      <div class="loading">Loading scans...</div>
    {:else if $scansQuery.isError}
      <div class="error">Error loading scans</div>
    {:else if $scansQuery.data}
      <div class="scans-list">
        {#each $scansQuery.data.slice(0, 5) as scan}
          <Link to="/scans/{scan.scan_id}">
            <div class="scan-card">
              <div class="scan-header">
                <span class="scan-target">{scan.target}</span>
                <span class="scan-status" class:completed={scan.status === 'completed'}>
                  {scan.status}
                </span>
              </div>
              <div class="scan-stats">
                <span class="stat critical">{scan.critical} Critical</span>
                <span class="stat high">{scan.high} High</span>
                <span class="stat medium">{scan.medium} Medium</span>
                <span class="stat low">{scan.low} Low</span>
              </div>
              <div class="scan-time">
                {new Date(scan.started_at).toLocaleString()}
              </div>
            </div>
          </Link>
        {/each}
      </div>
    {/if}
  </div>

  <!-- Quick Actions -->
  <div class="section">
    <h2>Quick Actions</h2>
    <div class="actions">
      <button class="action-btn primary">
        <span>Start New Scan</span>
      </button>
      <button class="action-btn">
        <span>View All Vulnerabilities</span>
      </button>
      <button class="action-btn">
        <span>Generate Report</span>
      </button>
    </div>
  </div>
</div>

<style>
  .dashboard {
    max-width: 1400px;
    margin: 0 auto;
  }

  .dashboard-header {
    margin-bottom: 2rem;
  }

  .dashboard-header h1 {
    font-size: 2.5rem;
    font-weight: 700;
    margin: 0;
    background: linear-gradient(135deg, var(--accent-red), var(--accent-purple));
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
    background-clip: text;
  }

  .tagline {
    color: var(--text-secondary);
    font-style: italic;
    margin-top: 0.5rem;
  }

  .stats-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
    gap: 1.5rem;
    margin-bottom: 3rem;
  }

  .stat-card {
    background: var(--bg-secondary);
    border: 1px solid var(--border-color);
    border-radius: var(--border-radius);
    padding: 1.5rem;
    display: flex;
    gap: 1rem;
    align-items: center;
  }

  .stat-icon {
    width: 48px;
    height: 48px;
    border-radius: 50%;
    background: var(--bg-tertiary);
    display: flex;
    align-items: center;
    justify-content: center;
    color: var(--accent-blue);
  }

  .stat-icon.critical {
    color: var(--critical);
  }

  .stat-value {
    font-size: 2rem;
    font-weight: 700;
    color: var(--text-primary);
  }

  .stat-label {
    color: var(--text-secondary);
    font-size: 0.875rem;
  }

  .section {
    margin-bottom: 3rem;
  }

  .section h2 {
    font-size: 1.5rem;
    margin-bottom: 1rem;
    color: var(--text-primary);
  }

  .scans-list {
    display: grid;
    gap: 1rem;
  }

  .scan-card {
    background: var(--bg-secondary);
    border: 1px solid var(--border-color);
    border-radius: var(--border-radius);
    padding: 1.5rem;
    transition: all 0.2s;
    cursor: pointer;
  }

  .scan-card:hover {
    border-color: var(--accent-blue);
    transform: translateY(-2px);
  }

  .scan-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 1rem;
  }

  .scan-target {
    font-weight: 600;
    color: var(--text-primary);
  }

  .scan-status {
    padding: 0.25rem 0.75rem;
    border-radius: 1rem;
    font-size: 0.75rem;
    background: var(--bg-tertiary);
    color: var(--text-secondary);
  }

  .scan-status.completed {
    background: var(--accent-green);
    color: white;
  }

  .scan-stats {
    display: flex;
    gap: 1rem;
    margin-bottom: 0.75rem;
  }

  .scan-stats .stat {
    font-size: 0.875rem;
    font-weight: 500;
  }

  .scan-stats .stat.critical {
    color: var(--critical);
  }

  .scan-stats .stat.high {
    color: var(--high);
  }

  .scan-stats .stat.medium {
    color: var(--medium);
  }

  .scan-stats .stat.low {
    color: var(--low);
  }

  .scan-time {
    font-size: 0.75rem;
    color: var(--text-tertiary);
  }

  .actions {
    display: flex;
    gap: 1rem;
    flex-wrap: wrap;
  }

  .action-btn {
    padding: 0.75rem 1.5rem;
    border-radius: var(--border-radius);
    border: 1px solid var(--border-color);
    background: var(--bg-secondary);
    color: var(--text-primary);
    font-weight: 500;
    cursor: pointer;
    transition: all 0.2s;
  }

  .action-btn:hover {
    background: var(--bg-tertiary);
    border-color: var(--accent-blue);
  }

  .action-btn.primary {
    background: var(--accent-blue);
    border-color: var(--accent-blue);
    color: white;
  }

  .action-btn.primary:hover {
    background: #2980b9;
  }

  .loading,
  .error {
    padding: 2rem;
    text-align: center;
    color: var(--text-secondary);
  }
</style>
