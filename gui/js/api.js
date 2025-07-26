/**
 * AI-Enhanced OpenVAS GUI - API Client
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

class AIServiceAPI {
    constructor(baseUrl = '/api/v1') {
        this.baseUrl = baseUrl;
        this.headers = {
            'Content-Type': 'application/json',
            'Accept': 'application/json'
        };
    }

    /**
     * Make HTTP request with error handling
     */
    async request(endpoint, options = {}) {
        const url = `${this.baseUrl}${endpoint}`;
        const config = {
            headers: this.headers,
            ...options
        };

        try {
            const response = await fetch(url, config);
            const data = await response.json();

            if (!response.ok) {
                throw new Error(data.message || `HTTP ${response.status}: ${response.statusText}`);
            }

            return data;
        } catch (error) {
            console.error(`API request failed: ${endpoint}`, error);
            throw error;
        }
    }

    /**
     * GET request
     */
    async get(endpoint) {
        return this.request(endpoint, { method: 'GET' });
    }

    /**
     * POST request
     */
    async post(endpoint, data) {
        return this.request(endpoint, {
            method: 'POST',
            body: JSON.stringify(data)
        });
    }

    /**
     * PUT request
     */
    async put(endpoint, data) {
        return this.request(endpoint, {
            method: 'PUT',
            body: JSON.stringify(data)
        });
    }

    /**
     * DELETE request
     */
    async delete(endpoint) {
        return this.request(endpoint, { method: 'DELETE' });
    }

    // Service Management
    async getServiceStatus() {
        return this.get('/service/status');
    }

    async getServiceHealth() {
        return this.get('/service/health');
    }

    async restartService() {
        return this.post('/service/restart');
    }

    // Providers
    async getProviders() {
        return this.get('/providers');
    }

    async getProvider(providerId) {
        return this.get(`/providers/${providerId}`);
    }

    async addProvider(providerData) {
        return this.post('/providers', providerData);
    }

    async updateProvider(providerId, providerData) {
        return this.put(`/providers/${providerId}`, providerData);
    }

    async deleteProvider(providerId) {
        return this.delete(`/providers/${providerId}`);
    }

    async testProvider(providerId) {
        return this.post(`/providers/${providerId}/test`);
    }

    // AI Requests
    async sendAIRequest(requestData) {
        return this.post('/requests', requestData);
    }

    async getRequestHistory(limit = 50) {
        return this.get(`/requests/history?limit=${limit}`);
    }

    async getRequest(requestId) {
        return this.get(`/requests/${requestId}`);
    }

    // Metrics and Monitoring
    async getMetrics() {
        return this.get('/metrics');
    }

    async getPerformanceMetrics() {
        return this.get('/metrics/performance');
    }

    async getProviderMetrics(providerId) {
        return this.get(`/metrics/providers/${providerId}`);
    }

    async getSystemMetrics() {
        return this.get('/metrics/system');
    }

    // Configuration
    async getConfiguration() {
        return this.get('/config');
    }

    async updateConfiguration(configData) {
        return this.put('/config', configData);
    }

    async resetConfiguration() {
        return this.post('/config/reset');
    }

    // Cache Management
    async getCacheStats() {
        return this.get('/cache/stats');
    }

    async clearCache() {
        return this.post('/cache/clear');
    }

    async invalidateCacheKey(key) {
        return this.delete(`/cache/${encodeURIComponent(key)}`);
    }

    // Rate Limiting
    async getRateLimitStats() {
        return this.get('/ratelimit/stats');
    }

    async resetRateLimit(providerId) {
        return this.post(`/ratelimit/${providerId}/reset`);
    }

    // Logs
    async getLogs(level = null, limit = 100) {
        const params = new URLSearchParams();
        if (level) params.append('level', level);
        params.append('limit', limit);
        return this.get(`/logs?${params}`);
    }

    async clearLogs() {
        return this.delete('/logs');
    }

    async exportLogs(format = 'json') {
        return this.get(`/logs/export?format=${format}`);
    }

    // Real-time updates via Server-Sent Events
    createEventSource(endpoint) {
        return new EventSource(`${this.baseUrl}${endpoint}`);
    }

    // WebSocket connection for real-time updates
    createWebSocket(endpoint) {
        const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
        const wsUrl = `${protocol}//${window.location.host}${this.baseUrl}${endpoint}`;
        return new WebSocket(wsUrl);
    }
}

// Mock API for development/demo purposes
class MockAIServiceAPI extends AIServiceAPI {
    constructor() {
        super();
        this.mockData = {
            providers: [
                {
                    id: 'openai-1',
                    name: 'OpenAI GPT-4',
                    type: 'openai',
                    status: 'healthy',
                    model: 'gpt-4',
                    requests_sent: 1247,
                    success_rate: 98.5,
                    avg_response_time: 1850,
                    last_used: new Date().toISOString()
                },
                {
                    id: 'claude-1',
                    name: 'Claude 3 Sonnet',
                    type: 'claude',
                    status: 'healthy',
                    model: 'claude-3-sonnet-20240229',
                    requests_sent: 892,
                    success_rate: 97.2,
                    avg_response_time: 2100,
                    last_used: new Date(Date.now() - 3600000).toISOString()
                }
            ],
            metrics: {
                total_requests: 2139,
                successful_requests: 2089,
                failed_requests: 50,
                success_rate: 97.7,
                avg_response_time: 1950,
                cache_hit_rate: 23.4,
                requests_per_minute: 12.5
            },
            requestHistory: [
                {
                    id: 'req-001',
                    provider: 'openai',
                    task_type: 'vulnerability_analysis',
                    status: 'success',
                    response_time: 1650,
                    confidence: 0.92,
                    timestamp: new Date().toISOString()
                },
                {
                    id: 'req-002',
                    provider: 'claude',
                    task_type: 'threat_modeling',
                    status: 'success',
                    response_time: 2200,
                    confidence: 0.88,
                    timestamp: new Date(Date.now() - 300000).toISOString()
                }
            ],
            logs: [
                {
                    timestamp: new Date().toISOString(),
                    level: 'INFO',
                    message: 'AI service started successfully'
                },
                {
                    timestamp: new Date(Date.now() - 60000).toISOString(),
                    level: 'INFO',
                    message: 'OpenAI provider health check passed'
                },
                {
                    timestamp: new Date(Date.now() - 120000).toISOString(),
                    level: 'WARN',
                    message: 'Rate limit approaching for OpenAI provider'
                }
            ]
        };
    }

    async request(endpoint, options = {}) {
        // Simulate network delay
        await new Promise(resolve => setTimeout(resolve, 200 + Math.random() * 800));

        // Mock responses based on endpoint
        if (endpoint === '/service/status') {
            return {
                status: 'running',
                uptime: 86400,
                version: '1.0.0',
                components: {
                    ai_service: 'running',
                    cache: 'running',
                    rate_limiter: 'running',
                    monitoring: 'running'
                }
            };
        }

        if (endpoint === '/providers') {
            return { providers: this.mockData.providers };
        }

        if (endpoint === '/metrics') {
            return this.mockData.metrics;
        }

        if (endpoint === '/requests/history') {
            return { requests: this.mockData.requestHistory };
        }

        if (endpoint === '/logs') {
            return { logs: this.mockData.logs };
        }

        if (endpoint === '/requests' && options.method === 'POST') {
            const requestData = JSON.parse(options.body);
            const response = {
                id: `req-${Date.now()}`,
                status: 'success',
                response_time: 1500 + Math.random() * 1000,
                confidence: 0.8 + Math.random() * 0.2,
                result: {
                    content: this.generateMockAIResponse(requestData.task_type),
                    provider: requestData.provider
                },
                vulnerability_scoring: this.generateMockVulnerabilityScoring(requestData),
                timestamp: new Date().toISOString()
            };

            // Add to history
            this.mockData.requestHistory.unshift(response);
            if (this.mockData.requestHistory.length > 10) {
                this.mockData.requestHistory.pop();
            }

            return response;
        }

        if (endpoint.startsWith('/providers/') && endpoint.endsWith('/test')) {
            return {
                status: 'success',
                response_time: 800 + Math.random() * 400,
                message: 'Provider test successful'
            };
        }

        // Default mock response
        return { success: true, message: 'Mock response' };
    }

    generateMockAIResponse(taskType) {
        const responses = {
            vulnerability_analysis: `VULNERABILITY ANALYSIS REPORT

RISK ASSESSMENT: HIGH
This SQL injection vulnerability poses a significant risk to the application and underlying database. The vulnerability allows attackers to manipulate database queries, potentially leading to unauthorized data access, modification, or deletion.

BUSINESS IMPACT:
- Data breach risk: HIGH
- Service disruption: MEDIUM
- Compliance violations: HIGH (GDPR, PCI-DSS)
- Financial impact: Estimated $50K-$500K

TECHNICAL DETAILS:
The vulnerability exists in the user input validation layer where SQL queries are constructed using string concatenation without proper parameterization.

REMEDIATION RECOMMENDATIONS:
1. IMMEDIATE: Implement parameterized queries/prepared statements
2. Deploy input validation and sanitization
3. Apply principle of least privilege to database accounts
4. Implement Web Application Firewall (WAF) rules
5. Conduct security code review

CONFIDENCE: 92%`,

            threat_modeling: `THREAT MODELING ANALYSIS

IDENTIFIED THREATS:
1. SQL Injection Attacks
   - Likelihood: HIGH
   - Impact: CRITICAL
   - Attack Vector: Web application input fields

2. Cross-Site Scripting (XSS)
   - Likelihood: MEDIUM
   - Impact: HIGH
   - Attack Vector: User-generated content

3. Authentication Bypass
   - Likelihood: LOW
   - Impact: CRITICAL
   - Attack Vector: Session management flaws

ATTACK SCENARIOS:
- Scenario 1: Attacker exploits SQL injection to extract customer data
- Scenario 2: Malicious script injection leads to session hijacking
- Scenario 3: Privilege escalation through authentication flaws

SECURITY CONTROLS:
- Input validation and output encoding
- Multi-factor authentication
- Session management improvements
- Regular security assessments

CONFIDENCE: 88%`,

            scan_optimization: `SCAN OPTIMIZATION RECOMMENDATIONS

CURRENT SCAN EFFICIENCY: 67%

OPTIMIZATION STRATEGIES:
1. Prioritize high-risk targets (web servers, databases)
2. Reduce scan intensity during business hours
3. Implement intelligent port selection
4. Use cached results for recent scans

RECOMMENDED SCAN ORDER:
1. 192.168.1.100 (Web Server) - Priority: HIGH
2. 192.168.1.50 (Database) - Priority: HIGH
3. 192.168.1.10-49 (Workstations) - Priority: MEDIUM

PERFORMANCE IMPROVEMENTS:
- Estimated time reduction: 35%
- Resource utilization: Optimized
- Detection accuracy: Maintained at 95%+

CONFIDENCE: 91%`
        };

        return responses[taskType] || 'AI analysis completed successfully.';
    }

    generateMockVulnerabilityScoring(requestData) {
        // Generate mock vulnerability scoring data
        const cveId = this.extractCVEFromRequest(requestData);
        
        return {
            cve_id: cveId,
            cvss_scores: {
                v4_0: null, // N/A for demonstration
                v3_1: {
                    base_score: 9.1,
                    severity: 'CRITICAL',
                    vector: 'CVSS:3.1/AV:N/AC:L/PR:N/UI:N/S:U/C:H/I:H/A:N',
                    attack_vector: 'NETWORK',
                    attack_complexity: 'LOW',
                    privileges_required: 'NONE',
                    user_interaction: 'NONE',
                    scope: 'UNCHANGED',
                    confidentiality: 'HIGH',
                    integrity: 'HIGH',
                    availability: 'NONE'
                },
                v3_0: null, // N/A for demonstration
                v2: null    // N/A for demonstration
            },
            kev_info: {
                is_kev: false,
                date_added: null,
                due_date: null,
                required_action: null,
                known_ransomware_use: null
            },
            epss_info: {
                score: 0.00052,
                percentile: 5.2,
                date: new Date().toISOString().split('T')[0],
                model_version: '3.0.0'
            },
            ssvc_info: {
                decision: 'ATTEND',
                exploitation: 'none',
                automatable: 'no',
                technical_impact: 'total',
                mission_impact: 'high'
            },
            ai_enhanced: {
                risk_score: 8.7,
                priority: 'HIGH',
                remediation_urgency: 'HIGH',
                composite_reasoning: 'High CVSS score (9.1) with total technical impact, despite low EPSS probability'
            },
            metadata: {
                published_date: '2023-12-15T10:30:00Z',
                last_modified: '2024-01-10T14:22:00Z',
                cwe_ids: ['CWE-89', 'CWE-20'],
                references: [
                    'https://nvd.nist.gov/vuln/detail/' + cveId,
                    'https://cve.mitre.org/cgi-bin/cvename.cgi?name=' + cveId
                ]
            }
        };
    }

    extractCVEFromRequest(requestData) {
        // Try to extract CVE ID from request data
        const inputData = requestData.input_data;
        
        if (inputData && inputData.cve_id) {
            return inputData.cve_id;
        }
        
        if (inputData && inputData.vulnerability) {
            // Generate a mock CVE ID based on vulnerability type
            const vulnType = inputData.vulnerability.toLowerCase();
            if (vulnType.includes('sql')) {
                return 'CVE-2023-12345';
            } else if (vulnType.includes('xss')) {
                return 'CVE-2023-23456';
            } else if (vulnType.includes('rce')) {
                return 'CVE-2023-34567';
            }
        }
        
        // Default mock CVE
        return 'CVE-2023-12345';
    }
}

// Initialize API client
const apiClient = new MockAIServiceAPI(); // Use MockAIServiceAPI for demo
// const apiClient = new AIServiceAPI(); // Use real API in production

// Export for use in other modules
window.apiClient = apiClient;