(() => {
  function base64EncodeUtf8(text) {
    const bytes = new TextEncoder().encode(text);
    let binary = "";
    const chunkSize = 0x8000;
    for (let i = 0; i < bytes.length; i += chunkSize) {
      const chunk = bytes.subarray(i, i + chunkSize);
      binary += String.fromCharCode(...chunk);
    }
    return btoa(binary);
  }

  function findCppCodeBlocks() {
    const selectors = [
      "pre > code.language-cpp",
      "pre > code.language-c\\+\\+",
      "pre > code.lang-cpp",
      "pre > code.lang-c\\+\\+",
    ];
    return Array.from(document.querySelectorAll(selectors.join(",")));
  }

  function codeBlockHasMain(code) {
    return /\bmain\s*\(/.test(code);
  }

  function getPreElement(codeEl) {
    if (!codeEl) return null;
    const pre = codeEl.closest("pre");
    return pre instanceof HTMLElement ? pre : null;
  }

  function getHighlightContainer(preEl) {
    const highlight = preEl.closest(".highlight");
    return (highlight instanceof HTMLElement ? highlight : preEl.parentElement) || preEl;
  }

  function createButton(onClick) {
    const btn = document.createElement("button");
    btn.type = "button";
    btn.className = "md-clipboard online-compile-button";
    btn.title = "在线编译（打开 Compiler Explorer）";
    btn.setAttribute("aria-label", btn.title);
    btn.addEventListener("click", (e) => {
      e.preventDefault();
      e.stopPropagation();
      onClick();
    });
    btn.textContent = "在线编译";
    return btn;
  }

  function buildCompilerExplorerUrl(source) {
    const clientState = {
      sessions: [
        {
          id: 1,
          language: "c++",
          source,
          compilers: [],
          executors: [
            {
              compiler: {
                id: "clang_trunk",
                libs: [],
                options: "-std=c++23 -O2",
              },
            },
          ],
        },
      ],
    };

    const json = JSON.stringify(clientState);
    const b64 = base64EncodeUtf8(json);
    const encoded = encodeURIComponent(b64);
    return `https://godbolt.org/clientstate/${encoded}`;
  }

  function addButtons() {
    const blocks = findCppCodeBlocks();
    for (const codeEl of blocks) {
      const preEl = getPreElement(codeEl);
      if (!preEl) continue;

      const container = getHighlightContainer(preEl);
      if (container.querySelector(".online-compile-button")) continue;

      const code = (codeEl.textContent || "").trim();
      if (!code) continue;

      const btn = createButton(() => {
        const url = buildCompilerExplorerUrl(code);
        window.open(url, "_blank", "noopener,noreferrer");
      });

      if (!codeBlockHasMain(code)) {
        btn.classList.add("online-compile-button--no-main");
        btn.title = "在线编译（片段可能需要补全 main）";
        btn.setAttribute("aria-label", btn.title);
      }

      container.appendChild(btn);
      container.classList.add("online-compile-container");
    }
  }

  function onReady(fn) {
    if (document.readyState === "loading") {
      document.addEventListener("DOMContentLoaded", fn, { once: true });
    } else {
      fn();
    }
  }

  onReady(() => {
    addButtons();

    const observer = new MutationObserver(() => addButtons());
    observer.observe(document.body, { childList: true, subtree: true });
  });
})();
