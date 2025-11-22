// CRYPTEX Desktop Application
// Native desktop wrapper for CRYPTEX using Tauri

#![cfg_attr(
    all(not(debug_assertions), target_os = "windows"),
    windows_subsystem = "windows"
)]

use tauri::{Manager, SystemTray, SystemTrayEvent, SystemTrayMenu, SystemTrayMenuItem};
use std::sync::{Arc, Mutex};

mod api_server;
mod database;
mod commands;

use api_server::ApiServer;

#[derive(Default)]
struct AppState {
    api_server: Arc<Mutex<Option<ApiServer>>>,
}

fn main() {
    // Create system tray
    let tray_menu = SystemTrayMenu::new()
        .add_item(SystemTrayMenuItem::new("Show CRYPTEX", "show"))
        .add_native_item(SystemTrayMenuItem::Separator)
        .add_item(SystemTrayMenuItem::new("Start API Server", "start_api"))
        .add_item(SystemTrayMenuItem::new("Stop API Server", "stop_api"))
        .add_native_item(SystemTrayMenuItem::Separator)
        .add_item(SystemTrayMenuItem::new("Quit", "quit"));

    let system_tray = SystemTray::new().with_menu(tray_menu);

    tauri::Builder::default()
        .manage(AppState::default())
        .system_tray(system_tray)
        .on_system_tray_event(|app, event| match event {
            SystemTrayEvent::MenuItemClick { id, .. } => {
                match id.as_str() {
                    "show" => {
                        let window = app.get_window("main").unwrap();
                        window.show().unwrap();
                        window.set_focus().unwrap();
                    }
                    "start_api" => {
                        // Start embedded API server
                        if let Err(e) = commands::start_api_server(app.state()) {
                            eprintln!("Failed to start API server: {}", e);
                        }
                    }
                    "stop_api" => {
                        commands::stop_api_server(app.state());
                    }
                    "quit" => {
                        std::process::exit(0);
                    }
                    _ => {}
                }
            }
            _ => {}
        })
        .invoke_handler(tauri::generate_handler![
            commands::start_api_server,
            commands::stop_api_server,
            commands::get_api_status,
            commands::assess_vulnerability,
            commands::start_scan,
            commands::get_scan_results,
            commands::generate_report,
            commands::get_archive_stats,
            commands::open_node_red,
            commands::connect_to_pyro,
        ])
        .setup(|app| {
            // Auto-start API server on launch
            if let Err(e) = commands::start_api_server(app.state()) {
                eprintln!("Failed to auto-start API server: {}", e);
            }
            Ok(())
        })
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
