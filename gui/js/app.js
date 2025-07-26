/**
 * AI-Enhanced OpenVAS GUI - Main Application
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

class AIOpenVASApp {
    constructor() {
        this.currentSection = 'dashboard';
        this.refreshInterval = null;
        this.realTimeUpdates = false;
        this.providers = [];
        this.requestHistory = [];
    }

    /**
     * Initialize the application
     */
    async initialize() {
        try {
            // Initialize components
            componentsManager.initialize();
            
            // Set up navigation
            this.setupNavigation();
            
            // Set up event listeners
            this.setupEventListeners();
            
            // Initialize charts
            chartsManager.initializeCharts();
            
            // Load initial data
            await this.loadInitialData();
            
            // Start auto-refresh
            this.startAutoRefresh();
            
            // Show success message
            componentsManager.showToast('AI-Enhanced OpenVAS GUI loaded successfully', 'success');
            
        } catch (error) {
            console.error('Failed to initialize application:', error);
            componentsManager.showToast('Failed to initialize application', 'error');
        }
    }

    /**
     * Set up navigation
     */
    setupNavigation() {
        const navItems = document.querySelectorAll('.nav-item');
        navItems.forEach(item => {
            item.addEventListener('click', (e) => {
                e.preventDefault();
                const section = item.dataset.section;
                this.navigateToSection(section);
            });
        });
    }

    /**
     * Navigate to section
     */
    navigateToSection(section) {
        // Update navigation
        document.querySelectorAll('.nav-item').forEach(item => {
            item.classList.remove('active');
            if (item.dataset.section === section) {
                item.classList.add('active');
            }
        });

        // Update content sections
        document.querySelectorAll('.content-section').forEach(sec => {
            sec.classList.remove('active');
        });
        
        const targetSection = document.getElementById(`${section}-section`);
        if (targetSection) {
            targetSection.classList.add('active');
            this.currentSection = section;
            
            // Load section-specific data
            this.loadSectionData(section);
        }
    }

    /**
     * Set up event listeners
     */
    setupEventListeners() {
        // Refresh button
        document.getElementById('refresh-btn')?.addEventListener('click', () => {
            this.refreshCurrentSection();
        });

        // Add provider button
        document.getElementById('add-provider-btn')?.addEventListener('click', () => {
            componentsManager.openModal('add-provider-modal');
        });

        // AI request form
        document.getElementById('ai-request-form')?.addEventListener('submit', (e) => {
            e.preventDefault();
            this.handleAIRequest();
        });

        // Clear form button
        document.getElementById('clear-form-btn')?.addEventListener('click', () => {
            this.clearRequestForm();
        });

        // Load example button
        document.getElementById('load-example-btn')?.addEventListener('click', () => {
            this.loadExampleRequest();
        });

        // Save provider button
        document.getElementById('save-provider-btn')?.addEventListener('click', () => {
            this.handleSaveProvider();
        });

        // Configuration save button
        document.getElementById('save-config-btn')?.addEventListener('click', () => {
            this.handleSaveConfiguration();
        });

        // Log controls
        document.getElementById('clear-logs-btn')?.addEventListener('click', () => {
            this.handleClearLogs();
        });

        document.getElementById('export-logs-btn')?.addEventListener('click', () => {
            this.handleExportLogs();
        });

        // Window resize handler
        window.addEventListener('resize', () => {
            chartsManager.resizeCharts();
        });
    }

    /**
     * Load initial data
     */
    async loadInitialData() {
        try {
            componentsManager.showLoading('Loading initial data...');
            
            // Load service status
            await this.loadServiceStatus();
            
            // Load providers
            await this.loadProviders();
            
            // Load metrics
            await this.loadMetrics();
            
            // Load request history
            await this.loadRequestHistory();
            
        } catch (error) {
            console.error('Failed to load initial data:', error);
            componentsManager.showToast('Failed to load initial data', 'error');
        } finally {
            componentsManager.hideLoading();
        }
    }

    /**
     * Load service status
     */
    async loadServiceStatus() {
        try {
            const status = await apiClient.getServiceStatus();
            this.updateServiceStatus(status);
        } catch (error) {
            console.error('Failed to load service status:', error);
        }
    }

    /**
     * Update service status display
     */
    updateServiceStatus(status) {
        // Update connection indicator
        const connectionStatus = document.getElementById('connection-status');
        if (connectionStatus) {
            const indicator = connectionStatus.querySelector('i');
            const text = connectionStatus.querySelector('span');
            
            if (status.status === 'running') {
                indicator.style.color = 'var(--success-color)';
                text.textContent = 'Connected';
            } else {
                indicator.style.color = 'var(--error-color)';
                text.textContent = 'Disconnected';
            }
        }

        // Update component status
        if (status.components) {
            Object.entries(status.components).forEach(([component, state]) => {
                const element = document.getElementById(`${component.replace('_', '-')}-status`);
                if (element) {
                    element.textContent = state === 'running' ? 'Running' : 'Stopped';
                    element.className = `status-badge ${state === 'running' ? 'running' : 'stopped'}`;
                }
            });
        }
    }

    /**
     * Load providers
     */
    async loadProviders() {
        try {
            const response = await apiClient.getProviders();
            this.providers = response.providers || [];
            this.updateProvidersDisplay();
        } catch (error) {
            console.error('Failed to load providers:', error);
        }
    }

    /**
     * Update providers display
     */
    updateProvidersDisplay() {
        // Update provider status list
        const statusList = document.getElementById('provider-status-list');
        if (statusList) {
            statusList.innerHTML = this.providers.map(provider => `
                <div class="status-item">
                    <span class="status-label">${provider.name}</span>
                    <span class="status-badge ${provider.status === 'healthy' ? 'running' : 'stopped'}">
                        ${provider.status === 'healthy' ? 'Healthy' : 'Unhealthy'}
                    </span>
                </div>
            `).join('');
        }

        // Update providers grid
        const providersGrid = document.getElementById('providers-grid');
        if (providersGrid) {
            providersGrid.innerHTML = this.providers.map(provider => `
                <div class="provider-card ${provider.status !== 'healthy' ? 'disabled' : ''}">
                    <div class="provider-header">
                        <div class="provider-info">
                            <div class="provider-icon ${provider.type}">
                                <i class="fas fa-robot"></i>
                            </div>
                            <div class="provider-details">
                                <h4>${provider.name}</h4>
                                <div class="provider-type">${provider.type}</div>
                            </div>
                        </div>
                        <div class="provider-actions">
                            <button class="btn btn-icon" onclick="app.testProvider('${provider.id}')" title="Test Provider">
                                <i class="fas fa-play"></i>
                            </button>
                            <button class="btn btn-icon" onclick="app.editProvider('${provider.id}')" title="Edit Provider">
                                <i class="fas fa-edit"></i>
                            </button>
                            <button class="btn btn-icon" onclick="app.deleteProvider('${provider.id}')" title="Delete Provider">
                                <i class="fas fa-trash"></i>
                            </button>
                        </div>
                    </div>
                    <div class="provider-stats">
                        <div class="provider-stat">
                            <div class="provider-stat-value">${provider.requests_sent || 0}</div>
                            <div class="provider-stat-label">Requests</div>
                        </div>
                        <div class="provider-stat">
                            <div class="provider-stat-value">${(provider.success_rate || 0).toFixed(1)}%</div>
                            <div class="provider-stat-label">Success Rate</div>
                        </div>
                    </div>
                    <div class="provider-status ${provider.status === 'healthy' ? 'healthy' : 'unhealthy'}">
                        <i class="fas fa-circle"></i>
                        ${provider.status === 'healthy' ? 'Healthy' : 'Unhealthy'}
                    </div>
                </div>
            `).join('');
        }

        // Update provider select options
        const providerSelect = document.getElementById('provider-select');
        if (providerSelect) {
            const currentValue = providerSelect.value;
            providerSelect.innerHTML = '<option value="">Select Provider</option>' +
                this.providers.map(provider => 
                    `<option value="${provider.id}">${provider.name}</option>`
                ).join('');
            providerSelect.value = currentValue;
        }
    }

    /**
     * Load metrics
     */
    async loadMetrics() {
        try {
            const metrics = await apiClient.getMetrics();
            this.updateMetricsDisplay(metrics);
        } catch (error) {
            console.error('Failed to load metrics:', error);
        }
    }

    /**
     * Update metrics display
     */
    updateMetricsDisplay(metrics) {
        // Update metric cards
        document.getElementById('total-requests').textContent = metrics.total_requests || 0;
        document.getElementById('success-rate').textContent = `${(metrics.success_rate || 0).toFixed(1)}%`;
        document.getElementById('avg-response-time').textContent = `${Math.round(metrics.avg_response_time || 0)}ms`;
        document.getElementById('cache-hit-rate').textContent = `${(metrics.cache_hit_rate || 0).toFixed(1)}%`;

        // Update change indicators (mock data for demo)
        this.updateChangeIndicator('requests-change', 12.5);
        this.updateChangeIndicator('success-change', -0.3);
        this.updateChangeIndicator('response-time-change', -8.2);
        this.updateChangeIndicator('cache-change', 5.1);
    }

    /**
     * Update change indicator
     */
    updateChangeIndicator(elementId, change) {
        const element = document.getElementById(elementId);
        if (element) {
            element.textContent = `${change > 0 ? '+' : ''}${change.toFixed(1)}%`;
            element.className = `metric-change ${change >= 0 ? 'positive' : 'negative'}`;
        }
    }

    /**
     * Load request history
     */
    async loadRequestHistory() {
        try {
            const response = await apiClient.getRequestHistory();
            this.requestHistory = response.requests || [];
            this.updateRequestHistoryDisplay();
        } catch (error) {
            console.error('Failed to load request history:', error);
        }
    }

    /**
     * Update request history display
     */
    updateRequestHistoryDisplay() {
        const historyContainer = document.getElementById('request-history');
        if (historyContainer) {
            historyContainer.innerHTML = this.requestHistory.map(request => `
                <div class="history-item" onclick="app.showRequestDetails('${request.id}')">
                    <div class="history-info">
                        <span class="history-provider">${request.provider}</span>
                        <span class="history-task">${request.task_type.replace('_', ' ')}</span>
                        <span class="history-time">${componentsManager.formatTimestamp(request.timestamp)}</span>
                    </div>
                    <div class="history-status ${request.status}">
                        ${request.status}
                    </div>
                </div>
            `).join('');
        }
    }

    /**
     * Handle AI request submission
     */
    async handleAIRequest() {
        try {
            const form = document.getElementById('ai-request-form');
            const formData = new FormData(form);
            
            const requestData = {
                provider: document.getElementById('provider-select').value,
                task_type: document.getElementById('task-type-select').value,
                input_data: JSON.parse(document.getElementById('request-data').value),
                context: document.getElementById('context-input').value
            };

            componentsManager.showLoading('Sending AI request...');
            
            const response = await apiClient.sendAIRequest(requestData);
            
            this.displayRequestResponse(response);
            this.loadRequestHistory(); // Refresh history
            
            componentsManager.showToast('AI request completed successfully', 'success');
            
        } catch (error) {
            console.error('AI request failed:', error);
            componentsManager.showToast(`AI request failed: ${error.message}`, 'error');
        } finally {
            componentsManager.hideLoading();
        }
    }

    /**
     * Display request response
     */
    displayRequestResponse(response) {
        const resultsContainer = document.getElementById('results-container');
        if (resultsContainer) {
            resultsContainer.style.display = 'block';
            
            // Update response info
            const statusElement = document.getElementById('response-status');
            statusElement.textContent = response.status;
            statusElement.className = `response-status ${response.status}`;
            
            document.getElementById('response-time').textContent = 
                `${Math.round(response.response_time)}ms`;
            document.getElementById('confidence-score').textContent = 
                `${(response.confidence * 100).toFixed(1)}% confidence`;
            
            // Update response content
            document.getElementById('response-content').textContent = 
                response.result?.content || 'No content available';
        }
    }

    /**
     * Clear request form
     */
    clearRequestForm() {
        document.getElementById('ai-request-form').reset();
        document.getElementById('results-container').style.display = 'none';
    }

    /**
     * Load example request
     */
    loadExampleRequest() {
        document.getElementById('provider-select').value = 'openai-1';
        document.getElementById('task-type-select').value = 'vulnerability_analysis';
        document.getElementById('request-data').value = JSON.stringify({
            vulnerability: "SQL Injection",
            severity: "High",
            cvss_score: "9.8",
            affected_component: "User login form",
            description: "Application is vulnerable to SQL injection attacks in the authentication module"
        }, null, 2);
        document.getElementById('context-input').value = 
            "This is a production e-commerce application processing customer payments. Please provide detailed remediation steps.";
    }

    /**
     * Test provider
     */
    async testProvider(providerId) {
        try {
            componentsManager.showLoading('Testing provider...');
            const result = await apiClient.testProvider(providerId);
            componentsManager.showToast(`Provider test successful: ${result.response_time}ms`, 'success');
        } catch (error) {
            componentsManager.showToast(`Provider test failed: ${error.message}`, 'error');
        } finally {
            componentsManager.hideLoading();
        }
    }

    /**
     * Delete provider
     */
    deleteProvider(providerId) {
        const provider = this.providers.find(p => p.id === providerId);
        if (!provider) return;

        componentsManager.showConfirmDialog(
            `Are you sure you want to delete the provider "${provider.name}"?`,
            async () => {
                try {
                    await apiClient.deleteProvider(providerId);
                    await this.loadProviders();
                    componentsManager.showToast('Provider deleted successfully', 'success');
                } catch (error) {
                    componentsManager.showToast(`Failed to delete provider: ${error.message}`, 'error');
                }
            }
        );
    }

    /**
     * Handle save provider
     */
    async handleSaveProvider() {
        try {
            const form = document.getElementById('add-provider-form');
            if (!componentsManager.validateForm(form)) {
                return;
            }

            const providerData = {
                type: document.getElementById('provider-type').value,
                name: document.getElementById('provider-name').value,
                api_key: document.getElementById('api-key').value,
                endpoint: document.getElementById('endpoint-url').value,
                model: document.getElementById('model-name').value,
                timeout: parseInt(document.getElementById('timeout-seconds').value)
            };

            componentsManager.showLoading('Saving provider...');
            
            await apiClient.addProvider(providerData);
            await this.loadProviders();
            
            componentsManager.closeModal('add-provider-modal');
            form.reset();
            componentsManager.showToast('Provider added successfully', 'success');
            
        } catch (error) {
            componentsManager.showToast(`Failed to save provider: ${error.message}`, 'error');
        } finally {
            componentsManager.hideLoading();
        }
    }

    /**
     * Load section-specific data
     */
    async loadSectionData(section) {
        switch (section) {
            case 'dashboard':
                await this.loadMetrics();
                break;
            case 'providers':
                await this.loadProviders();
                break;
            case 'requests':
                await this.loadRequestHistory();
                break;
            case 'monitoring':
                await this.loadMonitoringData();
                break;
            case 'logs':
                await this.loadLogs();
                break;
        }
    }

    /**
     * Load monitoring data
     */
    async loadMonitoringData() {
        try {
            const metrics = await apiClient.getPerformanceMetrics();
            this.updateMonitoringDisplay(metrics);
        } catch (error) {
            console.error('Failed to load monitoring data:', error);
        }
    }

    /**
     * Update monitoring display
     */
    updateMonitoringDisplay(metrics) {
        // Update metrics table
        const tableBody = document.querySelector('#metrics-table tbody');
        if (tableBody) {
            const metricsData = [
                { name: 'Total Requests', value: metrics.total_requests || 0, change: '+12.5%', status: 'Good' },
                { name: 'Success Rate', value: `${(metrics.success_rate || 0).toFixed(1)}%`, change: '-0.3%', status: 'Good' },
                { name: 'Avg Response Time', value: `${Math.round(metrics.avg_response_time || 0)}ms`, change: '-8.2%', status: 'Good' },
                { name: 'Cache Hit Rate', value: `${(metrics.cache_hit_rate || 0).toFixed(1)}%`, change: '+5.1%', status: 'Good' }
            ];

            tableBody.innerHTML = metricsData.map(metric => `
                <tr>
                    <td>${metric.name}</td>
                    <td>${metric.value}</td>
                    <td class="${metric.change.startsWith('+') ? 'text-success' : 'text-error'}">${metric.change}</td>
                    <td><span class="status-badge running">${metric.status}</span></td>
                    <td>${new Date().toLocaleTimeString()}</td>
                </tr>
            `).join('');
        }
    }

    /**
     * Load logs
     */
    async loadLogs() {
        try {
            const response = await apiClient.getLogs();
            this.updateLogsDisplay(response.logs || []);
        } catch (error) {
            console.error('Failed to load logs:', error);
        }
    }

    /**
     * Update logs display
     */
    updateLogsDisplay(logs) {
        const logViewer = document.getElementById('log-viewer');
        if (logViewer) {
            logViewer.innerHTML = logs.map(log => `
                <div class="log-entry">
                    <span class="log-timestamp">${componentsManager.formatTimestamp(log.timestamp)}</span>
                    <span class="log-level ${log.level}">${log.level}</span>
                    <span class="log-message">${log.message}</span>
                </div>
            `).join('');
        }
    }

    /**
     * Start auto-refresh
     */
    startAutoRefresh() {
        this.refreshInterval = setInterval(() => {
            this.refreshCurrentSection();
        }, 30000); // Refresh every 30 seconds
    }

    /**
     * Refresh current section
     */
    async refreshCurrentSection() {
        try {
            await this.loadSectionData(this.currentSection);
        } catch (error) {
            console.error('Failed to refresh section:', error);
        }
    }

    /**
     * Handle clear logs
     */
    handleClearLogs() {
        componentsManager.showConfirmDialog(
            'Are you sure you want to clear all logs?',
            async () => {
                try {
                    await apiClient.clearLogs();
                    await this.loadLogs();
                    componentsManager.showToast('Logs cleared successfully', 'success');
                } catch (error) {
                    componentsManager.showToast(`Failed to clear logs: ${error.message}`, 'error');
                }
            }
        );
    }

    /**
     * Handle export logs
     */
    async handleExportLogs() {
        try {
            const logs = await apiClient.exportLogs('json');
            const blob = new Blob([JSON.stringify(logs, null, 2)], { type: 'application/json' });
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = `ai-openvas-logs-${new Date().toISOString().split('T')[0]}.json`;
            document.body.appendChild(a);
            a.click();
            document.body.removeChild(a);
            URL.revokeObjectURL(url);
            
            componentsManager.showToast('Logs exported successfully', 'success');
        } catch (error) {
            componentsManager.showToast(`Failed to export logs: ${error.message}`, 'error');
        }
    }
}

// Initialize application when DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
    window.app = new AIOpenVASApp();
    app.initialize();
});

// Handle page visibility changes
document.addEventListener('visibilitychange', () => {
    if (document.hidden) {
        // Page is hidden, pause updates
        if (window.app && window.app.refreshInterval) {
            clearInterval(window.app.refreshInterval);
        }
    } else {
        // Page is visible, resume updates
        if (window.app) {
            window.app.startAutoRefresh();
        }
    }
});