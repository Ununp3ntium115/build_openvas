# AI-Enhanced OpenVAS Web GUI

A modern, responsive web-based graphical user interface for managing and monitoring the AI-Enhanced OpenVAS system.

## 🚀 Features

### **Dashboard**
- Real-time system metrics and performance indicators
- Interactive charts showing request volume and provider performance
- System status monitoring with health checks
- Key performance indicators (KPIs) with trend analysis

### **AI Providers Management**
- Add, edit, and delete AI providers (OpenAI, Claude, Custom, Local)
- Provider health monitoring and testing
- Performance metrics per provider
- Configuration management with validation

### **AI Request Interface**
- Interactive form for sending AI requests
- Support for all task types (vulnerability analysis, threat modeling, etc.)
- Real-time response display with syntax highlighting
- Request history with filtering and search

### **Monitoring & Metrics**
- Real-time performance charts and graphs
- Detailed metrics tables with historical data
- Error rate tracking and alerting
- Resource utilization monitoring

### **Configuration Management**
- Service configuration with live updates
- Cache settings and management
- Security configuration options
- Feature flags and toggles

### **System Logs**
- Real-time log viewer with filtering
- Log level filtering (DEBUG, INFO, WARN, ERROR)
- Export functionality for audit trails
- Search and highlighting capabilities

## 🛠️ Technology Stack

- **Frontend**: HTML5, CSS3, JavaScript (ES6+)
- **Charts**: Chart.js for data visualization
- **Icons**: Font Awesome for consistent iconography
- **Styling**: CSS Custom Properties (CSS Variables) for theming
- **Architecture**: Modular JavaScript with separation of concerns

## 📁 Project Structure

```
gui/
├── index.html              # Main HTML file
├── css/
│   ├── styles.css          # Main stylesheet
│   └── components.css      # Component-specific styles
├── js/
│   ├── app.js             # Main application logic
│   ├── api.js             # API client and mock data
│   ├── charts.js          # Chart management
│   └── components.js      # UI components and utilities
├── assets/
│   └── favicon.svg        # Application favicon
├── server.py              # Development server with mock API
└── README.md              # This file
```

## 🚀 Quick Start

### **Option 1: Development Server (Recommended)**

```bash
# Navigate to GUI directory
cd gui

# Start the development server with mock API
python3 server.py 8080

# Open browser
open http://localhost:8080
```

### **Option 2: Static File Server**

```bash
# Navigate to GUI directory
cd gui

# Start a simple HTTP server
python3 -m http.server 8080

# Open browser
open http://localhost:8080
```

### **Option 3: Production Deployment**

```bash
# Install GUI files (after building the project)
sudo make install

# GUI files will be installed to:
# /usr/local/share/ai-enhanced-openvas/gui/

# Configure your web server (nginx, apache) to serve the files
```

## 🔧 Configuration

### **API Endpoint Configuration**

The GUI communicates with the AI-Enhanced OpenVAS backend via REST API. Update the API base URL in `js/api.js`:

```javascript
// For production
const apiClient = new AIServiceAPI('/api/v1');

// For development with mock data
const apiClient = new MockAIServiceAPI();
```

### **Customization**

#### **Theming**
Customize the appearance by modifying CSS variables in `css/styles.css`:

```css
:root {
    --primary-color: #2563eb;
    --success-color: #10b981;
    --warning-color: #f59e0b;
    --error-color: #ef4444;
    /* ... more variables */
}
```

#### **Chart Configuration**
Modify chart settings in `js/charts.js`:

```javascript
this.defaultOptions = {
    responsive: true,
    maintainAspectRatio: false,
    // ... customize chart options
};
```

## 📊 API Integration

The GUI expects the following API endpoints:

### **Service Management**
- `GET /api/v1/service/status` - Get service status
- `GET /api/v1/service/health` - Health check
- `POST /api/v1/service/restart` - Restart service

### **Provider Management**
- `GET /api/v1/providers` - List providers
- `POST /api/v1/providers` - Add provider
- `PUT /api/v1/providers/{id}` - Update provider
- `DELETE /api/v1/providers/{id}` - Delete provider
- `POST /api/v1/providers/{id}/test` - Test provider

### **AI Requests**
- `POST /api/v1/requests` - Send AI request
- `GET /api/v1/requests/history` - Get request history
- `GET /api/v1/requests/{id}` - Get specific request

### **Metrics & Monitoring**
- `GET /api/v1/metrics` - Get system metrics
- `GET /api/v1/metrics/performance` - Performance metrics
- `GET /api/v1/metrics/providers/{id}` - Provider metrics

### **Configuration**
- `GET /api/v1/config` - Get configuration
- `PUT /api/v1/config` - Update configuration
- `POST /api/v1/config/reset` - Reset to defaults

### **Logs**
- `GET /api/v1/logs` - Get logs
- `DELETE /api/v1/logs` - Clear logs
- `GET /api/v1/logs/export` - Export logs

## 🎨 UI Components

### **Navigation**
- Responsive navigation bar with section switching
- Active state management
- Mobile-friendly hamburger menu (planned)

### **Cards & Metrics**
- Metric cards with trend indicators
- Status badges with color coding
- Interactive hover effects

### **Forms**
- Real-time validation
- Error messaging
- Auto-completion support

### **Modals**
- Overlay modals for forms and confirmations
- Keyboard navigation support
- Click-outside-to-close functionality

### **Toast Notifications**
- Success, warning, error, and info notifications
- Auto-dismiss with configurable timing
- Stack management for multiple notifications

### **Charts & Graphs**
- Real-time updating charts
- Interactive tooltips
- Responsive design for mobile devices

## 🔒 Security Considerations

### **API Key Handling**
- API keys are masked in the UI
- Secure transmission over HTTPS
- No client-side storage of sensitive data

### **Input Validation**
- Client-side validation for user inputs
- XSS prevention through proper escaping
- CSRF protection (when integrated with backend)

### **Content Security Policy**
Recommended CSP header for production:

```
Content-Security-Policy: default-src 'self'; script-src 'self' 'unsafe-inline' https://cdn.jsdelivr.net https://cdnjs.cloudflare.com; style-src 'self' 'unsafe-inline' https://cdnjs.cloudflare.com; font-src 'self' https://cdnjs.cloudflare.com; img-src 'self' data:;
```

## 📱 Responsive Design

The GUI is fully responsive and works on:
- **Desktop**: Full feature set with multi-column layouts
- **Tablet**: Adapted layouts with touch-friendly controls
- **Mobile**: Single-column layout with collapsible navigation

### **Breakpoints**
- Desktop: 1024px and above
- Tablet: 768px to 1023px
- Mobile: Below 768px

## 🧪 Testing

### **Manual Testing Checklist**
- [ ] Navigation between all sections works
- [ ] Forms validate input correctly
- [ ] Charts render and update properly
- [ ] Modals open and close correctly
- [ ] Toast notifications appear and dismiss
- [ ] Responsive design works on different screen sizes
- [ ] API integration functions properly

### **Browser Compatibility**
- Chrome 90+
- Firefox 88+
- Safari 14+
- Edge 90+

## 🚀 Production Deployment

### **Web Server Configuration**

#### **Nginx Example**
```nginx
server {
    listen 80;
    server_name your-domain.com;
    
    location / {
        root /usr/local/share/ai-enhanced-openvas/gui;
        index index.html;
        try_files $uri $uri/ /index.html;
    }
    
    location /api/ {
        proxy_pass http://localhost:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

#### **Apache Example**
```apache
<VirtualHost *:80>
    ServerName your-domain.com
    DocumentRoot /usr/local/share/ai-enhanced-openvas/gui
    
    <Directory "/usr/local/share/ai-enhanced-openvas/gui">
        AllowOverride All
        Require all granted
    </Directory>
    
    ProxyPass /api/ http://localhost:8080/api/
    ProxyPassReverse /api/ http://localhost:8080/api/
</VirtualHost>
```

### **SSL/TLS Configuration**
Always use HTTPS in production:

```bash
# Generate SSL certificate (use Let's Encrypt in production)
sudo certbot --nginx -d your-domain.com
```

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/gui-enhancement`
3. Make your changes following the coding standards
4. Test thoroughly across different browsers and devices
5. Submit a pull request

### **Coding Standards**
- Use ES6+ JavaScript features
- Follow consistent indentation (2 spaces)
- Use meaningful variable and function names
- Add comments for complex logic
- Maintain responsive design principles

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/yourusername/ai-enhanced-openvas/issues)
- **Documentation**: [Project Wiki](https://github.com/yourusername/ai-enhanced-openvas/wiki)
- **Discussions**: [GitHub Discussions](https://github.com/yourusername/ai-enhanced-openvas/discussions)

---

**🎉 Enjoy using the AI-Enhanced OpenVAS Web GUI!**