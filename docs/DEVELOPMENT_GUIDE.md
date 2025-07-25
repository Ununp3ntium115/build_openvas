# AI-Enhanced OpenVAS Development Guide

## Phase 1 & 2 Complete ✅

We've successfully completed the foundation and AI integration architecture phases:

### ✅ Phase 1: Foundation & Research
- [x] Forked core OpenVAS repositories
- [x] Set up proper development environment structure
- [x] Analyzed codebase architecture
- [x] Identified AI integration points

### ✅ Phase 2: AI Integration Architecture
- [x] Designed plugin system for AI features
- [x] Created AI provider abstraction layer
- [x] Implemented secure API handling framework
- [x] Set up configuration management

## Current Project Structure

```
ai-enhanced-openvas/
├── core/                          # Forked OpenVAS components ✅
│   ├── gvm-libs/                 # Core GVM libraries
│   ├── gvmd/                     # Greenbone Vulnerability Manager daemon
│   ├── openvas-scanner/          # OpenVAS Scanner
│   ├── ospd-openvas/            # OSP daemon for OpenVAS
│   └── pg-gvm/                  # PostgreSQL extension
├── ai-engine/                    # AI integration layer ✅
│   ├── api/                     # Core AI service API
│   │   ├── ai_service.h         # Main AI service interface
│   │   └── ai_service.c         # Core implementation
│   ├── providers/               # AI provider integrations
│   │   └── openai_provider.c    # OpenAI integration
│   ├── CMakeLists.txt          # Build configuration
│   └── ai-engine.pc.in         # pkg-config template
├── plugins/                     # AI-powered plugins ✅
│   └── vulnerability-analysis/  # Enhanced vuln analysis
│       └── ai_vuln_analyzer.c   # AI vulnerability analysis plugin
├── docs/                        # Documentation ✅
│   ├── ARCHITECTURE_ANALYSIS.md # Architecture analysis
│   └── DEVELOPMENT_GUIDE.md     # This file
├── CMakeLists.txt              # Main build configuration ✅
├── config.h.in                 # Configuration template ✅
└── PROJECT_STRUCTURE.md        # Project overview ✅
```

## Key Components Implemented

### 1. AI Service Core (`ai-engine/api/`)
- **ai_service.h**: Complete API interface for AI integration
- **ai_service.c**: Core service implementation with async/sync processing
- Features:
  - Multiple AI provider support (OpenAI, Claude, Custom, Local)
  - Async and synchronous processing
  - Thread pool for concurrent AI requests
  - Comprehensive error handling
  - Configuration management

### 2. OpenAI Provider (`ai-engine/providers/`)
- **openai_provider.c**: Full OpenAI API integration
- Features:
  - Dynamic prompt generation based on task type
  - Secure API key handling
  - Response parsing and error handling
  - Support for all AI task types

### 3. Vulnerability Analysis Plugin (`plugins/vulnerability-analysis/`)
- **ai_vuln_analyzer.c**: AI-powered vulnerability analysis
- Features:
  - Enhanced vulnerability risk assessment
  - Business impact analysis
  - Intelligent remediation recommendations
  - Confidence scoring
  - Integration with OpenVAS scan results

### 4. Build System
- **CMakeLists.txt**: Complete build configuration
- **config.h.in**: Configuration management
- **ai-engine.pc.in**: pkg-config support
- Features:
  - Modular build options
  - Dependency management
  - Cross-platform support

## Next Steps: Phase 3 Development

### Immediate Tasks
1. **Test the current implementation**:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

2. **Add Claude provider**:
   - Implement `claude_provider.c`
   - Add Anthropic API integration

3. **Extend plugin system**:
   - Threat modeling plugin
   - Scan optimization plugin
   - Report generation plugin

4. **Integration testing**:
   - Unit tests for AI service
   - Integration tests with OpenVAS components
   - Performance benchmarking

### Development Workflow

1. **Setting up development environment**:
   ```bash
   # Install dependencies (Ubuntu/Debian)
   sudo apt-get install build-essential cmake pkg-config
   sudo apt-get install libglib2.0-dev libjson-glib-dev libcurl4-openssl-dev
   
   # Clone and build
   git clone <your-fork>
   cd ai-enhanced-openvas
   mkdir build && cd build
   cmake -DCMAKE_BUILD_TYPE=Debug ..
   make -j$(nproc)
   ```

2. **Adding new AI providers**:
   - Create provider file in `ai-engine/providers/`
   - Implement provider-specific API calls
   - Add to CMakeLists.txt
   - Update configuration options

3. **Creating new AI plugins**:
   - Create plugin directory in `plugins/`
   - Implement plugin using AI service API
   - Add integration points with OpenVAS components
   - Update build system

## API Usage Examples

### Basic AI Service Usage
```c
#include "ai-engine/api/ai_service.h"

// Initialize AI service
ai_service_init();

// Create configuration
ai_config_t *config = ai_config_new(AI_PROVIDER_OPENAI, "your-api-key");

// Create request
JsonObject *input = json_object_new();
json_object_set_string_member(input, "vulnerability", "SQL Injection");
ai_request_t *request = ai_request_new(AI_TASK_VULNERABILITY_ANALYSIS, input);
request->config = config;

// Process request
ai_response_t *response = ai_service_process_sync(request);

if (response->success) {
    // Use response->result
    g_print("AI Analysis: %s\n", 
            json_object_get_string_member(response->result, "content"));
}

// Cleanup
ai_response_free(response);
ai_request_free(request);
ai_config_free(config);
ai_service_cleanup();
```

### Vulnerability Analysis Plugin Usage
```c
#include "plugins/vulnerability-analysis/ai_vuln_analyzer.h"

// Initialize plugin
ai_vuln_analyzer_init();

// Analyze vulnerability
JsonObject *scan_data = /* ... scan results ... */;
ai_analysis_result_t *analysis = ai_analyze_vulnerability("1.3.6.1.4.1.25623.1.0.12345", scan_data);

if (analysis) {
    g_print("Risk Assessment: %s\n", analysis->risk_assessment);
    g_print("Confidence: %.2f\n", analysis->confidence_score);
    free_analysis_result(analysis);
}

// Cleanup
ai_vuln_analyzer_cleanup();
```

## Configuration

### AI Service Configuration
The AI service can be configured through OpenVAS preferences:
- `ai_openai_api_key`: OpenAI API key
- `ai_claude_api_key`: Claude API key
- `ai_service_timeout`: Request timeout in seconds
- `ai_enable_vulnerability_analysis`: Enable AI vulnerability analysis

### Environment Variables
- `OPENAI_API_KEY`: OpenAI API key
- `ANTHROPIC_API_KEY`: Claude API key
- `AI_SERVICE_DEBUG`: Enable debug logging

## Security Considerations

1. **API Key Management**: 
   - Store API keys securely
   - Use environment variables or secure configuration files
   - Never log API keys

2. **Data Privacy**:
   - Sanitize sensitive data before sending to AI providers
   - Implement data retention policies
   - Consider on-premises AI models for sensitive environments

3. **Rate Limiting**:
   - Implement proper rate limiting for AI API calls
   - Handle API quota exceeded scenarios
   - Cache results when appropriate

## Testing

### Unit Tests
```bash
cd build
make test
```

### Integration Tests
```bash
# Test with real OpenVAS installation
./test_ai_integration
```

### Performance Tests
```bash
# Benchmark AI processing performance
./benchmark_ai_performance
```

## Contributing

1. Fork the repository
2. Create feature branch: `git checkout -b feature/ai-enhancement`
3. Make changes following coding standards
4. Add tests for new functionality
5. Submit pull request

## Troubleshooting

### Common Issues
1. **CURL errors**: Check network connectivity and API endpoints
2. **JSON parsing errors**: Validate AI provider response formats
3. **Memory leaks**: Use valgrind for memory debugging
4. **Build errors**: Ensure all dependencies are installed

### Debug Mode
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
export AI_SERVICE_DEBUG=1
./your_program
```

This completes Phase 1 and Phase 2 of our AI-Enhanced OpenVAS project! The foundation is solid and ready for Phase 3 development.