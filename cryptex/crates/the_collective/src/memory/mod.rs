//! The Memory - Caching layer for AI replies
//!
//! Traditional name: `Cache` or `CacheManager`
//!
//! Provides in-memory caching of AI replies to avoid redundant API calls
//! and improve response times.

use crate::{CryptexResult, TheReply};
use std::collections::HashMap;
use std::sync::Arc;
use std::time::{Duration, SystemTime};
use tokio::sync::RwLock;

/// Cache entry with expiration
#[derive(Debug, Clone)]
struct CacheEntry {
    reply: TheReply,
    expires_at: SystemTime,
}

/// The Memory - In-memory cache for AI replies
///
/// Traditional name: `Cache`
///
/// Stores AI replies to avoid redundant API calls. Entries are keyed by
/// inquiry fingerprint and automatically expire after TTL.
pub struct TheMemory {
    entries: Arc<RwLock<HashMap<String, CacheEntry>>>,
    ttl: Duration,
    max_size: usize,
}

impl TheMemory {
    /// The Awakening - Initialize the memory cache
    ///
    /// Traditional name: `new` or `init`
    pub fn the_awakening(max_size_mb: usize, ttl_seconds: u64) -> CryptexResult<Self> {
        tracing::info!(
            "The Memory awakening: max_size={}MB, ttl={}s",
            max_size_mb,
            ttl_seconds
        );

        Ok(Self {
            entries: Arc::new(RwLock::new(HashMap::new())),
            ttl: Duration::from_secs(ttl_seconds),
            max_size: max_size_mb,
        })
    }

    /// Store a reply in the cache
    ///
    /// Traditional name: `put` or `set`
    pub async fn store(&self, key: String, reply: TheReply) -> CryptexResult<()> {
        let expires_at = SystemTime::now() + self.ttl;

        let mut entries = self.entries.write().await;

        // Evict expired entries first
        self.evict_expired(&mut entries);

        // Check size limit (simple entry count for now)
        if entries.len() >= self.max_size {
            tracing::debug!("Cache full, evicting oldest entry");
            if let Some(oldest_key) = entries.keys().next().cloned() {
                entries.remove(&oldest_key);
            }
        }

        entries.insert(
            key.clone(),
            CacheEntry {
                reply,
                expires_at,
            },
        );

        tracing::debug!("Cached reply with key: {}", key);

        Ok(())
    }

    /// Retrieve a reply from the cache
    ///
    /// Traditional name: `get`
    pub async fn retrieve(&self, key: &str) -> CryptexResult<Option<TheReply>> {
        let mut entries = self.entries.write().await;

        // Check if entry exists and is not expired
        if let Some(entry) = entries.get(key) {
            if SystemTime::now() < entry.expires_at {
                tracing::debug!("Cache hit for key: {}", key);
                return Ok(Some(entry.reply.clone()));
            } else {
                // Remove expired entry
                tracing::debug!("Cache entry expired for key: {}", key);
                entries.remove(key);
            }
        }

        tracing::debug!("Cache miss for key: {}", key);
        Ok(None)
    }

    /// Check if a key exists in the cache (without retrieving)
    ///
    /// Traditional name: `contains` or `has`
    pub async fn contains(&self, key: &str) -> bool {
        let entries = self.entries.read().await;

        if let Some(entry) = entries.get(key) {
            SystemTime::now() < entry.expires_at
        } else {
            false
        }
    }

    /// The Flush - Clear all cached entries
    ///
    /// Traditional name: `clear` or `flush`
    pub async fn the_flush(&self) -> CryptexResult<()> {
        let mut entries = self.entries.write().await;
        let count = entries.len();
        entries.clear();

        tracing::info!("The Memory flushed: {} entries cleared", count);

        Ok(())
    }

    /// Get cache statistics
    pub async fn statistics(&self) -> CacheStatistics {
        let entries = self.entries.read().await;

        let total = entries.len();
        let expired = entries
            .values()
            .filter(|e| SystemTime::now() >= e.expires_at)
            .count();

        CacheStatistics {
            total_entries: total,
            expired_entries: expired,
            active_entries: total - expired,
        }
    }

    /// Evict expired entries (internal helper)
    fn evict_expired(&self, entries: &mut HashMap<String, CacheEntry>) {
        let now = SystemTime::now();
        entries.retain(|_, entry| now < entry.expires_at);
    }
}

/// Cache statistics
#[derive(Debug, Clone)]
pub struct CacheStatistics {
    pub total_entries: usize,
    pub expired_entries: usize,
    pub active_entries: usize,
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::{AgitatorType, TheReply};

    fn create_test_reply(content: &str) -> TheReply {
        TheReply {
            inquiry_id: "test-123".to_string(),
            content: content.to_string(),
            certainty: 0.95,
            agitator_used: AgitatorType::OpenAI,
            the_mind: "gpt-4".to_string(),
            duration_ms: 1500,
            tokens_used: Some(100),
            replied_at_ts: SystemTime::now()
                .duration_since(SystemTime::UNIX_EPOCH)
                .unwrap()
                .as_secs(),
        }
    }

    #[tokio::test]
    async fn test_memory_store_and_retrieve() {
        let memory = TheMemory::the_awakening(256, 3600).unwrap();
        let reply = create_test_reply("Test content");

        memory.store("test-key".to_string(), reply.clone()).await.unwrap();

        let retrieved = memory.retrieve("test-key").await.unwrap();
        assert!(retrieved.is_some());
        assert_eq!(retrieved.unwrap().content, "Test content");
    }

    #[tokio::test]
    async fn test_memory_cache_miss() {
        let memory = TheMemory::the_awakening(256, 3600).unwrap();

        let retrieved = memory.retrieve("nonexistent").await.unwrap();
        assert!(retrieved.is_none());
    }

    #[tokio::test]
    async fn test_memory_expiration() {
        let memory = TheMemory::the_awakening(256, 1).unwrap(); // 1 second TTL
        let reply = create_test_reply("Test content");

        memory.store("test-key".to_string(), reply).await.unwrap();

        // Should exist immediately
        assert!(memory.contains("test-key").await);

        // Wait for expiration
        tokio::time::sleep(Duration::from_secs(2)).await;

        // Should be expired
        let retrieved = memory.retrieve("test-key").await.unwrap();
        assert!(retrieved.is_none());
    }

    #[tokio::test]
    async fn test_memory_flush() {
        let memory = TheMemory::the_awakening(256, 3600).unwrap();
        let reply = create_test_reply("Test content");

        memory.store("key1".to_string(), reply.clone()).await.unwrap();
        memory.store("key2".to_string(), reply).await.unwrap();

        memory.the_flush().await.unwrap();

        let stats = memory.statistics().await;
        assert_eq!(stats.total_entries, 0);
    }

    #[tokio::test]
    async fn test_memory_statistics() {
        let memory = TheMemory::the_awakening(256, 3600).unwrap();
        let reply = create_test_reply("Test content");

        memory.store("key1".to_string(), reply.clone()).await.unwrap();
        memory.store("key2".to_string(), reply).await.unwrap();

        let stats = memory.statistics().await;
        assert_eq!(stats.total_entries, 2);
        assert_eq!(stats.active_entries, 2);
        assert_eq!(stats.expired_entries, 0);
    }
}
