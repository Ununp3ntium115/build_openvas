//! Anthropic Claude Provider Implementation
//!
//! Provides integration with Anthropic's Messages API for
//! AI-enhanced security analysis using Claude models.

use async_trait::async_trait;
use reqwest::Client;
use serde::{Deserialize, Serialize};
use std::time::{SystemTime, UNIX_EPOCH};
use the_foundation::TheAgitator;
use the_foundation::{AgitatorType, CryptexError, CryptexResult, ThePackage, TheReply};

const CLAUDE_API_URL: &str = "https://api.anthropic.com/v1/messages";
const CLAUDE_API_VERSION: &str = "2023-06-01";
const DEFAULT_TIMEOUT_SECS: u64 = 30;

/// Claude Messages API Request
#[derive(Debug, Serialize)]
struct ClaudeRequest {
    model: String,
    messages: Vec<Message>,
    max_tokens: u32,
    #[serde(skip_serializing_if = "Option::is_none")]
    system: Option<String>,
    #[serde(skip_serializing_if = "Option::is_none")]
    temperature: Option<f32>,
}

#[derive(Debug, Serialize, Deserialize)]
struct Message {
    role: String,
    content: String,
}

/// Claude Messages API Response
#[derive(Debug, Deserialize)]
struct ClaudeResponse {
    id: String,
    #[serde(rename = "type")]
    response_type: String,
    role: String,
    content: Vec<ContentBlock>,
    model: String,
    stop_reason: Option<String>,
    usage: Usage,
}

#[derive(Debug, Deserialize)]
struct ContentBlock {
    #[serde(rename = "type")]
    content_type: String,
    text: String,
}

#[derive(Debug, Deserialize)]
struct Usage {
    input_tokens: u64,
    output_tokens: u64,
}

/// Claude Agitator
pub struct ClaudeAgitator {
    api_key: String,
    model: String,
    client: Client,
    timeout: std::time::Duration,
}

impl ClaudeAgitator {
    /// The Awakening - Initialize Claude agitator
    ///
    /// Creates a new Claude agitator with the specified API key and model.
    /// Validates the API key format and creates an HTTP client.
    ///
    /// # Arguments
    ///
    /// * `api_key` - Anthropic API key
    /// * `model` - Model name (e.g., "claude-3-opus-20240229", "claude-3-sonnet-20240229")
    ///
    /// # Example
    ///
    /// ```no_run
    /// use the_agitator::ClaudeAgitator;
    ///
    /// #[tokio::main]
    /// async fn main() -> Result<(), Box<dyn std::error::Error>> {
    ///     let agitator = ClaudeAgitator::the_awakening(
    ///         std::env::var("ANTHROPIC_API_KEY")?,
    ///         "claude-3-sonnet-20240229".to_string(),
    ///     ).await?;
    ///     Ok(())
    /// }
    /// ```
    pub async fn the_awakening(api_key: String, model: String) -> CryptexResult<Self> {
        // Validate API key
        if api_key.is_empty() {
            return Err(CryptexError::agitator("Claude API key cannot be empty"));
        }

        // Validate model name
        if model.is_empty() {
            return Err(CryptexError::agitator("Claude model name cannot be empty"));
        }

        // Create HTTP client with timeout
        let client = Client::builder()
            .timeout(std::time::Duration::from_secs(DEFAULT_TIMEOUT_SECS))
            .build()
            .map_err(|e| CryptexError::network(format!("Failed to create HTTP client: {}", e)))?;

        tracing::info!("Claude agitator awakened with model: {}", model);

        Ok(Self {
            api_key,
            model,
            client,
            timeout: std::time::Duration::from_secs(DEFAULT_TIMEOUT_SECS),
        })
    }

    /// Set custom timeout for API requests
    pub fn with_timeout(mut self, timeout_secs: u64) -> Self {
        self.timeout = std::time::Duration::from_secs(timeout_secs);
        self
    }

    /// Build the request payload for Claude API
    fn build_request(&self, package: &ThePackage) -> ClaudeRequest {
        // Extract system context if provided
        let system = package.context.clone().or_else(|| {
            Some("You are a security expert assistant helping to analyze vulnerabilities and security issues. Provide accurate, detailed, and actionable security analysis.".to_string())
        });

        // Build user message with package data
        let user_content = if package.data.is_string() {
            package.data.as_str().unwrap_or("").to_string()
        } else {
            serde_json::to_string_pretty(&package.data)
                .unwrap_or_else(|_| package.data.to_string())
        };

        let messages = vec![Message {
            role: "user".to_string(),
            content: user_content,
        }];

        ClaudeRequest {
            model: self.model.clone(),
            messages,
            max_tokens: 4096, // Claude default max
            system,
            temperature: Some(0.7),
        }
    }

    /// Send request to Claude API
    async fn send_request(&self, request: &ClaudeRequest) -> CryptexResult<ClaudeResponse> {
        let start = std::time::Instant::now();

        let response = self
            .client
            .post(CLAUDE_API_URL)
            .header("x-api-key", &self.api_key)
            .header("anthropic-version", CLAUDE_API_VERSION)
            .header("Content-Type", "application/json")
            .json(request)
            .send()
            .await
            .map_err(|e| {
                CryptexError::network(format!("Failed to send request to Claude: {}", e))
            })?;

        let duration = start.elapsed();
        tracing::debug!("Claude API request took {:?}", duration);

        // Check for HTTP errors
        if !response.status().is_success() {
            let status = response.status();
            let error_body = response
                .text()
                .await
                .unwrap_or_else(|_| "Unable to read error response".to_string());

            return Err(CryptexError::agitator(format!(
                "Claude API error ({}): {}",
                status, error_body
            )));
        }

        // Parse response
        let claude_response: ClaudeResponse = response.json().await.map_err(|e| {
            CryptexError::agitator(format!("Failed to parse Claude response: {}", e))
        })?;

        Ok(claude_response)
    }

    /// Convert Claude response to TheReply
    fn convert_response(
        &self,
        response: ClaudeResponse,
        inquiry_id: String,
        duration_ms: u64,
    ) -> CryptexResult<TheReply> {
        // Extract text from content blocks
        let content = response
            .content
            .iter()
            .filter(|block| block.content_type == "text")
            .map(|block| block.text.clone())
            .collect::<Vec<_>>()
            .join("\n");

        if content.is_empty() {
            return Err(CryptexError::agitator("Claude response contains no text content"));
        }

        // Determine certainty based on stop_reason
        let certainty = match response.stop_reason.as_deref() {
            Some("end_turn") => 0.9,     // Normal completion
            Some("max_tokens") => 0.7,   // Hit token limit
            Some("stop_sequence") => 0.85, // Hit stop sequence
            _ => 0.6,                    // Unknown or null
        };

        let now = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_secs();

        let total_tokens = response.usage.input_tokens + response.usage.output_tokens;

        Ok(TheReply {
            inquiry_id,
            content,
            certainty,
            agitator_used: AgitatorType::Claude,
            the_mind: self.model.clone(),
            duration_ms,
            tokens_used: Some(total_tokens),
            replied_at_ts: now,
        })
    }
}

#[async_trait]
impl TheAgitator for ClaudeAgitator {
    fn agitator_type(&self) -> AgitatorType {
        AgitatorType::Claude
    }

    fn the_mind(&self) -> &str {
        &self.model
    }

    /// The Agitation - Process a package through Claude
    ///
    /// Sends the package to Anthropic's Messages API and returns
    /// the AI-generated response as TheReply.
    async fn the_agitation(&self, package: ThePackage) -> CryptexResult<TheReply> {
        tracing::debug!("Processing package through Claude with model: {}", self.model);

        let start = std::time::Instant::now();

        // Build request
        let request = self.build_request(&package);

        // Send to Claude
        let response = self.send_request(&request).await?;

        let duration_ms = start.elapsed().as_millis() as u64;

        // Convert to TheReply
        let inquiry_id = uuid::Uuid::new_v4().to_string();
        let reply = self.convert_response(response, inquiry_id, duration_ms)?;

        tracing::info!(
            "Claude agitation complete - certainty: {:.2}, tokens: {:?}",
            reply.certainty,
            reply.tokens_used
        );

        Ok(reply)
    }

    /// The Validation - Health check for Claude API
    ///
    /// Sends a minimal test request to verify API connectivity and credentials.
    async fn the_validation(&self) -> CryptexResult<bool> {
        tracing::debug!("Validating Claude agitator");

        // Create a simple test package
        let test_package = ThePackage {
            data: serde_json::json!("test"),
            context: Some("Respond with 'OK' if you can read this.".to_string()),
        };

        // Try to send request
        match self.the_agitation(test_package).await {
            Ok(_) => {
                tracing::info!("Claude agitator validation successful");
                Ok(true)
            }
            Err(e) => {
                tracing::error!("Claude agitator validation failed: {}", e);
                Err(e)
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_claude_awakening_valid() {
        let result = ClaudeAgitator::the_awakening(
            "test-key-12345".to_string(),
            "claude-3-sonnet-20240229".to_string(),
        )
        .await;

        assert!(result.is_ok());
        let agitator = result.unwrap();
        assert_eq!(agitator.the_mind(), "claude-3-sonnet-20240229");
        assert_eq!(agitator.agitator_type(), AgitatorType::Claude);
    }

    #[tokio::test]
    async fn test_claude_awakening_empty_key() {
        let result = ClaudeAgitator::the_awakening(
            "".to_string(),
            "claude-3-sonnet-20240229".to_string(),
        )
        .await;

        assert!(result.is_err());
    }

    #[tokio::test]
    async fn test_claude_awakening_empty_model() {
        let result =
            ClaudeAgitator::the_awakening("test-key".to_string(), "".to_string()).await;

        assert!(result.is_err());
    }

    #[test]
    fn test_build_request_with_context() {
        let rt = tokio::runtime::Runtime::new().unwrap();
        let agitator = rt
            .block_on(ClaudeAgitator::the_awakening(
                "test-key".to_string(),
                "claude-3-sonnet-20240229".to_string(),
            ))
            .unwrap();

        let package = ThePackage {
            data: serde_json::json!({"test": "data"}),
            context: Some("Custom context".to_string()),
        };

        let request = agitator.build_request(&package);

        assert_eq!(request.model, "claude-3-sonnet-20240229");
        assert_eq!(request.messages.len(), 1);
        assert_eq!(request.messages[0].role, "user");
        assert_eq!(request.system, Some("Custom context".to_string()));
    }

    #[test]
    fn test_build_request_default_context() {
        let rt = tokio::runtime::Runtime::new().unwrap();
        let agitator = rt
            .block_on(ClaudeAgitator::the_awakening(
                "test-key".to_string(),
                "claude-3-sonnet-20240229".to_string(),
            ))
            .unwrap();

        let package = ThePackage {
            data: serde_json::json!("test data"),
            context: None,
        };

        let request = agitator.build_request(&package);

        assert_eq!(request.messages.len(), 1);
        assert_eq!(request.messages[0].role, "user");
        assert!(request.system.is_some());
        assert!(request.system.unwrap().contains("security expert"));
    }
}
