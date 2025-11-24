# Deployment Guide
## openFireVulnerability + PYRO Platform Integration

**Version**: 1.0.0
**Date**: 2025-11-24
**Status**: Production Ready

---

## Table of Contents

1. [Deployment Overview](#deployment-overview)
2. [Pre-Deployment Checklist](#pre-deployment-checklist)
3. [Deployment Options](#deployment-options)
4. [Production Configuration](#production-configuration)
5. [Security Hardening](#security-hardening)
6. [Monitoring & Observability](#monitoring--observability)
7. [Backup & Recovery](#backup--recovery)
8. [Troubleshooting](#troubleshooting)

---

## Deployment Overview

### Architecture

```
┌──────────────────────────────────────────────────────────┐
│               Production Environment                      │
├──────────────────────────────────────────────────────────┤
│                                                           │
│  ┌─────────────────────────────────────────────────────┐ │
│  │            PYRO Platform (Fire Marshal)             │ │
│  │                Port: 8000                            │ │
│  └────────────────────┬────────────────────────────────┘ │
│                       │                                   │
│                       │ MCP Protocol (stdio)              │
│                       │                                   │
│  ┌────────────────────▼────────────────────────────────┐ │
│  │         openFireVulnerability MCP Server            │ │
│  │            cryptex-mcp-server                        │ │
│  │                                                      │ │
│  │  Components:                                         │ │
│  │  • The Assessor (CVE assessment)                    │ │
│  │  • The Infiltrator (scanning)                       │ │
│  │  • The Propagandist (reporting)                     │ │
│  └────────────────────┬────────────────────────────────┘ │
│                       │                                   │
│                       │                                   │
│  ┌────────────────────▼────────────────────────────────┐ │
│  │              redb Database                           │ │
│  │    ~/.local/share/openFireVulnerability/            │ │
│  │         openfire.redb                               │ │
│  └──────────────────────────────────────────────────────┘ │
│                                                           │
└──────────────────────────────────────────────────────────┘
```

### Components

| Component | Binary | Size | Port |
|-----------|--------|------|------|
| **CRYPTEX MCP Server** | `cryptex-mcp-server` | 1.6 MB | stdio |
| **Integration Helper** | `pyro-integration-mcp` | 2.1 MB | stdio |
| **Desktop UI** | `dist/` | 175 KB | 5173 (dev) |
| **redb Database** | `openfire.redb` | < 100 MB | N/A |

---

## Pre-Deployment Checklist

### System Requirements

- [ ] **OS**: Linux (Ubuntu 20.04+ / RHEL 8+ / Debian 11+)
- [ ] **CPU**: 2+ cores (4+ recommended)
- [ ] **RAM**: 4 GB minimum (8 GB recommended)
- [ ] **Disk**: 10 GB minimum (50 GB recommended)
- [ ] **Network**: Internet access for CVE data

### Software Requirements

- [ ] **Rust**: 1.70+ installed
- [ ] **Cargo**: Latest version
- [ ] **Node.js**: 18+ (for desktop UI)
- [ ] **npm**: 9+ (for desktop UI)
- [ ] **Git**: For repository access

### Validation

```bash
# Run comprehensive validation
cd /home/user/build_openvas
echo '{"jsonrpc":"2.0","id":1,"method":"tools/call","params":{"name":"validate_integration","arguments":{}}}' | \
  ./cryptex/target/release/pyro-integration-mcp

# Expected: 4/4 checks passed
```

### Build Verification

```bash
# 1. Build MCP server
cd /home/user/build_openvas/cryptex
cargo build --release -p the_commune

# 2. Build integration helper
cargo build --release -p pyro_integration_helper

# 3. Build desktop UI (if needed)
cd ../cryptex-desktop
npm install
npm run build

# 4. Verify binaries
ls -lh /home/user/build_openvas/cryptex/target/release/cryptex-mcp-server
ls -lh /home/user/build_openvas/cryptex/target/release/pyro-integration-mcp
```

---

## Deployment Options

### Option 1: Native Deployment (Recommended for PYRO Integration)

**Best for**: PYRO Platform integration, maximum performance

#### Step 1: Install Binaries

```bash
# Create installation directory
sudo mkdir -p /opt/openfirevulnerability/bin
sudo mkdir -p /opt/openfirevulnerability/config
sudo mkdir -p /var/lib/openfirevulnerability

# Copy binaries
sudo cp /home/user/build_openvas/cryptex/target/release/cryptex-mcp-server /opt/openfirevulnerability/bin/
sudo cp /home/user/build_openvas/cryptex/target/release/pyro-integration-mcp /opt/openfirevulnerability/bin/

# Set permissions
sudo chmod +x /opt/openfirevulnerability/bin/*
sudo chown -R $USER:$USER /var/lib/openfirevulnerability
```

#### Step 2: Install Configuration

```bash
# Copy configuration files
sudo cp /home/user/build_openvas/cryptex/openfire-config.toml /opt/openfirevulnerability/config/
sudo cp /home/user/build_openvas/mcp_servers.json /opt/openfirevulnerability/config/

# Update paths in mcp_servers.json
sudo sed -i 's|/home/user/build_openvas/cryptex/target/release/cryptex-mcp-server|/opt/openfirevulnerability/bin/cryptex-mcp-server|g' \
  /opt/openfirevulnerability/config/mcp_servers.json
```

#### Step 3: Setup Database

```bash
# Create database directory
mkdir -p ~/.local/share/openFireVulnerability

# Or system-wide
sudo mkdir -p /var/lib/openfirevulnerability/db
sudo chown -R $USER:$USER /var/lib/openfirevulnerability
```

#### Step 4: Create Systemd Service (Optional)

```bash
# Create service file
sudo tee /etc/systemd/system/openfire-mcp.service > /dev/null <<EOF
[Unit]
Description=openFireVulnerability MCP Server
After=network.target

[Service]
Type=simple
User=$USER
WorkingDirectory=/opt/openfirevulnerability
Environment="RUST_LOG=info"
Environment="CRYPTEX_DB_PATH=/var/lib/openfirevulnerability/db/openfire.redb"
Environment="CRYPTEX_CONFIG=/opt/openfirevulnerability/config/openfire-config.toml"
ExecStart=/opt/openfirevulnerability/bin/cryptex-mcp-server
Restart=on-failure
RestartSec=5s

[Install]
WantedBy=multi-user.target
EOF

# Enable and start service
sudo systemctl daemon-reload
sudo systemctl enable openfire-mcp.service
sudo systemctl start openfire-mcp.service
sudo systemctl status openfire-mcp.service
```

---

### Option 2: Docker Deployment

**Best for**: Isolated environments, easier scaling

#### Dockerfile

```dockerfile
# Create Dockerfile
cat > /home/user/build_openvas/Dockerfile <<'EOF'
FROM rust:1.91 as builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    pkg-config \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /build

# Copy Cargo files
COPY cryptex/Cargo.toml cryptex/Cargo.lock ./cryptex/
COPY cryptex/crates ./cryptex/crates

# Build release binaries
WORKDIR /build/cryptex
RUN cargo build --release -p the_commune
RUN cargo build --release -p pyro_integration_helper

# Runtime stage
FROM debian:bookworm-slim

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Create user
RUN useradd -m -u 1000 openfire

# Copy binaries
COPY --from=builder /build/cryptex/target/release/cryptex-mcp-server /usr/local/bin/
COPY --from=builder /build/cryptex/target/release/pyro-integration-mcp /usr/local/bin/

# Copy configuration
COPY cryptex/openfire-config.toml /etc/openfirevulnerability/
COPY mcp_servers.json /etc/openfirevulnerability/

# Set permissions
RUN chown -R openfire:openfire /usr/local/bin/cryptex-mcp-server \
    && chown -R openfire:openfire /usr/local/bin/pyro-integration-mcp \
    && chmod +x /usr/local/bin/*

# Create database directory
RUN mkdir -p /var/lib/openfirevulnerability \
    && chown -R openfire:openfire /var/lib/openfirevulnerability

# Switch to user
USER openfire

# Set environment
ENV RUST_LOG=info
ENV CRYPTEX_DB_PATH=/var/lib/openfirevulnerability/openfire.redb
ENV CRYPTEX_CONFIG=/etc/openfirevulnerability/openfire-config.toml

# Health check
HEALTHCHECK --interval=30s --timeout=3s \
  CMD echo '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{}}' | /usr/local/bin/cryptex-mcp-server || exit 1

# Entrypoint
ENTRYPOINT ["/usr/local/bin/cryptex-mcp-server"]
EOF
```

#### Build and Run

```bash
# Build Docker image
docker build -t openfirevulnerability:1.0.0 /home/user/build_openvas/

# Run container
docker run -d \
  --name openfire-mcp \
  -v openfire-data:/var/lib/openfirevulnerability \
  --restart unless-stopped \
  openfirevulnerability:1.0.0

# Verify
docker logs openfire-mcp
docker exec openfire-mcp /usr/local/bin/pyro-integration-mcp validate_integration
```

#### Docker Compose

```yaml
# docker-compose.yml
version: '3.8'

services:
  openfire-mcp:
    image: openfirevulnerability:1.0.0
    container_name: openfire-mcp
    restart: unless-stopped
    volumes:
      - openfire-data:/var/lib/openfirevulnerability
      - ./config:/etc/openfirevulnerability:ro
    environment:
      - RUST_LOG=info
      - CRYPTEX_DB_PATH=/var/lib/openfirevulnerability/openfire.redb
      - CRYPTEX_CONFIG=/etc/openfirevulnerability/openfire-config.toml
    healthcheck:
      test: ["CMD", "echo", '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{}}', "|", "/usr/local/bin/cryptex-mcp-server"]
      interval: 30s
      timeout: 3s
      retries: 3

volumes:
  openfire-data:
    driver: local
```

```bash
# Deploy with Docker Compose
docker-compose up -d

# View logs
docker-compose logs -f

# Stop
docker-compose down
```

---

### Option 3: Kubernetes Deployment

**Best for**: Large-scale deployments, high availability

#### Kubernetes Manifests

```yaml
# k8s/deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: openfire-mcp
  namespace: security
  labels:
    app: openfire-mcp
    version: "1.0.0"
spec:
  replicas: 3
  selector:
    matchLabels:
      app: openfire-mcp
  template:
    metadata:
      labels:
        app: openfire-mcp
    spec:
      containers:
      - name: mcp-server
        image: openfirevulnerability:1.0.0
        imagePullPolicy: IfNotPresent
        env:
        - name: RUST_LOG
          value: "info"
        - name: CRYPTEX_DB_PATH
          value: "/var/lib/openfirevulnerability/openfire.redb"
        - name: CRYPTEX_CONFIG
          value: "/etc/openfirevulnerability/openfire-config.toml"
        volumeMounts:
        - name: config
          mountPath: /etc/openfirevulnerability
          readOnly: true
        - name: data
          mountPath: /var/lib/openfirevulnerability
        resources:
          requests:
            memory: "256Mi"
            cpu: "250m"
          limits:
            memory: "512Mi"
            cpu: "500m"
        livenessProbe:
          exec:
            command:
            - /bin/sh
            - -c
            - echo '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{}}' | /usr/local/bin/cryptex-mcp-server
          initialDelaySeconds: 10
          periodSeconds: 30
        readinessProbe:
          exec:
            command:
            - /bin/sh
            - -c
            - echo '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{}}' | /usr/local/bin/cryptex-mcp-server
          initialDelaySeconds: 5
          periodSeconds: 10
      volumes:
      - name: config
        configMap:
          name: openfire-config
      - name: data
        persistentVolumeClaim:
          claimName: openfire-data
---
apiVersion: v1
kind: ConfigMap
metadata:
  name: openfire-config
  namespace: security
data:
  openfire-config.toml: |
    [archive]
    path = "/var/lib/openfirevulnerability/openfire.redb"
    max_size_gb = 10

    [memory]
    max_size_mb = 512
    ttl_seconds = 3600

    [interface]
    bind_address = "0.0.0.0:8080"
    cors_enabled = false
---
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: openfire-data
  namespace: security
spec:
  accessModes:
  - ReadWriteOnce
  resources:
    requests:
      storage: 10Gi
```

```bash
# Deploy to Kubernetes
kubectl create namespace security
kubectl apply -f k8s/deployment.yaml

# Check status
kubectl get pods -n security
kubectl logs -n security -l app=openfire-mcp
```

---

## Production Configuration

### openfire-config.toml

```toml
# Production configuration

[archive]
path = "/var/lib/openfirevulnerability/openfire.redb"
max_size_gb = 50  # Increased for production
compression = true
cache_size_mb = 512  # Increased for production

[memory]
max_size_mb = 1024  # Increased for production
ttl_seconds = 3600
enable_lru_cache = true

[[agitators]]
name = "claude"
agitator_type = "claude"
enabled = true
model = "claude-sonnet-4-5-20250929"
# API key from environment variable
# Set ANTHROPIC_API_KEY in environment

[interface]
bind_address = "127.0.0.1:8080"  # Localhost only for MCP
cors_enabled = false  # Disabled for security
max_request_size_mb = 10

[pyro]
enabled = true
protocol_version = "2024-11-05"

[security]
max_concurrent_scans = 100
max_scan_duration_seconds = 3600
max_report_size_mb = 50
rate_limit_per_minute = 1000

[logging]
level = "info"  # Options: error, warn, info, debug, trace
format = "json"  # Structured logging for production
```

---

## Security Hardening

### 1. Network Security

```toml
# Bind to localhost only for MCP (stdio communication)
[interface]
bind_address = "127.0.0.1:8080"
cors_enabled = false
```

### 2. API Key Protection

```bash
# NEVER commit API keys
# Use environment variables

export ANTHROPIC_API_KEY="your-claude-api-key"
export OPENAI_API_KEY="your-openai-api-key"

# Or use secrets management
# AWS Secrets Manager, HashiCorp Vault, etc.
```

### 3. File Permissions

```bash
# Restrict access to configuration
chmod 600 /opt/openfirevulnerability/config/openfire-config.toml
chmod 700 /var/lib/openfirevulnerability

# Restrict binary execution
chmod 755 /opt/openfirevulnerability/bin/*
```

### 4. Resource Limits

```toml
[security]
max_concurrent_scans = 100
max_scan_duration_seconds = 3600
max_report_size_mb = 50
rate_limit_per_minute = 1000
```

### 5. Input Validation

All inputs are validated:
- CVE IDs: `CVE-\d{4}-\d{4,7}`
- IP addresses: IPv4 format
- CIDR ranges: Valid CIDR notation
- Scan IDs: UUID format

---

## Monitoring & Observability

### 1. Logging

```toml
[logging]
level = "info"
format = "json"
output = "/var/log/openfirevulnerability/mcp-server.log"
```

### 2. Metrics (Prometheus)

```yaml
# prometheus.yml
scrape_configs:
  - job_name: 'openfire-mcp'
    static_configs:
      - targets: ['localhost:9090']
```

### 3. Health Checks

```bash
# MCP server health check
echo '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{}}' | \
  /opt/openfirevulnerability/bin/cryptex-mcp-server

# Integration validation
/opt/openfirevulnerability/bin/pyro-integration-mcp validate_integration
```

### 4. Alerting

Monitor these metrics:
- MCP server response time (< 200ms for assess_vulnerability)
- Database size (< 80% of max_size_gb)
- Memory usage (< 80% of max_size_mb)
- Error rate (< 1% of requests)
- Concurrent scans (< max_concurrent_scans)

---

## Backup & Recovery

### 1. Database Backup

```bash
# Backup redb database
tar -czf openfire-backup-$(date +%Y%m%d).tar.gz \
  ~/.local/share/openFireVulnerability/openfire.redb

# Or system-wide
sudo tar -czf /backup/openfire-backup-$(date +%Y%m%d).tar.gz \
  /var/lib/openfirevulnerability/openfire.redb
```

### 2. Automated Backups

```bash
# Cron job for daily backups at 2 AM
0 2 * * * /opt/openfirevulnerability/scripts/backup.sh
```

### 3. Recovery

```bash
# Restore from backup
tar -xzf openfire-backup-20251124.tar.gz -C ~/.local/share/openFireVulnerability/

# Verify
ls -lh ~/.local/share/openFireVulnerability/openfire.redb
```

---

## Troubleshooting

### Issue 1: MCP Server Won't Start

**Symptoms**: No response from MCP server

**Solutions**:
```bash
# Check binary permissions
ls -l /opt/openfirevulnerability/bin/cryptex-mcp-server
chmod +x /opt/openfirevulnerability/bin/cryptex-mcp-server

# Check logs
export RUST_LOG=debug
/opt/openfirevulnerability/bin/cryptex-mcp-server

# Verify database directory
mkdir -p ~/.local/share/openFireVulnerability
```

### Issue 2: Database Corruption

**Symptoms**: Read/write errors

**Solutions**:
```bash
# Check database integrity
file ~/.local/share/openFireVulnerability/openfire.redb

# Restore from backup
cp /backup/openfire-backup-latest.tar.gz .
tar -xzf openfire-backup-latest.tar.gz

# If unrecoverable, reset database
rm ~/.local/share/openFireVulnerability/openfire.redb
# Database will be recreated on next start
```

### Issue 3: Performance Degradation

**Symptoms**: Slow response times

**Solutions**:
```toml
# Increase cache size
[memory]
max_size_mb = 2048  # Increase
cache_size_mb = 1024  # Increase

# Check database size
[archive]
max_size_gb = 100  # Increase if needed
```

---

## Rollback Procedure

If deployment fails:

```bash
# 1. Stop service
sudo systemctl stop openfire-mcp.service

# 2. Restore previous version
sudo cp /opt/openfirevulnerability/bin/cryptex-mcp-server.backup \
  /opt/openfirevulnerability/bin/cryptex-mcp-server

# 3. Restore configuration
sudo cp /opt/openfirevulnerability/config/openfire-config.toml.backup \
  /opt/openfirevulnerability/config/openfire-config.toml

# 4. Restore database
cp /backup/openfire-backup-previous.tar.gz .
tar -xzf openfire-backup-previous.tar.gz

# 5. Restart service
sudo systemctl start openfire-mcp.service
sudo systemctl status openfire-mcp.service
```

---

## Post-Deployment Validation

```bash
# Run full validation
cd /home/user/build_openvas
./setup_pyro_integration.sh

# Expected output:
# ✅ 4/4 validation checks passed
# ✅ 5/5 MCP tools available
# ✅ All dependencies met
# ✅ Integration ready
```

---

## Support & Maintenance

### Regular Maintenance Tasks

**Daily**:
- Check logs for errors
- Monitor resource usage

**Weekly**:
- Database backup
- Review security logs

**Monthly**:
- Update CVE database
- Review performance metrics
- Update dependencies

### Contact

- **GitHub Issues**: [build_openvas/issues](https://github.com/Ununp3ntium115/build_openvas/issues)
- **Documentation**: See repository docs/
- **Security Issues**: Report privately via GitHub Security

---

**"No gods, no masters, production deployed."**

*openFireVulnerability Deployment Guide - v1.0.0*
