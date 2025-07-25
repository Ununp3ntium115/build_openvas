# AI-Enhanced OpenVAS Examples

This directory contains example programs demonstrating the AI integration capabilities of the enhanced OpenVAS platform.

## Available Examples

### 1. AI Vulnerability Demo (`ai_vulnerability_demo`)

A comprehensive demonstration of AI-powered vulnerability analysis features.

**Features demonstrated:**
- Basic AI vulnerability analysis with business impact assessment
- AI-powered threat modeling for system architecture
- Intelligent scan optimization recommendations

**Usage:**
```bash
# Set your OpenAI API key (optional for demo)
export OPENAI_API_KEY="your-api-key-here"

# Run the demo
./ai_vulnerability_demo
```

**Sample Output:**
The demo will show:
- Vulnerability risk assessment with business context
- Threat modeling analysis for web applications
- Scan optimization recommendations
- Processing times and confidence scores

## Configuration

### API Keys

The examples can use real AI providers if you configure API keys:

1. **Environment Variables:**
   ```bash
   export OPENAI_API_KEY="your-openai-key"
   export ANTHROPIC_API_KEY="your-claude-key"
   ```

2. **Configuration Files:**
   Create `demo_api_key.txt` in the examples directory with your API key.

3. **Simulated Mode:**
   If no API key is provided, examples run with simulated AI responses.

### Sample Data

The examples use realistic vulnerability data including:
- CVE information
- CVSS scores and vectors
- Host and service details
- Business context (compliance requirements, data classification)

## Building Examples

Examples are built automatically when you build the main project:

```bash
mkdir build && cd build
cmake -DBUILD_EXAMPLES=ON ..
make
```

Or build examples separately:
```bash
cd examples
mkdir build && cd build
cmake ..
make
```

## Integration with OpenVAS

To integrate these AI features with your OpenVAS installation:

1. **Install the AI engine library:**
   ```bash
   sudo make install
   ```

2. **Configure OpenVAS preferences:**
   ```bash
   # Add to /etc/openvas/openvas.conf
   ai_openai_api_key = your-api-key
   ai_enable_vulnerability_analysis = yes
   ai_enable_threat_modeling = yes
   ai_enable_scan_optimization = yes
   ```

3. **Load AI plugins:**
   The vulnerability analysis plugin will be automatically loaded by OpenVAS.

## Customization

### Custom AI Prompts

You can customize AI prompts by modifying the provider implementations in `ai-engine/providers/`.

### Additional Providers

To add support for other AI providers:
1. Create a new provider file in `ai-engine/providers/`
2. Implement the provider interface
3. Update the configuration system

### Plugin Development

Create custom AI plugins by:
1. Following the plugin template in `plugins/`
2. Implementing the AI service API calls
3. Adding to the build system

## Troubleshooting

### Common Issues

1. **Missing API Key:**
   - Set environment variable or create config file
   - Examples will run in simulated mode without API key

2. **Network Connectivity:**
   - Check internet connection for AI provider APIs
   - Verify firewall settings

3. **Build Errors:**
   - Ensure all dependencies are installed
   - Check CMake configuration

4. **Runtime Errors:**
   - Verify library installation
   - Check LD_LIBRARY_PATH if needed

### Debug Mode

Enable debug logging:
```bash
export AI_SERVICE_DEBUG=1
./ai_vulnerability_demo
```

## Performance Considerations

- AI API calls can take 1-5 seconds depending on complexity
- Consider caching results for repeated analyses
- Use async processing for better user experience
- Monitor API rate limits and costs

## Security Notes

- Never log or expose API keys
- Sanitize sensitive data before sending to AI providers
- Consider using local AI models for sensitive environments
- Implement proper access controls for AI features

## Contributing

To contribute new examples:
1. Create a new `.c` file in this directory
2. Add to `CMakeLists.txt`
3. Update this README
4. Submit a pull request

## Support

For questions about the examples:
- Check the main project documentation
- Review the AI service API documentation
- Open an issue on GitHub