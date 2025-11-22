# CRYPTEX Deployment Guide

**"No gods, no masters, no memory leaks."**

This guide covers deploying CRYPTEX in production environments for vulnerability assessment and AI-enhanced security analysis.

## 📋 Table of Contents

- [Overview](#overview)
- [Prerequisites](#prerequisites)
- [Deployment Options](#deployment-options)
- [Configuration](#configuration)
- [Running Services](#running-services)
- [Monitoring & Observability](#monitoring--observability)
- [Security Hardening](#security-hardening)
- [Scaling & Performance](#scaling--performance)
- [Troubleshooting](#troubleshooting)

---

## Overview

CRYPTEX provides three deployment modes:

1. **MCP Server Mode** - For PYRO Platform integration (AI agents)
2. **REST API Mode** - For traditional HTTP/JSON integrations
3. **Standalone Mode** - For direct CLI usage and automation

All modes can run concurrently for maximum flexibility.

---

## Prerequisites

### System Requirements

**Minimum:**
- CPU: 2 cores
- RAM: 2 GB
- Disk: 10 GB (for database and logs)
- OS: Linux, macOS, or Windows

**Recommended:**
- CPU: 4+ cores
- RAM: 4+ GB
- Disk: 50 GB SSD
- OS: Linux (Ubuntu 22.04+ or RHEL 8+)

### Software Dependencies

```bash
# Rust toolchain (1.70+)
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh

# System dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install -y build-essential pkg-config libssl-dev

# System dependencies (RHEL/CentOS)
sudo yum groupinstall -y "Development Tools"
sudo yum install -y openssl-devel

# System dependencies (macOS)
xcode-select --install
brew install openssl
```

---

## Deployment Options

### Option 1: Binary Release

```bash
# Clone repository
git clone https://github.com/Ununp3ntium115/build_openvas.git
cd build_openvas/cryptex

# Build release binaries
cargo build --release

# Binaries location
ls -lh target/release/cryptex-mcp-server  # MCP server
# Note: REST API server requires additional implementation
```

### Option 2: Docker Deployment

Create `Dockerfile`:

```dockerfile
FROM rust:1.75 as builder

WORKDIR /build
COPY cryptex ./cryptex
WORKDIR /build/cryptex

RUN cargo build --release

FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
    ca-certificates \
    libssl3 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /build/cryptex/target/release/cryptex-mcp-server /usr/local/bin/

# Database directory
RUN mkdir -p /var/lib/cryptex

EXPOSE 8080
VOLUME ["/var/lib/cryptex"]

ENTRYPOINT ["/usr/local/bin/cryptex-mcp-server"]
```

Build and run:

```bash
# Build image
docker build -t cryptex:latest .

# Run MCP server
docker run -d \
  --name cryptex-mcp \
  -v cryptex-data:/var/lib/cryptex \
  -e RUST_LOG=info \
  -e OPENAI_API_KEY=${OPENAI_API_KEY} \
  -e ANTHROPIC_API_KEY=${ANTHROPIC_API_KEY} \
  cryptex:latest
```

### Option 3: Kubernetes Deployment

See [kubernetes/README.md](kubernetes/README.md) for full manifests.

Quick start:

```bash
# Create namespace
kubectl create namespace cryptex

# Deploy ConfigMap
kubectl apply -f kubernetes/configmap.yaml

# Deploy Secret (API keys)
kubectl create secret generic cryptex-secrets \
  --from-literal=openai-api-key=${OPENAI_API_KEY} \
  --from-literal=anthropic-api-key=${ANTHROPIC_API_KEY} \
  -n cryptex

# Deploy MCP server
kubectl apply -f kubernetes/mcp-server.yaml

# Deploy REST API (if needed)
kubectl apply -f kubernetes/rest-api.yaml
```

---

## Configuration

### Environment Variables

| Variable | Description | Default | Required |
|----------|-------------|---------|----------|
| `RUST_LOG` | Logging level (error/warn/info/debug/trace) | `info` | No |
| `CRYPTEX_CONFIG` | Path to config file | `./cryptex-config.toml` | No |
| `CRYPTEX_DB_PATH` | Database file path | `./cryptex.db` | No |
| `OPENAI_API_KEY` | OpenAI API key for AI features | - | Optional* |
| `ANTHROPIC_API_KEY` | Anthropic API key for Claude | - | Optional* |
| `CRYPTEX_BIND_ADDR` | REST API bind address | `0.0.0.0:8080` | No |

*At least one AI provider key required for AI-enhanced features.

### Configuration File

Create `cryptex-config.toml`:

```toml
[commune]
name = "CRYPTEX Production"
host = "0.0.0.0"
port = 8080

[archive]
path = "/var/lib/cryptex/cryptex.db"
max_size_gb = 50
auto_vacuum = true

[memory]
max_size_mb = 512
ttl_seconds = 3600
cache_threshold = 0.85

[[agitators]]
name = "openai-gpt4"
agitator_type = "openai"
enabled = true
api_key_env = "OPENAI_API_KEY"
model = "gpt-4"
timeout_seconds = 30

[[agitators]]
name = "claude-sonnet"
agitator_type = "claude"
enabled = true
api_key_env = "ANTHROPIC_API_KEY"
model = "claude-3-sonnet-20240229"
timeout_seconds = 30

[assessor]
enable_nvd_api = true
enable_kev_lookup = true
enable_epss_lookup = true
cache_ttl_hours = 24

[infiltrator]
enable_ai_enhancement = true
max_concurrent_scans = 100
scan_timeout_seconds = 3600

[propagandist]
default_format = "html"
include_remediation = true
include_executive_summary = true
```

---

## Running Services

### MCP Server (PYRO Integration)

```bash
# Export API keys
export OPENAI_API_KEY="sk-your-key"
export ANTHROPIC_API_KEY="your-key"

# Run MCP server
./target/release/cryptex-mcp-server

# The server communicates via STDIN/STDOUT
# Configure in PYRO as:
{
  "servers": {
    "cryptex": {
      "command": "/path/to/cryptex-mcp-server",
      "env": {
        "OPENAI_API_KEY": "...",
        "ANTHROPIC_API_KEY": "..."
      }
    }
  }
}
```

### REST API Server

Create a binary wrapper for The Interface:

`crates/cryptex-server/src/main.rs`:

```rust
use std::path::PathBuf;
use the_interface::TheInterface;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    // Initialize logging
    tracing_subscriber::fmt::init();

    // Configuration
    let bind_addr = std::env::var("CRYPTEX_BIND_ADDR")
        .unwrap_or_else(|_| "0.0.0.0:8080".to_string());

    let db_path = std::env::var("CRYPTEX_DB_PATH")
        .unwrap_or_else(|_| "./cryptex.db".to_string());

    // Initialize and start server
    let interface = TheInterface::the_awakening(
        bind_addr,
        PathBuf::from(db_path)
    ).await?;

    tracing::info!("CRYPTEX REST API starting...");

    interface.the_manifestation().await?;

    Ok(())
}
```

Then build and run:

```bash
cargo build --release -p cryptex-server
./target/release/cryptex-server
```

### Systemd Service

Create `/etc/systemd/system/cryptex-mcp.service`:

```ini
[Unit]
Description=CRYPTEX MCP Server
After=network.target

[Service]
Type=simple
User=cryptex
Group=cryptex
WorkingDirectory=/opt/cryptex
Environment="RUST_LOG=info"
Environment="CRYPTEX_DB_PATH=/var/lib/cryptex/cryptex.db"
EnvironmentFile=/etc/cryptex/environment
ExecStart=/opt/cryptex/bin/cryptex-mcp-server
Restart=on-failure
RestartSec=10s

# Security hardening
NoNewPrivileges=true
PrivateTmp=true
ProtectSystem=strict
ProtectHome=true
ReadWritePaths=/var/lib/cryptex

[Install]
WantedBy=multi-user.target
```

Enable and start:

```bash
sudo systemctl daemon-reload
sudo systemctl enable cryptex-mcp
sudo systemctl start cryptex-mcp
sudo systemctl status cryptex-mcp
```

---

## Monitoring & Observability

### Logging

CRYPTEX uses structured logging with `tracing`:

```bash
# Set log level
export RUST_LOG=cryptex=debug,the_assessor=trace

# JSON output for log aggregation
export RUST_LOG_FORMAT=json
```

Log locations:
- **stdout** - Default (container-friendly)
- **File** - Configure via `tracing-appender`
- **Syslog** - Available via `tracing-journald`

### Metrics

CRYPTEX exposes metrics for monitoring:

**Archive Statistics:**
```bash
curl http://localhost:8080/api/v1/archive/stats

{
  "total_scans": 142,
  "total_vulnerabilities": 3582,
  "total_results": 8921
}
```

**Health Check:**
```bash
curl http://localhost:8080/health

{
  "status": "healthy",
  "version": "1.0.0"
}
```

### Prometheus Integration

Expose metrics endpoint (requires additional implementation):

```rust
// Add to The Interface
.route("/metrics", get(metrics_handler))
```

Example metrics to track:
- `cryptex_scans_total` - Total scans processed
- `cryptex_vulnerabilities_assessed` - CVEs assessed
- `cryptex_api_requests_total` - API request count
- `cryptex_cache_hit_ratio` - Cache effectiveness
- `cryptex_db_operations_duration_seconds` - Database latency

### Alerting

Key metrics to alert on:

1. **High Error Rate**: > 5% API errors
2. **Database Growth**: > 80% disk usage
3. **Scan Failures**: > 10% scan failure rate
4. **Cache Miss Rate**: > 50% cache misses
5. **Response Time**: p95 > 2 seconds

---

## Security Hardening

### API Authentication

Add authentication middleware to The Interface:

```rust
// Example JWT authentication
use axum::middleware;

let app = Router::new()
    .route("/api/v1/*", /* routes */)
    .layer(middleware::from_fn(auth_middleware))
    .with_state(state);

async fn auth_middleware(
    req: Request,
    next: Next,
) -> Result<Response, StatusCode> {
    // Verify JWT token
    let token = req.headers()
        .get("Authorization")
        .and_then(|v| v.to_str().ok())
        .ok_or(StatusCode::UNAUTHORIZED)?;

    // Validate token...

    Ok(next.run(req).await)
}
```

### TLS/HTTPS

Use reverse proxy for TLS termination:

**Nginx:**
```nginx
upstream cryptex_backend {
    server 127.0.0.1:8080;
}

server {
    listen 443 ssl http2;
    server_name cryptex.yourdomain.com;

    ssl_certificate /etc/ssl/certs/cryptex.crt;
    ssl_certificate_key /etc/ssl/private/cryptex.key;

    ssl_protocols TLSv1.2 TLSv1.3;
    ssl_ciphers HIGH:!aNULL:!MD5;

    location / {
        proxy_pass http://cryptex_backend;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }
}
```

### API Rate Limiting

Add tower-governor for rate limiting:

```rust
use tower_governor::{GovernorLayer, governor::GovernorConfigBuilder};

let governor_conf = Box::new(
    GovernorConfigBuilder::default()
        .per_second(10)
        .burst_size(50)
        .finish()
        .unwrap()
);

let app = Router::new()
    .layer(GovernorLayer {
        config: Box::leak(governor_conf),
    });
```

### Database Encryption

For sensitive data, encrypt at rest:

```bash
# Use LUKS for disk encryption
cryptsetup luksFormat /dev/sdb
cryptsetup luksOpen /dev/sdb cryptex-data
mkfs.ext4 /dev/mapper/cryptex-data
mount /dev/mapper/cryptex-data /var/lib/cryptex
```

### Secret Management

**Recommended**: Use external secret management:

- **HashiCorp Vault**
- **AWS Secrets Manager**
- **Azure Key Vault**
- **Google Secret Manager**

Example with Vault:

```bash
# Store API keys
vault kv put secret/cryptex \
  openai_api_key="sk-..." \
  anthropic_api_key="..."

# Retrieve in deployment
export OPENAI_API_KEY=$(vault kv get -field=openai_api_key secret/cryptex)
```

---

## Scaling & Performance

### Horizontal Scaling

CRYPTEX is stateless except for the database:

```yaml
# Kubernetes HorizontalPodAutoscaler
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: cryptex-mcp
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: cryptex-mcp
  minReplicas: 2
  maxReplicas: 10
  metrics:
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70
```

### Database Optimization

**Tuning redb:**

```rust
// Configure in TheArchive::the_awakening()
let db = Database::builder()
    .set_cache_size(256 * 1024 * 1024)  // 256 MB cache
    .set_region_size(16 * 1024)         // 16 KB regions
    .create(&path)?;
```

**Database Maintenance:**

```bash
# Monitor database size
ls -lh /var/lib/cryptex/cryptex.db

# Backup database
cp /var/lib/cryptex/cryptex.db /backup/cryptex-$(date +%Y%m%d).db

# Vacuum (compact) database periodically
# (Add maintenance endpoint to The Interface)
curl -X POST http://localhost:8080/api/v1/admin/vacuum
```

### Caching Strategy

CRYPTEX uses multi-level caching:

1. **Memory Cache** - Recent vulnerability assessments (TheMemory)
2. **Database Cache** - Historical vulnerability data (TheArchive)
3. **External APIs** - NVD/KEV/EPSS with TTL

Configure cache sizes:

```toml
[memory]
max_size_mb = 512      # In-memory cache
ttl_seconds = 3600     # 1 hour TTL
cache_threshold = 0.85 # Eviction threshold

[assessor]
cache_ttl_hours = 24   # Database cache TTL
```

### Load Testing

Example with `hey`:

```bash
# Install hey
go install github.com/rakyll/hey@latest

# Test vulnerability assessment endpoint
hey -n 10000 -c 100 -m GET \
  http://localhost:8080/api/v1/vulnerabilities/CVE-2021-44228

# Test scan creation
hey -n 1000 -c 50 -m POST \
  -H "Content-Type: application/json" \
  -d '{"target":"192.168.1.0/24"}' \
  http://localhost:8080/api/v1/scans
```

Expected performance:
- **Vulnerability Assessment**: 100-500 req/s (cached)
- **Scan Operations**: 50-100 req/s
- **Report Generation**: 10-50 req/s

---

## Troubleshooting

### Common Issues

#### 1. Database Lock Errors

**Symptom**: `Database is locked` or `Transaction conflict`

**Solution**:
```bash
# Check for stale processes
ps aux | grep cryptex

# Kill stale processes
pkill -9 cryptex-mcp-server

# Restart service
systemctl restart cryptex-mcp
```

#### 2. High Memory Usage

**Symptom**: OOM kills or excessive RAM consumption

**Solution**:
```toml
# Reduce cache size in config
[memory]
max_size_mb = 256  # Reduce from 512
cache_threshold = 0.75  # More aggressive eviction
```

#### 3. Slow API Responses

**Symptom**: High latency on API calls

**Diagnosis**:
```bash
# Check logs for slow operations
journalctl -u cryptex-mcp | grep "duration_ms"

# Profile with tracing
RUST_LOG=the_assessor=trace ./cryptex-mcp-server
```

**Solutions**:
- Increase cache size
- Add database indexes
- Scale horizontally

#### 4. MCP Connection Failures

**Symptom**: PYRO can't connect to MCP server

**Diagnosis**:
```bash
# Test MCP server manually
echo '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{}}' | \
  ./cryptex-mcp-server

# Check STDIN/STDOUT
strace -e read,write ./cryptex-mcp-server
```

**Solutions**:
- Verify binary path in PYRO config
- Check environment variables
- Review logs for startup errors

### Debug Mode

Enable debug logging:

```bash
# Maximum verbosity
export RUST_LOG=trace

# Specific module debugging
export RUST_LOG=the_assessor=debug,the_infiltrator=trace

# With backtrace
export RUST_BACKTRACE=1
```

### Health Checks

```bash
# Quick health check
curl -f http://localhost:8080/health || echo "Service unhealthy"

# Database stats check
curl http://localhost:8080/api/v1/archive/stats | jq

# Test vulnerability assessment
curl http://localhost:8080/api/v1/vulnerabilities/CVE-2021-44228 | jq
```

---

## Production Checklist

Before deploying to production:

- [ ] Configure authentication/authorization
- [ ] Enable TLS/HTTPS
- [ ] Set up log aggregation
- [ ] Configure metrics collection
- [ ] Implement backup strategy
- [ ] Set up monitoring alerts
- [ ] Document runbooks
- [ ] Load test at expected scale
- [ ] Review security hardening
- [ ] Configure rate limiting
- [ ] Set up secret management
- [ ] Enable database encryption (if required)
- [ ] Configure CORS properly
- [ ] Test disaster recovery
- [ ] Document incident response

---

## Support

For issues, questions, or contributions:

- **Issues**: [GitHub Issues](https://github.com/Ununp3ntium115/build_openvas/issues)
- **Documentation**: See [README.md](README.md) and [PYRO_INTEGRATION.md](PYRO_INTEGRATION.md)
- **PYRO Platform**: [PYRO GitHub](https://github.com/Ununp3ntium115/PYRO_Platform_Ignition.git)

---

**"The revolution will not be interpreted, it will be compiled."**

*CRYPTEX - Cross-Platform Rust Yielding Penetration Testing Execution eXperience*
