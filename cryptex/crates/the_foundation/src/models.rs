//! Core data models for The Collective
//!
//! All models follow the anarchist Cryptex naming convention.

use serde::{Deserialize, Serialize};
use std::time::SystemTime;
use uuid::Uuid;

/// The Inquiry - Request for AI processing
///
/// Traditional name: `Request` or `AIRequest`
///
/// Represents a request to the AI collective for analysis, assessment,
/// or other cognitive tasks.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TheInquiry {
    /// Unique identifier for this inquiry
    pub id: String,

    /// Type of task to perform
    pub task: TaskType,

    /// Data package for processing
    pub package: ThePackage,

    /// Preferred AI provider (agitator)
    #[serde(skip_serializing_if = "Option::is_none")]
    pub preferred_agitator: Option<AgitatorType>,

    /// When this inquiry was created (Unix timestamp)
    pub created_at_ts: u64,
}

impl TheInquiry {
    /// Create a new inquiry
    pub fn new(task: TaskType, data: serde_json::Value) -> Self {
        let now = SystemTime::now()
            .duration_since(SystemTime::UNIX_EPOCH)
            .unwrap()
            .as_secs();

        Self {
            id: Uuid::new_v4().to_string(),
            task,
            package: ThePackage {
                data,
                context: None,
            },
            preferred_agitator: None,
            created_at_ts: now,
        }
    }

    /// Create inquiry with specific agitator preference
    pub fn with_agitator(mut self, agitator: AgitatorType) -> Self {
        self.preferred_agitator = Some(agitator);
        self
    }

    /// Add context to the inquiry
    pub fn with_context(mut self, context: String) -> Self {
        self.package.context = Some(context);
        self
    }

    /// Generate a fingerprint for caching
    pub fn fingerprint(&self) -> String {
        use std::collections::hash_map::DefaultHasher;
        use std::hash::{Hash, Hasher};

        let mut hasher = DefaultHasher::new();
        self.task.hash(&mut hasher);
        self.package.data.to_string().hash(&mut hasher);
        format!("inquiry-{:x}", hasher.finish())
    }
}

/// The Reply - Response from AI processing
///
/// Traditional name: `Response` or `AIResponse`
///
/// Contains the result of an AI inquiry along with metadata about
/// the processing.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TheReply {
    /// ID of the inquiry this replies to
    pub inquiry_id: String,

    /// The actual content/result
    pub content: String,

    /// Confidence/certainty score (0.0 to 1.0)
    pub certainty: TheCertainty,

    /// Which agitator processed this
    pub agitator_used: AgitatorType,

    /// The specific model/mind used
    pub the_mind: String,

    /// Processing duration in milliseconds
    pub duration_ms: u64,

    /// Number of tokens used (if applicable)
    #[serde(skip_serializing_if = "Option::is_none")]
    pub tokens_used: Option<u64>,

    /// When this reply was generated (Unix timestamp)
    pub replied_at_ts: u64,
}

/// The Package - Data payload for inquiries
///
/// Traditional name: `Payload` or `RequestData`
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ThePackage {
    /// The actual data being sent
    pub data: serde_json::Value,

    /// Optional context/instructions
    #[serde(skip_serializing_if = "Option::is_none")]
    pub context: Option<String>,
}

/// The Certainty - Confidence score
///
/// Traditional name: `Confidence` or `ConfidenceScore`
///
/// A value between 0.0 and 1.0 representing how certain the AI
/// is about its analysis.
pub type TheCertainty = f64;

/// Task Type - What kind of AI processing to perform
///
/// Traditional name: `TaskType` or `RequestType`
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
#[serde(rename_all = "snake_case")]
pub enum TaskType {
    /// Analyze vulnerability details
    VulnerabilityAnalysis,

    /// Model threats and attack vectors
    ThreatModeling,

    /// Optimize scan parameters
    ScanOptimization,

    /// Generate security reports
    ReportGeneration,

    /// Custom task type
    Custom,
}

/// Agitator Type - Which AI provider to use
///
/// Traditional name: `AIProvider` or `ModelProvider`
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Serialize, Deserialize)]
#[serde(rename_all = "lowercase")]
pub enum AgitatorType {
    /// OpenAI (GPT models)
    OpenAI,

    /// Anthropic Claude
    Claude,

    /// Automatic selection based on availability/health
    Auto,
}

impl std::fmt::Display for AgitatorType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            AgitatorType::OpenAI => write!(f, "OpenAI"),
            AgitatorType::Claude => write!(f, "Claude"),
            AgitatorType::Auto => write!(f, "Auto"),
        }
    }
}

impl std::fmt::Display for TaskType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            TaskType::VulnerabilityAnalysis => write!(f, "Vulnerability Analysis"),
            TaskType::ThreatModeling => write!(f, "Threat Modeling"),
            TaskType::ScanOptimization => write!(f, "Scan Optimization"),
            TaskType::ReportGeneration => write!(f, "Report Generation"),
            TaskType::Custom => write!(f, "Custom"),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_inquiry_creation() {
        let inquiry = TheInquiry::new(
            TaskType::VulnerabilityAnalysis,
            serde_json::json!({"cve": "CVE-2023-12345"}),
        );

        assert!(!inquiry.id.is_empty());
        assert_eq!(inquiry.task, TaskType::VulnerabilityAnalysis);
        assert!(inquiry.preferred_agitator.is_none());
    }

    #[test]
    fn test_inquiry_with_agitator() {
        let inquiry = TheInquiry::new(
            TaskType::ThreatModeling,
            serde_json::json!({}),
        )
        .with_agitator(AgitatorType::Claude);

        assert_eq!(inquiry.preferred_agitator, Some(AgitatorType::Claude));
    }

    #[test]
    fn test_inquiry_fingerprint() {
        let inquiry1 = TheInquiry::new(
            TaskType::VulnerabilityAnalysis,
            serde_json::json!({"test": "data"}),
        );

        let inquiry2 = TheInquiry::new(
            TaskType::VulnerabilityAnalysis,
            serde_json::json!({"test": "data"}),
        );

        // Same task and data should produce same fingerprint
        assert_eq!(inquiry1.fingerprint(), inquiry2.fingerprint());
    }

    #[test]
    fn test_serialization() {
        let inquiry = TheInquiry::new(
            TaskType::ScanOptimization,
            serde_json::json!({"targets": ["192.168.1.1"]}),
        );

        let json = serde_json::to_string(&inquiry).unwrap();
        let deserialized: TheInquiry = serde_json::from_str(&json).unwrap();

        assert_eq!(inquiry.id, deserialized.id);
        assert_eq!(inquiry.task, deserialized.task);
    }
}
