/**
 * CRYPTEX API Client
 *
 * TypeScript client for CRYPTEX REST API
 */

import type { Vulnerability, Scan, ScanResult, ArchiveStats } from '../types';

export class CryptexAPI {
  private baseUrl: string;

  constructor(baseUrl: string = 'http://localhost:8080') {
    this.baseUrl = baseUrl;
  }

  private async fetch<T>(endpoint: string, options?: RequestInit): Promise<T> {
    const response = await fetch(`${this.baseUrl}${endpoint}`, {
      ...options,
      headers: {
        'Content-Type': 'application/json',
        ...options?.headers,
      },
    });

    if (!response.ok) {
      const error = await response.json().catch(() => ({ message: response.statusText }));
      throw new Error(error.message || `HTTP ${response.status}`);
    }

    return response.json();
  }

  // Health check
  async healthCheck(): Promise<{ status: string; version: string }> {
    return this.fetch('/health');
  }

  // Vulnerability assessment
  async assessVulnerability(cveId: string): Promise<Vulnerability> {
    return this.fetch(`/api/v1/vulnerabilities/${cveId}`);
  }

  // Scan management
  async startScan(target: string): Promise<{ scan_id: string; target: string; started_at: string }> {
    return this.fetch('/api/v1/scans', {
      method: 'POST',
      body: JSON.stringify({ target }),
    });
  }

  async listScans(): Promise<Scan[]> {
    return this.fetch('/api/v1/scans');
  }

  async getScan(scanId: string): Promise<Scan> {
    return this.fetch(`/api/v1/scans/${scanId}`);
  }

  async endScan(scanId: string): Promise<any> {
    return this.fetch(`/api/v1/scans/${scanId}/end`, {
      method: 'POST',
    });
  }

  async getScanResults(scanId: string): Promise<ScanResult[]> {
    return this.fetch(`/api/v1/scans/${scanId}/results`);
  }

  // Reports
  async generateReport(scanId: string, format: 'json' | 'html' | 'markdown' | 'text' = 'json'): Promise<string> {
    const response = await fetch(`${this.baseUrl}/api/v1/scans/${scanId}/report?format=${format}`);
    return response.text();
  }

  async getExecutiveSummary(scanId: string): Promise<string> {
    const response = await fetch(`${this.baseUrl}/api/v1/scans/${scanId}/executive-summary`);
    return response.text();
  }

  // Archive statistics
  async getArchiveStats(): Promise<ArchiveStats> {
    return this.fetch('/api/v1/archive/stats');
  }
}
