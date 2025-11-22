import { writable, derived } from 'svelte/store';
import type { Scan } from '@types';

export interface ScansStore {
  scans: Scan[];
  activeScans: Scan[];
  loading: boolean;
  error: string | null;
}

function createScansStore() {
  const { subscribe, set, update } = writable<ScansStore>({
    scans: [],
    activeScans: [],
    loading: false,
    error: null,
  });

  return {
    subscribe,
    set,
    update,

    // Set all scans
    setScans: (scans: Scan[]) => {
      update(current => ({
        ...current,
        scans,
        activeScans: scans.filter(s => s.status === 'running' || s.status === 'pending'),
        loading: false,
        error: null,
      }));
    },

    // Add a new scan
    addScan: (scan: Scan) => {
      update(current => ({
        ...current,
        scans: [scan, ...current.scans],
        activeScans: scan.status === 'running' || scan.status === 'pending'
          ? [scan, ...current.activeScans]
          : current.activeScans,
      }));
    },

    // Update an existing scan
    updateScan: (scanId: string, updates: Partial<Scan>) => {
      update(current => {
        const scans = current.scans.map(s =>
          s.scan_id === scanId ? { ...s, ...updates } : s
        );
        return {
          ...current,
          scans,
          activeScans: scans.filter(s => s.status === 'running' || s.status === 'pending'),
        };
      });
    },

    // Remove a scan
    removeScan: (scanId: string) => {
      update(current => ({
        ...current,
        scans: current.scans.filter(s => s.scan_id !== scanId),
        activeScans: current.activeScans.filter(s => s.scan_id !== scanId),
      }));
    },

    // Set loading state
    setLoading: (loading: boolean) => {
      update(current => ({ ...current, loading }));
    },

    // Set error
    setError: (error: string | null) => {
      update(current => ({ ...current, error, loading: false }));
    },

    // Clear all scans
    clear: () => {
      set({
        scans: [],
        activeScans: [],
        loading: false,
        error: null,
      });
    },
  };
}

export const scansStore = createScansStore();

// Derived stores for common queries
export const activeScanCount = derived(
  scansStore,
  $store => $store.activeScans.length
);

export const completedScans = derived(
  scansStore,
  $store => $store.scans.filter(s => s.status === 'completed')
);

export const failedScans = derived(
  scansStore,
  $store => $store.scans.filter(s => s.status === 'failed')
);

export const recentScans = derived(
  scansStore,
  $store => $store.scans.slice(0, 10)
);

// Stats derived store
export const scanStats = derived(
  scansStore,
  $store => {
    const total = $store.scans.length;
    const active = $store.activeScans.length;
    const completed = $store.scans.filter(s => s.status === 'completed').length;
    const failed = $store.scans.filter(s => s.status === 'failed').length;

    const totalVulns = $store.scans.reduce((acc, s) => acc + s.total_vulnerabilities, 0);
    const critical = $store.scans.reduce((acc, s) => acc + s.critical, 0);
    const high = $store.scans.reduce((acc, s) => acc + s.high, 0);
    const medium = $store.scans.reduce((acc, s) => acc + s.medium, 0);
    const low = $store.scans.reduce((acc, s) => acc + s.low, 0);

    return {
      total,
      active,
      completed,
      failed,
      totalVulnerabilities: totalVulns,
      critical,
      high,
      medium,
      low,
    };
  }
);
