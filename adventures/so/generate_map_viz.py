#!/usr/bin/env python3
# generate_map_viz.py — 尋仙問道 地圖視覺化生成工具 (Mermaid & SVG)
#
# 用法：
#   python3 generate_map_viz.py novice_map.yaml
#
import os
import sys
import yaml
from pathlib import Path

# 顏色配置 (對應 FSE Node 類型)
TYPE_COLORS = {
    "cultivation":    "#d4af37", # 金色 — 修行
    "travel":         "#708090", # 灰石 — 山路/通道
    "sect_heritage":  "#4169e1", # 寶藍 — 門派傳承
    "crafting":       "#cd7f32", # 青銅 — 百工鍛造
    "alchemy":        "#228b22", # 森林綠 — 煉丹
    "logical_array":  "#9370db", # 紫羅蘭 — 陣法/邏輯
    "market":         "#ff4500", # 橙紅 — 集市/商業
    "social_info":    "#8b4513", # 鞍褐 — 酒館
    "nature_resonance":"#3cb371", # 嫩綠 — 靈獸
    "karmic_broker":  "#2f4f4f", # 暗灰 — 當舖/業力交易
    "spiritual_market":"#da70d6", # 蘭花紫 — 拍賣行
    "spiritual_trial": "#ff1493", # 深粉 — 青樓/心魔試煉
    "domain":         "#00ffff", # 青色 — 秘境
}

SO_ROOT = Path(__file__).parent
DOCS_DIR = SO_ROOT / "docs"

def build_mermaid(nodes):
    """建構 Markdown 內的 Mermaid 流程圖"""
    lines = ["```mermaid", "graph TD"]
    
    # 1. 定義節點與樣式
    for n in nodes:
        n_id = n["id"]
        n_name = n["name"]
        n_type = n.get("type", "cultivation")
        
        # 定義節點形狀 (依據類型)
        if n_type == "cultivation":
            shape = f'["{n_name} ({n_type})"]'
        elif n_type == "spiritual_trial":
            shape = f'(("{n_name} ({n_type})"))'
        elif n_type == "domain":
            shape = f'{{"{n_name} ({n_type})"}}'
        else:
            shape = f'["{n_name} ({n_type})"]'
            
        lines.append(f"    {n_id}{shape}")
        
    # 2. 定義連結路徑 (普通與 Reveal)
    for n in nodes:
        n_id = n["id"]
        paths = n.get("paths", {})
        for direction, dest in paths.items():
            lines.append(f"    {n_id} -->|{direction}| {dest}")
            
        reveal_paths = n.get("reveal_paths", {})
        for direction, rdata in reveal_paths.items():
            dest = rdata["dest"]
            req = rdata.get("requires_factor", "stillness_resonance")
            lines.append(f"    {n_id} -.->|{direction} (requires: {req})| {dest}")

    # 3. 綁定色彩樣式
    for i, n in enumerate(nodes):
        n_id = n["id"]
        n_type = n.get("type", "cultivation")
        color = TYPE_COLORS.get(n_type, "#cccccc")
        lines.append(f"    style {n_id} fill:{color},stroke:#333,stroke-width:2px,color:#fff")
        
    lines.append("```")
    return "\n".join(lines)

def build_svg(nodes):
    """建構一個簡易分層佈局的 SVG 地圖向量圖檔"""
    # 為了簡化不依賴 graphviz 的佈局計算，我們使用網格座標佈局
    # 座標對照表（手動佈局讓地圖呈現漂亮的樹狀/網狀）
    coords = {
        "meditation_cliff":  (400, 50),
        "mountain_path":     (400, 150),
        "sutra_pavilion":    (650, 150),
        "town_entrance":     (400, 250),
        "town_center":       (400, 380),
        "craft_workshop":    (400, 520),
        "herb_garden":       (620, 380),
        "array_platform":    (180, 380),
        "wind_tavern":       (400, 640),
        "pet_sanctuary":     (620, 640),
        "destiny_pawnshop":  (620, 500),
        "spirit_auction":    (180, 500),
        "desire_pavilion":   (180, 640),
        "hidden_shrine":     (800, 380) # 隱藏秘境
    }

    svg_header = """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000 800" width="100%" height="100%" style="background-color: #1a1a1a; font-family: sans-serif;">
    <!-- 定義箭頭與漸變 -->
    <defs>
        <marker id="arrow" viewBox="0 0 10 10" refX="25" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
            <path d="M 0 0 L 10 5 L 0 10 z" fill="#888" />
        </marker>
        <marker id="arrow-reveal" viewBox="0 0 10 10" refX="25" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
            <path d="M 0 0 L 10 5 L 0 10 z" fill="#da70d6" />
        </marker>
    </defs>
    
    <text x="500" y="30" font-size="22" fill="#fff" font-weight="bold" text-anchor="middle">尋仙問道 — 沙盒地圖 (Novice Map) 視覺化</text>
    <text x="500" y="55" font-size="12" fill="#888" text-anchor="middle">實線：普通路徑 | 虛線：Reveal 隱秘路徑</text>
"""
    svg_footer = "</svg>"
    
    elements = []
    
    # 1. 繪製路徑連線 (避免線壓在文字上，先畫線)
    for n in nodes:
        n_id = n["id"]
        if n_id not in coords: continue
        x1, y1 = coords[n_id]
        
        # 普通路徑
        paths = n.get("paths", {})
        for direction, dest in paths.items():
            if dest in coords:
                x2, y2 = coords[dest]
                elements.append(f'    <line x1="{x1}" y1="{y1}" x2="{x2}" y2="{y2}" stroke="#555" stroke-width="2" marker-end="url(#arrow)" />')
                # 繪製方向標籤
                lx, ly = (x1 + x2) / 2, (y1 + y2) / 2
                elements.append(f'    <text x="{lx}" y="{ly - 5}" font-size="9" fill="#aaa" text-anchor="middle">{direction}</text>')
                
        # Reveal 路徑
        reveal_paths = n.get("reveal_paths", {})
        for direction, rdata in reveal_paths.items():
            dest = rdata["dest"]
            if dest in coords:
                x2, y2 = coords[dest]
                elements.append(f'    <line x1="{x1}" y1="{y1}" x2="{x2}" y2="{y2}" stroke="#da70d6" stroke-width="2" stroke-dasharray="5,5" marker-end="url(#arrow-reveal)" />')
                lx, ly = (x1 + x2) / 2, (y1 + y2) / 2
                elements.append(f'    <text x="{lx}" y="{ly - 5}" font-size="9" fill="#da70d6" text-anchor="middle">{direction}</text>')

    # 2. 繪製節點卡片
    for n in nodes:
        n_id = n["id"]
        n_name = n["name"]
        n_type = n.get("type", "cultivation")
        if n_id not in coords: continue
        x, y = coords[n_id]
        
        color = TYPE_COLORS.get(n_type, "#cccccc")
        
        # 繪製卡片背景
        elements.append(f'    <rect x="{x-70}" y="{y-20}" width="140" height="40" rx="5" fill="{color}" stroke="#333" stroke-width="2" />')
        # 寫入文字
        elements.append(f'    <text x="{x}" y="{y-4}" font-size="12" fill="#fff" font-weight="bold" text-anchor="middle">{n_name}</text>')
        elements.append(f'    <text x="{x}" y="{y+12}" font-size="9" fill="#ddd" text-anchor="middle">{n_type}</text>')

    # 3. 繪製 Legend 說明
    legend_start_y = 700
    elements.append(f'    <rect x="50" y="{legend_start_y-20}" width="900" height="80" rx="8" fill="#222" stroke="#444" />')
    elements.append(f'    <text x="70" y="{legend_start_y}" font-size="12" fill="#fff" font-weight="bold">節點類型說明：</text>')
    
    lx, ly = 70, legend_start_y + 20
    for idx, (t, col) in enumerate(TYPE_COLORS.items()):
        elements.append(f'    <rect x="{lx}" y="{ly}" width="12" height="12" rx="2" fill="{col}" />')
        elements.append(f'    <text x="{lx+18}" y="{ly+10}" font-size="9" fill="#ccc">{t}</text>')
        lx += 120
        if (idx + 1) % 7 == 0:
            lx = 70
            ly += 20

    return svg_header + "\n".join(elements) + "\n" + svg_footer

def main():
    if len(sys.argv) < 2:
        print("❌ 錯誤: 請指定地圖檔名")
        print("💡 用法: python3 generate_map_viz.py novice_map.yaml")
        sys.exit(1)
        
    map_file = sys.argv[1]
    if not Path(map_file).exists():
        print(f"❌ 錯誤: 找不到地圖檔 {map_file}")
        sys.exit(1)
        
    with open(map_file, "r", encoding="utf-8") as f:
        map_data = yaml.safe_load(f)
        
    nodes = map_data.get("nodes", [])
    
    # 取得輸入檔案的名稱主體 (e.g. novice_map)
    map_stem = Path(map_file).stem
    
    # 1. 輸出 Mermaid Markdown
    mermaid_content = f"# Shushan Map Visualization (地圖可視化 - {map_stem})\n\n此文件是由 `generate_map_viz.py` 讀取 `{map_file}` 後自動生成。\n\n## 門派與紅塵地圖結構\n\n"
    mermaid_content += build_mermaid(nodes)
    
    os.makedirs(DOCS_DIR, exist_ok=True)
    md_path = DOCS_DIR / f"{map_stem}.md"
    with open(md_path, "w", encoding="utf-8") as f:
        f.write(mermaid_content)
    print(f"✓ 成功生成 Markdown (Mermaid) 地圖: {md_path}")
    
    # 2. 輸出 SVG 圖檔
    svg_content = build_svg(nodes)
    svg_path = SO_ROOT / f"{map_stem}.svg"
    with open(svg_path, "w", encoding="utf-8") as f:
        f.write(svg_content)
    print(f"✓ 成功生成 SVG 向量地圖: {svg_path}")

if __name__ == "__main__":
    main()
