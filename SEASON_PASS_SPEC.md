# Season Pass UI: Unreal C++ / UMG / MVVM Build Spec

**Purpose:** a small portfolio project for the Epic Games UI Programmer Intern (R27436). It shows C++ game-UI architecture: data model, then ViewModel, then UMG widgets.
**Deadline:** Epic's deadline is **Fri Oct 23** (rolling review). Submit by Friday afternoon at the latest.
**Owner:** Neo. Claude Code writes the C++, and Neo builds the widgets in the editor.
**Environment:** Windows desktop, Unreal Engine 5.8.4, Visual Studio 2026 (MSVC v143 toolset installed), project at `C:\Dev\SeasonPassUI`, repo `https://github.com/Neo-Land/season-pass-ui`.

---

## 1. What it is

A Fortnite-style **season progression screen**:
- A horizontal track of **20 reward tiers**, each with a **free** row and a **premium** row.
- An **XP bar** showing the current level and progress to the next level.
- Each tile is **Locked**, **Claimable**, or **Claimed**. A **Claim** button appears on claimable tiles.
- Reaching a new level plays a **level-up toast** animation.
- **Gamify.Life tie-in:** some tiers unlock a **skill node** (for example "Cooking II"). A small skill panel shows nodes lighting up as they unlock.
- **Debug controls** (+100 XP, +1000 XP, Toggle Premium, Reset) so the demo can be recorded in 60 seconds.

**Out of scope:** networking, real purchases, 3D gameplay, and custom art. Use colored rectangles and engine fonts.

---

## 2. Setup (done on Windows)

1. Visual Studio 2026 with "Game development with C++" + "Desktop development with C++", MSVC v143, Windows 11 SDK. ✅
2. Unreal Engine 5.8.4 via the Epic Games Launcher (no extra target platforms). ✅
3. Project: **Games → Blank → C++**, Desktop, Scalable, no starter content, named `SeasonPassUI` in `C:\Dev`. ✅
4. Enable plugins under Edit → Plugins:
   - **UMG Viewmodel** (the `ModelViewViewModel` plugin)
   - **Common UI** (optional, stretch goal for gamepad focus)
   Restart the editor after enabling them.
5. Add the module dependencies in `SeasonPassUI.Build.cs`:
   `"UMG", "Slate", "SlateCore", "ModelViewViewModel", "FieldNotification"`
6. Git with a UE `.gitignore` (ignore `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`, `.vs/`, `*.sln`). Use **Git LFS** for `*.uasset` and `*.umap`. Remote: `https://github.com/Neo-Land/season-pass-ui.git` (public).

**Build workflow on Windows:** for header (.h) changes, close the editor, build **Development Editor / Win64** in Visual Studio, then reopen the editor. For .cpp-only changes, Live Coding (Ctrl+Alt+F11 in the editor) is fine.

---

## 3. Architecture

```
DT_SeasonTiers (DataTable, FSeasonTierRow)
        │
        ▼
USeasonProgressSubsystem   ← the MODEL (game state, rules, events)
        │  broadcasts OnXPChanged / OnLevelUp / OnTierClaimed
        ▼
USeasonPassViewModel       ← VIEWMODEL (FieldNotify properties for the UI)
  └─ TArray<USeasonTierViewModel*>   (one per tier)
  └─ TArray<USkillNodeViewModel*>    (Gamify.Life tie-in)
        │  MVVM bindings (no Blueprint graph logic)
        ▼
WBP_SeasonPass → WBP_XPBar, ListView of WBP_TierTile, WBP_SkillPanel, WBP_LevelUpToast
```

**Rule:** widgets contain **no game logic**. They bind to ViewModel properties and call ViewModel functions such as `ClaimTier(i)`. The subsystem holds all rules. This separation is the main thing the project demonstrates, so explain it in the README.

---

## 4. C++ classes (Claude Code writes these)

Put everything under `Source/SeasonPassUI/`, organized into `Model/`, `ViewModel/` and `UI/`.

### 4.1 `FSeasonTierRow : FTableRowBase` (Model/SeasonTypes.h)
| Field | Type | Notes |
|---|---|---|
| TierIndex | int32 | 1–20 |
| XPRequired | int32 | Cumulative XP to reach this tier |
| FreeRewardName | FText | |
| PremiumRewardName | FText | |
| Rarity | ESeasonRarity | Common / Rare / Epic / Legendary (drives tile color) |
| UnlocksSkillNode | FName | Optional; NAME_None if none |

Also define these enums in the same header:
- `ESeasonRarity`
- `ETierState { Locked, Claimable, Claimed }`

### 4.2 `USeasonProgressSubsystem : UGameInstanceSubsystem` (Model)
- **State:**
  - `int32 TotalXP`
  - `bool bHasPremium`
  - `TSet<int32> ClaimedFree`
  - `TSet<int32> ClaimedPremium`
  - `TSet<FName> UnlockedSkills`
  - `UDataTable* TierTable` (loaded from a soft path in `Initialize`)
- **API:**
  - `AddXP(int32)`
  - `GetCurrentLevel()`
  - `GetProgressToNext()` → 0..1
  - `GetTierState(int32 Tier, bool bPremium)`
  - `ClaimTier(int32 Tier, bool bPremium)` → bool
  - `SetPremium(bool)`
  - `ResetProgress()`
- **Rules:**
  - A tier is claimable when the player's level is at least the tier and it isn't already claimed.
  - A premium tier also requires `bHasPremium`.
  - Claiming a tier that has `UnlocksSkillNode` adds that node to `UnlockedSkills`.
- **Events:** declare `DECLARE_MULTICAST_DELEGATE`s for `OnProgressChanged`, `OnLevelUp(int32 NewLevel)` and `OnTierClaimed(int32, bool)`.
- **Testability:** allow injecting tier data directly (e.g. a `SetTierRowsForTest` helper) so the automation spec doesn't depend on the DataTable asset.
- **Stretch:** persist progress with a `USaveGame` subclass.

### 4.3 `USeasonPassViewModel : UMVVMViewModelBase` (ViewModel)
**FieldNotify properties.** Declare each with `UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)` and set it with `UE_MVVM_SET_PROPERTY_VALUE`:
- `int32 CurrentLevel`
- `float XPProgress`
- `FText XPLabel` ("1,250 / 2,000 XP")
- `bool bHasPremium`
- `FText LevelUpText`
- `bool bShowLevelUp`

**Holds:**
- `TArray<TObjectPtr<USeasonTierViewModel>> Tiers`
- `TArray<TObjectPtr<USkillNodeViewModel>> SkillNodes`

**Functions:**
- `Initialize(USeasonProgressSubsystem*)`: subscribe to the subsystem's events, then call `RefreshAll()`.
- `UFUNCTION(BlueprintCallable)`:
  - `ClaimTier(int32, bool)`
  - `DebugAddXP(int32)`
  - `DebugTogglePremium()`
  - `DebugReset()`
  - `DismissLevelUp()`

### 4.4 `USeasonTierViewModel : UMVVMViewModelBase`
FieldNotify properties:
- `TierIndex`
- `FreeRewardName`, `PremiumRewardName`
- `Rarity`
- `ETierState FreeState`, `PremiumState`
- `bool bIsCurrentTier` (highlights the tile)
- `FLinearColor RarityColor`: compute this in C++ and keep color logic out of the widget.

### 4.5 `USkillNodeViewModel : UMVVMViewModelBase`
FieldNotify properties: `FName NodeId`, `FText DisplayName`, `bool bUnlocked`.

### 4.6 Hookup
- Create `ASeasonPassHUD` (AHUD) or have the PlayerController create `WBP_SeasonPass` in `BeginPlay`.
- Construct the ViewModel, call `Initialize(subsystem)`, and assign it to the widget's ViewModel slot. Set the slot's creation type to **Manual** in the widget's View Bindings.
- Set the input mode to UI and show the mouse cursor.

### 4.7 Tests (shows engineering discipline)
Write an Automation Spec at `Source/SeasonPassUI/Tests/SeasonProgressSpec.cpp` covering:
- Level math at the XP boundaries
- You can't claim a locked tier
- A premium tier needs premium
- A tier can't be claimed twice
- Claiming a tier unlocks its skill node

Run the tests from **Tools → Session Frontend → Automation** (filter "SeasonPass").

---

## 5. Widgets (Neo builds these in the editor)

Claude Code can't create `.uasset` widgets, so build these by hand. They're mostly drag-and-drop plus bindings.

| Widget | Contents | Bindings |
|---|---|---|
| **WBP_SeasonPass** | Root: header, XP bar, tier ListView, skill panel, debug buttons, toast | ViewModel = USeasonPassViewModel (Manual) |
| **WBP_XPBar** | ProgressBar + "LEVEL 7" + XP label | `XPProgress` → Percent; `CurrentLevel`, `XPLabel` → Text |
| **WBP_TierTile** | Tier number, free and premium reward boxes, rarity border, Claim buttons, lock/check icons | Implements **IUserObjectListEntry**; item = USeasonTierViewModel; state → visibility of Claim/Lock/Check; `RarityColor` → border tint |
| **WBP_SkillPanel** | Row of node chips | `bUnlocked` → opacity/color |
| **WBP_LevelUpToast** | "LEVEL UP! 8" with scale + fade **UMG animation** | `bShowLevelUp` → plays the animation; auto-dismiss after 2 s |

**Build order:** WBP_TierTile → WBP_XPBar → WBP_LevelUpToast → WBP_SeasonPass. WBP_SkillPanel only if time allows.

**Polish targets** (only after the claim flow works end to end):
- The XP bar fills with interpolation, not an instant jump.
- The level-up toast animation.
- Stretch (cut if short on time): auto-scroll to current tier, pulsing claimable tiles, keyboard/gamepad focus via CommonUI.

---

## 6. Data

Create `DT_SeasonTiers` from `FSeasonTierRow` by importing a CSV. Claude Code should generate `Content/Data/SeasonTiers.csv` with:
- 20 tiers, with XP scaling roughly +500 per tier
- Rarities mixed in
- 5 tiers that unlock skill nodes: Cooking II, Guitar II, Running II, Drawing II, Coding II

---

## 7. Schedule

| When | Goal | Done when |
|---|---|---|
| **Wed Oct 21 (night)** | Project created, plugin enabled, Git repo pushed. Claude Code writes Model + DataTable CSV + tests | Project compiles; tests pass |
| **Thu Oct 22** | Claude Code writes the ViewModels and hookup; Neo builds the widgets and bindings | XP and claim flow works end to end |
| **Fri Oct 23 (morning)** | Light polish, README, GIF/video, push. Update the Epic form (C++/Unreal = Yes), add resume bullet, **submit** | Application submitted |

**Fallback:** if widgets aren't working by Thursday night, submit Friday morning with the tested C++ architecture plus whatever widgets work.

---

## 8. Deliverables

- **Public GitHub repo** with a README containing:
  - A GIF at the top
  - The architecture diagram from §3
  - A "Why MVVM" paragraph
  - How to run the project and the tests
- **45–60 s screen recording** (Win+Alt+R or OBS; GIF via ScreenToGif):
  1. Add XP
  2. Bar fills
  3. Level-up toast
  4. Claim free and premium tiers
  5. Premium is locked until toggled
  6. Skill node unlocks (if built)
- **Portfolio entry** on nehemiaslandaverde.com (can come after submission).
- **One-line resume bullet**, for example: "Built a Fortnite-style season pass UI in Unreal Engine 5 (C++, UMG, MVVM) with data-driven tiers, a tested progression subsystem, and animated level-up feedback."

---

## 9. Prompts for Claude Code

**Prompt 0 (Git setup, safe to run while the editor is open):**

> In C:\Dev\SeasonPassUI, initialize Git: add an Unreal Engine .gitignore (Binaries/, Intermediate/, Saved/, DerivedDataCache/, .vs/, *.sln, *.VC.db) and a .gitattributes that tracks *.uasset and *.umap with Git LFS. Commit everything, set the branch to main, add remote https://github.com/Neo-Land/season-pass-ui.git, and push.

**Prompt 1 (Model):**

> Read SEASON_PASS_SPEC.md. In this Unreal 5.8 C++ project (`SeasonPassUI`, Windows, Visual Studio 2026), implement sections 4.1, 4.2, 4.7 and 6 first: the types header, the progression subsystem, the automation spec and the tier CSV. Update Build.cs per §2.5. Keep the code compiling after each step and explain anything I must do in the editor. Don't create .uasset files.

**Prompt 2 (after that compiles and the tests pass):**

> Now implement §4.3–4.6 (the ViewModels and the HUD hookup). Then give me step-by-step editor instructions for building each widget in §5 and wiring its MVVM bindings.
