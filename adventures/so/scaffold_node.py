#!/usr/bin/env python3
# scaffold_node.py — 蜀山問道 FSE Node 快速搭建與地圖批量生成工具
#
# 用法 1：批量讀取地圖檔生成（推薦）
#   python3 scaffold_node.py --import_map world_map.yaml
#
# 用法 2：單一節點生成
#   python3 scaffold_node.py green_bamboo_forest --name "清淨竹林" --desc "竹影婆娑..."
#
import os
import sys
import yaml
import argparse
from pathlib import Path

# ─── 路徑設定 ─────────────────────────────────────────────
SO_ROOT      = Path(__file__).parent
MUDLIB_ROOT  = SO_ROOT / "mudlib"
CONTENT_ROOT = MUDLIB_ROOT / "content"

def create_single_node(node_id, node_name, node_desc, node_type="cultivation", paths=None, reveal_paths=None, node_dict=None):
    """生成單一節點的 YAML、挑戰 YAML 與 語系註冊"""
    if paths is None:
        paths = {}
    if reveal_paths is None:
        reveal_paths = {}
    if node_dict is None:
        node_dict = {}

    node_dir = CONTENT_ROOT / "nodes" / node_id
    challenges_dir = node_dir / "challenges"
    discoveries_dir = node_dir / "discoveries"

    os.makedirs(node_dir, exist_ok=True)
    os.makedirs(challenges_dir, exist_ok=True)
    os.makedirs(discoveries_dir, exist_ok=True)

    # 1. 轉譯 paths 格式符合 MUD 虛擬路徑規則 (e.g. /nodes/mountain_path/node)
    paths_config = {}
    for direction, dest_id in paths.items():
        paths_config[direction] = f"/nodes/{dest_id}/node"

    # 轉譯 reveal_paths 格式 (使用 FSE 術語 reveal_paths)
    reveal_config = {}
    for direction, data in reveal_paths.items():
        reveal_config[direction] = {
            "dest": f"/nodes/{data['dest']}/node",
            "requires_factor": data.get("requires_factor", "stillness_resonance"),
            "reveal_msg": data.get("reveal_msg", "【 🔍 發現 】一條隱秘的路徑在你的理解中顯現了。")
        }

    # 2. 建立 node.yaml 并處理宣告式直通欄位 (使用 paths 取代 exits，使用 desc 取代 description)
    node_yaml_path = node_dir / "node.yaml"
    node_data = {
        "version": "1.0",
        "node_id": node_id,
        "name": node_id,
        "type": node_type,
        "desc": f"{node_id}_desc",
        "paths": paths_config
    }
    if reveal_config:
        node_data["reveal_paths"] = reveal_config

    # 自動直通拷貝 (複製進階欄位直接注入 node.yaml)
    passthrough_keys = [
        "sensory_signals", 
        "interactions", 
        "presence", 
        "environmental_multipliers", 
        "difficulty_override",
        "karma_loop_threshold"
    ]
    for key in passthrough_keys:
        if key in node_dict:
            node_data[key] = node_dict[key]

    node_data["challenges"] = [{"id": f"{node_id}_first_contact"}]

    with open(node_yaml_path, "w", encoding="utf-8") as f:
        yaml.safe_dump(node_data, f, allow_unicode=True, default_flow_style=False)
    print(f"  ✓ 成功創建 Node 設定檔: {node_yaml_path}")

    # 3. 建立第一個挑戰 YAML 模板 (對齊 evolve 和 adventure_effects 結構)
    challenge_yaml_path = challenges_dir / f"{node_id}_first_contact.yaml"
    challenge_data = {
        "challenge_id": f"{node_id}_first_contact",
        "executor": "reality_resolver",
        "knowledges": [
            f"spiritual.karma.sutra_focus"
        ],
        "discover_factor": "stillness_resonance",
        "success_msg": "🎉 你的心神與天地靈氣交融，進入定境。\n",
        "success_progress": 10,
        "failure_warning": "【 🌀 產生困惑 】你感到氣血逆流，無法安神，周圍的法則產生了斷裂。\n",
        "evolve": {
            "understanding": {
                "world_change": "actor_aligned_with_stillness",
                "new_signals": [f"{node_id}_aligned"],
                "adventure_effects": {
                    "spiritual_energy": 20,
                    "karma_change": 0
                }
            },
            "misunderstanding": {
                "world_change": "actor_tension_persists",
                "new_signals": ["chest_tightness"],
                "adventure_effects": {
                    "spiritual_energy": -5,
                    "karma_change": 1
                }
            },
            "misconception": {
                "world_change": "actor_qi_backlash",
                "new_signals": ["mind_chaos"],
                "adventure_effects": {
                    "spiritual_energy": -10,
                    "karma_change": 5
                }
            }
        }
    }

    with open(challenge_yaml_path, "w", encoding="utf-8") as f:
        yaml.safe_dump(challenge_data, f, allow_unicode=True, default_flow_style=False)
    print(f"  ✓ 成功創建 挑戰檔: {challenge_yaml_path}")

    # 3.5 建立第一個探索發現 (Discovery) 模板檔
    discovery_yaml_path = discoveries_dir / f"{node_id}_secret_niche.yaml"
    discovery_data = {
        "discovery_id": f"{node_id}_secret_niche",
        "name": "石縫隱秘",
        "description": "角落的石縫裡，似乎有些前人遺留的痕跡。",
        "requires_observation": "stillness_resonance", # 需要悟得特定因果才能看見此發現
        "on_reveal": {
            "msg": "【 🔍 發現 】你在角落的石縫中找到了一卷古老的手札殘頁！",
            "adventure_effects": {
                "spiritual_energy": 5
            },
            "give_item": {
                "id": "old_page",
                "name": "古老手札殘頁",
                "desc": "一頁泛黃發霉的殘破紙張，上面依稀寫著些無關緊要的修行筆記。"
            }
        }
    }
    with open(discovery_yaml_path, "w", encoding="utf-8") as f:
        yaml.safe_dump(discovery_data, f, allow_unicode=True, default_flow_style=False)
    print(f"  ✓ 成功創建 發現物模板: {discovery_yaml_path}")

    # 4. 註冊多語系 locales
    locales_path = CONTENT_ROOT / "locales" / "zh_TW.yaml"
    locales_data = {}
    if locales_path.exists():
        with open(locales_path, "r", encoding="utf-8") as f:
            locales_data = yaml.safe_load(f) or {}

    if "zh_TW" not in locales_data:
        locales_data["zh_TW"] = {}

    locales_data["zh_TW"][node_id] = node_name
    locales_data["zh_TW"][f"{node_id}_desc"] = node_desc

    with open(locales_path, "w", encoding="utf-8") as f:
        yaml.safe_dump(locales_data, f, allow_unicode=True, default_flow_style=False)
    print(f"  ✓ 成功於 locales 中註冊: {node_name}")


def main():
    parser = argparse.ArgumentParser(description="尋仙問道 Node/Challenge Scaffolder & World Map Scaffolder")
    parser.add_argument("node_id", type=str, nargs="?", help="Node 唯一標識符 (批量生成時免填)")
    parser.add_argument("--name", type=str, help="Node 的中文名稱 (單獨生成時必填)")
    parser.add_argument("--desc", type=str, help="Node 的中文場景詳細描述 (單獨生成時必填)")
    parser.add_argument("--type", type=str, default="cultivation", help="Node 類型 (預設: cultivation)")
    parser.add_argument("--import_map", type=str, metavar="world_map.yaml", help="讀取指定的地圖定義檔進行批量生成與路徑連結")
    args = parser.parse_args()

    # 處理批量導入
    if args.import_map:
        map_path = SO_ROOT / args.import_map
        if not map_path.exists():
            print(f"❌ 錯誤: 找不到地圖定義檔 {map_path}")
            sys.exit(1)

        print(f"📂 正在讀取地圖檔並批量生成: {map_path}...")
        with open(map_path, "r", encoding="utf-8") as f:
            map_data = yaml.safe_load(f)

        if not map_data or "nodes" not in map_data:
            print("❌ 錯誤: 地圖格式無效，必須包含 'nodes' 列表。")
            sys.exit(1)

        for n in map_data["nodes"]:
            n_id = n["id"]
            n_name = n["name"]
            n_desc = n["desc"]
            n_type = n.get("type", "cultivation")
            n_paths = n.get("paths", {})
            n_reveal_paths = n.get("reveal_paths", {})

            print(f"\n⚡ 正在生成節點: {n_id} ({n_name})...")
            create_single_node(n_id, n_name, n_desc, n_type, n_paths, n_reveal_paths, n)

        print("\n🏆 地圖批量生成並自動連結成功！")
        sys.exit(0)

    # 處理單獨生成
    if not args.node_id or not args.name or not args.desc:
        print("❌ 錯誤: 單獨生成需要指定 node_id, --name 與 --desc。")
        print("💡 提示: 或者使用 --import_map world_map.yaml 來進行批量生成。")
        sys.exit(1)

    print(f"\n🔮 開始單獨搭建 Node: {args.node_id} ({args.name})...")
    create_single_node(args.node_id, args.name, args.desc, args.type)
    print(f"🏆 {args.node_id} 節點單獨搭建成功！")

if __name__ == "__main__":
    main()
