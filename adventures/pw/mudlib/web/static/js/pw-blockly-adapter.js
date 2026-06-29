/**
 * pw-blockly-adapter.js
 * PW Adventure 前端 Blockly 通訊適配器
 *
 * 職責：
 *   - 管理 WebSocket 連線生命週期
 *   - 依照後端 JSON 協定，發送 EXECUTE / REQUEST_TOOLBOX 請求
 *   - 接收並分派 TOOLBOX_UPDATE / EXECUTION_RESULT / WORLD_STATE / I18N_UPDATE
 *   - 動態更新 Blockly Toolbox（後端主導）
 *
 * 使用方式：
 *   const adapter = new PWBlocklyAdapter(workspace, { onLog, onMemoryUpdate, onWorldState });
 *   adapter.connect("ws://host/ws");
 */

class PWBlocklyAdapter {
    /**
     * @param {Blockly.WorkspaceSvg} workspace - 已初始化的 Blockly 工作區
     * @param {Object} options
     * @param {function(string, string)} options.onLog - 終端機日誌回調 (text, cssClass)
     * @param {function(number)} options.onMemoryUpdate - 記憶體更新回調 (level 0~100)
     * @param {function(Object)} options.onWorldState - 世界狀態更新回調 (payload)
     * @param {function()} options.onUnlock - 積木解鎖時的特效回調
     * @param {function(boolean)} options.onExecutionResult - 執行結果回調 (success)
     */
    constructor(workspace, options = {}) {
        this.workspace = workspace;
        this.opts = options;

        this._socket = null;
        this._connected = false;
        this._reqCounter = 0;

        // 已知的積木類別 hash，用於偵測 toolbox 是否真的有變化
        this._lastToolboxHash = "";
    }

    // ─────────────────────────────────────────
    // 連線管理
    // ─────────────────────────────────────────

    connect(wsUrl) {
        this._log("🌐 正在連接 FSE 核心虛擬機...", "blue-txt");
        this._socket = new WebSocket(wsUrl);

        this._socket.onopen = () => {
            this._connected = true;
            this._log("✅ 已成功連接至 FSE 核心虛擬機。", "green-txt");
            // 連線後立即請求工具箱
            this.requestToolbox();
        };

        this._socket.onmessage = (event) => {
            this._handleRawMessage(event.data);
        };

        this._socket.onclose = () => {
            this._connected = false;
            this._log("⚠️ 連線已中斷。請重新整理頁面。", "yellow-txt");
        };

        this._socket.onerror = (e) => {
            this._log("❌ WebSocket 連線錯誤。", "red-txt");
            console.error("[PWBlocklyAdapter] WebSocket error:", e);
        };
    }

    disconnect() {
        if (this._socket) {
            this._socket.close();
            this._socket = null;
        }
    }

    // ─────────────────────────────────────────
    // 前端 → 後端：發送訊息
    // ─────────────────────────────────────────

    /**
     * 執行積木：將 Blockly 工作區頂層積木轉換為 AST 後送出
     * @param {Object} ast - 已轉換好的積木 AST
     */
    execute(ast) {
        if (!this._connected) {
            this._log("⚠️ 尚未連線，無法執行。", "yellow-txt");
            return;
        }
        const reqId = this._nextReqId();
        this._send({
            type: "EXECUTE",
            payload: { ast },
            request_id: reqId,
        });
        this._log("▶ 正在傳送積木邏輯至虛擬機執行...", "blue-txt");
    }

    /**
     * 向後端請求當前 Toolbox
     */
    requestToolbox() {
        if (!this._connected) return;
        this._send({
            type: "REQUEST_TOOLBOX",
            request_id: this._nextReqId(),
        });
    }

    // ─────────────────────────────────────────
    // 後端 → 前端：接收訊息分派
    // ─────────────────────────────────────────

    _handleRawMessage(rawData) {
        let outer;
        try {
            outer = JSON.parse(rawData);
        } catch (e) {
            console.warn("[PWBlocklyAdapter] Non-JSON message:", rawData);
            return;
        }

        // 向下相容：舊版 mud_text / output / welcome 格式
        if (outer.type === "mud_text" || outer.type === "output" || outer.type === "welcome") {
            const payload = outer.payload || "";
            this._handleLegacyPayload(payload);
            return;
        }

        // 標準 JSON 協定
        this._dispatch(outer);
    }

    _dispatch(msg) {
        switch (msg.type) {
            case "TOOLBOX_UPDATE":
                this._handleToolboxUpdate(msg.payload);
                break;
            case "EXECUTION_RESULT":
                this._handleExecutionResult(msg.payload);
                break;
            case "WORLD_STATE":
                this._handleWorldState(msg.payload);
                break;
            case "I18N_UPDATE":
                this._handleI18nUpdate(msg.payload);
                break;
            default:
                console.log("[PWBlocklyAdapter] Unknown message type:", msg.type, msg);
        }
    }

    // TOOLBOX_UPDATE：動態更新 Blockly 工具箱（後端主導）
    _handleToolboxUpdate(payload) {
        if (!payload || !payload.toolbox) return;
        const toolboxCfg = payload.toolbox;
        const categories = toolboxCfg.categories || [];

        // 偵測是否真的有變化（避免不必要的 DOM 重建）
        const hash = JSON.stringify(categories);
        if (hash === this._lastToolboxHash) return;

        const prevHash = this._lastToolboxHash;
        this._lastToolboxHash = hash;

        // 組裝 Blockly XML toolbox 字串
        const xmlStr = this._buildToolboxXml(categories);

        try {
            this.workspace.updateToolbox(xmlStr);
        } catch (e) {
            console.error("[PWBlocklyAdapter] updateToolbox failed:", e);
            return;
        }

        // 如果不是初次載入（prevHash 有值），表示解鎖了新積木
        if (prevHash !== "") {
            this._log("🛠️ [工具箱更新] 後端偵測到新積木已解鎖！", "green-txt");
            if (this.opts.onUnlock) this.opts.onUnlock();
        }

        // 顯示已解鎖的因素
        const factors = toolboxCfg.unlocked_factors || [];
        if (factors.length > 0) {
            this._log("📌 已解鎖概念因素：" + factors.join("、"), "yellow-txt");
        }
    }

    // EXECUTION_RESULT：執行結果回饋
    _handleExecutionResult(payload) {
        if (!payload) return;
        const success = payload.success;
        const message = payload.message || "";
        const memoryCost = payload.memory_cost || 0;
        const newFactors = payload.new_factors || [];

        if (message) {
            this._log(message, success ? "green-txt" : "red-txt");
        }

        // 如果有新因素解鎖，顯示提示
        newFactors.forEach(f => {
            this._log("✨ [成就解鎖] 你領悟了新概念：" + f + "！", "yellow-txt");
        });

        // toolbox_delta：若有立即更新的 toolbox，套用它
        if (payload.toolbox_delta && payload.toolbox_delta.toolbox) {
            this._handleToolboxUpdate(payload.toolbox_delta);
        }

        if (this.opts.onExecutionResult) {
            this.opts.onExecutionResult(!!success);
        }
    }

    // WORLD_STATE：世界狀態同步
    _handleWorldState(payload) {
        if (!payload) return;

        if (typeof payload.memory !== "undefined") {
            if (this.opts.onMemoryUpdate) {
                this.opts.onMemoryUpdate(payload.memory);
            }
        }

        if (this.opts.onWorldState) {
            this.opts.onWorldState(payload);
        }
    }

    // I18N_UPDATE：翻譯文字更新（可擴充）
    _handleI18nUpdate(payload) {
        if (!payload) return;
        console.log("[PWBlocklyAdapter] i18n update:", payload.key, "->", payload.text);
    }

    // 向下相容舊版 mud_text 格式處理
    _handleLegacyPayload(payload) {
        if (!payload) return;

        // 過濾 ANSI 控制碼
        let text = payload.replace(/\u001b\[\d+m/g, "").replace(/\$\w+\$/g, "");
        if (!text.trim()) return;

        if (text.includes("🎉") || text.includes("成功")) {
            this._log(text, "green-txt");
            if (this.opts.onExecutionResult) this.opts.onExecutionResult(true);
        } else if (text.includes("💥") || text.includes("無窮迴圈")) {
            this._log(text, "red-txt");
            if (this.opts.onExecutionResult) this.opts.onExecutionResult(false);
        } else {
            this._log(text, "blue-txt");
        }
    }

    // ─────────────────────────────────────────
    // Blockly Toolbox XML 建構
    // ─────────────────────────────────────────

    /**
     * 將後端 categories 陣列轉為 Blockly XML toolbox 字串
     * @param {Array} categories - [{ name, colour, blocks: [{ type }] }]
     * @returns {string} XML string
     */
    _buildToolboxXml(categories) {
        let xml = '<xml>';
        for (const cat of categories) {
            const colour = cat.colour || "#888888";
            const name = this._escapeXml(cat.name || "");
            xml += `<category name="${name}" colour="${colour}">`;
            for (const block of (cat.blocks || [])) {
                xml += `<block type="${this._escapeXml(block.type)}"></block>`;
            }
            xml += '</category>';
        }
        xml += '</xml>';
        return xml;
    }

    // ─────────────────────────────────────────
    // 工具函式
    // ─────────────────────────────────────────

    _send(obj) {
        if (!this._socket || this._socket.readyState !== WebSocket.OPEN) {
            console.warn("[PWBlocklyAdapter] Cannot send, socket not open.");
            return;
        }
        this._socket.send(JSON.stringify(obj));
    }

    _log(text, cssClass) {
        if (this.opts.onLog) {
            this.opts.onLog(text, cssClass || "");
        }
    }

    _nextReqId() {
        return "req-" + (++this._reqCounter);
    }

    _escapeXml(str) {
        return String(str)
            .replace(/&/g, "&amp;")
            .replace(/</g, "&lt;")
            .replace(/>/g, "&gt;")
            .replace(/"/g, "&quot;");
    }
}

// 匯出給全域（無模組系統環境）
if (typeof window !== "undefined") {
    window.PWBlocklyAdapter = PWBlocklyAdapter;
}
