#!/bin/bash
# Standalone CRYPTEX Launcher
# Starts all CRYPTEX services in standalone mode

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Find CRYPTEX installation
if [ -n "$CRYPTEX_HOME" ]; then
    CRYPTEX_DIR="$CRYPTEX_HOME"
elif [ -d "$HOME/.cryptex" ]; then
    CRYPTEX_DIR="$HOME/.cryptex"
elif [ -d "$SCRIPT_DIR/cryptex" ]; then
    CRYPTEX_DIR="$SCRIPT_DIR/cryptex"
else
    echo -e "${RED}Error: CRYPTEX installation not found${NC}"
    echo "Set CRYPTEX_HOME environment variable or run install script"
    exit 1
fi

BIN_DIR="$CRYPTEX_DIR/bin"
DATA_DIR="$CRYPTEX_DIR/data"
PID_DIR="$CRYPTEX_DIR/run"

mkdir -p "$PID_DIR"

# Banner
echo -e "${BLUE}"
cat << 'EOF'
╔═══════════════════════════════════════════════════╗
║   _____ ______   ______ _______ _______ _    _   ║
║  / ____|  _ \ \ / /  _ \__   __|__   __| |  | |  ║
║ | |    | |_) \ V /| |_) | | |     | |  | |  | |  ║
║ | |    |  _ < | | |  __/  | |     | |  | |  | |  ║
║ | |____| |_) || | | |     | |     | |  | |__| |  ║
║  \_____|____/ |_| |_|     |_|     |_|   \____/   ║
║                                                   ║
║        The Anarchist Vulnerability Platform      ║
╚═══════════════════════════════════════════════════╝
EOF
echo -e "${NC}"

# Command
CMD="${1:-start}"

start_service() {
    local service=$1
    local binary=$2
    local pid_file="$PID_DIR/$service.pid"

    if [ -f "$pid_file" ] && kill -0 $(cat "$pid_file") 2>/dev/null; then
        echo -e "${YELLOW}⚠ $service already running (PID: $(cat "$pid_file"))${NC}"
        return 0
    fi

    echo -e "${YELLOW}Starting $service...${NC}"

    if [ ! -f "$BIN_DIR/$binary" ]; then
        echo -e "${RED}✗ Binary not found: $BIN_DIR/$binary${NC}"
        return 1
    fi

    nohup "$BIN_DIR/$binary" > "$PID_DIR/$service.log" 2>&1 &
    echo $! > "$pid_file"

    sleep 1

    if kill -0 $(cat "$pid_file") 2>/dev/null; then
        echo -e "${GREEN}✓ $service started (PID: $(cat "$pid_file"))${NC}"
        return 0
    else
        echo -e "${RED}✗ Failed to start $service${NC}"
        cat "$PID_DIR/$service.log"
        return 1
    fi
}

stop_service() {
    local service=$1
    local pid_file="$PID_DIR/$service.pid"

    if [ ! -f "$pid_file" ]; then
        echo -e "${YELLOW}⚠ $service not running${NC}"
        return 0
    fi

    local pid=$(cat "$pid_file")
    if ! kill -0 $pid 2>/dev/null; then
        echo -e "${YELLOW}⚠ $service not running (stale PID)${NC}"
        rm -f "$pid_file"
        return 0
    fi

    echo -e "${YELLOW}Stopping $service (PID: $pid)...${NC}"
    kill $pid

    # Wait for process to stop
    for i in {1..10}; do
        if ! kill -0 $pid 2>/dev/null; then
            echo -e "${GREEN}✓ $service stopped${NC}"
            rm -f "$pid_file"
            return 0
        fi
        sleep 1
    done

    # Force kill if still running
    echo -e "${YELLOW}Forcing stop...${NC}"
    kill -9 $pid 2>/dev/null || true
    rm -f "$pid_file"
    echo -e "${GREEN}✓ $service stopped${NC}"
}

status_service() {
    local service=$1
    local pid_file="$PID_DIR/$service.pid"

    if [ -f "$pid_file" ]; then
        local pid=$(cat "$pid_file")
        if kill -0 $pid 2>/dev/null; then
            echo -e "${GREEN}● $service running${NC} (PID: $pid)"
        else
            echo -e "${RED}● $service dead${NC} (stale PID)"
        fi
    else
        echo -e "${YELLOW}● $service stopped${NC}"
    fi
}

case "$CMD" in
    start)
        echo "Starting CRYPTEX services..."
        start_service "cryptex-api" "the_interface_server"
        echo ""
        echo -e "${GREEN}CRYPTEX is running!${NC}"
        echo -e "API: ${BLUE}http://localhost:8080${NC}"
        echo -e "Health check: ${BLUE}curl http://localhost:8080/health${NC}"
        ;;

    stop)
        echo "Stopping CRYPTEX services..."
        stop_service "cryptex-api"
        ;;

    restart)
        echo "Restarting CRYPTEX services..."
        stop_service "cryptex-api"
        sleep 2
        start_service "cryptex-api" "the_interface_server"
        ;;

    status)
        echo "CRYPTEX Service Status:"
        status_service "cryptex-api"
        ;;

    logs)
        service="${2:-cryptex-api}"
        log_file="$PID_DIR/$service.log"
        if [ -f "$log_file" ]; then
            tail -f "$log_file"
        else
            echo -e "${RED}Log file not found: $log_file${NC}"
        fi
        ;;

    *)
        echo "Usage: $0 {start|stop|restart|status|logs}"
        echo ""
        echo "Commands:"
        echo "  start    - Start all services"
        echo "  stop     - Stop all services"
        echo "  restart  - Restart all services"
        echo "  status   - Show service status"
        echo "  logs     - Show service logs (use Ctrl+C to exit)"
        exit 1
        ;;
esac
