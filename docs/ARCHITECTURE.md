# Blueprint版からの一対一C++移植

## 基準

参照元: `mrtksHub/codex_ue_sample` の `docs/BLUEPRINT_SPEC.md`。
設計を整理し直さず、同じクラス分割と振る舞いを再現します。
実際の `.uasset` の全ノードをこの環境から実行したわけではないため、Blueprint実装との細部の照合はUE Editorで実施してください。

## クラス対応

| Blueprint版 | 本C++版 | 親クラス |
| --- | --- | --- |
| BP_GameInstance | USampleGameInstance | UGameInstance |
| BP_GameModeBase | ASampleGameModeBase | AGameModeBase |
| BP_OpeningGameMode | AOpeningGameMode | ASampleGameModeBase |
| BP_MainTitleGameMode | AMainTitleGameMode | ASampleGameModeBase |
| BP_GameGameMode | AGameGameMode | ASampleGameModeBase |
| BP_EndingGameMode | AEndingGameMode | ASampleGameModeBase |
| BP_OpeningController | AOpeningController | APlayerController |
| BP_TitleController | ATitleController | APlayerController |
| BP_GameController | AGameController | APlayerController |
| BP_EndingController | AEndingController | APlayerController |
| E_ScreenState | ESampleScreen | UENUM |
| ST_PlayerData / ST_Settings / ST_SaveData | FSamplePlayerData / FSampleSettings / FSampleSaveData | USTRUCT |

**4つのControllerは共通C++ Controllerを継承しません。** 各画面で必要なBeginPlay処理と入力を独立実装します。

## 起動フロー（Opening）

1. Engineが `L_Opening` をロード。
2. World Settingsで `AOpeningGameMode` を選択（またはMap prefix設定）。
3. Engineが `AOpeningController` をPlayerControllerとして生成。
4. Engineによって `AOpeningController::BeginPlay()` が実行される。
5. `Super::BeginPlay()`。
6. `USampleGameInstance::NotifyScreenReady(Opening)` で `GameState` を更新、遷移中フラグを解除。
7. `CreateWidget(WBP_Opening)` → `AddToViewport()`、カーソル・InputMode設定。
8. Any Key入力で `USampleGameInstance::GoToScreen(MainTitle)`。

Title/Game/Endingでも各ControllerのBeginPlayが対応する画面IDでNotifyScreenReadyを呼び、対応するWBPを生成します。

## 元のStart → Gameフロー

1. `WBP_MainTitle.Btn_Start.OnClicked` (Blueprint) が `USampleGameInstance::GoToScreen(Game)` を呼びます。
2. GoToScreenは `bTransitionInProgress` がtrueならreturn。
3. falseならtrueにして、Enumから `L_Game` を選び `UGameplayStatics::OpenLevel()`。
4. EngineがGameModeに指定された `AGameController` を生成し、その `BeginPlay()` を呼ぶ。
5. `NotifyScreenReady(Game)` がGameStateと遷移中フラグを更新。
6. `AGameController::BeginPlay()` が `WBP_GameHUD` を生成・Viewportへ追加。

Game HUDのGame Clearは同様にGameInstance.GoToScreen(Ending)。EndingのFinishはGoToScreen(Opening)。
GameModeは対応Controllerの選択のみを担当し、遷移ロジックを持ちません。

## Pause / Confirm / Esc

- TitleのEscまたはExit Game: ATitleController::RequestExitGameでConfirmを一つ生成。
- TitleのConfirm: YesでQuitGame、No/EscでDialogだけを閉じる。
- Game通常状態のEsc: AGameController::ShowPauseMenu、SetGamePaused(true)。
- Pause中のEsc/Resume: ResumeGame、SetGamePaused(false)。
- PauseのReturn to Title/Exit Game: Confirmを表示。YesでPause解除後に遷移または終了。
- PauseのConfirmでNo/Esc: Confirmだけを閉じ、Pause状態は維持。
- Esc処理優先順位: Confirm → Pause → Game。EscはPause中にも受け付ける。

元のWBP_ConfirmDialogの `OnConfirmed` / `OnCancelled` というイベントディスパッチャをC++から型安全に購読するため、
`WBP_ConfirmDialog` だけは親を `USampleConfirmDialogWidget` とします。
ButtonのOnClickedグラフはBlueprintに残し、`ConfirmYes` / `ConfirmNo` を呼んで元と同名のディスパッチャを発火させます。
その他のWBPは通常の `UUserWidget` を親にでき、C++によるボタンの自動OnClicked接続は行いません。

## GameInstanceの管理変数

| 既存BP | C++ | 初期値 |
| --- | --- | --- |
| GameState | ESampleScreen GameState | Opening |
| bTransitionInProgress | bool | false |
| CurrentChapter | int32 | 0 |
| PlayerData | FSamplePlayerData | PlayerLevel=1 |
| Settings | FSampleSettings | 3つの音量=1.0 |
| SaveData | FSampleSaveData | SaveVersion=1, SavedChapter=0, bHasSave=false |

保存予定データはメモリ上のみ。新しいデータ通知、SaveGame永続化、遷移許可表、Map存在検証、独自の失敗時復元は追加しません。

## 本版と元の実アセットとの境界

- C++のGoToScreenとNotifyScreenReadyは元のBPの制御フローに対応します。
- WBPのDesignerは既存のものを移植できますが、BP_GameInstance型や旧Enumを参照するグラフは新しいC++型に配線し直す必要があります。
- すべてのMapのGameMode Overrideは新しいC++クラスへ変更する必要があります。
- C++コードだけでは元のWBP/Mapは生成されません。実アセットはEditorで作成・保存します。

参考: [EDITOR_SETUP.md](EDITOR_SETUP.md)、[VALIDATION.md](VALIDATION.md)