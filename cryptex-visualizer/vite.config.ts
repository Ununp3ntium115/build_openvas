import { defineConfig } from 'vite'
import { svelte } from '@sveltejs/vite-plugin-svelte'
import path from 'path'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [svelte()],

  resolve: {
    alias: {
      '@': path.resolve(__dirname, './src'),
      '@components': path.resolve(__dirname, './src/components'),
      '@stores': path.resolve(__dirname, './src/stores'),
      '@services': path.resolve(__dirname, './src/services'),
      '@types': path.resolve(__dirname, './src/types'),
    }
  },

  server: {
    port: 5173,
    proxy: {
      '/api': {
        target: 'http://localhost:8080',
        changeOrigin: true,
      },
      '/ws': {
        target: 'ws://localhost:8080',
        ws: true,
      }
    }
  },

  build: {
    // Build both standalone app and embeddable library
    lib: {
      entry: {
        // Standalone app entry
        app: path.resolve(__dirname, 'src/main.ts'),
        // Embeddable module for PYRO integration
        module: path.resolve(__dirname, 'src/module.ts'),
      },
      name: 'CryptexVisualizer',
      formats: ['es', 'umd'],
    },
    rollupOptions: {
      // Make sure to externalize deps for library build
      external: ['svelte'],
      output: {
        globals: {
          svelte: 'Svelte'
        }
      }
    }
  }
})
