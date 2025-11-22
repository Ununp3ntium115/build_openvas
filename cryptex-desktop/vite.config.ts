import { defineConfig } from 'vite';
import { svelte } from '@sveltejs/vite-plugin-svelte';

export default defineConfig({
  plugins: [svelte()],

  // Tauri expects a fixed port 1420 by default
  server: {
    port: 5173,
    strictPort: true,
  },

  // Build configuration for Tauri
  build: {
    target: ['es2021', 'chrome100', 'safari13'],
    minify: !process.env.TAURI_DEBUG ? 'esbuild' : false,
    sourcemap: !!process.env.TAURI_DEBUG,
  },

  clearScreen: false,

  envPrefix: ['VITE_', 'TAURI_'],
});
