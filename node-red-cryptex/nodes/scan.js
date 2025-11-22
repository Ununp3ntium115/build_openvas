/**
 * CRYPTEX Scan Node for Node-RED
 * Start and monitor vulnerability scans
 */

module.exports = function(RED) {
    const fetch = require('node-fetch');

    function CryptexScanNode(config) {
        RED.nodes.createNode(this, config);
        const node = this;

        this.cryptexConfig = RED.nodes.getNode(config.cryptex);
        this.target = config.target;
        this.action = config.action || 'start'; // 'start', 'status', 'end', 'results'

        node.on('input', async function(msg, send, done) {
            send = send || function() { node.send.apply(node, arguments); };
            done = done || function(err) { if (err) node.error(err, msg); };

            try {
                const action = msg.action || node.action;
                const target = msg.target || msg.payload.target || node.target;
                const scanId = msg.scanId || msg.payload.scanId;

                if (!node.cryptexConfig || node.cryptexConfig.mode !== 'api') {
                    throw new Error('CRYPTEX API configuration required');
                }

                const apiUrl = node.cryptexConfig.apiUrl;

                node.status({ fill: 'yellow', shape: 'ring', text: `${action}...` });

                let result;

                switch (action) {
                    case 'start':
                        if (!target) {
                            throw new Error('Target is required to start scan');
                        }

                        const startResponse = await fetch(`${apiUrl}/api/v1/scans`, {
                            method: 'POST',
                            headers: { 'Content-Type': 'application/json' },
                            body: JSON.stringify({ target })
                        });

                        if (!startResponse.ok) {
                            throw new Error(`API error: ${startResponse.statusText}`);
                        }

                        result = await startResponse.json();
                        msg.scanId = result.scan_id;
                        node.status({ fill: 'green', shape: 'dot', text: `Started: ${result.scan_id}` });
                        break;

                    case 'status':
                        if (!scanId) {
                            throw new Error('Scan ID is required');
                        }

                        const statusResponse = await fetch(`${apiUrl}/api/v1/scans/${scanId}`);

                        if (!statusResponse.ok) {
                            throw new Error(`API error: ${statusResponse.statusText}`);
                        }

                        result = await statusResponse.json();
                        node.status({ fill: 'blue', shape: 'dot', text: `${scanId}: ${result.status}` });
                        break;

                    case 'end':
                        if (!scanId) {
                            throw new Error('Scan ID is required');
                        }

                        const endResponse = await fetch(`${apiUrl}/api/v1/scans/${scanId}/end`, {
                            method: 'POST'
                        });

                        if (!endResponse.ok) {
                            throw new Error(`API error: ${endResponse.statusText}`);
                        }

                        result = await endResponse.json();
                        node.status({ fill: 'green', shape: 'dot', text: `Ended: ${scanId}` });
                        break;

                    case 'results':
                        if (!scanId) {
                            throw new Error('Scan ID is required');
                        }

                        const resultsResponse = await fetch(`${apiUrl}/api/v1/scans/${scanId}/results`);

                        if (!resultsResponse.ok) {
                            throw new Error(`API error: ${resultsResponse.statusText}`);
                        }

                        result = await resultsResponse.json();
                        node.status({ fill: 'green', shape: 'dot', text: `Results: ${result.length} vulns` });
                        break;

                    default:
                        throw new Error(`Unknown action: ${action}`);
                }

                msg.payload = result;
                send(msg);
                done();

            } catch (error) {
                node.status({ fill: 'red', shape: 'ring', text: 'Error' });
                done(error);
            }
        });
    }

    RED.nodes.registerType('cryptex-scan', CryptexScanNode);
};
