docs/mudlib/
```
00)vision.md: 已經在 docs/canon/ 中
01)architecture.md
02)data_model.md
03)daemons.md
04)player_model.md
05)world_model.md
06)settlement_model.md
07)memory_model.md
08)footprint_model.md
09)timeline_model.md
10)profession_model.md

01_architecture.md
↓
02_domain_model.md
↓
03_world_engine.md
↓
04_event_system.md
↓
05_data_storage.md

小說
↓
Canon
↓
Domain Model
↓
World Engine
↓
MUD Interface


在這個世界裡，究竟有哪些「存在」？

例如：

Entity（實體）

World
Era
Settlement
Site
Player
Faction
Event
Memory
Footprint

Value Object（值物件）

Coordinate
Population
Industry
Culture
Faith
Reputation

Service（服務）

Memory Service
Timeline Service
Settlement Service
Footprint Service

Event（事件）

MemoryUnlocked
FootprintGained
SettlementUpgraded
EraShifted

上面用 yaml 來表達的話，類似
world/

settlements/
    minxiong.yaml
    lukang.yaml
    tainan.yaml

events/
    xilai_an.yaml
    wushe.yaml
    yiwei.yaml

factions/
    lin_family.yaml
    gu_family.yaml

eras/
    v0_1.yaml
    v0_2.yaml


mudlib 系統架構
Go Driver
+
YAML
+
JSON
+
LPC VM

```
