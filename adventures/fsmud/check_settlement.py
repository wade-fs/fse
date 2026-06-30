#!/usr/bin/env python3
# check_settlement.py — P23.1 聚落驗收標準稽核工具
#
# 用法：
#   python3 check_settlement.py [settlement_id]          # 自動偵測 tier
#   python3 check_settlement.py [settlement_id] --tier B # 強制指定 tier
#   python3 check_settlement.py --all                    # 稽核所有聚落
#
# Tier 分級說明（詳見 docs/settlement-standards.md）：
#   S = 直轄大城 (tier 1)
#   A = 重要市鎮 (tier 2–3)
#   B = 特色鄉村 (tier 4 / town)
#   C = 路過據點 (tier village / C)
#
# 回傳值：0 = 全部通過，1 = 有項目未達標（供 CI 使用）

import os
import sys
import glob
import yaml
import argparse
from pathlib import Path
from dataclasses import dataclass, field
from typing import Optional

# ─── 路徑設定 ─────────────────────────────────────────────
REPO_ROOT   = Path(__file__).parent
YAML_ROOT   = REPO_ROOT / "mudlib" / "content"

SETTLEMENTS_DIR  = YAML_ROOT / "settlements"
SITES_DIR        = YAML_ROOT / "sites"
MEMORIES_DIR     = YAML_ROOT / "memories"
NPCS_DIR         = YAML_ROOT / "npcs"
INCIDENTS_DIR    = YAML_ROOT / "incidents"
WORLD_EVENTS_DIR = YAML_ROOT / "world_events"
ROUTES_DIR       = YAML_ROOT / "routes"

# ─── 分級驗收門檻（詳見 docs/settlement-standards.md）────
# key 順序與報告欄位對應，era_layers_min 為品質指標最低 era 數
TIER_THRESHOLDS = {
    "S": {   # 直轄大城 (tier=1)
        "sites": 50, "npcs": 60, "incidents": 12,
        "memories": 40, "quests": 30, "rumors": 20,
        "dynamic_events": 8, "hidden_areas": 6, "era_layers_min": 3,
        "era_pct_target": 60,   # Site 達 era≥3 的目標比例%
    },
    "A": {   # 重要市鎮 (tier=2–3)
        "sites": 25, "npcs": 25, "incidents": 6,
        "memories": 20, "quests": 12, "rumors": 10,
        "dynamic_events": 4, "hidden_areas": 3, "era_layers_min": 3,
        "era_pct_target": 40,
    },
    "B": {   # 特色鄉村 (tier=4/town)
        "sites": 12, "npcs": 12, "incidents": 3,
        "memories": 8, "quests": 5, "rumors": 4,
        "dynamic_events": 2, "hidden_areas": 1, "era_layers_min": 3,
        "era_pct_target": 20,
    },
    "C": {   # 路過據點 (tier=village/C)
        "sites": 5, "npcs": 4, "incidents": 1,
        "memories": 3, "quests": 1, "rumors": 0,
        "dynamic_events": 0, "hidden_areas": 0, "era_layers_min": 3,
        "era_pct_target": 0,
    },
}

# ─── Tier 自動偵測 ────────────────────────────────────────
def detect_tier(sdata: dict) -> str:
    """從 settlement YAML 的 tier 欄位自動判斷 S/A/B/C 分級。

    對照表（詳見 docs/settlement-standards.md）：
      tier=1          → S  直轄大城
      tier=2/3        → A  重要市鎮
      tier=4/town/village → B  特色鄉村（village = 有故事特色的村落，非路過據點）
      tier=c/waypoint → C  路過據點（無特色，走過就走）
    """
    raw = str(sdata.get("tier", "")).strip().lower()
    if raw == "1":                          return "S"
    if raw in ("2", "3"):                   return "A"
    if raw in ("4", "town", "village"):     return "B"  # village = 特色鄉村 → Tier B
    if raw in ("c", "waypoint", "5", "6"): return "C"  # 明確路過據點
    # 找不到時依 admin_level 猜測
    level = str(sdata.get("admin_level", "")).strip()
    if level in ("直轄市",):     return "S"
    if level in ("縣", "縣轄市"): return "A"
    if level in ("鎮", "市"):    return "B"
    return "B"  # 預設 Tier B

# ─── 顏色輸出 ─────────────────────────────────────────────
GREEN  = "\033[92m"
YELLOW = "\033[93m"
RED    = "\033[91m"
CYAN   = "\033[96m"
BOLD   = "\033[1m"
RESET  = "\033[0m"

def ok(msg):    return f"{GREEN}✓{RESET} {msg}"
def warn(msg):  return f"{YELLOW}△{RESET} {msg}"
def fail(msg):  return f"{RED}✗{RESET} {msg}"
def info(msg):  return f"{CYAN}ℹ{RESET} {msg}"

# ─── YAML 讀取工具 ────────────────────────────────────────
def load_yaml(path: Path) -> dict:
    try:
        with open(path, "r", encoding="utf-8") as f:
            return yaml.safe_load(f) or {}
    except Exception as e:
        return {}

def load_all_yaml_in(directory: Path) -> list[dict]:
    results = []
    for p in directory.glob("**/*.yaml"):
        data = load_yaml(p)
        if data:
            data["__path__"] = str(p)
            results.append(data)
    return results

# ─── 稽核邏輯 ─────────────────────────────────────────────
@dataclass
class AuditResult:
    category: str
    count: int
    threshold: int
    details: list[str] = field(default_factory=list)
    note: str = ""

    @property
    def passed(self) -> bool:
        return self.count >= self.threshold

    @property
    def pct(self) -> float:
        return min(100.0, self.count / self.threshold * 100) if self.threshold else 100.0

    def render(self) -> str:
        bar_len = 20
        filled = int(self.pct / 100 * bar_len)
        bar = "█" * filled + "░" * (bar_len - filled)
        color = GREEN if self.passed else (YELLOW if self.pct >= 50 else RED)
        icon = "✓" if self.passed else "✗"
        line = (f"  {color}{icon}{RESET} {self.category:<22}"
                f"  {color}{self.count:>4}{RESET} / {self.threshold:<4}"
                f"  [{color}{bar}{RESET}] {color}{self.pct:5.1f}%{RESET}")
        if self.note:
            line += f"  {CYAN}({self.note}){RESET}"
        return line


def audit_settlement(settlement_id: str, tier: str = "B") -> tuple[list[AuditResult], list[str], str]:
    results: list[AuditResult] = []
    warnings: list[str] = []

    # ── 讀取 settlement.yaml ──────────────────────────────
    settlement_file = SETTLEMENTS_DIR / f"{settlement_id}.yaml"
    if not settlement_file.exists():
        # 可能在子目錄（例如 changhua/changhua_city.yaml）
        matches = list(SETTLEMENTS_DIR.glob(f"**/{settlement_id}.yaml"))
        if matches:
            settlement_file = matches[0]
        else:
            print(fail(f"找不到聚落設定檔：{settlement_id}.yaml"))
            sys.exit(2)

    sdata = load_yaml(settlement_file)
    s_name = sdata.get("canonical_name") or sdata.get("name") or settlement_id

    # ── 套用分級門檻 ──────────────────────────────────────
    THRESHOLDS = TIER_THRESHOLDS[tier]

    # ── 1. Sites ──────────────────────────────────────────
    site_dir = SITES_DIR / settlement_id
    site_files = list(site_dir.glob("*.yaml")) if site_dir.exists() else []

    # 也計算 map_data 裡的隱式 sites（settlement YAML 裡 map_data 欄位）
    map_data_keys = set(sdata.get("map_data", {}).keys())
    declared_sites = set(sdata.get("sites", []))
    all_site_ids = {f.stem for f in site_files} | map_data_keys | declared_sites

    site_count = len(all_site_ids)

    # 分析每個 site 的 era 數量、rumors、hidden
    site_yaml_data = {}
    for sf in site_files:
        d = load_yaml(sf)
        if d:
            site_yaml_data[sf.stem] = d

    # map_data 裡的 site 可能沒有獨立 yaml，算作「骨架」
    skeleton_sites = map_data_keys - {f.stem for f in site_files}
    if skeleton_sites:
        warnings.append(f"map_data 中有 {len(skeleton_sites)} 個地標尚無獨立 YAML："
                        f" {', '.join(sorted(skeleton_sites)[:5])}"
                        + (" ..." if len(skeleton_sites) > 5 else ""))

    results.append(AuditResult("Sites（地標）", site_count, THRESHOLDS["sites"],
                                note=f"含 map_data 中隱式地標"))

    # ── 2. NPCs ───────────────────────────────────────────
    all_npcs = load_all_yaml_in(NPCS_DIR)
    npc_count = sum(
        1 for n in all_npcs
        if n.get("settlement") == settlement_id
        or n.get("home_site", "").startswith(settlement_id)
    )
    # 也計算 site YAML 裡內嵌的 npc 欄位
    embedded_npcs = set()
    for sd in site_yaml_data.values():
        for layer in sd.get("reveal_layers", []):
            if "npc" in layer:
                embedded_npcs.add(layer["npc"])
        if "npc" in sd:
            embedded_npcs.add(sd["npc"])
    npc_count += len(embedded_npcs)

    results.append(AuditResult("NPCs（登場人物）", npc_count, THRESHOLDS["npcs"],
                                note=f"獨立檔 + site 內嵌"))

    # ── 3. Incidents ──────────────────────────────────────────
    all_incidents = load_all_yaml_in(INCIDENTS_DIR)
    def incident_belongs(i: dict) -> bool:
        if settlement_id in (i.get("scope") or []):
            return True
        if i.get("settlement") == settlement_id:
            return True
        # 透過 clues 的 source_id 判斷（e.g. "memory:minxiong_ghost_001"）
        for clue in i.get("clues", []):
            src = clue.get("source_id", "")
            if settlement_id in src:
                return True
        # incident_id 本身以 settlement_id 為前綴
        if i.get("incident_id", "").startswith(settlement_id):
            return True
        return False
    inc_count = sum(1 for i in all_incidents if incident_belongs(i))
    results.append(AuditResult("Incidents（歷史事件）", inc_count, THRESHOLDS["incidents"]))

    # ── 4. Memories ───────────────────────────────────────
    all_mems = load_all_yaml_in(MEMORIES_DIR)
    mem_count = sum(
        1 for m in all_mems
        if m.get("settlement") == settlement_id
        or m.get("trigger_site", "").startswith(settlement_id + "_")
        or (SITES_DIR / settlement_id).as_posix() in m.get("__path__", "")
    )
    # 也計算以 settlement_id prefix 命名的記憶
    mem_prefix_count = len([m for m in all_mems
                             if m.get("id", "").startswith(settlement_id)])
    mem_count = max(mem_count, mem_prefix_count)
    results.append(AuditResult("Memories（記憶碎片）", mem_count, THRESHOLDS["memories"]))

    # ── 5. Quests ─────────────────────────────────────────────
    # 支援 YAML（未來）與 Markdown（現行）兩種格式
    quest_count = 0
    quests_yaml_dir = YAML_ROOT / "quests"
    if quests_yaml_dir.exists():
        for qf in quests_yaml_dir.glob("**/*.yaml"):
            d = load_yaml(qf)
            if d.get("settlement") == settlement_id or \
               settlement_id in (d.get("scope") or []):
                quest_count += 1
    # 掃 docs/quests/*.md，透過檔案內容判斷是否屬於該聚落
    docs_quests_dir = REPO_ROOT / "docs" / "quests"
    if docs_quests_dir.exists():
        for qf in docs_quests_dir.glob("*.md"):
            try:
                content = qf.read_text(encoding="utf-8").lower()
                # 檔名或內容含 settlement_id 就計入
                if settlement_id in qf.stem or settlement_id in content:
                    quest_count += 1
            except Exception:
                pass
    note = "含 docs/quests/ MD 格式" if docs_quests_dir.exists() else "⚠ 目錄可能未建立"
    results.append(AuditResult("Quests（任務故事）", quest_count, THRESHOLDS["quests"],
                                note=note))

    # ── 6. Rumors ─────────────────────────────────────────────
    rumor_count = 0
    for sd in site_yaml_data.values():
        rumors = sd.get("rumors", [])
        if isinstance(rumors, list):
            rumor_count += len(rumors)
        elif isinstance(rumors, dict):
            rumor_count += len(rumors)
        # 也計算 reveal_layers 裡標記 is_rumor 的條目
        for layer in sd.get("reveal_layers", []):
            if layer.get("is_rumor"):
                rumor_count += 1
    # 也掃獨立的 rumors YAML
    rumors_dir = YAML_ROOT / "rumors"
    if rumors_dir.exists():
        for rf in rumors_dir.glob(f"**/{settlement_id}*.yaml"):
            d = load_yaml(rf)
            if isinstance(d.get("rumors"), list):
                rumor_count += len(d["rumors"])
            else:
                rumor_count += 1
    results.append(AuditResult("Rumors（傳言）", rumor_count, THRESHOLDS["rumors"]))

    # ── 7. Dynamic Events ─────────────────────────────────────
    all_events = load_all_yaml_in(WORLD_EVENTS_DIR)
    def event_belongs(e: dict) -> bool:
        if settlement_id in (e.get("affected_sites") or []):
            return True
        if settlement_id in (e.get("scope") or []):
            return True
        # effects.site_desc_override 的 key 包含 settlement_id 的 site
        for site_key in (e.get("effects") or {}).get("site_desc_override", {}).keys():
            if settlement_id in site_key or site_key in all_site_ids:
                return True
        # trigger 的 site 屬於該 settlement
        trigger_site = (e.get("trigger") or {}).get("site", "")
        if trigger_site in all_site_ids:
            return True
        # 事件 id 本身以 settlement_id 前綴
        if e.get("id", "").startswith(settlement_id):
            return True
        return False
    event_count = sum(1 for e in all_events if event_belongs(e))
    results.append(AuditResult("Dynamic Events（動態事件）", event_count,
                                THRESHOLDS["dynamic_events"]))

    # ── 8. Hidden Areas ───────────────────────────────────────
    hidden_keywords = {"underground", "secret", "hidden", "inner", "dungeon"}
    hidden_count = 0
    for stem, sd in site_yaml_data.items():
        if (sd.get("hidden") is True
                or sd.get("is_dungeon") is True
                or any(kw in stem for kw in hidden_keywords)):
            hidden_count += 1
    # 也計算 map_data 裡有 hidden 關鍵字但尚無獨立 YAML 的 site
    hidden_count += sum(
        1 for stem in (map_data_keys - set(site_yaml_data.keys()))
        if any(kw in stem for kw in hidden_keywords)
    )
    results.append(AuditResult("Hidden Areas（隱藏地區）", hidden_count,
                                THRESHOLDS["hidden_areas"]))

    # ── 9. Era Layers per Site ────────────────────────────
    sites_checked = 0
    sites_ok = 0
    sites_lacking: list[str] = []

    for site_id, sd in site_yaml_data.items():
        sites_checked += 1
        # 計算 era 數：names 列表 + reveal_layers 的 era 條件 + history_layers
        era_set = set()
        for name_entry in sd.get("names", []):
            if "era" in name_entry:
                era_set.add(name_entry["era"])
        for layer in sd.get("reveal_layers", []):
            for chk in layer.get("checks", []):
                if chk.get("type") == "era":
                    era_set.add(chk.get("value", ""))
        for hl in sd.get("history_layers", []):
            era_set.add(hl.get("era", ""))
        era_set.discard("")

        if len(era_set) >= THRESHOLDS["era_layers_min"]:
            sites_ok += 1
        else:
            sites_lacking.append(f"{site_id}({len(era_set)} eras)")

    era_pct = (sites_ok / sites_checked * 100) if sites_checked else 0
    era_result = AuditResult(
        f"Era ≥{THRESHOLDS['era_layers_min']} / Site（品質）",
        sites_ok, sites_checked,
        note=f"{era_pct:.0f}% 達標"
    )
    # 覆蓋門檻：品質指標要求 era_pct_target% 的 site 達標（依 Tier）
    era_result.threshold = max(1, int(sites_checked * THRESHOLDS["era_pct_target"] / 100))
    results.append(era_result)
    if sites_lacking:
        warnings.append(f"Era 不足 {THRESHOLDS['era_layers_min']} 層的 Sites："
                        f" {', '.join(sites_lacking[:5])}"
                        + (f" 等 {len(sites_lacking)} 個" if len(sites_lacking) > 5 else ""))

    return results, warnings, s_name


# ─── 主程式 ───────────────────────────────────────────────
def run_audit(settlement_id: str, forced_tier: Optional[str] = None, simple: bool = False):
    # 先載入 settlement YAML 偵測 tier（若未強制指定）
    settlement_file = SETTLEMENTS_DIR / f"{settlement_id}.yaml"
    if not settlement_file.exists():
        matches = list(SETTLEMENTS_DIR.glob(f"**/{settlement_id}.yaml"))
        if matches:
            settlement_file = matches[0]
    sdata = load_yaml(settlement_file) if settlement_file.exists() else {}
    tier = forced_tier if forced_tier else detect_tier(sdata)

    results, warnings, s_name = audit_settlement(settlement_id, tier=tier)

    passed = sum(1 for r in results if r.passed)
    total = len(results)
    is_ok = passed == total

    if simple:
        status_str = f"{GREEN}PASS{RESET}" if is_ok else f"{RED}FAIL ({passed}/{total}){RESET}"
        print(f"  [{status_str}] {s_name} ({settlement_id}) [Tier {tier}]")
    else:
        print(f"\n{BOLD}{'═' * 62}{RESET}")
        print(f"{BOLD}  聚落驗收稽核 — P23.1 Settlement Definition of Done{RESET}")
        print(f"{BOLD}{'═' * 62}{RESET}")
        print(f"\n  {BOLD}聚落：{s_name}（{settlement_id}）  [Tier {tier}]{RESET}\n")
        print(f"  {'類別':<24} {'數量':>6}   {'門檻':<6}  進度")
        print(f"  {'─' * 58}")

        failed_items = []
        for r in results:
            print(r.render())
            if not r.passed:
                failed_items.append(r.category)

        print(f"\n  {'─' * 58}")
        score_color = GREEN if is_ok else (YELLOW if passed >= total * 0.6 else RED)
        print(f"  {BOLD}通過：{score_color}{passed}{RESET}{BOLD} / {total} 項{RESET}")

        if warnings:
            print(f"\n  {BOLD}{YELLOW}⚠ 警告{RESET}")
            for w in warnings:
                print(f"    {YELLOW}·{RESET} {w}")

        if failed_items:
            print(f"\n  {BOLD}{RED}✗ 未達標項目{RESET}")
            for fi in failed_items:
                print(f"    {RED}·{RESET} {fi}")

        print(f"\n{BOLD}{'═' * 62}{RESET}\n")
        
    return 0 if is_ok else 1


def main():
    parser = argparse.ArgumentParser(
        description="P23.1 聚落驗收標準稽核工具（分級版）"
    )
    parser.add_argument("settlement", nargs="?", default="minxiong",
                        help="聚落 ID（預設：minxiong）")
    parser.add_argument("--all", action="store_true",
                        help="稽核所有聚落")
    parser.add_argument("--simple", action="store_true",
                        help="僅顯示最終的 PASS / FAIL 結果")
    parser.add_argument("--tier", choices=["S", "A", "B", "C"],
                        help="強制指定 Tier（預設自動從 settlement YAML 偵測）")
    args = parser.parse_args()

    if args.all:
        settlements = [p.stem for p in SETTLEMENTS_DIR.glob("*.yaml")]
        exit_code = 0
        if args.simple:
            print(f"\n{BOLD}  所有聚落簡要稽核結果：{RESET}\n")
        for sid in sorted(settlements):
            code = run_audit(sid, forced_tier=args.tier, simple=args.simple)
            exit_code = max(exit_code, code)
        sys.exit(exit_code)
    else:
        sys.exit(run_audit(args.settlement, forced_tier=args.tier, simple=args.simple))


if __name__ == "__main__":
    main()
