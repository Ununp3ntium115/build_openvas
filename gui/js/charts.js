/**
 * AI-Enhanced OpenVAS GUI - Charts Module
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

class ChartsManager {
    constructor() {
        this.charts = {};
        this.defaultOptions = {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: {
                    position: 'bottom',
                    labels: {
                        usePointStyle: true,
                        padding: 20
                    }
                }
            },
            scales: {
                y: {
                    beginAtZero: true,
                    grid: {
                        color: 'rgba(0, 0, 0, 0.1)'
                    }
                },
                x: {
                    grid: {
                        color: 'rgba(0, 0, 0, 0.1)'
                    }
                }
            }
        };
    }

    /**
     * Initialize all charts
     */
    initializeCharts() {
        this.createRequestsChart();
        this.createProvidersChart();
        this.createRealtimeChart();
        this.createErrorChart();
    }

    /**
     * Create requests volume chart
     */
    createRequestsChart() {
        const ctx = document.getElementById('requests-chart');
        if (!ctx) return;

        // Generate sample data for the last 24 hours
        const hours = [];
        const requestData = [];
        const successData = [];
        const errorData = [];

        for (let i = 23; i >= 0; i--) {
            const hour = new Date();
            hour.setHours(hour.getHours() - i);
            hours.push(hour.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' }));
            
            const requests = Math.floor(Math.random() * 50) + 10;
            const errors = Math.floor(requests * (Math.random() * 0.1));
            const success = requests - errors;
            
            requestData.push(requests);
            successData.push(success);
            errorData.push(errors);
        }

        this.charts.requests = new Chart(ctx, {
            type: 'line',
            data: {
                labels: hours,
                datasets: [
                    {
                        label: 'Total Requests',
                        data: requestData,
                        borderColor: '#3b82f6',
                        backgroundColor: 'rgba(59, 130, 246, 0.1)',
                        fill: true,
                        tension: 0.4
                    },
                    {
                        label: 'Successful',
                        data: successData,
                        borderColor: '#10b981',
                        backgroundColor: 'rgba(16, 185, 129, 0.1)',
                        fill: false,
                        tension: 0.4
                    },
                    {
                        label: 'Errors',
                        data: errorData,
                        borderColor: '#ef4444',
                        backgroundColor: 'rgba(239, 68, 68, 0.1)',
                        fill: false,
                        tension: 0.4
                    }
                ]
            },
            options: {
                ...this.defaultOptions,
                plugins: {
                    ...this.defaultOptions.plugins,
                    title: {
                        display: false
                    }
                },
                scales: {
                    ...this.defaultOptions.scales,
                    y: {
                        ...this.defaultOptions.scales.y,
                        title: {
                            display: true,
                            text: 'Requests'
                        }
                    }
                }
            }
        });
    }

    /**
     * Create providers performance chart
     */
    createProvidersChart() {
        const ctx = document.getElementById('providers-chart');
        if (!ctx) return;

        this.charts.providers = new Chart(ctx, {
            type: 'doughnut',
            data: {
                labels: ['OpenAI GPT-4', 'Claude 3', 'Custom Model', 'Local Model'],
                datasets: [{
                    data: [45, 30, 15, 10],
                    backgroundColor: [
                        '#10a37f',
                        '#ff6b35',
                        '#667eea',
                        '#f093fb'
                    ],
                    borderWidth: 2,
                    borderColor: '#ffffff'
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: {
                        position: 'bottom',
                        labels: {
                            usePointStyle: true,
                            padding: 15
                        }
                    },
                    tooltip: {
                        callbacks: {
                            label: function(context) {
                                return `${context.label}: ${context.parsed}%`;
                            }
                        }
                    }
                }
            }
        });
    }

    /**
     * Create real-time performance chart
     */
    createRealtimeChart() {
        const ctx = document.getElementById('realtime-chart');
        if (!ctx) return;

        const initialData = Array(20).fill(0).map(() => Math.random() * 100);

        this.charts.realtime = new Chart(ctx, {
            type: 'line',
            data: {
                labels: Array(20).fill('').map((_, i) => `${i * 3}s`),
                datasets: [
                    {
                        label: 'Response Time (ms)',
                        data: initialData.map(v => v * 30 + 500),
                        borderColor: '#3b82f6',
                        backgroundColor: 'rgba(59, 130, 246, 0.1)',
                        fill: true,
                        tension: 0.4,
                        pointRadius: 0
                    },
                    {
                        label: 'CPU Usage (%)',
                        data: initialData.map(v => v * 0.8),
                        borderColor: '#10b981',
                        backgroundColor: 'rgba(16, 185, 129, 0.1)',
                        fill: false,
                        tension: 0.4,
                        pointRadius: 0
                    }
                ]
            },
            options: {
                ...this.defaultOptions,
                animation: false,
                scales: {
                    y: {
                        beginAtZero: true,
                        max: 3000,
                        grid: {
                            color: 'rgba(0, 0, 0, 0.1)'
                        }
                    },
                    x: {
                        grid: {
                            color: 'rgba(0, 0, 0, 0.1)'
                        }
                    }
                }
            }
        });

        // Start real-time updates
        this.startRealtimeUpdates();
    }

    /**
     * Create error rate chart
     */
    createErrorChart() {
        const ctx = document.getElementById('error-chart');
        if (!ctx) return;

        const days = [];
        const errorRates = [];

        for (let i = 6; i >= 0; i--) {
            const date = new Date();
            date.setDate(date.getDate() - i);
            days.push(date.toLocaleDateString([], { month: 'short', day: 'numeric' }));
            errorRates.push(Math.random() * 5 + 1); // 1-6% error rate
        }

        this.charts.errors = new Chart(ctx, {
            type: 'bar',
            data: {
                labels: days,
                datasets: [{
                    label: 'Error Rate (%)',
                    data: errorRates,
                    backgroundColor: errorRates.map(rate => 
                        rate > 4 ? '#ef4444' : rate > 2 ? '#f59e0b' : '#10b981'
                    ),
                    borderColor: errorRates.map(rate => 
                        rate > 4 ? '#dc2626' : rate > 2 ? '#d97706' : '#059669'
                    ),
                    borderWidth: 1
                }]
            },
            options: {
                ...this.defaultOptions,
                scales: {
                    y: {
                        beginAtZero: true,
                        max: 10,
                        title: {
                            display: true,
                            text: 'Error Rate (%)'
                        },
                        grid: {
                            color: 'rgba(0, 0, 0, 0.1)'
                        }
                    },
                    x: {
                        grid: {
                            color: 'rgba(0, 0, 0, 0.1)'
                        }
                    }
                },
                plugins: {
                    legend: {
                        display: false
                    },
                    tooltip: {
                        callbacks: {
                            label: function(context) {
                                return `Error Rate: ${context.parsed.y.toFixed(2)}%`;
                            }
                        }
                    }
                }
            }
        });
    }

    /**
     * Start real-time chart updates
     */
    startRealtimeUpdates() {
        if (!this.charts.realtime) return;

        setInterval(() => {
            const chart = this.charts.realtime;
            const responseTime = Math.random() * 1000 + 500;
            const cpuUsage = Math.random() * 80 + 10;

            // Add new data point
            chart.data.datasets[0].data.push(responseTime);
            chart.data.datasets[1].data.push(cpuUsage);

            // Remove old data point
            if (chart.data.datasets[0].data.length > 20) {
                chart.data.datasets[0].data.shift();
                chart.data.datasets[1].data.shift();
            }

            chart.update('none');
        }, 3000);
    }

    /**
     * Update chart data
     */
    updateChart(chartName, data) {
        const chart = this.charts[chartName];
        if (!chart) return;

        switch (chartName) {
            case 'requests':
                this.updateRequestsChart(chart, data);
                break;
            case 'providers':
                this.updateProvidersChart(chart, data);
                break;
            case 'errors':
                this.updateErrorChart(chart, data);
                break;
        }
    }

    /**
     * Update requests chart with new data
     */
    updateRequestsChart(chart, data) {
        if (data.hourly) {
            chart.data.labels = data.hourly.map(item => item.hour);
            chart.data.datasets[0].data = data.hourly.map(item => item.total);
            chart.data.datasets[1].data = data.hourly.map(item => item.success);
            chart.data.datasets[2].data = data.hourly.map(item => item.errors);
            chart.update();
        }
    }

    /**
     * Update providers chart with new data
     */
    updateProvidersChart(chart, data) {
        if (data.providers) {
            chart.data.labels = data.providers.map(p => p.name);
            chart.data.datasets[0].data = data.providers.map(p => p.usage_percentage);
            chart.update();
        }
    }

    /**
     * Update error chart with new data
     */
    updateErrorChart(chart, data) {
        if (data.daily) {
            chart.data.labels = data.daily.map(item => item.date);
            chart.data.datasets[0].data = data.daily.map(item => item.error_rate);
            chart.data.datasets[0].backgroundColor = data.daily.map(item => 
                item.error_rate > 4 ? '#ef4444' : item.error_rate > 2 ? '#f59e0b' : '#10b981'
            );
            chart.update();
        }
    }

    /**
     * Destroy all charts
     */
    destroyCharts() {
        Object.values(this.charts).forEach(chart => {
            if (chart) chart.destroy();
        });
        this.charts = {};
    }

    /**
     * Resize charts
     */
    resizeCharts() {
        Object.values(this.charts).forEach(chart => {
            if (chart) chart.resize();
        });
    }
}

// Initialize charts manager
const chartsManager = new ChartsManager();

// Export for use in other modules
window.chartsManager = chartsManager;