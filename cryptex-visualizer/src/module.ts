/**
 * CRYPTEX Visualizer - Embeddable Module Entry
 *
 * This entry point exports CRYPTEX components for embedding
 * into PYRO or other parent applications.
 *
 * Usage in PYRO:
 * ```typescript
 * import { CryptexDashboard, CryptexScanView } from 'cryptex-visualizer';
 *
 * // Embed in PYRO UI
 * new CryptexDashboard({
 *   target: document.getElementById('cryptex-corner'),
 *   props: {
 *     mode: 'embedded',
 *     apiUrl: 'http://localhost:8080',
 *   }
 * });
 * ```
 */

// Export main components for embedding
export { default as CryptexDashboard } from './components/Dashboard.svelte'
export { default as CryptexScanView } from './components/ScanView.svelte'
export { default as CryptexVulnList } from './components/VulnerabilityList.svelte'
export { default as CryptexReportView } from './components/ReportView.svelte'

// Export stores for shared state
export { cryptexStore, cryptexConfig, cryptexState } from './stores/cryptex'
export { scansStore, scanStats, recentScans } from './stores/scans'
export { vulnerabilitiesStore, vulnerabilityStats, topRiskVulnerabilities } from './stores/vulnerabilities'

// Export services for direct API access
export { CryptexAPI } from './services/api'
export { CryptexWebSocket, getCryptexWebSocket, disconnectCryptexWebSocket } from './services/websocket'

// Export types
export type { Vulnerability, Scan, ScanResult, ArchiveStats } from './types'
