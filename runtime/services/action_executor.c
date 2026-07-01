// /runtime/services/action_executor.c
// FSE 通用行動執行管理器 (Action Executor Daemon)
// 職責：
//   - 接收來自 Node 的玩家行動 (Action) 與目標 (Target)
//   - 根據行動類型，調度至對應專屬的邏輯解算器 (Resolver Strategy)
//   - 支持：fight (戰鬥)、craft (百工)、trade (紅塵交易)、dialogue (對話) 與 認識論 (reality) 評估。
#include "/runtime/include/ansi.h"

private nosave mapping resolvers;

void create() {
    resolvers = ([
        "meditate"   : "/runtime/executors/reality_resolver.c",
        "observe_breath": "/runtime/executors/reality_resolver.c",
        "strike_pivot": "/runtime/executors/reality_resolver.c",
        // 預留其他百工與物理戰鬥 Resolver
        "fight"      : "/runtime/executors/combat_resolver.c",
        "craft"      : "/runtime/executors/crafting_resolver.c",
        "trade"      : "/runtime/executors/commerce_resolver.c",
        "dialogue"   : "/runtime/executors/dialogue_resolver.c"
    ]);
}

// 註冊新的動態 Action 執行器映射
void register_resolver(string action, string resolver_path) {
    if (!resolvers) resolvers = ([]);
    resolvers[action] = resolver_path;
}

// 行動派發入口
// 參數：
//   - node_obj: 發起行動的節點物件
//   - actor: 發起行動的玩家/NPC 物件
//   - action: 行動動詞 (e.g. meditate, craft, fight)
//   - target: 行動目標 (e.g. spiritual_current, iron_ore, bandit)
//   - act_cfg: node.yaml 宣告的互動配置 mapping
int dispatch_action(object node_obj, object actor, string action, string target, mapping act_cfg) {
    if (!node_obj || !actor || !act_cfg) return 0;

    string res_path = resolvers[action];
    
    // Fallback: 如果沒有宣告專屬的 Action 映射，但互動宣告了 resolver yaml，則使用通用 reality_resolver
    if (!res_path && act_cfg["resolver"]) {
        res_path = "/runtime/executors/reality_resolver.c";
    }

    if (!res_path) {
        return 0; // 未能響應，交回給第一代二元物理互動
    }

    if (file_size(res_path) <= 0) {
        write(HIR "⚠️ [ActionExecutor] 找不到指定的 Resolver 執行器: " + res_path + "\n" NOR);
        return 0;
    }

    object resolver = load_object(res_path);
    if (!resolver) {
        write(HIR "⚠️ [ActionExecutor] 無法加載 Resolver: " + res_path + "\n" NOR);
        return 0;
    }

    // 構造標準 AST 輸入
    mapping ast = ([
        "action": action,
        "target": target
    ]);

    string res_challenge = act_cfg["resolver"] || sprintf("%s_challenge", action);
    string node_dir = node_obj->query_node_dir();
    string chal_path = sprintf("%schallenges/%s.yaml", node_dir, res_challenge);
    
    mapping chal_data = ([]);
    if (file_size(chal_path) > 0) {
        string raw = read_file(chal_path);
        if (raw) {
            chal_data = yaml_decode(raw);
        }
    } else {
        // 為了相容性，若無挑戰 YAML 則由 resolver 執行基礎比對
        chal_data = ([
            "challenge_id": res_challenge,
            "success_msg": act_cfg["success_msg"],
            "failure_warning": act_cfg["failure_msg"]
        ]);
    }

    // 執行專屬 Resolver，回傳 1 表示該行動已被完全接管與處理
    return resolver->execute(node_obj, actor, ast, chal_data, res_challenge);
}
