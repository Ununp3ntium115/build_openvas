// CRYPTEX REST API Server Binary
// Standalone server for running The Interface

use the_interface::run_server;
use tracing_subscriber::{layer::SubscriberExt, util::SubscriberInitExt};

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    // Initialize tracing
    tracing_subscriber::registry()
        .with(
            tracing_subscriber::EnvFilter::try_from_default_env()
                .unwrap_or_else(|_| "info,the_interface=debug,tower_http=debug".into()),
        )
        .with(tracing_subscriber::fmt::layer())
        .init();

    // Get configuration from environment
    let db_path = std::env::var("CRYPTEX_DB_PATH")
        .unwrap_or_else(|_| "./data/cryptex.db".to_string());

    let bind_addr = std::env::var("CRYPTEX_BIND_ADDR")
        .unwrap_or_else(|_| "0.0.0.0:8080".to_string());

    tracing::info!("Starting CRYPTEX API Server");
    tracing::info!("Database path: {}", db_path);
    tracing::info!("Binding to: {}", bind_addr);

    // Run the server
    run_server(&db_path, &bind_addr).await?;

    Ok(())
}
