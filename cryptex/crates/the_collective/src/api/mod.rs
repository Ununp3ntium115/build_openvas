//! The Collective - Core AI Service API
//!
//! Coordinates AI providers, caching, and metrics to provide AI-enhanced
//! security assessment capabilities.

use crate::{
    memory::TheMemory,
    observer::TheObserver,
};
use the_foundation::{
    AgitatorType, CryptexError, CryptexResult, TheAgitator, TheInquiry, ThePackage, TheReply,
};
use std::sync::Arc;
use std::time::Instant;
use the_agitator::{ClaudeAgitator, OpenAIAgitator};
use the_coordinator::TheCharter;

/// The Collective - Main AI service coordination
///
/// Traditional name: `AIService` or `ServiceCore`
///
/// Coordinates all AI operations, including provider selection, caching,
/// and metrics collection.
pub struct TheCollective {
    charter: Arc<TheCharter>,
    memory: Arc<TheMemory>,
    observer: Arc<TheObserver>,
    agitators: Vec<Arc<dyn TheAgitator>>,
}

impl TheCollective {
    /// The Awakening - Initialize the collective
    ///
    /// Traditional name: `init` or `initialize`
    ///
    /// Creates a new instance of The Collective with all subsystems initialized.
    pub async fn the_awakening(charter: TheCharter) -> CryptexResult<Self> {
        tracing::info!("The Collective is awakening...");

        // Validate charter
        charter.validate().map_err(|e| CryptexError::charter(e))?;

        // Initialize The Memory (cache)
        let memory = Arc::new(TheMemory::the_awakening(
            charter.memory.max_size_mb,
            charter.memory.ttl_seconds,
        )?);

        // Initialize The Observer (metrics)
        let observer = Arc::new(TheObserver::the_awakening());

        // Initialize Agitators (AI Providers)
        let mut agitators: Vec<Arc<dyn TheAgitator>> = Vec::new();

        for agitator_config in &charter.agitators {
            if !agitator_config.enabled {
                tracing::debug!(
                    "Skipping disabled agitator: {}",
                    agitator_config.name
                );
                continue;
            }

            // Get API key from config or environment
            let api_key = if let Some(ref key) = agitator_config.api_key {
                key.clone()
            } else if let Some(ref env_var) = agitator_config.api_key_env {
                std::env::var(env_var).map_err(|_| {
                    CryptexError::charter(format!(
                        "API key environment variable '{}' not found for agitator '{}'",
                        env_var, agitator_config.name
                    ))
                })?
            } else {
                return Err(CryptexError::charter(format!(
                    "No API key configured for agitator '{}'",
                    agitator_config.name
                )));
            };

            let model = agitator_config.model.clone().unwrap_or_else(|| {
                match agitator_config.agitator_type.as_str() {
                    "openai" => "gpt-4".to_string(),
                    "claude" => "claude-3-sonnet-20240229".to_string(),
                    _ => "unknown".to_string(),
                }
            });

            match agitator_config.agitator_type.as_str() {
                "openai" => {
                    let agitator = OpenAIAgitator::the_awakening(api_key, model)
                        .await
                        .map_err(|e| {
                            CryptexError::agitator(format!(
                                "Failed to initialize OpenAI agitator '{}': {}",
                                agitator_config.name, e
                            ))
                        })?
                        .with_timeout(agitator_config.timeout_seconds);

                    agitators.push(Arc::new(agitator));
                    tracing::info!(
                        "OpenAI agitator '{}' awakened successfully",
                        agitator_config.name
                    );
                }
                "claude" => {
                    let agitator = ClaudeAgitator::the_awakening(api_key, model)
                        .await
                        .map_err(|e| {
                            CryptexError::agitator(format!(
                                "Failed to initialize Claude agitator '{}': {}",
                                agitator_config.name, e
                            ))
                        })?
                        .with_timeout(agitator_config.timeout_seconds);

                    agitators.push(Arc::new(agitator));
                    tracing::info!(
                        "Claude agitator '{}' awakened successfully",
                        agitator_config.name
                    );
                }
                _ => {
                    tracing::warn!(
                        "Unknown agitator type '{}' for agitator '{}', skipping",
                        agitator_config.agitator_type,
                        agitator_config.name
                    );
                }
            }
        }

        if agitators.is_empty() {
            tracing::warn!("No agitators initialized - AI features will be unavailable");
        }

        tracing::info!(
            "The Collective has awakened with {} active agitators",
            agitators.len()
        );

        Ok(Self {
            charter: Arc::new(charter),
            memory,
            observer,
            agitators,
        })
    }

    /// The Inquiry (Synchronous) - Process an AI inquiry
    ///
    /// Traditional name: `process_request` or `handle_request`
    ///
    /// Processes an inquiry through the AI collective, utilizing caching
    /// and metrics collection.
    pub async fn the_inquiry_sync(&self, inquiry: TheInquiry) -> CryptexResult<TheReply> {
        tracing::debug!("Processing inquiry: {}", inquiry.id);

        // Record inquiry start
        self.observer.record_inquiry_start(&inquiry.id);
        let start_time = Instant::now();

        // Check cache first
        let fingerprint = inquiry.fingerprint();
        if let Some(cached_reply) = self.memory.retrieve(&fingerprint).await? {
            tracing::debug!("Cache hit for inquiry: {}", inquiry.id);
            self.observer.record_cache_hit();
            self.observer
                .record_inquiry_complete(&inquiry.id, start_time.elapsed(), cached_reply.certainty);
            return Ok(cached_reply);
        }

        self.observer.record_cache_miss();

        // Check if we have any agitators available
        if self.agitators.is_empty() {
            let error_msg = "No agitators available - please configure AI providers";
            self.observer.record_inquiry_failed(&inquiry.id, error_msg);
            return Err(CryptexError::agitator(error_msg));
        }

        // Select agitator based on preference or default to first available
        let agitator = if let Some(preferred) = &inquiry.preferred_agitator {
            // Find agitator matching the preferred type
            self.agitators
                .iter()
                .find(|a| a.agitator_type() == *preferred)
                .ok_or_else(|| {
                    CryptexError::agitator(format!(
                        "Preferred agitator type {:?} not available",
                        preferred
                    ))
                })?
        } else {
            // Use first available agitator
            &self.agitators[0]
        };

        tracing::debug!(
            "Selected agitator: {:?} ({})",
            agitator.agitator_type(),
            agitator.the_mind()
        );

        // Convert inquiry to package
        let package = ThePackage {
            data: inquiry.package.data.clone(),
            context: inquiry.package.context.clone(),
        };

        // Send to agitator
        let agitator_name = format!("{:?}", agitator.agitator_type());
        let result = agitator.the_agitation(package).await;

        // Record agitator metrics
        self.observer
            .record_agitator_request(&agitator_name, result.is_ok())
            .await;

        match result {
            Ok(mut reply) => {
                // Update reply with inquiry ID
                reply.inquiry_id = inquiry.id.clone();

                let certainty = reply.certainty;
                let duration = start_time.elapsed();

                // Cache reply if certainty meets threshold
                if certainty >= self.charter.cache_threshold {
                    if let Err(e) = self.memory.store(fingerprint, reply.clone()).await {
                        tracing::warn!("Failed to cache reply: {}", e);
                    } else {
                        tracing::debug!("Reply cached with certainty {:.2}", certainty);
                    }
                }

                // Record successful inquiry
                self.observer
                    .record_inquiry_complete(&inquiry.id, duration, certainty);

                tracing::info!(
                    "Inquiry {} completed successfully in {:?} with certainty {:.2}",
                    inquiry.id,
                    duration,
                    certainty
                );

                Ok(reply)
            }
            Err(e) => {
                let error_msg = e.to_string();
                self.observer.record_inquiry_failed(&inquiry.id, &error_msg);

                tracing::error!("Inquiry {} failed: {}", inquiry.id, error_msg);

                Err(e)
            }
        }
    }

    /// The Inquiry (Asynchronous) - Process inquiry with callback
    ///
    /// Traditional name: `process_async`
    ///
    /// Processes an inquiry asynchronously and invokes a callback with the result.
    pub async fn the_inquiry_async<F>(
        &self,
        inquiry: TheInquiry,
        callback: F,
    ) -> CryptexResult<()>
    where
        F: FnOnce(CryptexResult<TheReply>) + Send + 'static,
    {
        let collective = self.clone_arc();
        let inquiry_clone = inquiry.clone();

        tokio::spawn(async move {
            let result = collective.the_inquiry_sync(inquiry_clone).await;
            callback(result);
        });

        Ok(())
    }

    /// Get current metrics
    ///
    /// Traditional name: `get_metrics`
    pub async fn the_metrics(&self) -> crate::observer::ObserverMetrics {
        self.observer.the_metrics().await
    }

    /// Get cache statistics
    pub async fn the_memory_statistics(&self) -> crate::memory::CacheStatistics {
        self.memory.statistics().await
    }

    /// The Rest - Graceful shutdown
    ///
    /// Traditional name: `shutdown` or `cleanup`
    ///
    /// Shuts down The Collective gracefully, flushing caches and recording
    /// final metrics.
    pub async fn the_rest(self) -> CryptexResult<()> {
        tracing::info!("The Collective is entering rest state");

        // Flush memory
        self.memory.the_flush().await?;

        // Record final metrics
        self.observer.record_final_metrics();

        tracing::info!("The Collective has entered rest");

        Ok(())
    }

    /// Helper to clone Arc references for async operations
    fn clone_arc(&self) -> Arc<Self> {
        Arc::new(Self {
            charter: Arc::clone(&self.charter),
            memory: Arc::clone(&self.memory),
            observer: Arc::clone(&self.observer),
            agitators: self.agitators.clone(),
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use the_foundation::TaskType;
    use the_coordinator::{
        AgitatorConfig, ArchiveConfig, CommuneConfig, MemoryConfig,
    };

    fn create_test_charter() -> TheCharter {
        TheCharter {
            commune: CommuneConfig {
                name: "Test Commune".to_string(),
                host: "127.0.0.1".to_string(),
                port: 8080,
                ws_port: 9090,
            },
            agitators: vec![AgitatorConfig {
                name: "test-openai".to_string(),
                agitator_type: "openai".to_string(),
                enabled: true,
                api_key: Some("test-key".to_string()),
                api_key_env: None,
                model: Some("gpt-4".to_string()),
                timeout_seconds: 30,
            }],
            archive: ArchiveConfig {
                path: "./test-archive.redb".into(),
                max_size_gb: 1,
            },
            memory: MemoryConfig {
                max_size_mb: 256,
                ttl_seconds: 3600,
            },
            infiltrator: None,
            cache_threshold: 0.8,
        }
    }

    #[tokio::test]
    async fn test_collective_awakening() {
        let charter = create_test_charter();
        let collective = TheCollective::the_awakening(charter).await;

        assert!(collective.is_ok());
    }

    #[tokio::test]
    async fn test_collective_metrics() {
        let charter = create_test_charter();
        let collective = TheCollective::the_awakening(charter).await.unwrap();

        let metrics = collective.the_metrics().await;

        assert_eq!(metrics.inquiries_total, 0);
        assert_eq!(metrics.cache_hits, 0);
    }

    #[tokio::test]
    async fn test_collective_inquiry_no_agitators() {
        // Create charter with no agitators
        let charter = TheCharter {
            commune: CommuneConfig {
                name: "Test Commune".to_string(),
                host: "127.0.0.1".to_string(),
                port: 8080,
                ws_port: 9090,
            },
            agitators: vec![], // No agitators configured
            archive: ArchiveConfig {
                path: "./test-archive.redb".into(),
                max_size_gb: 1,
            },
            memory: MemoryConfig {
                max_size_mb: 256,
                ttl_seconds: 3600,
            },
            infiltrator: None,
            cache_threshold: 0.8,
        };

        let collective = TheCollective::the_awakening(charter).await.unwrap();

        let inquiry = TheInquiry::new(
            TaskType::VulnerabilityAnalysis,
            serde_json::json!({"test": "data"}),
        );

        // Should fail with "no agitators available" error
        let result = collective.the_inquiry_sync(inquiry).await;

        assert!(result.is_err());
        assert!(result
            .unwrap_err()
            .to_string()
            .contains("No agitators available"));
    }

    #[tokio::test]
    async fn test_collective_rest() {
        let charter = create_test_charter();
        let collective = TheCollective::the_awakening(charter).await.unwrap();

        let result = collective.the_rest().await;

        assert!(result.is_ok());
    }
}
