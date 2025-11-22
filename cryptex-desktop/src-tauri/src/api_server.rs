// Embedded API Server
// Runs CRYPTEX REST API in the background

use std::sync::Arc;
use tokio::runtime::Runtime;
use the_foundation::platform::get_data_dir;
use the_interface::TheInterface;

pub struct ApiServer {
    runtime: Runtime,
    server_handle: Option<tokio::task::JoinHandle<()>>,
}

impl ApiServer {
    pub fn new() -> Result<Self, Box<dyn std::error::Error>> {
        let runtime = Runtime::new()?;
        Ok(Self {
            runtime,
            server_handle: None,
        })
    }

    pub fn start(&mut self) -> Result<String, Box<dyn std::error::Error>> {
        if self.server_handle.is_some() {
            return Err("API server already running".into());
        }

        let db_path = get_data_dir().join("cryptex.db");
        let bind_addr = "127.0.0.1:8080";
        let bind_addr_clone = bind_addr.to_string();

        // Start server in background task
        let handle = self.runtime.spawn(async move {
            // Initialize tracing
            tracing_subscriber::fmt::init();

            match TheInterface::the_awakening(bind_addr_clone.clone(), db_path).await {
                Ok(interface) => {
                    if let Err(e) = interface.the_manifestation().await {
                        eprintln!("API server error: {}", e);
                    }
                }
                Err(e) => {
                    eprintln!("Failed to initialize API server: {}", e);
                }
            }
        });

        self.server_handle = Some(handle);
        Ok(format!("http://{}", bind_addr))
    }

    pub fn stop(&mut self) {
        if let Some(handle) = self.server_handle.take() {
            handle.abort();
        }
    }

    pub fn is_running(&self) -> bool {
        self.server_handle.is_some()
    }
}

impl Drop for ApiServer {
    fn drop(&mut self) {
        self.stop();
    }
}
