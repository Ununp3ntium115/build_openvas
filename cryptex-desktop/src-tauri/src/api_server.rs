// Embedded API Server
// Runs CRYPTEX REST API in the background

use std::sync::Arc;
use tokio::runtime::Runtime;
use the_foundation::get_default_db_path;

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

        let db_path = get_default_db_path();
        let bind_addr = "127.0.0.1:8080";

        // Start server in background task
        let handle = self.runtime.spawn(async move {
            if let Err(e) = the_interface::run_server(
                db_path.to_str().unwrap(),
                bind_addr
            ).await {
                eprintln!("API server error: {}", e);
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
