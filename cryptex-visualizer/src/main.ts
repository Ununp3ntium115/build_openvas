/**
 * CRYPTEX Visualizer - Standalone Application Entry
 *
 * This entry point creates a standalone CRYPTEX dashboard
 * that runs independently of PYRO.
 */

import App from './App.svelte'
import './app.css'

const app = new App({
  target: document.getElementById('app')!,
  props: {
    // Standalone mode
    mode: 'standalone',
    apiUrl: import.meta.env.VITE_API_URL || 'http://localhost:8080',
  }
})

export default app
