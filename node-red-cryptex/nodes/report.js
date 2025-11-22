/**
 * CRYPTEX Report Node for Node-RED
 * Generate vulnerability reports
 */

module.exports = function(RED) {
    const fetch = require('node-fetch');

    function CryptexReportNode(config) {
        RED.nodes.createNode(this, config);
        const node = this;

        this.cryptexConfig = RED.nodes.getNode(config.cryptex);
        this.format = config.format || 'json'; // 'json', 'html', 'markdown'

        node.on('input', async function(msg, send, done) {
            send = send || function() { node.send.apply(node, arguments); };
            done = done || function(err) { if (err) node.error(err, msg); };

            try {
                const scanId = msg.scanId || msg.payload.scanId;
                const format = msg.format || node.format;

                if (!scanId) {
                    throw new Error('Scan ID is required');
                }

                if (!node.cryptexConfig || node.cryptexConfig.mode !== 'api') {
                    throw new Error('CRYPTEX API configuration required');
                }

                const apiUrl = node.cryptexConfig.apiUrl;

                node.status({ fill: 'yellow', shape: 'ring', text: `Generating ${format}...` });

                const response = await fetch(`${apiUrl}/api/v1/reports/${scanId}`, {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({ format })
                });

                if (!response.ok) {
                    throw new Error(`API error: ${response.statusText}`);
                }

                const report = await response.text();

                msg.payload = report;
                msg.format = format;
                msg.scanId = scanId;

                node.status({ fill: 'green', shape: 'dot', text: `Report generated (${format})` });

                send(msg);
                done();

            } catch (error) {
                node.status({ fill: 'red', shape: 'ring', text: 'Error' });
                done(error);
            }
        });
    }

    RED.nodes.registerType('cryptex-report', CryptexReportNode);
};
