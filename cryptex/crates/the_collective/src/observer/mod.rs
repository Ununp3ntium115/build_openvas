//! The Observer - Metrics and observability for The Collective
//!
//! Traditional name: `Metrics` or `Monitor`
//!
//! Tracks performance metrics, request statistics, and system health.

use std::sync::atomic::{AtomicU64, Ordering};
use std::sync::Arc;
use std::time::Duration;
use tokio::sync::RwLock;

/// The Observer - Metrics collection and monitoring
///
/// Traditional name: `MetricsCollector` or `Monitor`
///
/// Tracks various metrics about AI inquiries, responses, and system performance.
pub struct TheObserver {
    inquiries_total: Arc<AtomicU64>,
    inquiries_success: Arc<AtomicU64>,
    inquiries_failure: Arc<AtomicU64>,
    cache_hits: Arc<AtomicU64>,
    cache_misses: Arc<AtomicU64>,
    total_duration_ms: Arc<AtomicU64>,
    agitator_stats: Arc<RwLock<AgitatorStatistics>>,
}

/// Statistics per agitator (AI provider)
#[derive(Debug, Clone, Default)]
struct AgitatorStatistics {
    openai_requests: u64,
    openai_successes: u64,
    openai_failures: u64,
    claude_requests: u64,
    claude_successes: u64,
    claude_failures: u64,
}

impl TheObserver {
    /// The Awakening - Initialize the observer
    ///
    /// Traditional name: `new` or `init`
    pub fn the_awakening() -> Self {
        tracing::info!("The Observer awakening - metrics collection enabled");

        Self {
            inquiries_total: Arc::new(AtomicU64::new(0)),
            inquiries_success: Arc::new(AtomicU64::new(0)),
            inquiries_failure: Arc::new(AtomicU64::new(0)),
            cache_hits: Arc::new(AtomicU64::new(0)),
            cache_misses: Arc::new(AtomicU64::new(0)),
            total_duration_ms: Arc::new(AtomicU64::new(0)),
            agitator_stats: Arc::new(RwLock::new(AgitatorStatistics::default())),
        }
    }

    /// Record the start of an inquiry
    ///
    /// Traditional name: `record_request_start`
    pub fn record_inquiry_start(&self, _inquiry_id: &str) {
        self.inquiries_total.fetch_add(1, Ordering::Relaxed);
        tracing::trace!("Inquiry started");
    }

    /// Record successful completion of an inquiry
    ///
    /// Traditional name: `record_request_complete`
    pub fn record_inquiry_complete(&self, _inquiry_id: &str, duration: Duration, _certainty: f64) {
        self.inquiries_success.fetch_add(1, Ordering::Relaxed);
        self.total_duration_ms
            .fetch_add(duration.as_millis() as u64, Ordering::Relaxed);

        tracing::trace!("Inquiry completed in {}ms", duration.as_millis());
    }

    /// Record failed inquiry
    ///
    /// Traditional name: `record_request_failed`
    pub fn record_inquiry_failed(&self, _inquiry_id: &str, _error: &str) {
        self.inquiries_failure.fetch_add(1, Ordering::Relaxed);
        tracing::trace!("Inquiry failed");
    }

    /// Record cache hit
    pub fn record_cache_hit(&self) {
        self.cache_hits.fetch_add(1, Ordering::Relaxed);
        tracing::trace!("Cache hit");
    }

    /// Record cache miss
    pub fn record_cache_miss(&self) {
        self.cache_misses.fetch_add(1, Ordering::Relaxed);
        tracing::trace!("Cache miss");
    }

    /// Record agitator (AI provider) usage
    pub async fn record_agitator_request(&self, agitator: &str, success: bool) {
        let mut stats = self.agitator_stats.write().await;

        match agitator.to_lowercase().as_str() {
            "openai" => {
                stats.openai_requests += 1;
                if success {
                    stats.openai_successes += 1;
                } else {
                    stats.openai_failures += 1;
                }
            }
            "claude" => {
                stats.claude_requests += 1;
                if success {
                    stats.claude_successes += 1;
                } else {
                    stats.claude_failures += 1;
                }
            }
            _ => {}
        }
    }

    /// Get current metrics snapshot
    ///
    /// Traditional name: `get_metrics` or `collect_metrics`
    pub async fn the_metrics(&self) -> ObserverMetrics {
        let total = self.inquiries_total.load(Ordering::Relaxed);
        let success = self.inquiries_success.load(Ordering::Relaxed);
        let failure = self.inquiries_failure.load(Ordering::Relaxed);
        let cache_hits = self.cache_hits.load(Ordering::Relaxed);
        let cache_misses = self.cache_misses.load(Ordering::Relaxed);
        let total_duration = self.total_duration_ms.load(Ordering::Relaxed);

        let success_rate = if total > 0 {
            (success as f64 / total as f64) * 100.0
        } else {
            0.0
        };

        let cache_hit_rate = if cache_hits + cache_misses > 0 {
            (cache_hits as f64 / (cache_hits + cache_misses) as f64) * 100.0
        } else {
            0.0
        };

        let avg_duration_ms = if success > 0 {
            total_duration / success
        } else {
            0
        };

        let agitator_stats = self.agitator_stats.read().await.clone();

        ObserverMetrics {
            inquiries_total: total,
            inquiries_success: success,
            inquiries_failure: failure,
            success_rate,
            cache_hits,
            cache_misses,
            cache_hit_rate,
            avg_response_time_ms: avg_duration_ms,
            openai_requests: agitator_stats.openai_requests,
            openai_success_rate: calculate_success_rate(
                agitator_stats.openai_successes,
                agitator_stats.openai_requests,
            ),
            claude_requests: agitator_stats.claude_requests,
            claude_success_rate: calculate_success_rate(
                agitator_stats.claude_successes,
                agitator_stats.claude_requests,
            ),
        }
    }

    /// Record final metrics (called during shutdown)
    ///
    /// Traditional name: `record_shutdown_metrics`
    pub fn record_final_metrics(&self) {
        let total = self.inquiries_total.load(Ordering::Relaxed);
        let success = self.inquiries_success.load(Ordering::Relaxed);
        let failure = self.inquiries_failure.load(Ordering::Relaxed);

        tracing::info!(
            "The Observer final metrics: total={}, success={}, failure={}",
            total,
            success,
            failure
        );
    }
}

/// Calculate success rate percentage
fn calculate_success_rate(successes: u64, total: u64) -> f64 {
    if total > 0 {
        (successes as f64 / total as f64) * 100.0
    } else {
        0.0
    }
}

/// Metrics snapshot
#[derive(Debug, Clone, serde::Serialize)]
pub struct ObserverMetrics {
    pub inquiries_total: u64,
    pub inquiries_success: u64,
    pub inquiries_failure: u64,
    pub success_rate: f64,
    pub cache_hits: u64,
    pub cache_misses: u64,
    pub cache_hit_rate: f64,
    pub avg_response_time_ms: u64,
    pub openai_requests: u64,
    pub openai_success_rate: f64,
    pub claude_requests: u64,
    pub claude_success_rate: f64,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_observer_basic_metrics() {
        let observer = TheObserver::the_awakening();

        observer.record_inquiry_start("test-1");
        observer.record_inquiry_complete("test-1", Duration::from_millis(1500), 0.95);

        let metrics = observer.the_metrics().await;

        assert_eq!(metrics.inquiries_total, 1);
        assert_eq!(metrics.inquiries_success, 1);
        assert_eq!(metrics.success_rate, 100.0);
        assert_eq!(metrics.avg_response_time_ms, 1500);
    }

    #[tokio::test]
    async fn test_observer_failure_tracking() {
        let observer = TheObserver::the_awakening();

        observer.record_inquiry_start("test-1");
        observer.record_inquiry_failed("test-1", "timeout");

        let metrics = observer.the_metrics().await;

        assert_eq!(metrics.inquiries_failure, 1);
        assert_eq!(metrics.success_rate, 0.0);
    }

    #[tokio::test]
    async fn test_observer_cache_metrics() {
        let observer = TheObserver::the_awakening();

        observer.record_cache_hit();
        observer.record_cache_hit();
        observer.record_cache_miss();

        let metrics = observer.the_metrics().await;

        assert_eq!(metrics.cache_hits, 2);
        assert_eq!(metrics.cache_misses, 1);
        assert!((metrics.cache_hit_rate - 66.67).abs() < 0.1);
    }

    #[tokio::test]
    async fn test_observer_agitator_stats() {
        let observer = TheObserver::the_awakening();

        observer.record_agitator_request("openai", true).await;
        observer.record_agitator_request("openai", true).await;
        observer.record_agitator_request("openai", false).await;

        observer.record_agitator_request("claude", true).await;

        let metrics = observer.the_metrics().await;

        assert_eq!(metrics.openai_requests, 3);
        assert!((metrics.openai_success_rate - 66.67).abs() < 0.1);
        assert_eq!(metrics.claude_requests, 1);
        assert_eq!(metrics.claude_success_rate, 100.0);
    }
}
