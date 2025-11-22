# CRYPTEX Visualizer - The Interface

Interactive Svelte frontend for CRYPTEX vulnerability assessment platform. Supports both standalone deployment and embedding within PYRO Platform.

## Features

- **Real-time Vulnerability Monitoring** - Live scan progress with WebSocket updates
- **Risk Analysis Dashboard** - Composite risk scoring and KEV tracking
- **Interactive Reports** - Generate HTML, JSON, and Markdown reports
- **Dark Anarchist Theme** - High-contrast UI optimized for security work
- **Dual-Mode Architecture** - Standalone app + embeddable module

## Architecture

```
cryptex-visualizer/
├── src/
│   ├── components/          # Svelte UI components
│   │   ├── Dashboard.svelte        # Main dashboard
│   │   ├── ScanView.svelte         # Scan details view
│   │   ├── VulnerabilityList.svelte # Vulnerability grid
│   │   ├── ReportView.svelte       # Report viewer
│   │   └── Header.svelte           # App header
│   ├── stores/              # Svelte stores for state
│   │   ├── cryptex.ts      # Global config
│   │   ├── scans.ts        # Scan state
│   │   └── vulnerabilities.ts # Vuln state
│   ├── services/            # API & WebSocket
│   │   ├── api.ts          # REST API client
│   │   └── websocket.ts    # WebSocket service
│   ├── types/               # TypeScript types
│   ├── main.ts             # Standalone entry point
│   ├── module.ts           # Embeddable module exports
│   ├── App.svelte          # Root component
│   └── app.css             # Global styles
├── public/                  # Static assets
├── vite.config.ts          # Vite build configuration
├── tailwind.config.js      # Tailwind CSS config
├── tsconfig.json           # TypeScript config
├── package.json            # NPM dependencies
├── Dockerfile              # Container build
└── nginx.conf              # Nginx configuration

## Quick Start

### Development

```bash
# Install dependencies
npm install

# Start dev server
npm run dev

# Access at http://localhost:5173
```

### Production Build

```bash
# Build both standalone app and embeddable library
npm run build

# Preview production build
npm run preview
```

### Docker

```bash
# Build container
docker build -t cryptex-visualizer .

# Run container
docker run -p 5173:80 \
  -e VITE_API_URL=http://cryptex-api:8080 \
  cryptex-visualizer
```

## Usage Modes

### Standalone Mode

Full-featured application with header, navigation, and routing.

```typescript
// src/main.ts
import App from './App.svelte';
import './app.css';

const app = new App({
  target: document.getElementById('app')!,
  props: {
    mode: 'standalone',
    apiUrl: 'http://localhost:8080',
  }
});
```

### Embedded Mode (PYRO Integration)

Embeddable components for integration into PYRO or other applications.

```typescript
// In PYRO frontend
import { CryptexDashboard } from 'cryptex-visualizer';

new CryptexDashboard({
  target: document.getElementById('cryptex-corner'),
  props: {
    mode: 'embedded',
    apiUrl: 'http://cryptex-api:8080',
  }
});
```

## Components

### Dashboard

Main overview with stats, recent scans, and quick actions.

```svelte
<Dashboard />
```

**Features**:
- Archive statistics
- Recent scans list
- Quick scan initiation
- Vulnerability counters

### ScanView

Detailed view of individual scan results.

```svelte
<ScanView scanId={scan_id} />
```

**Features**:
- Scan metadata and status
- Real-time progress updates
- Vulnerability list with filters
- Report generation and download

### VulnerabilityList

Sortable, filterable vulnerability grid.

```svelte
<VulnerabilityList vulnerabilities={results} />
```

**Features**:
- Sort by severity, CVSS, risk score, CVE ID
- Filter by severity and KEV status
- Interactive severity badges
- CVE links to NVD

### ReportView

Multi-format report viewer.

```svelte
<ReportView scanId={scan_id} format="html" />
```

**Formats**:
- HTML (styled report)
- JSON (machine-readable)
- Markdown (documentation-ready)

## State Management

### Stores

**cryptexStore** - Global configuration
```typescript
import { cryptexStore } from '@stores/cryptex';

cryptexStore.setConfig({
  mode: 'standalone',
  apiUrl: 'http://localhost:8080',
});
```

**scansStore** - Scan state management
```typescript
import { scansStore, scanStats } from '@stores/scans';

// Add scan
scansStore.addScan(scan);

// Update scan
scansStore.updateScan(scanId, { status: 'completed' });

// Derived stats
$scanStats.total; // Total scans
$scanStats.critical; // Critical vulnerabilities
```

**vulnerabilitiesStore** - Vulnerability state
```typescript
import { vulnerabilitiesStore, topRiskVulnerabilities } from '@stores/vulnerabilities';

// Set vulnerabilities
vulnerabilitiesStore.setVulnerabilities(vulns);

// Get top risks
$topRiskVulnerabilities; // Top 10 by risk score
```

## Services

### API Client

TypeScript client for CRYPTEX REST API.

```typescript
import { CryptexAPI } from '@services/api';

const api = new CryptexAPI('http://localhost:8080');

// Assess vulnerability
const vuln = await api.assessVulnerability('CVE-2024-1234');

// Start scan
const { scan_id } = await api.startScan('192.168.1.0/24');

// Get results
const results = await api.getScanResults(scan_id);
```

**Methods**:
- `assessVulnerability(cveId)` - Assess single CVE
- `startScan(target)` - Initiate scan
- `getScan(scanId)` - Get scan status
- `endScan(scanId)` - Complete scan
- `getScanResults(scanId)` - Fetch results
- `listScans()` - List all scans
- `getArchiveStats()` - Archive statistics
- `generateReport(scanId, format)` - Generate report

### WebSocket Service

Real-time updates via WebSocket connection.

```typescript
import { getCryptexWebSocket } from '@services/websocket';

const ws = getCryptexWebSocket();

// Listen for scan updates
ws.on('scan_progress', (data) => {
  console.log('Scan progress:', data);
});

// Listen for vulnerabilities
ws.on('vulnerability_found', (data) => {
  console.log('New vulnerability:', data);
});
```

**Message Types**:
- `scan_started` - Scan initiated
- `scan_progress` - Progress update
- `scan_completed` - Scan finished
- `scan_failed` - Scan error
- `vulnerability_found` - New vulnerability detected

## Styling

### Theme Variables

```css
:root {
  /* Background */
  --bg-primary: #0a0a0a;
  --bg-secondary: #111111;
  --bg-tertiary: #1a1a1a;

  /* Text */
  --text-primary: #e0e0e0;
  --text-secondary: #a0a0a0;

  /* Accent */
  --accent: #00ff00;

  /* Severity */
  --critical: #ff0033;
  --high: #ff6600;
  --medium: #ffaa00;
  --low: #ffff00;
}
```

### Customization

Override CSS variables in `app.css` or component styles:

```css
/* Custom theme */
:root {
  --accent: #00ffff; /* Cyan accent */
  --bg-primary: #000000; /* Pure black */
}
```

## Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `VITE_API_URL` | `http://localhost:8080` | CRYPTEX API endpoint |

Set in `.env`:

```env
VITE_API_URL=http://cryptex-api:8080
```

## Building

### Dual Build Output

Vite configuration builds both standalone app and embeddable library:

```typescript
// vite.config.ts
export default defineConfig({
  build: {
    lib: {
      entry: {
        app: 'src/main.ts',      // Standalone
        module: 'src/module.ts',  // Embeddable
      },
      name: 'CryptexVisualizer',
      formats: ['es', 'umd'],
    },
  },
});
```

**Outputs**:
- `dist/app.js` - Standalone application (ES module)
- `dist/module.js` - Embeddable library (ES + UMD)

### Production Optimizations

```bash
# Minify with Terser
npm run build -- --mode production

# Analyze bundle size
npx vite-bundle-visualizer
```

## Integration Examples

### PYRO Platform

```typescript
// PYRO UI integration
import {
  CryptexDashboard,
  CryptexScanView,
  cryptexStore,
  CryptexAPI
} from 'cryptex-visualizer';

// Configure
cryptexStore.setConfig({
  mode: 'embedded',
  apiUrl: PYRO_CONFIG.cryptex_api_url,
});

// Mount dashboard
new CryptexDashboard({
  target: document.querySelector('#cryptex-widget'),
});
```

### Custom Integration

```typescript
// Standalone integration
import { CryptexAPI, vulnerabilitiesStore } from 'cryptex-visualizer';

const api = new CryptexAPI('http://localhost:8080');

// Fetch and store vulnerabilities
const vulns = await api.assessVulnerability('CVE-2024-1234');
vulnerabilitiesStore.setVulnerability(vulns);
```

## Development

### Prerequisites

- Node.js 20+
- npm 10+

### Project Structure

```
src/
  components/   - Reusable Svelte components
  stores/       - Svelte stores (reactive state)
  services/     - API clients and services
  types/        - TypeScript type definitions
  main.ts       - Standalone app entry
  module.ts     - Embeddable library entry
  App.svelte    - Root component
  app.css       - Global styles
```

### Type Safety

All components and services use TypeScript for type safety:

```typescript
// src/types/index.ts
export interface Vulnerability {
  cve_id: string;
  severity: 'Critical' | 'High' | 'Medium' | 'Low' | 'None';
  cvss_base_score: number;
  is_kev: boolean;
  composite_risk_score: number;
  description?: string;
}
```

### Testing

```bash
# Type checking
npm run check

# Linting
npm run lint

# Format code
npm run format
```

## Deployment

### Docker Compose

Included in parent `docker-compose.yml`:

```yaml
cryptex-visualizer:
  build:
    context: ./cryptex-visualizer
    dockerfile: Dockerfile
  ports:
    - "5173:80"
  environment:
    - VITE_API_URL=http://cryptex-api:8080
  networks:
    - cryptex-network
```

### Nginx Serving

Production deployment with nginx:

```nginx
server {
  listen 80;
  root /usr/share/nginx/html;

  location / {
    try_files $uri $uri/ /index.html;
  }

  location /api/ {
    proxy_pass http://cryptex-api:8080/api/;
  }
}
```

## Troubleshooting

### API Connection Issues

```bash
# Check API is reachable
curl http://localhost:8080/health

# Verify VITE_API_URL
docker exec cryptex-visualizer env | grep VITE_API_URL

# Check browser console for CORS errors
```

### Build Failures

```bash
# Clear cache
rm -rf node_modules dist .svelte-kit
npm install
npm run build

# Check TypeScript errors
npm run check
```

### WebSocket Not Connecting

```bash
# Verify WebSocket endpoint
wscat -c ws://localhost:8080/ws

# Check nginx WebSocket proxy configuration
```

## Performance

### Bundle Size

- Standalone app: ~200KB (gzipped)
- Embeddable module: ~150KB (gzipped)

### Optimization

```javascript
// Vite config optimizations
build: {
  rollupOptions: {
    output: {
      manualChunks: {
        vendor: ['svelte', '@tanstack/svelte-query'],
        charts: ['chart.js'],
      },
    },
  },
}
```

## Browser Support

- Chrome/Edge 90+
- Firefox 88+
- Safari 14+

Modern browsers with ES2020 support.

## Contributing

See main CRYPTEX repository for contribution guidelines.

## License

See LICENSE file in repository root.
