# CRYPTEX Full-Stack Deployment Guide

This guide covers deploying the complete CRYPTEX stack with Rust backend, Svelte frontend, Node-RED automation, and PYRO integration.

## Architecture Overview

```
┌─────────────────────────────────────────────────────┐
│                   NGINX (Port 80/443)                │
│              Reverse Proxy & Load Balancer           │
└────┬────────────────┬─────────────────┬─────────────┘
     │                │                 │
     v                v                 v
┌─────────┐    ┌──────────┐     ┌──────────────┐
│ Svelte  │    │ REST API │     │  Node-RED    │
│Frontend │    │ (Rust)   │     │  Automation  │
│Port 5173│    │Port 8080 │     │  Port 1880   │
└─────────┘    └────┬─────┘     └──────────────┘
                    │
                    v
              ┌──────────┐         ┌──────────────┐
              │  redb    │◄────────┤  MCP Server  │
              │ Database │         │  (PYRO Link) │
              └──────────┘         └──────────────┘
```

## Quick Start (Docker Compose)

### Prerequisites

- Docker 24.0+
- Docker Compose 2.20+
- 4GB RAM minimum
- 10GB disk space

### 1. Clone and Setup

```bash
git clone https://github.com/Ununp3ntium115/build_openvas.git
cd build_openvas
```

### 2. Configure Environment

Create `.env` file:

```env
# API Keys (optional, for enhanced features)
OPENAI_API_KEY=your_openai_key_here
ANTHROPIC_API_KEY=your_anthropic_key_here

# CRYPTEX Configuration
CRYPTEX_DB_PATH=/data/cryptex.db
CRYPTEX_BIND_ADDR=0.0.0.0:8080
RUST_LOG=info
```

### 3. Create PYRO Network (if integrating with PYRO)

```bash
docker network create pyro-network
```

### 4. Launch Stack

```bash
# Full stack
docker-compose up -d

# Specific services
docker-compose up -d cryptex-api cryptex-visualizer

# With build
docker-compose up -d --build
```

### 5. Access Services

- **CRYPTEX Frontend**: http://localhost:5173
- **REST API**: http://localhost:8080/api/v1
- **Node-RED**: http://localhost:1880
- **Nginx (unified)**: http://localhost

## Service Details

### CRYPTEX REST API (the_interface)

**Purpose**: REST API server providing HTTP/JSON endpoints for vulnerability assessment

**Port**: 8080
**Database**: redb at `/data/cryptex.db`
**Endpoints**:
- `GET /api/v1/vulnerabilities/:cve_id` - Assess vulnerability
- `POST /api/v1/scans` - Start scan
- `GET /api/v1/scans/:scan_id` - Get scan status
- `POST /api/v1/scans/:scan_id/end` - End scan
- `GET /api/v1/scans/:scan_id/results` - Get results
- `GET /api/v1/archive/stats` - Archive statistics
- `POST /api/v1/reports/:scan_id` - Generate report

**Health Check**: `curl http://localhost:8080/health`

### CRYPTEX Visualizer (Svelte)

**Purpose**: Interactive web UI for vulnerability visualization

**Port**: 5173 (dev), 80 (production)
**Features**:
- Real-time scan monitoring
- Vulnerability risk analysis
- Interactive dashboards
- Report generation
- Dark anarchist theme

**Modes**:
- **Standalone**: Full UI with header/nav
- **Embedded**: Embeddable in PYRO UI

### CRYPTEX MCP Server (the_commune)

**Purpose**: Model Context Protocol server for AI agent integration with PYRO

**Communication**: STDIN/STDOUT (JSON-RPC 2.0)
**Tools Exposed**:
- `assess_vulnerability` - Assess CVE risk
- `start_scan` - Initiate scan
- `end_scan` - Complete scan
- `get_scan_results` - Retrieve results
- `generate_report` - Create report

**PYRO Integration**:
```json
{
  "mcpServers": {
    "cryptex": {
      "command": "docker",
      "args": ["exec", "-i", "cryptex-mcp-server", "/app/the_commune"]
    }
  }
}
```

### Node-RED Automation (The Flow)

**Purpose**: Workflow automation for scheduled scans and alerting

**Port**: 1880
**Flows**:
1. **Scheduled Scanning**: Hourly network scans
2. **KEV Monitoring**: Daily CISA KEV catalog checks
3. **Alert Generation**: Critical vulnerability notifications
4. **PYRO Integration**: Send alerts to PYRO platform

**Access**: http://localhost:1880

### Nginx Reverse Proxy

**Purpose**: Unified entry point with SSL termination

**Ports**: 80 (HTTP), 443 (HTTPS)
**Routes**:
- `/cryptex/` → Svelte frontend
- `/api/` → REST API
- `/ws` → WebSocket
- `/node-red/` → Node-RED UI

## Building from Source

### Rust Backend

```bash
cd cryptex

# Build all crates
cargo build --release

# Build specific binary
cargo build --release --bin the_interface_server
cargo build --release --bin the_commune

# Run tests
cargo test --workspace

# Run API server
./target/release/the_interface_server
```

### Svelte Frontend

```bash
cd cryptex-visualizer

# Install dependencies
npm install

# Development mode
npm run dev

# Build for production (standalone + embeddable)
npm run build

# Preview production build
npm run preview
```

## Docker Build

### API Server

```bash
cd cryptex
docker build -f Dockerfile.api -t cryptex-api:latest .
docker run -p 8080:8080 -v cryptex-data:/data cryptex-api:latest
```

### MCP Server

```bash
cd cryptex
docker build -f Dockerfile.mcp -t cryptex-mcp:latest .
docker run -i cryptex-mcp:latest
```

### Visualizer

```bash
cd cryptex-visualizer
docker build -t cryptex-visualizer:latest .
docker run -p 5173:80 cryptex-visualizer:latest
```

## PYRO Integration

### Embedding CRYPTEX UI in PYRO

```typescript
// In PYRO frontend code
import { CryptexDashboard } from 'cryptex-visualizer';

new CryptexDashboard({
  target: document.getElementById('cryptex-corner'),
  props: {
    mode: 'embedded',
    apiUrl: 'http://cryptex-api:8080',
  }
});
```

### Using CRYPTEX MCP Server in PYRO

Add to PYRO's MCP configuration:

```json
{
  "mcpServers": {
    "cryptex": {
      "command": "docker",
      "args": [
        "exec",
        "-i",
        "cryptex-mcp-server",
        "/app/the_commune"
      ],
      "env": {
        "CRYPTEX_DB_PATH": "/data/cryptex.db",
        "RUST_LOG": "info"
      }
    }
  }
}
```

## Environment Variables

### Backend (Rust)

| Variable | Default | Description |
|----------|---------|-------------|
| `CRYPTEX_DB_PATH` | `./data/cryptex.db` | redb database path |
| `CRYPTEX_BIND_ADDR` | `0.0.0.0:8080` | API server bind address |
| `RUST_LOG` | `info` | Log level |
| `OPENAI_API_KEY` | - | OpenAI API key (optional) |
| `ANTHROPIC_API_KEY` | - | Anthropic API key (optional) |

### Frontend (Svelte)

| Variable | Default | Description |
|----------|---------|-------------|
| `VITE_API_URL` | `http://localhost:8080` | Backend API URL |

## Production Deployment

### SSL/TLS Configuration

1. Generate certificates:

```bash
mkdir -p nginx/ssl
openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
  -keyout nginx/ssl/key.pem \
  -out nginx/ssl/cert.pem
```

2. Update `nginx/nginx.conf` to enable HTTPS server block

3. Restart nginx:

```bash
docker-compose restart nginx
```

### Persistence

Data is persisted in Docker volumes:

```bash
# List volumes
docker volume ls | grep cryptex

# Backup database
docker run --rm -v cryptex-data:/data -v $(pwd):/backup \
  alpine tar czf /backup/cryptex-backup.tar.gz /data

# Restore database
docker run --rm -v cryptex-data:/data -v $(pwd):/backup \
  alpine tar xzf /backup/cryptex-backup.tar.gz -C /
```

### Scaling

Scale horizontally with Docker Compose:

```bash
# Scale API servers
docker-compose up -d --scale cryptex-api=3

# Update nginx upstream for load balancing
```

## Monitoring

### Health Checks

```bash
# API health
curl http://localhost:8080/health

# Frontend health
curl http://localhost:5173/health

# Stack status
docker-compose ps
```

### Logs

```bash
# All services
docker-compose logs -f

# Specific service
docker-compose logs -f cryptex-api

# Last 100 lines
docker-compose logs --tail=100 cryptex-api
```

### Metrics

```bash
# Resource usage
docker stats

# Database size
docker exec cryptex-api-server ls -lh /data/cryptex.db
```

## Troubleshooting

### API Server Won't Start

```bash
# Check logs
docker-compose logs cryptex-api

# Check database permissions
docker exec cryptex-api-server ls -la /data

# Recreate container
docker-compose up -d --force-recreate cryptex-api
```

### Frontend Can't Connect to API

```bash
# Check network connectivity
docker network inspect cryptex-network

# Verify API is running
curl http://cryptex-api:8080/health

# Check environment variables
docker exec cryptex-visualizer env | grep VITE_API_URL
```

### PYRO Network Not Found

```bash
# Create external network
docker network create pyro-network

# Verify network exists
docker network ls | grep pyro
```

### Node-RED Flows Not Loading

```bash
# Check volume mount
docker-compose down
docker-compose up -d

# Import flows manually via Node-RED UI
```

## Security Considerations

1. **Change default ports** in production
2. **Use SSL/TLS** for all external communication
3. **Restrict CORS origins** in API server
4. **Use secrets management** for API keys
5. **Enable authentication** for Node-RED
6. **Regular security updates** for base images

## Performance Tuning

### Rust Backend

```toml
# Cargo.toml
[profile.release]
opt-level = 3
lto = true
codegen-units = 1
```

### Database

```rust
// Increase cache size for large datasets
let db = Database::builder()
    .set_cache_size(100 * 1024 * 1024) // 100 MB
    .create(path)?;
```

### Frontend

```javascript
// vite.config.ts
build: {
  minify: 'terser',
  terserOptions: {
    compress: {
      drop_console: true,
    },
  },
}
```

## Support

For issues, questions, or contributions:

- GitHub Issues: https://github.com/Ununp3ntium115/build_openvas/issues
- PYRO Integration: https://github.com/Ununp3ntium115/PYRO_Platform_Ignition.git

## License

See LICENSE file in repository root.
