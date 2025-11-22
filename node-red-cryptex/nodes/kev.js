/**
 * CRYPTEX KEV Node for Node-RED
 * Monitor and check CISA Known Exploited Vulnerabilities catalog
 */

module.exports = function(RED) {
    const fetch = require('node-fetch');

    function CryptexKEVNode(config) {
        RED.nodes.createNode(this, config);
        const node = this;

        this.cryptexConfig = RED.nodes.getNode(config.cryptex);

        node.on('input', async function(msg, send, done) {
            send = send || function() { node.send.apply(node, arguments); };
            done = done || function(err) { if (err) node.error(err, msg); };

            try {
                node.status({ fill: 'yellow', shape: 'ring', text: 'Fetching KEV catalog...' });

                // Fetch CISA KEV catalog
                const kevUrl = 'https://www.cisa.gov/sites/default/files/feeds/known_exploited_vulnerabilities.json';
                const response = await fetch(kevUrl);

                if (!response.ok) {
                    throw new Error(`Failed to fetch KEV catalog: ${response.statusText}`);
                }

                const kevData = await response.json();
                const vulnerabilities = kevData.vulnerabilities || [];

                // Filter and transform
                const kevList = vulnerabilities.map(vuln => ({
                    cveId: vuln.cveID,
                    vendorProject: vuln.vendorProject,
                    product: vuln.product,
                    vulnerabilityName: vuln.vulnerabilityName,
                    dateAdded: vuln.dateAdded,
                    shortDescription: vuln.shortDescription,
                    requiredAction: vuln.requiredAction,
                    dueDate: vuln.dueDate
                }));

                msg.payload = kevList;
                msg.totalKEV = kevList.length;
                msg.catalogVersion = kevData.catalogVersion;
                msg.dateReleased = kevData.dateReleased;

                node.status({ fill: 'green', shape: 'dot', text: `${kevList.length} KEVs` });

                send(msg);
                done();

            } catch (error) {
                node.status({ fill: 'red', shape: 'ring', text: 'Error' });
                done(error);
            }
        });
    }

    RED.nodes.registerType('cryptex-kev', CryptexKEVNode);
};
