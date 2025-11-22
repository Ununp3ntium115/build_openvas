import { writable, derived } from 'svelte/store';
import type { Vulnerability } from '@types';

export interface VulnerabilitiesStore {
  vulnerabilities: Map<string, Vulnerability>;
  loading: boolean;
  error: string | null;
}

function createVulnerabilitiesStore() {
  const { subscribe, set, update } = writable<VulnerabilitiesStore>({
    vulnerabilities: new Map(),
    loading: false,
    error: null,
  });

  return {
    subscribe,
    set,
    update,

    // Add or update a vulnerability
    setVulnerability: (vuln: Vulnerability) => {
      update(current => {
        const vulnerabilities = new Map(current.vulnerabilities);
        vulnerabilities.set(vuln.cve_id, vuln);
        return {
          ...current,
          vulnerabilities,
          error: null,
        };
      });
    },

    // Add or update multiple vulnerabilities
    setVulnerabilities: (vulns: Vulnerability[]) => {
      update(current => {
        const vulnerabilities = new Map(current.vulnerabilities);
        vulns.forEach(v => vulnerabilities.set(v.cve_id, v));
        return {
          ...current,
          vulnerabilities,
          loading: false,
          error: null,
        };
      });
    },

    // Get a vulnerability by CVE ID
    getVulnerability: (cveId: string): Vulnerability | undefined => {
      let result: Vulnerability | undefined;
      subscribe(current => {
        result = current.vulnerabilities.get(cveId);
      })();
      return result;
    },

    // Remove a vulnerability
    removeVulnerability: (cveId: string) => {
      update(current => {
        const vulnerabilities = new Map(current.vulnerabilities);
        vulnerabilities.delete(cveId);
        return {
          ...current,
          vulnerabilities,
        };
      });
    },

    // Set loading state
    setLoading: (loading: boolean) => {
      update(current => ({ ...current, loading }));
    },

    // Set error
    setError: (error: string | null) => {
      update(current => ({ ...current, error, loading: false }));
    },

    // Clear all vulnerabilities
    clear: () => {
      set({
        vulnerabilities: new Map(),
        loading: false,
        error: null,
      });
    },
  };
}

export const vulnerabilitiesStore = createVulnerabilitiesStore();

// Derived store for vulnerability array
export const vulnerabilitiesArray = derived(
  vulnerabilitiesStore,
  $store => Array.from($store.vulnerabilities.values())
);

// Derived store for KEV vulnerabilities
export const kevVulnerabilities = derived(
  vulnerabilitiesArray,
  $vulns => $vulns.filter(v => v.is_kev)
);

// Derived store for critical vulnerabilities
export const criticalVulnerabilities = derived(
  vulnerabilitiesArray,
  $vulns => $vulns.filter(v => v.severity === 'Critical')
);

// Derived store for high severity vulnerabilities
export const highVulnerabilities = derived(
  vulnerabilitiesArray,
  $vulns => $vulns.filter(v => v.severity === 'High')
);

// Derived store for vulnerability stats
export const vulnerabilityStats = derived(
  vulnerabilitiesArray,
  $vulns => {
    const total = $vulns.length;
    const critical = $vulns.filter(v => v.severity === 'Critical').length;
    const high = $vulns.filter(v => v.severity === 'High').length;
    const medium = $vulns.filter(v => v.severity === 'Medium').length;
    const low = $vulns.filter(v => v.severity === 'Low').length;
    const kev = $vulns.filter(v => v.is_kev).length;

    const avgCvss = total > 0
      ? $vulns.reduce((acc, v) => acc + v.cvss_base_score, 0) / total
      : 0;

    const avgRisk = total > 0
      ? $vulns.reduce((acc, v) => acc + v.composite_risk_score, 0) / total
      : 0;

    return {
      total,
      critical,
      high,
      medium,
      low,
      kev,
      avgCvss,
      avgRisk,
    };
  }
);

// Derived store for sorted vulnerabilities by risk score
export const vulnerabilitiesByRisk = derived(
  vulnerabilitiesArray,
  $vulns => [...$vulns].sort((a, b) => b.composite_risk_score - a.composite_risk_score)
);

// Derived store for top 10 highest risk vulnerabilities
export const topRiskVulnerabilities = derived(
  vulnerabilitiesByRisk,
  $vulns => $vulns.slice(0, 10)
);
