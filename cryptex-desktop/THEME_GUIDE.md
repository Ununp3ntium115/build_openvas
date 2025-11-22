# openFireVulnerability Theme Guide

**Fire-Themed Professional Security UI**

---

## Color Palette

### Primary Colors - Fire Theme

```css
--primary-color: #ff6b35        /* Flame Orange */
--primary-hover: #ff8c5a        /* Lighter Flame */
--primary-dark: #e55a2b          /* Darker Flame */

--secondary-color: #ffa500       /* Ember Orange */
--secondary-hover: #ffb733       /* Lighter Ember */
--accent-fire: #ff4500           /* Pure Fire Red-Orange */
--accent-ember: #d4621e          /* Deep Ember */
```

### Background Colors - Dark Professional

```css
--bg-primary: #0f0f0f           /* Near Black */
--bg-secondary: #1a1a1a         /* Charcoal */
--bg-tertiary: #242424          /* Lighter Charcoal */
--bg-hover: #2e2e2e             /* Hover State */
--bg-active: #383838            /* Active/Pressed */
```

### Severity Colors - Fire Gradient

```css
--critical-color: #dc2626       /* Critical Red */
--high-color: #fb923c           /* High Orange */
--medium-color: #facc15         /* Medium Yellow */
--low-color: #4ade80            /* Low Green */
--info-color: #60a5fa           /* Info Blue */
```

### Fire Gradients

```css
--gradient-fire: linear-gradient(135deg, #ff4500 0%, #ff6b35 50%, #ffa500 100%)
--gradient-ember: linear-gradient(135deg, #d4621e 0%, #e55a2b 100%)
--gradient-dark: linear-gradient(180deg, #1a1a1a 0%, #0f0f0f 100%)
```

### Shadows with Fire Glow

```css
--shadow-sm: 0 1px 3px rgba(0, 0, 0, 0.5)
--shadow: 0 2px 8px rgba(0, 0, 0, 0.4)
--shadow-md: 0 4px 12px rgba(0, 0, 0, 0.5)
--shadow-lg: 0 8px 24px rgba(0, 0, 0, 0.6)
--shadow-fire: 0 0 20px rgba(255, 107, 53, 0.15)  /* Subtle fire glow */
```

---

## Component Styling Patterns

### Fire Gradient Text

```css
.logo-text {
  background: var(--gradient-fire);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
}
```

**Usage**: Logos, headings, important text

### Fire Gradient Buttons

```css
.btn-primary {
  background: var(--gradient-fire);
  color: var(--text-inverse);
  box-shadow: var(--shadow-fire);
  transition: all var(--transition-normal);
}

.btn-primary:hover {
  background: var(--gradient-ember);
  box-shadow: var(--shadow-md);
  transform: translateY(-1px);
}
```

**Effect**: Smooth fire gradient with lift animation on hover

### Fire Glow on Focus

```css
*:focus-visible {
  outline: 2px solid var(--primary-color);
  outline-offset: 2px;
  box-shadow: var(--shadow-fire);
}
```

**Effect**: Subtle orange glow around focused elements

### Active Navigation Items

```css
.nav-item:global(.active) {
  background: var(--bg-tertiary);
  color: var(--primary-color);
  border-left: 3px solid var(--primary-color);
  box-shadow: var(--shadow-fire);
}

.nav-item:global(.active)::before {
  background: var(--gradient-fire);
}
```

**Effect**: Fire gradient border with glow

---

## Usage Examples

### Dashboard Stat Cards

```svelte
<div class="stat-card primary">
  <!-- Card content -->
</div>

<style>
.stat-card::before {
  content: '';
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  height: 3px;
}

.stat-card.primary::before {
  background: var(--gradient-fire);
}
</style>
```

### Chart Values

```svelte
<div class="chart-value">245</div>

<style>
.chart-value {
  font-size: 2.5rem;
  font-weight: 700;
  background: var(--gradient-fire);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
}
</style>
```

### Progress Bars

```svelte
<div class="progress-bar">
  <div class="progress-fill" style="width: {progress}%"></div>
</div>

<style>
.progress-fill {
  background: var(--gradient-fire);
  transition: width 0.3s ease;
}
</style>
```

### Status Indicators

```svelte
<span class="status-dot online"></span>

<style>
.status-dot.online {
  background: var(--primary-color);
  box-shadow: 0 0 8px rgba(255, 107, 53, 0.5);
}
</style>
```

---

## Animation & Transitions

### Standard Transitions

```css
--transition-fast: 150ms cubic-bezier(0.4, 0, 0.2, 1)
--transition-normal: 250ms cubic-bezier(0.4, 0, 0.2, 1)
--transition-slow: 350ms cubic-bezier(0.4, 0, 0.2, 1)
```

### Hover Effects with Fire

```css
.element {
  transition: all var(--transition-normal);
}

.element:hover {
  color: var(--primary-color);
  box-shadow: var(--shadow-fire);
  transform: translateY(-1px);
}
```

### Pulse Animation (for status indicators)

```css
@keyframes pulse {
  0%, 100% { opacity: 1; }
  50% { opacity: 0.5; }
}

.status-dot {
  animation: pulse 2s infinite;
}
```

---

## Typography

### Font Stack

```css
font-family: 'Segoe UI', 'Roboto', 'Oxygen', 'Ubuntu', 'Cantarell', 'Inter', sans-serif;
```

### Heading Styles

```css
h1, h2, h3, h4, h5, h6 {
  font-weight: 600;
  line-height: 1.2;
  letter-spacing: -0.02em;
}
```

### Fire Gradient Headings

```css
.page-title {
  background: var(--gradient-fire);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
}
```

---

## Best Practices

### 1. Use Fire Gradients Sparingly

✅ **Do**: Use on primary CTAs, logos, important headings
❌ **Don't**: Apply to all text or backgrounds

### 2. Maintain Contrast

```css
/* Good - high contrast */
.card {
  background: var(--bg-secondary);
  color: var(--text-primary);
}

/* Bad - low contrast */
.card {
  background: var(--bg-primary);
  color: var(--bg-tertiary);
}
```

### 3. Consistent Shadow Usage

```css
/* Cards */
box-shadow: var(--shadow-sm);

/* Buttons */
box-shadow: var(--shadow-fire);

/* Modals */
box-shadow: var(--shadow-lg);
```

### 4. Fire Glow on Interactive Elements

```css
button:hover,
input:focus,
.nav-item.active {
  box-shadow: var(--shadow-fire);
}
```

---

## Component-Specific Guidelines

### Buttons

**Primary Actions** - Fire gradient with glow
```css
background: var(--gradient-fire);
box-shadow: var(--shadow-fire);
```

**Secondary Actions** - Subtle borders
```css
background: transparent;
border: 1px solid var(--border-color);
```

### Cards

**Top Border Accent**
```css
.card::before {
  content: '';
  height: 3px;
  background: var(--gradient-fire);
}
```

### Tables

**Row Hover** - Subtle background change
```css
tr:hover {
  background: var(--bg-hover);
}
```

**Active Row** - Fire accent
```css
tr.active {
  background: var(--bg-tertiary);
  border-left: 3px solid var(--primary-color);
}
```

### Forms

**Input Focus** - Fire border and glow
```css
input:focus {
  outline: none;
  border-color: var(--primary-color);
  box-shadow: var(--shadow-fire);
}
```

---

## Accessibility

### Color Contrast Ratios

| Combination | Ratio | WCAG Level |
|-------------|-------|------------|
| --text-primary on --bg-primary | 13.5:1 | AAA ✓ |
| --text-secondary on --bg-secondary | 7.2:1 | AAA ✓ |
| --primary-color on --bg-primary | 4.8:1 | AA ✓ |

### Focus Indicators

Always include visible focus indicators:

```css
*:focus-visible {
  outline: 2px solid var(--primary-color);
  outline-offset: 2px;
  box-shadow: var(--shadow-fire);
}
```

### Text Selection

```css
::selection {
  background: var(--primary-color);
  color: var(--text-inverse);
}
```

---

## Dark Mode Only

This theme is optimized for dark mode. Light mode is not supported as it doesn't align with the security tool aesthetic.

---

## Logo & Branding

### SVG Fire Shield Logo

```svelte
<svg width="28" height="28" viewBox="0 0 64 64">
  <defs>
    <linearGradient id="fireGradient" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" stop-color="#ff4500"/>
      <stop offset="50%" stop-color="#ff6b35"/>
      <stop offset="100%" stop-color="#ffa500"/>
    </linearGradient>
  </defs>
  <path d="M32 4L8 14V28C8 42 16 54 32 60C48 54 56 42 56 28V14L32 4Z"
        fill="url(#fireGradient)"
        stroke="var(--primary-hover)"
        stroke-width="2"/>
  <path d="M32 20C28 24 28 28 32 32C36 28 36 24 32 20Z"
        fill="var(--accent-fire)"
        opacity="0.8"/>
</svg>
```

### Logo Text

```css
.logo-text {
  font-size: 1.1rem;
  font-weight: 700;
  background: var(--gradient-fire);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
  letter-spacing: 0.5px;
}
```

---

## File Structure

```
cryptex-desktop/src/
├── App.svelte              # Root theme variables
├── app.css                 # Global fire theme styles
├── components/
│   ├── TopBar.svelte       # Fire gradient logo
│   └── Sidebar.svelte      # Fire gradient active states
└── views/
    ├── Dashboard.svelte    # Fire gradient stat cards
    ├── Scans.svelte        # Fire gradient progress bars
    └── ...
```

---

## Migration from Greenbone Theme

### Color Replacements

| Old (Greenbone) | New (Fire) |
|----------------|------------|
| `--gsa-green` | `--primary-color` or `--gradient-fire` |
| `--gsa-green-dark` | `--primary-dark` |
| `--gsa-green-light` | `--primary-hover` |

### Component Updates

1. Replace all `var(--gsa-green)` with `var(--primary-color)`
2. Update button backgrounds to use `var(--gradient-fire)`
3. Add `box-shadow: var(--shadow-fire)` to interactive elements
4. Use gradient text for headings and logos

---

## Examples

See complete implementations in:

- **App.svelte** - Theme variable definitions
- **TopBar.svelte** - Fire gradient logo and title
- **Sidebar.svelte** - Active navigation with fire glow
- **Dashboard.svelte** - Stat cards with fire gradients
- **Scans.svelte** - Progress bars and buttons

---

**🔥 openFireVulnerability - Professional security with fire!**
