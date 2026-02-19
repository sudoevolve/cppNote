(() => {
  const STORAGE_KEY = "qrcraft_palette_v1";
  const STYLE_TAG_ID = "qrcraft-theme-palette-style";

  const presets = [
    {
      id: "md3",
      name: "MD3 默认",
      primary: "#6750A4",
      accent: "#EFB8C8",
    },
    {
      id: "teal_purple",
      name: "Teal + Purple",
      primary: "#009688",
      accent: "#9C27B0",
    },
    {
      id: "blue_amber",
      name: "Blue + Amber",
      primary: "#1E88E5",
      accent: "#FFB300",
    },
  ];

  function safeParse(json) {
    try {
      return JSON.parse(json);
    } catch {
      return null;
    }
  }

  function loadConfig() {
    const raw = localStorage.getItem(STORAGE_KEY);
    const data = raw ? safeParse(raw) : null;
    if (!data || typeof data !== "object") return null;
    return data;
  }

  function saveConfig(cfg) {
    localStorage.setItem(STORAGE_KEY, JSON.stringify(cfg));
  }

  function isHexColor(s) {
    return typeof s === "string" && /^#[0-9a-fA-F]{6}$/.test(s);
  }

  function hexToRgb(hex) {
    if (!isHexColor(hex)) return null;
    const r = parseInt(hex.slice(1, 3), 16);
    const g = parseInt(hex.slice(3, 5), 16);
    const b = parseInt(hex.slice(5, 7), 16);
    return { r, g, b };
  }

  function ensureStyleTag() {
    const existing = document.getElementById(STYLE_TAG_ID);
    if (existing instanceof HTMLStyleElement) return existing;
    const style = document.createElement("style");
    style.id = STYLE_TAG_ID;
    document.head.appendChild(style);
    return style;
  }

  function clearStyleTag() {
    const el = document.getElementById(STYLE_TAG_ID);
    if (el && el.parentNode) el.parentNode.removeChild(el);
  }

  function applyColors(primary, accent) {
    const p = isHexColor(primary) ? primary : null;
    const a = isHexColor(accent) ? accent : null;
    const pRgb = p ? hexToRgb(p) : null;
    const aRgb = a ? hexToRgb(a) : null;

    const lines = [];
    lines.push(":root, body, body[data-md-color-scheme], body[data-md-color-primary], body[data-md-color-accent] {");
    if (p) {
      lines.push(`  --md-primary-fg-color: ${p} !important;`);
      lines.push(`  --md-primary-fg-color--light: ${p} !important;`);
      lines.push(`  --md-primary-fg-color--dark: ${p} !important;`);
      if (pRgb) lines.push(`  --md-primary-fg-color--rgb: ${pRgb.r}, ${pRgb.g}, ${pRgb.b} !important;`);
      if (pRgb) lines.push(`  --md-primary-fg-color--transparent: rgba(${pRgb.r}, ${pRgb.g}, ${pRgb.b}, 0.12) !important;`);
    }
    if (a) {
      lines.push(`  --md-accent-fg-color: ${a} !important;`);
      if (aRgb) lines.push(`  --md-accent-fg-color--rgb: ${aRgb.r}, ${aRgb.g}, ${aRgb.b} !important;`);
      if (aRgb) lines.push(`  --md-accent-fg-color--transparent: rgba(${aRgb.r}, ${aRgb.g}, ${aRgb.b}, 0.12) !important;`);
      lines.push(`  --md-typeset-a-color: ${a} !important;`);
      lines.push(`  --md-typeset-a-color--hover: ${a} !important;`);
      lines.push(`  --md-typeset-mark-color: ${a} !important;`);
    }
    lines.push("}");

    const style = ensureStyleTag();
    style.textContent = lines.join("\n");
  }

  function applyFromStorage() {
    const cfg = loadConfig();
    if (!cfg) return;
    applyColors(cfg.primary, cfg.accent);
  }

  function createHeaderButton() {
    const btn = document.createElement("button");
    btn.type = "button";
    btn.className = "md-header__button md-icon theme-palette-header-button";
    btn.title = "配色设置";
    btn.setAttribute("aria-label", btn.title);
    btn.innerHTML =
      '<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" aria-hidden="true">' +
      '<path d="M12 22a10 10 0 1 1 10-10c0 1.1-.9 2-2 2h-1a2 2 0 0 0-2 2c0 2.2-1.8 4-4 4Zm0-18a8 8 0 0 0 0 16c1.1 0 2-.9 2-2 0-1.7 1.3-3 3-3h1c0-2.2-1.8-4-4-4H9a4 4 0 0 1 0-8h3Zm-5 6a1 1 0 1 0 0-2 1 1 0 0 0 0 2Zm3-3a1 1 0 1 0 0-2 1 1 0 0 0 0 2Zm4 0a1 1 0 1 0 0-2 1 1 0 0 0 0 2Zm3 3a1 1 0 1 0 0-2 1 1 0 0 0 0 2Z"/>' +
      "</svg>";
    return btn;
  }

  function ensureHeaderButton(onClick) {
    const header = document.querySelector("header.md-header") || document.querySelector(".md-header");
    if (!(header instanceof HTMLElement)) return;

    const search = header.querySelector(".md-search");
    const target =
      (search && search.parentElement instanceof HTMLElement ? search.parentElement : null) ||
      header.querySelector(".md-header__inner") ||
      header;

    if (!(target instanceof HTMLElement)) return;
    if (target.querySelector(".theme-palette-header-button")) return;

    const btn = createHeaderButton();
    btn.addEventListener("click", (e) => {
      e.preventDefault();
      e.stopPropagation();
      onClick();
    });

    if (search && search instanceof HTMLElement) {
      const before = target.querySelector(".md-search") || search;
      target.insertBefore(btn, before);
    } else {
      target.appendChild(btn);
    }
  }

  function createDialog() {
    const overlay = document.createElement("div");
    overlay.className = "theme-palette-overlay";
    overlay.hidden = true;

    const panel = document.createElement("div");
    panel.className = "theme-palette-panel";
    panel.setAttribute("role", "dialog");
    panel.setAttribute("aria-modal", "true");
    panel.setAttribute("aria-label", "配色设置");

    const header = document.createElement("div");
    header.className = "theme-palette-panel__header";

    const title = document.createElement("div");
    title.className = "theme-palette-panel__title";
    title.textContent = "配色设置";

    const close = document.createElement("button");
    close.type = "button";
    close.className = "md-button theme-palette-close";
    close.textContent = "关闭";

    header.appendChild(title);
    header.appendChild(close);

    const body = document.createElement("div");
    body.className = "theme-palette-panel__body";

    const presetLabel = document.createElement("div");
    presetLabel.className = "theme-palette-section-title";
    presetLabel.textContent = "预设";

    const presetList = document.createElement("div");
    presetList.className = "theme-palette-presets";

    for (const p of presets) {
      const item = document.createElement("button");
      item.type = "button";
      item.className = "md-button theme-palette-preset";
      item.textContent = p.name;
      item.dataset.primary = p.primary;
      item.dataset.accent = p.accent;
      presetList.appendChild(item);
    }

    const customLabel = document.createElement("div");
    customLabel.className = "theme-palette-section-title";
    customLabel.textContent = "自定义";

    const customRow = document.createElement("div");
    customRow.className = "theme-palette-custom";

    const primaryWrap = document.createElement("label");
    primaryWrap.className = "theme-palette-field";
    const primaryText = document.createElement("span");
    primaryText.textContent = "主色";
    const primaryInput = document.createElement("input");
    primaryInput.type = "color";
    primaryInput.value = "#6750A4";
    primaryWrap.appendChild(primaryText);
    primaryWrap.appendChild(primaryInput);

    const accentWrap = document.createElement("label");
    accentWrap.className = "theme-palette-field";
    const accentText = document.createElement("span");
    accentText.textContent = "强调色";
    const accentInput = document.createElement("input");
    accentInput.type = "color";
    accentInput.value = "#EFB8C8";
    accentWrap.appendChild(accentText);
    accentWrap.appendChild(accentInput);

    const applyBtn = document.createElement("button");
    applyBtn.type = "button";
    applyBtn.className = "md-button md-button--primary theme-palette-apply";
    applyBtn.textContent = "应用";

    const resetBtn = document.createElement("button");
    resetBtn.type = "button";
    resetBtn.className = "md-button theme-palette-reset";
    resetBtn.textContent = "恢复默认";

    customRow.appendChild(primaryWrap);
    customRow.appendChild(accentWrap);
    customRow.appendChild(applyBtn);
    customRow.appendChild(resetBtn);

    body.appendChild(presetLabel);
    body.appendChild(presetList);
    body.appendChild(customLabel);
    body.appendChild(customRow);

    panel.appendChild(header);
    panel.appendChild(body);

    overlay.appendChild(panel);

    function open() {
      const cfg = loadConfig();
      if (cfg?.primary && isHexColor(cfg.primary)) primaryInput.value = cfg.primary;
      if (cfg?.accent && isHexColor(cfg.accent)) accentInput.value = cfg.accent;
      overlay.hidden = false;
      overlay.style.removeProperty("display");
      document.documentElement.classList.add("theme-palette-open");
    }

    function closePanel() {
      overlay.hidden = true;
      overlay.style.display = "none";
      document.documentElement.classList.remove("theme-palette-open");
    }

    function applyAndSave(primary, accent) {
      applyColors(primary, accent);
      saveConfig({ primary, accent });
    }

    panel.addEventListener("click", (e) => {
      e.stopPropagation();
    });

    overlay.addEventListener("click", (e) => {
      if (e.target === overlay) closePanel();
    });

    close.addEventListener("click", (e) => {
      e.preventDefault();
      e.stopPropagation();
      closePanel();
    });
    document.addEventListener("keydown", (e) => {
      if (!overlay.hidden && e.key === "Escape") closePanel();
    });

    presetList.addEventListener("click", (e) => {
      const t = e.target;
      if (!(t instanceof HTMLElement)) return;
      if (!(t instanceof HTMLButtonElement)) return;
      const primary = t.dataset.primary;
      const accent = t.dataset.accent;
      if (!isHexColor(primary) || !isHexColor(accent)) return;
      applyAndSave(primary, accent);
    });

    applyBtn.addEventListener("click", () => {
      applyAndSave(primaryInput.value, accentInput.value);
    });

    resetBtn.addEventListener("click", () => {
      localStorage.removeItem(STORAGE_KEY);
      clearStyleTag();
      primaryInput.value = "#6750A4";
      accentInput.value = "#EFB8C8";
    });

    return { overlay, open, close: closePanel };
  }

  function mount() {
    applyFromStorage();

    if (document.querySelector(".theme-palette-overlay")) return;
    const { overlay, open } = createDialog();
    document.body.appendChild(overlay);

    overlay.hidden = true;
    overlay.style.display = "none";
    document.documentElement.classList.remove("theme-palette-open");

    ensureHeaderButton(() => open());
  }

  function onReady(fn) {
    if (document.readyState === "loading") {
      document.addEventListener("DOMContentLoaded", fn, { once: true });
    } else {
      fn();
    }
  }

  onReady(() => {
    const doc$ = window.document$;
    if (doc$ && typeof doc$.subscribe === "function") {
      doc$.subscribe(() => mount());
    } else {
      mount();
    }
  });
})();
