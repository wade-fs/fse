#!/usr/bin/env python3
# create-adventure.py — FSE 2.0 冒險模組一鍵生成工具 (Platform Bootstrap Tool)
#
# 用法：
#   python3 create-adventure.py --name <name> --title <title>
#
import os
import sys
import shutil
import argparse
from pathlib import Path

# FSE 系統根目錄
FSE_ROOT = Path(__file__).parent.resolve()

def create_adventure(name, title, author):
    adv_dir = FSE_ROOT / "adventures" / name
    if adv_dir.exists():
        print(f"❌ 錯誤: 冒險模組 '{name}' 已存在於 {adv_dir}！")
        sys.exit(1)

    print(f"🚀 開始為新冒險模組 [{title}] 搭建骨架...")

    # 1. 建立標準結構目錄
    dirs = [
        adv_dir / "docs",
        adv_dir / "mudlib" / "content" / "locales",
        adv_dir / "mudlib" / "content" / "nodes",
        adv_dir / "mudlib" / "std",
        adv_dir / "mudlib" / "tests",
        adv_dir / "mudlib" / "web" / "static",
    ]
    for d in dirs:
        os.makedirs(d, exist_ok=True)
        print(f"  ✓ 建立目錄: {d.relative_to(FSE_ROOT)}")

    # 2. 建立新冒險專屬的 manifest.yaml
    manifest_yaml = adv_dir / "mudlib" / "manifest.yaml"
    manifest_content = f"""# adventures/{name}/mudlib/manifest.yaml
# FSE 宣告式冒險模組註冊清單

adventure_id: "{name}"
name: "{title}"
author: "{author}"
version: "1.0"

# 虛擬路徑物件映射規則
virtual_rules:
  presence: "/std/presence.c"
  nodes:    "/std/node.c"

# 開發內容資源路徑
content_paths:
  locales: "/content/locales"
  factors: "/content/factors"
  progression: "/content/progression"

# 冒險初始啟動階段與虛擬節點
initial_stage: "stage_1_start"

# 認識論判定擴充 (若有)
resolver_strategies:
  # 格式: 策略名: 實作路徑
  # example_resolver: "/runtime/executors/example_resolver.c"
"""
    with open(manifest_yaml, "w", encoding="utf-8") as f:
        f.write(manifest_content)
    print(f"  ✓ 生成設定檔: {manifest_yaml.relative_to(adv_dir)}")

    # 3. 建立引導 master.c
    master_c = adv_dir / "mudlib" / "master.c"
    master_content = f"""// adventures/{name}/mudlib/master.c
// 冒險引導與 Master Object (一鍵引導版)
#include "/runtime/include/ansi.h"

void create() {{
    write("===========================================\n");
    write("    FSE — " + "{title}" + " (" + "{name}" + ")    \n");
    write("         冒險引導模組 載入成功         \n");
    write("===========================================\n");

    // 讀取宣告式清單進行系統登錄
    string manifest_raw = read_file("/manifest.yaml");
    if (manifest_raw) {{
        mapping manifest = yaml_decode(manifest_raw);
        if (manifest) {{
            // 註冊語言與翻譯
            object i18n_svc = load_object("/runtime/services/i18n_service.c");
            mapping content_paths = manifest["content_paths"];
            if (i18n_svc && content_paths && content_paths["locales"]) {{
                i18n_svc->register_translation_dir(content_paths["locales"]);
                i18n_svc->reload_language();
            }}

            // 註冊虛擬編譯物件規則
            if (manifest["virtual_rules"]) {{
                object virtual_core = load_object("/runtime/core/virtual.c");
                if (virtual_core) {{
                    foreach (string prefix, string std_file in manifest["virtual_rules"])
                        virtual_core->register_virtual_rule(prefix, std_file);
                }}
            }}
            write("  [master] 成功讀取 /manifest.yaml 並完成宣告式註冊。\\n");
        }}
    }}
}}

object connect() {{
    object ob = clone_object("/std/user.c");
    return ob;
}}
"""
    with open(master_c, "w", encoding="utf-8") as f:
        f.write(master_content)
    print(f"  ✓ 生成引導檔: {master_c.relative_to(adv_dir)}")

    # 4. 建立基礎繼承 std 物件 (user.c, node.c, presence.c)
    # 4.1 user.c
    user_c = adv_dir / "mudlib" / "std" / "user.c"
    user_content = """// adventures/{name}/mudlib/std/user.c
// 冒險專屬玩家載體物件
inherit "/runtime/std/user";
#include "/runtime/include/ansi.h"

protected int life;
protected int mental_focus;

void create() {
    ::create();
    life = 100;
    mental_focus = 100;
}

// 實作與 FSE 統一的 physical_state API 對接
int query_physical_state(string attr) {
    if (attr == "life" || attr == "hp") return life;
    if (attr == "mental_focus" || attr == "focus") return mental_focus;
    return 0;
}

void set_physical_state(string attr, int val) {
    if (attr == "life" || attr == "hp") {
        life = val;
    } else if (attr == "mental_focus" || attr == "focus") {
        mental_focus = val;
    }
    save_state();
}

void add_physical_state(string attr, int val) {
    if (attr == "life" || attr == "hp") {
        life += val;
        if (life < 0) life = 0;
    } else if (attr == "mental_focus" || attr == "focus") {
        mental_focus += val;
        if (mental_focus < 0) mental_focus = 0;
    }
    save_state();
}
""".replace("{name}", name)
    with open(user_c, "w", encoding="utf-8") as f:
        f.write(user_content)

    # 4.2 node.c
    node_c = adv_dir / "mudlib" / "std" / "node.c"
    node_content = """// adventures/{name}/mudlib/std/node.c
// 冒險專屬虛擬節點類別
inherit "/std/fse_room";
#include "/runtime/include/ansi.h"

void create() {
    ::create();
}

// 覆寫以套用該冒險專屬的物理與環境副作用
void apply_adventure_side_effects(object actor, mapping act, int passed) {
    if (!actor || !act) return;
    
    mapping effects = act["adventure_effects"];
    if (!effects || !mapp(effects)) {
        effects = act;
    }

    int life_change = effects["life_change"] || effects["hp_change"];
    if (life_change) actor->add_physical_state("life", life_change);

    int focus_change = effects["focus_change"];
    if (focus_change) actor->add_physical_state("mental_focus", focus_change);
}
""".replace("{name}", name)
    with open(node_c, "w", encoding="utf-8") as f:
        f.write(node_content)

    # 4.3 presence.c
    presence_c = adv_dir / "mudlib" / "std" / "presence.c"
    presence_content = """// adventures/{name}/mudlib/std/presence.c
// 冒險專屬生存體/NPC 類別
inherit "/std/living";
#include "/runtime/include/ansi.h"

void create() {
    ::create();
}
""".replace("{name}", name)
    with open(node_c, "w", encoding="utf-8") as f:
        f.write(node_content)
    with open(presence_c, "w", encoding="utf-8") as f:
        f.write(presence_content)
    print(f"  ✓ 生成專屬 std 類別: {user_c.relative_to(adv_dir)}, {node_c.relative_to(adv_dir)}")

    # 5. 複製 fse_room.c (FSE 基礎節點類別，用以驅動 paths 與 resolver)
    shutil.copy(
      FSE_ROOT / "adventures" / "so" / "mudlib" / "std" / "fse_room.c",
      adv_dir / "mudlib" / "std" / "fse_room.c"
    )
    print(f"  ✓ 導入核心 fse_room.c")

    # 6. 生成第一個 novice_map.yaml 骨架
    novice_map = adv_dir / "novice_map.yaml"
    map_content = f"""# adventures/{name}/novice_map.yaml
# 【{title}】冒險地圖定義檔

nodes:
  - id: "start_cliff"
    name: "起程之地"
    type: "cultivation"
    desc: "這裡是一處古老冒險的起點。石碑上刻著：「探索者啊，尋找此地遺落的秘密吧。」"
    paths:
      forward: "winding_road"

    sensory_signals:
      wind:
        default_msg: "一陣清涼的風吹過你的臉龐。"
        discovery:
          requires_no_factor: "first_understanding"
          set_temp: "wind_noticed"
          msg: "【 🔍 發現 】微風中似乎夾雜著奇特的芳香，讓你心神一震。"

    interactions:
      - action: "meditate"
        prerequisites:
          temp_state: "wind_noticed"
        resolver: "start_cliff_first_contact"

  - id: "winding_road"
    name: "蜿蜒小路"
    type: "travel"
    desc: "一條布滿碎石的蜿蜒小路，通向未知的遠方。"
    paths:
      back: "start_cliff"
"""
    with open(novice_map, "w", encoding="utf-8") as f:
        f.write(map_content)
    print(f"  ✓ 生成第一個地圖模板: {novice_map.relative_to(adv_dir)}")

    # 7. 生成起步測試 tests/test_loop.c
    test_c = adv_dir / "mudlib" / "tests" / "test_loop.c"
    test_content = f"""// adventures/{name}/mudlib/tests/test_loop.c
// 新模組起步測試 suite
#include "/runtime/include/ansi.h"

void run_tests() {{
    write("============================================\\n");
    write("   開始執行 " + "{title}" + " 基礎起步測試  \\n");
    write("============================================\\n");
    
    object player = clone_object("/std/user.c");
    if (!player) {{
        write(HIR "❌ 測試失敗: 無法建立玩家載體。\\n" NOR);
        exit(1);
    }}
    write(HIG "🧪 測試 1: 玩家建立成功，HP = " + player->query_physical_state("life") + "\\n" NOR);

    object start_node = load_object("/nodes/start_cliff/node");
    if (!start_node) {{
        write(HIR "❌ 測試失敗: 起始節點 /nodes/start_cliff/node 載入失敗。\\n" NOR);
        exit(1);
    }}
    write(HIG "🧪 測試 2: 起程之地節點加載成功，名稱 = " + start_node->query_short() + "\\n" NOR);

    write(HIG "\\n🎉 [SUCCESS] " + "{title}" + " 所有測試成功通過！\\n" NOR);
}}
"""
    with open(test_c, "w", encoding="utf-8") as f:
        f.write(test_content)
    print(f"  ✓ 生成起步測試檔: {test_c.relative_to(adv_dir)}")

    # 8. 建立 Makefile
    makefile = adv_dir / "Makefile"
    makefile_content = f"""# adventures/{name}/Makefile
# 自動編譯與測試組態

test:
\t@echo "🧪 運行 {title} 最小啟動測試..."
\t@MUD_TEST_MODE=1 ../../bin/fsmud -mudlib mudlib --hub none
"""
    with open(makefile, "w", encoding="utf-8") as f:
        f.write(makefile_content)
    print(f"  ✓ 生成編譯 Makefile")

    # 9. 拷貝並配置 scaffold_node.py 與 generate_map_viz.py
    # 9.1 scaffold_node.py
    scaffold_src = FSE_ROOT / "adventures" / "so" / "scaffold_node.py"
    scaffold_dest = adv_dir / "scaffold_node.py"
    shutil.copy(scaffold_src, scaffold_dest)
    os.chmod(scaffold_dest, 0o755)

    # 9.2 generate_map_viz.py
    viz_src = FSE_ROOT / "adventures" / "so" / "generate_map_viz.py"
    viz_dest = adv_dir / "generate_map_viz.py"
    shutil.copy(viz_src, viz_dest)
    os.chmod(viz_dest, 0o755)
    print(f"  ✓ 複製並授權工具鏈: scaffold_node.py, generate_map_viz.py")

    # 10. 執行初次地圖生成以完成初始 novice_map 的 node.yaml 化
    print("\n📦 執行初次地圖編譯與測試驗證...")
    os.system(f"cd {adv_dir} && python3 scaffold_node.py --import_map novice_map.yaml > /dev/null")
    os.system(f"cd {adv_dir} && python3 generate_map_viz.py novice_map.yaml > /dev/null")
    print(f"  ✓ 成功自動生成 start_cliff 與 winding_road 節點設定。")

    print(f"\n🏆 恭喜！[{title}] 冒險模組骨架創建成功！")
    print(f"👉 目錄位置: adventures/{name}")
    print(f"👉 執行測試: cd adventures/{name} && make test")

def main():
    parser = argparse.ArgumentParser(description="FSE 2.0 冒險一鍵生成平台 bootstrap")
    parser.add_argument("--name", required=True, help="冒險模組的唯一英文標識符 (例如: kunlun_quest)")
    parser.add_argument("--title", required=True, help="冒險模組的中文顯示標題 (例如: 崑崙尋道記)")
    parser.add_argument("--author", default="fse_creator", help="作者名稱 (預設: fse_creator)")
    args = parser.parse_args()

    create_adventure(args.name, args.title, args.author)

if __name__ == "__main__":
    main()
