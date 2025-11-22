/**
 * CRYPTEX TypeScript Types
 */

export interface Vulnerability {
  cve_id: string;
  severity: 'Critical' | 'High' | 'Medium' | 'Low' | 'None';
  cvss_base_score: number;
  cvss_vector?: string;
  is_kev: boolean;
  kev_date_added?: string;
  epss_score?: number;
  epss_percentile?: number;
  composite_risk_score: number;
  ai_risk_score?: number;
  ai_priority?: string;
  remediation_urgency?: string;
  description?: string;
  ai_analysis?: string;
}

export interface Scan {
  scan_id: string;
  target: string;
  started_at: string;
  ended_at?: string;
  status: 'pending' | 'running' | 'completed' | 'failed';
  total_vulnerabilities: number;
  critical: number;
  high: number;
  medium: number;
  low: number;
}

export interface ScanResult {
  cve_id: string;
  host: string;
  port: number;
  plugin_oid: string;
  description: string;
  vulnerability_score?: Vulnerability;
  remediation_guidance?: string;
  detection_time: number;
  scanner_version: string;
}

export interface Report {
  scan_id: string;
  target: string;
  generated_at: string;
  summary: {
    total_vulnerabilities: number;
    critical: number;
    high: number;
    medium: number;
    low: number;
    kev_vulnerabilities: number;
  };
  findings: Array<{
    cve_id: string;
    severity: string;
    affected_hosts: string[];
    cvss_score: number;
    is_kev: boolean;
    remediation: string;
  }>;
}

export interface ArchiveStats {
  total_scans: number;
  total_vulnerabilities: number;
  total_results: number;
}
