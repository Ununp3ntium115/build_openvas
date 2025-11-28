/**
 * CRYPTEX N-API TypeScript Definitions
 *
 * High-performance Rust bindings for CRYPTEX vulnerability assessment platform.
 */

/**
 * Query filters for vulnerability searches
 */
export interface JsQueryFilters {
  /** Filter by severity (e.g., "critical", "high", "medium", "low") */
  severity?: string;
  /** Filter by KEV status */
  is_kev?: boolean;
  /** Minimum CVSS score (0-10) */
  min_cvss?: number;
  /** Maximum CVSS score (0-10) */
  max_cvss?: number;
  /** Filter vulnerabilities cached after this date (ISO 8601) */
  cached_after?: string;
  /** Filter vulnerabilities cached before this date (ISO 8601) */
  cached_before?: string;
  /** Sort by field */
  sort_by?: 'cve_id' | 'severity' | 'cvss_score' | 'cached_at';
  /** Sort order */
  sort_order?: 'asc' | 'desc';
  /** Maximum number of results */
  limit?: number;
  /** Number of results to skip */
  offset?: number;
}

/**
 * Query filters for scan searches
 */
export interface JsScanFilters {
  /** Filter by target */
  target?: string;
  /** Filter by scan status */
  status?: string;
  /** Minimum number of vulnerabilities */
  min_vulnerabilities?: number;
  /** Filter scans started after this date (ISO 8601) */
  started_after?: string;
  /** Filter scans started before this date (ISO 8601) */
  started_before?: string;
  /** Maximum number of results */
  limit?: number;
  /** Number of results to skip */
  offset?: number;
}

/**
 * Vulnerability result
 */
export interface JsVulnerability {
  /** CVE identifier */
  cve_id: string;
  /** Severity level */
  severity: string;
  /** CVSS base score */
  cvss_score: number;
  /** Known Exploited Vulnerability status */
  is_kev: boolean;
  /** Timestamp when cached (ISO 8601) */
  cached_at: string;
  /** Vulnerability description */
  description?: string;
}

/**
 * Scan metadata result
 */
export interface JsScanMetadata {
  /** Scan identifier */
  scan_id: string;
  /** Scan target */
  target: string;
  /** Scan start time (ISO 8601) */
  started_at: string;
  /** Scan end time (ISO 8601) */
  ended_at?: string;
  /** Scan status */
  status: string;
  /** Total vulnerabilities found */
  total_vulnerabilities: number;
  /** Critical severity count */
  critical: number;
  /** High severity count */
  high: number;
  /** Medium severity count */
  medium: number;
  /** Low severity count */
  low: number;
}

/**
 * CVE assessment result
 */
export interface JsAssessment {
  /** CVE identifier */
  cve_id: string;
  /** Severity level */
  severity: string;
  /** CVSS base score */
  cvss_score: number;
  /** Known Exploited Vulnerability status */
  is_kev: boolean;
  /** Composite risk score (0-1) */
  composite_risk_score: number;
  /** Vulnerability description */
  description?: string;
  /** Vulnerability name */
  vulnerability_name?: string;
  /** Publication date */
  published_date?: string;
  /** Last modified date */
  last_modified?: string;
}

/**
 * Scan result
 */
export interface JsScanResult {
  /** Target host */
  host: string;
  /** Target port */
  port: number;
  /** CVE identifier */
  cve_id: string;
  /** Service name */
  service?: string;
  /** Detection confidence */
  confidence: string;
}

/**
 * Report format enumeration
 */
export enum ReportFormat {
  Html = 'Html',
  Pdf = 'Pdf',
  Json = 'Json',
  Markdown = 'Markdown'
}

/**
 * Archive query interface for direct redb access
 */
export class CryptexArchiveQuery {
  /**
   * Create new archive query instance
   * @param dbPath Path to redb database file
   */
  constructor(dbPath: string);

  /**
   * Query vulnerabilities with filters
   * @param filters Query filters
   * @returns Promise resolving to array of vulnerabilities
   */
  queryVulnerabilities(filters: JsQueryFilters): Promise<JsVulnerability[]>;

  /**
   * Query scans with filters
   * @param filters Scan filters
   * @returns Promise resolving to array of scan metadata
   */
  queryScans(filters: JsScanFilters): Promise<JsScanMetadata[]>;

  /**
   * Export vulnerabilities as JSON
   * @param filters Query filters
   * @returns Promise resolving to JSON string
   */
  exportJson(filters: JsQueryFilters): Promise<string>;

  /**
   * Export vulnerabilities as CSV
   * @param filters Query filters
   * @returns Promise resolving to CSV string
   */
  exportCsv(filters: JsQueryFilters): Promise<string>;

  /**
   * Count vulnerabilities matching filters
   * @param filters Query filters
   * @returns Promise resolving to count
   */
  countVulnerabilities(filters: JsQueryFilters): Promise<number>;

  /**
   * Count scans matching filters
   * @param filters Scan filters
   * @returns Promise resolving to count
   */
  countScans(filters: JsScanFilters): Promise<number>;
}

/**
 * CVE assessment and scoring interface
 */
export class CryptexAssessor {
  /**
   * Create new assessor instance
   * @returns Promise resolving to assessor instance
   */
  static create(): Promise<CryptexAssessor>;

  /**
   * Assess a vulnerability by CVE ID
   * @param cveId CVE identifier
   * @returns Promise resolving to assessment result
   */
  assessVulnerability(cveId: string): Promise<JsAssessment>;

  /**
   * Batch assess multiple vulnerabilities
   * @param cveIds Array of CVE identifiers
   * @returns Promise resolving to array of assessments
   */
  assessBatch(cveIds: string[]): Promise<JsAssessment[]>;
}

/**
 * Vulnerability scanning interface
 */
export class CryptexInfiltrator {
  /**
   * Create new infiltrator instance
   * @returns Promise resolving to infiltrator instance
   */
  static create(): Promise<CryptexInfiltrator>;

  /**
   * Start a vulnerability scan
   * @param target Scan target (IP, hostname, or CIDR)
   * @returns Promise resolving to scan ID
   */
  scan(target: string): Promise<string>;

  /**
   * Get scan status
   * @param scanId Scan identifier
   * @returns Promise resolving to scan status
   */
  getScanStatus(scanId: string): Promise<string>;
}

/**
 * Report generation interface
 */
export class CryptexPropagandist {
  /**
   * Create new propagandist instance
   * @returns Promise resolving to propagandist instance
   */
  static create(): Promise<CryptexPropagandist>;

  /**
   * Generate a vulnerability report
   * @param scanId Scan identifier
   * @param format Report format
   * @returns Promise resolving to report content
   */
  generateReport(scanId: string, format: ReportFormat): Promise<string>;
}

/**
 * Get CRYPTEX version
 * @returns Version string
 */
export function getVersion(): string;

/**
 * Initialize CRYPTEX logging
 * @param level Log level (e.g., "info", "debug", "trace")
 */
export function initLogging(level?: string): void;
