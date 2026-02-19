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

  function findCppCodeBlocks(root) {
    const selectors = [
      "pre > code.language-cpp",
      "pre > code.language-c\\+\\+",
      "pre > code.lang-cpp",
      "pre > code.lang-c\\+\\+",
    ];
    return Array.from((root || document).querySelectorAll(selectors.join(",")));
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

  function getSourceForCurrentPage() {
    const content = document.querySelector(".md-content");
    const blocks = findCppCodeBlocks(content || document);
    const first = blocks[0];
    const code = (first?.textContent || "").trim();
    if (code) return code;
    return "#include <iostream>\n\nint main() {\n    std::cout << \"Hello, C++!\" << '\\n';\n    return 0;\n}\n";
  }

  function openCompilerExplorer() {
    const url = buildCompilerExplorerUrl(getSourceForCurrentPage());
    window.open(url, "_blank", "noopener,noreferrer");
  }

  function createHeaderButton() {
    const a = document.createElement("a");
    a.href = "https://godbolt.org/";
    a.className = "md-header__button md-icon online-compile-header-button";
    a.title = "在线编译（打开 Compiler Explorer）";
    a.setAttribute("aria-label", a.title);
    a.innerHTML =
      '<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" aria-hidden="true">' +
      '<path d="M10 17l6-5-6-5v10ZM19 19H5V5h14v14Zm0-16H5a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2V5a2 2 0 0 0-2-2Z"/>' +
      "</svg>";

    a.addEventListener("click", (e) => {
      e.preventDefault();
      openCompilerExplorer();
    });

    return a;
  }

  function ensureHeaderButton() {
    const headerInner =
      document.querySelector(".md-header__inner") ||
      document.querySelector("header.md-header") ||
      document.body;

    if (!(headerInner instanceof HTMLElement)) return;
    if (headerInner.querySelector(".online-compile-header-button")) return;

    headerInner.appendChild(createHeaderButton());
  }

  function onReady(fn) {
    if (document.readyState === "loading") {
      document.addEventListener("DOMContentLoaded", fn, { once: true });
    } else {
      fn();
    }
  }

  onReady(() => {
    ensureHeaderButton();

    const observer = new MutationObserver(() => ensureHeaderButton());
    observer.observe(document.body, { childList: true, subtree: true });
  });
})();
