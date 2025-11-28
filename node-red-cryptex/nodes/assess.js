/**
 * CRYPTEX Assess Node for Node-RED
 * Assess vulnerability risk for a given CVE
 *
 * PERFORMANCE: N-API mode is 10-500x faster than HTTP API
 * - HTTP API: 100-200ms latency
 * - N-API: 1-5ms latency
 */

module.exports = function(RED) {
    // Lazy-load dependencies based on mode
    let fetch, CryptexAssessor;
    let assessorInstance = null;

    async function getAssessor() {
        if (!assessorInstance) {
            if (!CryptexAssessor) {
                const cryptexNapi = require('../../../cryptex/crates/cryptex_napi');
                CryptexAssessor = cryptexNapi.CryptexAssessor;
            }
            assessorInstance = await CryptexAssessor.create();
        }
        return assessorInstance;
    }

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
                const mode = node.cryptexConfig ? node.cryptexConfig.mode : 'napi';

                if (mode === 'api') {
                    // HTTP API mode (legacy, slow)
                    if (!fetch) {
                        fetch = require('node-fetch');
                    }

                    const apiUrl = node.cryptexConfig.apiUrl;
                    const response = await fetch(`${apiUrl}/api/v1/vulnerabilities/${cveId}`);

                    if (!response.ok) {
                        throw new Error(`API error: ${response.statusText}`);
                    }

                    vulnerability = await response.json();

                } else if (mode === 'napi') {
                    // N-API mode (default, fast - 10-500x faster than HTTP)
                    const assessor = await getAssessor();
                    const assessment = await assessor.assessVulnerability(cveId);

                    // Convert N-API result to same format as HTTP API
                    vulnerability = {
                        cve_id: assessment.cve_id,
                        severity: assessment.severity,
                        cvss_score: assessment.cvss_score,
                        is_kev: assessment.is_kev,
                        composite_risk_score: assessment.composite_risk_score,
                        description: assessment.description,
                        vulnerability_name: assessment.vulnerability_name,
                        published_date: assessment.published_date,
                        last_modified: assessment.last_modified
                    };

                } else {
                    // Binary mode via MCP (future implementation)
                    throw new Error('Binary mode not yet implemented. Use "napi" or "api" mode.');
                }

                // Enrich message with assessment data
                msg.payload = vulnerability;
                msg.cveId = cveId;
                msg.riskScore = vulnerability.composite_risk_score;
                msg.severity = vulnerability.severity;
                msg.isKEV = vulnerability.is_kev;
                msg.cvssScore = vulnerability.cvss_score;

                const statusText = mode === 'napi'
                    ? `⚡${cveId}: ${vulnerability.severity}`
                    : `${cveId}: ${vulnerability.severity}`;

                node.status({ fill: 'green', shape: 'dot', text: statusText });

                send(msg);
                done();

            } catch (error) {
                node.status({ fill: 'red', shape: 'ring', text: 'Error' });
                node.error(`Assessment failed: ${error.message}`, msg);
                done(error);
            }
        });

        // Cleanup on node removal
        node.on('close', function() {
            // N-API instances are ref-counted and will be cleaned up automatically
            assessorInstance = null;
        });
    }

    RED.nodes.registerType('cryptex-assess', CryptexAssessNode);
};
