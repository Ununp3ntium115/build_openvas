/**
 * CRYPTEX Assess Node for Node-RED
 * Assess vulnerability risk for a given CVE
 */

module.exports = function(RED) {
    const fetch = require('node-fetch');

    function CryptexAssessNode(config) {
        RED.nodes.createNode(this, config);
        const node = this;

        this.cryptexConfig = RED.nodes.getNode(config.cryptex);
        this.cveId = config.cveId;

        node.on('input', async function(msg, send, done) {
            send = send || function() { node.send.apply(node, arguments); };
            done = done || function(err) { if (err) node.error(err, msg); };

            try {
                const cveId = msg.cveId || msg.payload.cveId || node.cveId;

                if (!cveId) {
                    throw new Error('CVE ID is required');
                }

                node.status({ fill: 'yellow', shape: 'ring', text: `Assessing ${cveId}` });

                let vulnerability;

                if (node.cryptexConfig && node.cryptexConfig.mode === 'api') {
                    // Call REST API
                    const apiUrl = node.cryptexConfig.apiUrl;
                    const response = await fetch(`${apiUrl}/api/v1/vulnerabilities/${cveId}`);

                    if (!response.ok) {
                        throw new Error(`API error: ${response.statusText}`);
                    }

                    vulnerability = await response.json();
                } else {
                    // Future: Call local binary via MCP
                    throw new Error('Binary mode not yet implemented');
                }

                msg.payload = vulnerability;
                msg.cveId = cveId;
                msg.riskScore = vulnerability.composite_risk_score;
                msg.severity = vulnerability.severity;
                msg.isKEV = vulnerability.is_kev;

                node.status({ fill: 'green', shape: 'dot', text: `${cveId}: ${vulnerability.severity}` });

                send(msg);
                done();

            } catch (error) {
                node.status({ fill: 'red', shape: 'ring', text: 'Error' });
                done(error);
            }
        });
    }

    RED.nodes.registerType('cryptex-assess', CryptexAssessNode);
};
