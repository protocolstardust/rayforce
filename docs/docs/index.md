---
hide:
  - navigation.top
  - toc
  - title
template: main.html
---

<div class="hero-section">
  <div class="hero-background"></div>
  <div class="hero-container">
    <div class="hero-left">
      <h1 class="hero-title">
        <img src="assets/inverted-logo-full-transparent.png" alt="RayforceDB" class="hero-logo">
      </h1>
      <p class="hero-description">
        The <b>ultra-fast</b> columnar database engineered to deliver the <b>best high-speed performance</b> for your real-time storage solutions.
      </p>
      <div class="hero-buttons">
        <a href="content2/get-started/overview.html" class="hero-btn hero-btn-primary">
          <span class="btn-icon">↓</span>
          <span>Get Started</span>
        </a>
        <a href="https://rayforcedb.zulipchat.com" class="hero-btn hero-btn-secondary">
          <span class="btn-icon">💬</span>
          <span>Join Zulip</span>
        </a>
      </div>
    </div>
    <div class="hero-right">
      <div class="code-editor">
        <div class="code-window-controls">
          <span class="window-dot window-dot-red"></span>
          <span class="window-dot window-dot-yellow"></span>
          <span class="window-dot window-dot-green"></span>
        </div>
        <div class="code-content">
          <pre><code><span class="code-line">(<span class="code-keyword">if</span> (<span class="code-keyword">nil?</span> (<span class="code-keyword">resolve</span> <span class="code-string">'n</span>)) (<span class="code-keyword">set</span> n <span class="code-number">10000000</span>))</span>
<span class="code-line"></span>
<span class="code-line">(<span class="code-keyword">set</span> t (<span class="code-keyword">table</span> [<span class="code-keyword">OrderId</span> <span class="code-keyword">Symbol</span> <span class="code-keyword">Price</span> <span class="code-keyword">Size</span> <span class="code-keyword">Tape</span> <span class="code-keyword">Timestamp</span>]</span>
<span class="code-line"></span>
<span class="code-line">        (<span class="code-keyword">list</span> (<span class="code-keyword">take</span> n (<span class="code-keyword">guid</span> <span class="code-number">1000000</span>))</span>
<span class="code-line">              (<span class="code-keyword">take</span> n [<span class="code-string">AAPL</span> <span class="code-string">GOOG</span> <span class="code-string">MSFT</span> <span class="code-string">IBM</span> <span class="code-string">AMZN</span> <span class="code-string">FB</span> <span class="code-string">BABA</span>])</span>
<span class="code-line">              (<span class="code-keyword">as</span> <span class="code-string">'F64</span> (<span class="code-keyword">til</span> n))</span>
<span class="code-line"></span>
<span class="code-line">              (<span class="code-keyword">take</span> n (<span class="code-keyword">+</span> <span class="code-number">1</span> (<span class="code-keyword">til</span> <span class="code-number">3</span>)))</span>
<span class="code-line"></span>
<span class="code-line">              (<span class="code-keyword">map</span> (<span class="code-keyword">fn</span> [x] (<span class="code-keyword">as</span> <span class="code-string">'String</span> x)) (<span class="code-keyword">take</span> n (<span class="code-keyword">til</span> <span class="code-number">10</span>)))</span>
<span class="code-line"></span>
<span class="code-line">              (<span class="code-keyword">as</span> <span class="code-string">'Timestamp</span> (<span class="code-keyword">til</span> n))</span>
<span class="code-line"></span>
<span class="code-line">        ))</span>
<span class="code-line"></span>
<span class="code-line">)</span>
<span class="code-line"></span>
<span class="code-line">(<span class="code-keyword">select</span> {</span>
<span class="code-line">    <span class="code-keyword">from</span>: t <span class="code-keyword">by</span>: <span class="code-keyword">Symbol</span></span>
<span class="code-line">    <span class="code-keyword">count</span>: (<span class="code-keyword">count</span> <span class="code-keyword">OrderId</span>)</span>
<span class="code-line">    <span class="code-keyword">total</span>: (<span class="code-keyword">sum</span> <span class="code-keyword">Price</span>)</span>
<span class="code-line">    <span class="code-keyword">size</span>: (<span class="code-keyword">sum</span> <span class="code-keyword">Size</span>)</span>
<span class="code-line">    <span class="code-keyword">where</span>: (<span class="code-keyword">like</span> <span class="code-keyword">Tape</span> <span class="code-string">"1*"</span>)</span>
<span class="code-line">})</span>
<span class="code-line"></span></code></pre>
        </div>
      </div>
    </div>
  </div>
</div>

<script>
// Hide "Home" label/page title that appears above hero section
(function() {
  function hideHomeLabel() {
    const contentInner = document.querySelector('.md-content__inner');
    if (!contentInner) return;
    
    // Hide all h1 elements that are not in hero-section
    const allH1s = contentInner.querySelectorAll('h1:not(.hero-title)');
    allH1s.forEach(h1 => {
      if (!h1.closest('.hero-section')) {
        h1.style.cssText = 'display: none !important; visibility: hidden !important; height: 0 !important; margin: 0 !important; padding: 0 !important;';
      }
    });
    
    // Hide any element before hero-section
    const heroSection = contentInner.querySelector('.hero-section');
    if (heroSection) {
      let element = heroSection.previousElementSibling;
      while (element) {
        const text = (element.textContent || '').trim();
        if (text === 'Home' || element.tagName === 'H1' || 
            element.classList.contains('md-typeset') ||
            element.querySelector('h1')) {
          element.style.cssText = 'display: none !important; visibility: hidden !important; height: 0 !important; margin: 0 !important; padding: 0 !important; overflow: hidden !important;';
        }
        element = element.previousElementSibling;
      }
    }
    
    // Hide navigation.top element if it exists (multiple selectors)
    const navTopSelectors = [
      '.md-top',
      '[class*="md-top"]',
      '[data-md-component="title"]',
      '[data-md-component="page-title"]',
      '.md-content__inner > .md-typeset:first-child',
      '.md-content__inner > h1:first-child'
    ];
    
    navTopSelectors.forEach(selector => {
      const elements = document.querySelectorAll(selector);
      elements.forEach(el => {
        if (!el.closest('.hero-section') && !el.closest('nav') && !el.closest('.md-header')) {
          const text = (el.textContent || '').trim();
          if (text === 'Home' || el.tagName === 'H1') {
            el.style.cssText = 'display: none !important; visibility: hidden !important; height: 0 !important; margin: 0 !important; padding: 0 !important; overflow: hidden !important;';
          }
        }
      });
    });
    
    // Hide any element with text "Home" that's not in navigation or hero
    const allElements = contentInner.querySelectorAll('*');
    allElements.forEach(el => {
      if (el.closest('.hero-section') || el.closest('nav') || el.closest('.md-header') || el.closest('.md-tabs')) {
        return; // Skip elements in hero, nav, header, or tabs
      }
      const text = (el.textContent || '').trim();
      // Hide if it's exactly "Home" or if it's an h1
      if ((text === 'Home' && el.children.length === 0) || (el.tagName === 'H1' && !el.closest('.hero-section'))) {
        el.style.cssText = 'display: none !important; visibility: hidden !important; height: 0 !important; margin: 0 !important; padding: 0 !important; overflow: hidden !important;';
      }
    });
    
    // Specifically target the first child of md-content__inner if it's not hero-section
    const firstChild = contentInner.firstElementChild;
    if (firstChild && !firstChild.classList.contains('hero-section')) {
      const text = (firstChild.textContent || '').trim();
      if (text === 'Home' || firstChild.tagName === 'H1' || firstChild.querySelector('h1')) {
        firstChild.style.cssText = 'display: none !important; visibility: hidden !important; height: 0 !important; margin: 0 !important; padding: 0 !important; overflow: hidden !important;';
      }
    }
  }
  
  // Run immediately and also on DOMContentLoaded
  if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', hideHomeLabel);
  } else {
    hideHomeLabel();
  }
  
  // Also run after a short delay to catch dynamically added content
  setTimeout(hideHomeLabel, 100);
  setTimeout(hideHomeLabel, 500);
  setTimeout(hideHomeLabel, 1000);
  
  // Use MutationObserver to catch dynamically added content
  const observer = new MutationObserver(function(mutations) {
    let shouldHide = false;
    mutations.forEach(function(mutation) {
      if (mutation.addedNodes.length > 0) {
        shouldHide = true;
      }
    });
    if (shouldHide) {
      hideHomeLabel();
    }
  });
  
  // Start observing
  if (contentInner) {
    observer.observe(contentInner, {
      childList: true,
      subtree: true
    });
  }
})();
</script>
