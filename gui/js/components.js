/**
 * AI-Enhanced OpenVAS GUI - Components Module
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

class ComponentsManager {
    constructor() {
        this.modals = {};
        this.toasts = [];
        this.loadingOverlay = null;
    }

    /**
     * Initialize all components
     */
    initialize() {
        this.initializeModals();
        this.initializeToasts();
        this.initializeLoadingOverlay();
        this.initializeTabs();
        this.initializeFormValidation();
    }

    /**
     * Initialize modal components
     */
    initializeModals() {
        const modals = document.querySelectorAll('.modal');
        modals.forEach(modal => {
            const modalId = modal.id;
            this.modals[modalId] = modal;

            // Close button handlers
            const closeButtons = modal.querySelectorAll('.modal-close, [data-modal]');
            closeButtons.forEach(btn => {
                btn.addEventListener('click', (e) => {
                    e.preventDefault();
                    this.closeModal(modalId);
                });
            });

            // Click outside to close
            modal.addEventListener('click', (e) => {
                if (e.target === modal) {
                    this.closeModal(modalId);
                }
            });
        });

        // ESC key to close modals
        document.addEventListener('keydown', (e) => {
            if (e.key === 'Escape') {
                this.closeAllModals();
            }
        });
    }

    /**
     * Open modal
     */
    openModal(modalId) {
        const modal = this.modals[modalId];
        if (modal) {
            modal.classList.add('active');
            document.body.style.overflow = 'hidden';
            
            // Focus first input
            const firstInput = modal.querySelector('input, select, textarea');
            if (firstInput) {
                setTimeout(() => firstInput.focus(), 100);
            }
        }
    }

    /**
     * Close modal
     */
    closeModal(modalId) {
        const modal = this.modals[modalId];
        if (modal) {
            modal.classList.remove('active');
            document.body.style.overflow = '';
        }
    }

    /**
     * Close all modals
     */
    closeAllModals() {
        Object.keys(this.modals).forEach(modalId => {
            this.closeModal(modalId);
        });
    }

    /**
     * Initialize toast notifications
     */
    initializeToasts() {
        this.toastContainer = document.getElementById('toast-container');
    }

    /**
     * Show toast notification
     */
    showToast(message, type = 'info', duration = 5000) {
        const toast = document.createElement('div');
        toast.className = `toast ${type}`;
        
        const icon = this.getToastIcon(type);
        
        toast.innerHTML = `
            <div class="toast-header">
                <div class="toast-title">
                    <i class="${icon}"></i>
                    ${this.getToastTitle(type)}
                </div>
                <button class="toast-close">
                    <i class="fas fa-times"></i>
                </button>
            </div>
            <div class="toast-message">${message}</div>
        `;

        // Close button handler
        const closeBtn = toast.querySelector('.toast-close');
        closeBtn.addEventListener('click', () => {
            this.removeToast(toast);
        });

        // Auto-remove after duration
        if (duration > 0) {
            setTimeout(() => {
                this.removeToast(toast);
            }, duration);
        }

        this.toastContainer.appendChild(toast);
        this.toasts.push(toast);

        // Limit number of toasts
        if (this.toasts.length > 5) {
            this.removeToast(this.toasts[0]);
        }
    }

    /**
     * Remove toast
     */
    removeToast(toast) {
        if (toast && toast.parentNode) {
            toast.style.animation = 'toastSlideOut 0.3s ease-in forwards';
            setTimeout(() => {
                if (toast.parentNode) {
                    toast.parentNode.removeChild(toast);
                }
                const index = this.toasts.indexOf(toast);
                if (index > -1) {
                    this.toasts.splice(index, 1);
                }
            }, 300);
        }
    }

    /**
     * Get toast icon based on type
     */
    getToastIcon(type) {
        const icons = {
            success: 'fas fa-check-circle',
            warning: 'fas fa-exclamation-triangle',
            error: 'fas fa-times-circle',
            info: 'fas fa-info-circle'
        };
        return icons[type] || icons.info;
    }

    /**
     * Get toast title based on type
     */
    getToastTitle(type) {
        const titles = {
            success: 'Success',
            warning: 'Warning',
            error: 'Error',
            info: 'Information'
        };
        return titles[type] || titles.info;
    }

    /**
     * Initialize loading overlay
     */
    initializeLoadingOverlay() {
        this.loadingOverlay = document.getElementById('loading-overlay');
    }

    /**
     * Show loading overlay
     */
    showLoading(message = 'Processing...') {
        if (this.loadingOverlay) {
            const messageElement = this.loadingOverlay.querySelector('p');
            if (messageElement) {
                messageElement.textContent = message;
            }
            this.loadingOverlay.classList.add('active');
        }
    }

    /**
     * Hide loading overlay
     */
    hideLoading() {
        if (this.loadingOverlay) {
            this.loadingOverlay.classList.remove('active');
        }
    }

    /**
     * Initialize tab components
     */
    initializeTabs() {
        const tabButtons = document.querySelectorAll('.tab-btn');
        tabButtons.forEach(btn => {
            btn.addEventListener('click', (e) => {
                e.preventDefault();
                const tabName = btn.dataset.tab;
                this.switchTab(tabName, btn.parentElement);
            });
        });
    }

    /**
     * Switch tab
     */
    switchTab(tabName, tabContainer) {
        // Update tab buttons
        const tabButtons = tabContainer.querySelectorAll('.tab-btn');
        tabButtons.forEach(btn => {
            btn.classList.remove('active');
            if (btn.dataset.tab === tabName) {
                btn.classList.add('active');
            }
        });

        // Update tab panels
        const tabPanels = document.querySelectorAll('.config-panel');
        tabPanels.forEach(panel => {
            panel.classList.remove('active');
            if (panel.id === `${tabName}-config`) {
                panel.classList.add('active');
            }
        });
    }

    /**
     * Initialize form validation
     */
    initializeFormValidation() {
        const forms = document.querySelectorAll('form');
        forms.forEach(form => {
            form.addEventListener('submit', (e) => {
                if (!this.validateForm(form)) {
                    e.preventDefault();
                }
            });

            // Real-time validation
            const inputs = form.querySelectorAll('input, select, textarea');
            inputs.forEach(input => {
                input.addEventListener('blur', () => {
                    this.validateField(input);
                });
            });
        });
    }

    /**
     * Validate form
     */
    validateForm(form) {
        let isValid = true;
        const inputs = form.querySelectorAll('input[required], select[required], textarea[required]');
        
        inputs.forEach(input => {
            if (!this.validateField(input)) {
                isValid = false;
            }
        });

        return isValid;
    }

    /**
     * Validate individual field
     */
    validateField(field) {
        const value = field.value.trim();
        let isValid = true;
        let errorMessage = '';

        // Required field validation
        if (field.hasAttribute('required') && !value) {
            isValid = false;
            errorMessage = 'This field is required';
        }

        // Type-specific validation
        if (value && field.type === 'email') {
            const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
            if (!emailRegex.test(value)) {
                isValid = false;
                errorMessage = 'Please enter a valid email address';
            }
        }

        if (value && field.type === 'url') {
            try {
                new URL(value);
            } catch {
                isValid = false;
                errorMessage = 'Please enter a valid URL';
            }
        }

        if (value && field.type === 'number') {
            const min = field.getAttribute('min');
            const max = field.getAttribute('max');
            const numValue = parseFloat(value);
            
            if (min && numValue < parseFloat(min)) {
                isValid = false;
                errorMessage = `Value must be at least ${min}`;
            }
            
            if (max && numValue > parseFloat(max)) {
                isValid = false;
                errorMessage = `Value must be at most ${max}`;
            }
        }

        // Update field appearance
        this.updateFieldValidation(field, isValid, errorMessage);
        
        return isValid;
    }

    /**
     * Update field validation appearance
     */
    updateFieldValidation(field, isValid, errorMessage) {
        // Remove existing validation classes
        field.classList.remove('valid', 'invalid');
        
        // Remove existing error message
        const existingError = field.parentNode.querySelector('.field-error');
        if (existingError) {
            existingError.remove();
        }

        if (!isValid) {
            field.classList.add('invalid');
            
            // Add error message
            const errorElement = document.createElement('div');
            errorElement.className = 'field-error';
            errorElement.textContent = errorMessage;
            errorElement.style.color = 'var(--error-color)';
            errorElement.style.fontSize = '0.75rem';
            errorElement.style.marginTop = 'var(--spacing-xs)';
            
            field.parentNode.appendChild(errorElement);
        } else if (field.value.trim()) {
            field.classList.add('valid');
        }
    }

    /**
     * Create confirmation dialog
     */
    showConfirmDialog(message, onConfirm, onCancel = null) {
        const dialog = document.createElement('div');
        dialog.className = 'modal active';
        dialog.innerHTML = `
            <div class="modal-content">
                <div class="modal-header">
                    <h3>Confirm Action</h3>
                </div>
                <div class="modal-body">
                    <p>${message}</p>
                </div>
                <div class="modal-footer">
                    <button class="btn btn-secondary" id="cancel-btn">Cancel</button>
                    <button class="btn btn-primary" id="confirm-btn">Confirm</button>
                </div>
            </div>
        `;

        document.body.appendChild(dialog);
        document.body.style.overflow = 'hidden';

        const confirmBtn = dialog.querySelector('#confirm-btn');
        const cancelBtn = dialog.querySelector('#cancel-btn');

        const cleanup = () => {
            document.body.removeChild(dialog);
            document.body.style.overflow = '';
        };

        confirmBtn.addEventListener('click', () => {
            cleanup();
            if (onConfirm) onConfirm();
        });

        cancelBtn.addEventListener('click', () => {
            cleanup();
            if (onCancel) onCancel();
        });

        // ESC key to cancel
        const escHandler = (e) => {
            if (e.key === 'Escape') {
                cleanup();
                if (onCancel) onCancel();
                document.removeEventListener('keydown', escHandler);
            }
        };
        document.addEventListener('keydown', escHandler);
    }

    /**
     * Format bytes for display
     */
    formatBytes(bytes) {
        if (bytes === 0) return '0 Bytes';
        const k = 1024;
        const sizes = ['Bytes', 'KB', 'MB', 'GB'];
        const i = Math.floor(Math.log(bytes) / Math.log(k));
        return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
    }

    /**
     * Format duration for display
     */
    formatDuration(ms) {
        if (ms < 1000) return `${ms}ms`;
        if (ms < 60000) return `${(ms / 1000).toFixed(1)}s`;
        if (ms < 3600000) return `${(ms / 60000).toFixed(1)}m`;
        return `${(ms / 3600000).toFixed(1)}h`;
    }

    /**
     * Format timestamp for display
     */
    formatTimestamp(timestamp) {
        const date = new Date(timestamp);
        return date.toLocaleString();
    }

    /**
     * Debounce function
     */
    debounce(func, wait) {
        let timeout;
        return function executedFunction(...args) {
            const later = () => {
                clearTimeout(timeout);
                func(...args);
            };
            clearTimeout(timeout);
            timeout = setTimeout(later, wait);
        };
    }

    /**
     * Copy text to clipboard
     */
    async copyToClipboard(text) {
        try {
            await navigator.clipboard.writeText(text);
            this.showToast('Copied to clipboard', 'success', 2000);
        } catch (err) {
            // Fallback for older browsers
            const textArea = document.createElement('textarea');
            textArea.value = text;
            document.body.appendChild(textArea);
            textArea.select();
            document.execCommand('copy');
            document.body.removeChild(textArea);
            this.showToast('Copied to clipboard', 'success', 2000);
        }
    }
}

// Add CSS for toast slide out animation
const style = document.createElement('style');
style.textContent = `
    @keyframes toastSlideOut {
        from { transform: translateX(0); opacity: 1; }
        to { transform: translateX(100%); opacity: 0; }
    }
    
    .field-error {
        color: var(--error-color);
        font-size: 0.75rem;
        margin-top: var(--spacing-xs);
    }
    
    .invalid {
        border-color: var(--error-color) !important;
        box-shadow: 0 0 0 3px rgba(239, 68, 68, 0.1) !important;
    }
    
    .valid {
        border-color: var(--success-color) !important;
        box-shadow: 0 0 0 3px rgba(16, 185, 129, 0.1) !important;
    }
`;
document.head.appendChild(style);

// Initialize components manager
const componentsManager = new ComponentsManager();

// Export for use in other modules
window.componentsManager = componentsManager;