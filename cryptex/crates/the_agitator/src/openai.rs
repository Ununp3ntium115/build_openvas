//! OpenAI Provider Implementation
//!
//! Provides integration with OpenAI's Chat Completions API for
//! AI-enhanced security analysis.

use async_trait::async_trait;
use reqwest::Client;
use serde::{Deserialize, Serialize};
use std::time::{SystemTime, UNIX_EPOCH};
use the_foundation::TheAgitator;
use the_foundation::{AgitatorType, CryptexError, CryptexResult, ThePackage, TheReply};

const OPENAI_API_URL: &str = "https://api.openai.com/v1/chat/completions";
const DEFAULT_TIMEOUT_SECS: u64 = 30;

/// OpenAI Chat Completions API Request
#[derive(Debug, Serialize)]
struct OpenAIRequest {
    model: String,
    messages: Vec<Message>,
    #[serde(skip_serializing_if = "Option::is_none")]
    temperature: Option<f32>,
    #[serde(skip_serializing_if = "Option::is_none")]
    max_tokens: Option<u32>,
}

#[derive(Debug, Serialize, Deserialize)]
struct Message {
    role: String,
    content: String,
}

/// OpenAI Chat Completions API Response
#[derive(Debug, Deserialize)]
struct OpenAIResponse {
    id: String,
    choices: Vec<Choice>,
    usage: Usage,
}

#[derive(Debug, Deserialize)]
struct Choice {
    message: Message,
    finish_reason: Option<String>,
}

#[derive(Debug, Deserialize)]
struct Usage {
    total_tokens: u64,
}

/// OpenAI Agitator
pub struct OpenAIAgitator {
    api_key: String,
    model: String,
    client: Client,
    timeout: std::time::Duration,
}

impl OpenAIAgitator {
    /// The Awakening - Initialize OpenAI agitator
    ///
    /// Creates a new OpenAI agitator with the specified API key and model.
    /// Validates the API key format and creates an HTTP client.
    ///
    /// # Arguments
    ///
    /// * `api_key` - OpenAI API key (should start with "sk-")
    /// * `model` - Model name (e.g., "gpt-4", "gpt-3.5-turbo")
    ///
    /// # Example
    ///
    /// ```no_run
    /// use the_agitator::OpenAIAgitator;
    ///
    /// #[tokio::main]
    /// async fn main() -> Result<(), Box<dyn std::error::Error>> {
    ///     let agitator = OpenAIAgitator::the_awakening(
    ///         std::env::var("OPENAI_API_KEY")?,
    ///         "gpt-4".to_string(),
    ///     ).await?;
    ///     Ok(())
    /// }
    /// ```
    pub async fn the_awakening(api_key: String, model: String) -> CryptexResult<Self> {
        // Validate API key format
        if api_key.is_empty() {
            return Err(CryptexError::agitator("OpenAI API key cannot be empty"));
        }

        if !api_key.starts_with("sk-") {
            tracing::warn!("OpenAI API key doesn't start with 'sk-' - may be invalid");
        }

        // Validate model name
        if model.is_empty() {
            return Err(CryptexError::agitator("OpenAI model name cannot be empty"));
        }

        // Create HTTP client with timeout
        let client = Client::builder()
            .timeout(std::time::Duration::from_secs(DEFAULT_TIMEOUT_SECS))
            .build()
            .map_err(|e| CryptexError::network(format!("Failed to create HTTP client: {}", e)))?;

        tracing::info!("OpenAI agitator awakened with model: {}", model);

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

    /// Build the request payload for OpenAI API
    fn build_request(&self, package: &ThePackage) -> OpenAIRequest {
        let mut messages = Vec::new();

        // Add system context if provided
        if let Some(context) = &package.context {
            messages.push(Message {
                role: "system".to_string(),
                content: context.clone(),
            });
        } else {
            // Default system prompt for security analysis
            messages.push(Message {
                role: "system".to_string(),
                content: "You are a security expert assistant helping to analyze vulnerabilities and security issues. Provide accurate, detailed, and actionable security analysis.".to_string(),
            });
        }

        // Add user message with the package data
        let user_content = if package.data.is_string() {
            package.data.as_str().unwrap_or("").to_string()
        } else {
            serde_json::to_string_pretty(&package.data).unwrap_or_else(|_| package.data.to_string())
        };

        messages.push(Message {
            role: "user".to_string(),
            content: user_content,
        });

        OpenAIRequest {
            model: self.model.clone(),
            messages,
            temperature: Some(0.7),
            max_tokens: None,
        }
    }

    /// Send request to OpenAI API
    async fn send_request(&self, request: &OpenAIRequest) -> CryptexResult<OpenAIResponse> {
        let start = std::time::Instant::now();

        let response = self
            .client
            .post(OPENAI_API_URL)
            .header("Authorization", format!("Bearer {}", self.api_key))
            .header("Content-Type", "application/json")
            .json(request)
            .send()
            .await
            .map_err(|e| {
                CryptexError::network(format!("Failed to send request to OpenAI: {}", e))
            })?;

        let duration = start.elapsed();
        tracing::debug!("OpenAI API request took {:?}", duration);

        // Check for HTTP errors
        if !response.status().is_success() {
            let status = response.status();
            let error_body = response
                .text()
                .await
                .unwrap_or_else(|_| "Unable to read error response".to_string());

            return Err(CryptexError::agitator(format!(
                "OpenAI API error ({}): {}",
                status, error_body
            )));
        }

        // Parse response
        let openai_response: OpenAIResponse = response.json().await.map_err(|e| {
            CryptexError::agitator(format!("Failed to parse OpenAI response: {}", e))
        })?;

        Ok(openai_response)
    }

    /// Convert OpenAI response to TheReply
    fn convert_response(
        &self,
        response: OpenAIResponse,
        inquiry_id: String,
        duration_ms: u64,
    ) -> CryptexResult<TheReply> {
        // Extract the first choice
        let choice = response.choices.first().ok_or_else(|| {
            CryptexError::agitator("OpenAI response contains no choices")
        })?;

        // Determine certainty based on finish_reason
        let certainty = match choice.finish_reason.as_deref() {
            Some("stop") => 0.9,       // Normal completion
            Some("length") => 0.7,     // Hit token limit
            Some("content_filter") => 0.5, // Content filtered
            _ => 0.6,                  // Unknown or null
        };

        let now = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_secs();

        Ok(TheReply {
            inquiry_id,
            content: choice.message.content.clone(),
            certainty,
            agitator_used: AgitatorType::OpenAI,
            the_mind: self.model.clone(),
            duration_ms,
            tokens_used: Some(response.usage.total_tokens),
            replied_at_ts: now,
        })
    }
}

#[async_trait]
impl TheAgitator for OpenAIAgitator {
    fn agitator_type(&self) -> AgitatorType {
        AgitatorType::OpenAI
    }

    fn the_mind(&self) -> &str {
        &self.model
    }

    /// The Agitation - Process a package through OpenAI
    ///
    /// Sends the package to OpenAI's Chat Completions API and returns
    /// the AI-generated response as TheReply.
    async fn the_agitation(&self, package: ThePackage) -> CryptexResult<TheReply> {
        tracing::debug!("Processing package through OpenAI with model: {}", self.model);

        let start = std::time::Instant::now();

        // Build request
        let request = self.build_request(&package);

        // Send to OpenAI
        let response = self.send_request(&request).await?;

        let duration_ms = start.elapsed().as_millis() as u64;

        // Convert to TheReply
        let inquiry_id = uuid::Uuid::new_v4().to_string();
        let reply = self.convert_response(response, inquiry_id, duration_ms)?;

        tracing::info!(
            "OpenAI agitation complete - certainty: {:.2}, tokens: {:?}",
            reply.certainty,
            reply.tokens_used
        );

        Ok(reply)
    }

    /// The Validation - Health check for OpenAI API
    ///
    /// Sends a minimal test request to verify API connectivity and credentials.
    async fn the_validation(&self) -> CryptexResult<bool> {
        tracing::debug!("Validating OpenAI agitator");

        // Create a simple test package
        let test_package = ThePackage {
            data: serde_json::json!("test"),
            context: Some("Respond with 'OK' if you can read this.".to_string()),
        };

        // Try to send request
        match self.the_agitation(test_package).await {
            Ok(_) => {
                tracing::info!("OpenAI agitator validation successful");
                Ok(true)
            }
            Err(e) => {
                tracing::error!("OpenAI agitator validation failed: {}", e);
                Err(e)
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_openai_awakening_valid() {
        let result = OpenAIAgitator::the_awakening(
            "sk-test-key-12345".to_string(),
            "gpt-4".to_string(),
        )
        .await;

        assert!(result.is_ok());
        let agitator = result.unwrap();
        assert_eq!(agitator.the_mind(), "gpt-4");
        assert_eq!(agitator.agitator_type(), AgitatorType::OpenAI);
    }

    #[tokio::test]
    async fn test_openai_awakening_empty_key() {
        let result =
            OpenAIAgitator::the_awakening("".to_string(), "gpt-4".to_string()).await;

        assert!(result.is_err());
    }

    #[tokio::test]
    async fn test_openai_awakening_empty_model() {
        let result =
            OpenAIAgitator::the_awakening("sk-test-key".to_string(), "".to_string()).await;

        assert!(result.is_err());
    }

    #[test]
    fn test_build_request_with_context() {
        let rt = tokio::runtime::Runtime::new().unwrap();
        let agitator = rt
            .block_on(OpenAIAgitator::the_awakening(
                "sk-test".to_string(),
                "gpt-4".to_string(),
            ))
            .unwrap();

        let package = ThePackage {
            data: serde_json::json!({"test": "data"}),
            context: Some("Custom context".to_string()),
        };

        let request = agitator.build_request(&package);

        assert_eq!(request.model, "gpt-4");
        assert_eq!(request.messages.len(), 2);
        assert_eq!(request.messages[0].role, "system");
        assert_eq!(request.messages[0].content, "Custom context");
        assert_eq!(request.messages[1].role, "user");
    }

    #[test]
    fn test_build_request_default_context() {
        let rt = tokio::runtime::Runtime::new().unwrap();
        let agitator = rt
            .block_on(OpenAIAgitator::the_awakening(
                "sk-test".to_string(),
                "gpt-4".to_string(),
            ))
            .unwrap();

        let package = ThePackage {
            data: serde_json::json!("test data"),
            context: None,
        };

        let request = agitator.build_request(&package);

        assert_eq!(request.messages.len(), 2);
        assert_eq!(request.messages[0].role, "system");
        assert!(request.messages[0].content.contains("security expert"));
    }
}
