/**
 * CRYPTEX Configuration Node for Node-RED
 * Configures connection to CRYPTEX REST API or local binary
 */

module.exports = function(RED) {
    const path = require('path');
    const os = require('os');
    const fs = require('fs');

    function CryptexConfigNode(config) {
        RED.nodes.createNode(this, config);

        this.mode = config.mode || 'api'; // 'api' or 'binary'
        this.apiUrl = config.apiUrl || 'http://localhost:8080';
        this.binaryPath = config.binaryPath || this.getDefaultBinaryPath();
        this.dataDir = config.dataDir || this.getDefaultDataDir();
    }

    CryptexConfigNode.prototype.getDefaultBinaryPath = function() {
        const platform = os.platform();
        const exeName = platform === 'win32' ? 'the_interface_server.exe' : 'the_interface_server';

        // Check common installation locations
        const possiblePaths = [
            path.join(process.cwd(), 'cryptex', 'bin', exeName),
            path.join(os.homedir(), '.cryptex', 'bin', exeName),
            path.join('/usr/local/bin', exeName),
            path.join('C:\\Program Files\\CRYPTEX\\bin', exeName)
        ];

        for (const binPath of possiblePaths) {
            if (fs.existsSync(binPath)) {
                return binPath;
            }
        }

        return exeName; // Assume it's in PATH
    };

    CryptexConfigNode.prototype.getDefaultDataDir = function() {
        const platform = os.platform();

        switch (platform) {
            case 'win32':
                return path.join(process.env.APPDATA || os.homedir(), 'CRYPTEX');
            case 'darwin':
                return path.join(os.homedir(), 'Library', 'Application Support', 'CRYPTEX');
            default:
                return path.join(process.env.XDG_DATA_HOME || path.join(os.homedir(), '.local', 'share'), 'CRYPTEX');
        }
    };

    RED.nodes.registerType('cryptex-config', CryptexConfigNode);
};
