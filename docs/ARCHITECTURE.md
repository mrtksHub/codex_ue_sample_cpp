# C++ / Blueprintの役割と接続

## 基本方針

Blueprintは見た目と演出を作る場所、C++は状態を変更する場所です。
Level Blueprintに管理処理を置きません。GameInstanceはLevelをまたいでデータを保持します。
このサンプルは単一プレイヤー用です。ネットワーク同期・セーブ永続化・音量の実オーディオ反映は未実装です。

## クラス

| クラス | 役割 |
| --- | --- |
| `USampleGameInstance` | 現在画面、遷移先、遷移ロック、プレイヤー・設定・セーブ予定データを保持 |
| `ASampleGameMode` | 共通設定。PlayerControllerはSamplePlayerController、DefaultPawnなし |
| `ASampleOpeningGameMode` | Opening用Widgetと画面ID |
| `ASampleTitleGameMode` | MainTitle用Widgetと画面ID |
| `ASampleGameGameMode` | Game用Widgetと画面ID |
| `ASampleEndingGameMode` | Ending用Widgetと画面ID |
| `ASamplePlayerController` | UIライフサイクル、入力、Pause、確認ダイアログと終了処理 |
| `USampleScreenWidget` | Blueprint画面の共通親、名前付き部品の接続、表示更新とUI入力 |

`CurrentScreen`は独自の画面状態です。UEのGameStateクラスとは別です。
GameModeはC++クラスを直接使用できます。見た目のクラスを差し替える場合には、各GameModeのBlueprint子クラスでWidgetClassを指定できます。

## 画面遷移

通常の入口はControllerの`StartGame` / `ClearGame` / `FinishEnding`です。
GameInstanceの`GoToScreen`が、許可された遷移、既存マップ、遷移中でないことを確認します。
遷移要求を受理するとロックを取得し、Pauseを解除して`OpenLevel`を実行します。
遷移前のWidgetは操作不可にし、ControllerのEndPlayで取り外します。
遷移先ControllerのBeginPlayがMapと画面IDを照合して`NotifyScreenReady`を呼び、現在画面を更新してロックを解除します。

許可する遷移:

- Opening → MainTitle
- MainTitle → Game
- Game → Ending
- Game → MainTitle（PauseのReturn to Title）
- Ending → MainTitle

Mapの直接PIE開始にも対応します。各Mapの正しいGameModeが必要です。
存在しないMapへの要求は拒否します。UEのTravelFailure通知時はロックを解除し、元のUIの操作とPauseを復元します。
異なるMap/GameModeの組み合わせはログへエラーを出します。任意のMapからの復帰を保証する仕組みではありません。

## Pauseと確認

Confirm → Pause → Gameの順に最前面の状態を優先し、処理後すぐreturnします。
Confirm中は下のPauseと画面を無効化し、Pause中はゲーム画面を無効化します。
ダイアログの多重生成を拒否します。

- GameでEsc: Pauseを開きゲームを停止。
- PauseでEsc / Resume: Pauseを閉じゲームを再開。
- PauseのReturn to Title: 確認を開く。YesでPause解除とタイトル遷移。
- Title / PauseのExit Game: 確認を開く。YesでPause解除とQuitGame。
- ConfirmのNo / Esc: Confirmだけを閉じる。元がPauseなら停止を維持。

EscはWidgetのNativeOnPreviewKeyDownで消費し、UIにFocusがないときはControllerのBindKeyが処理します。
同じキーをUIとControllerで二重処理しない設計です。キーリピートはUIで消費し、新しい状態変更を起こしません。
ControllerのEscバインドはPause中の実行を許可します。
Openingのキーは遷移用に消費し、次画面の操作に使い回しません。

## Blueprintへの管理データ反映

GameInstanceのデータはBlueprintReadOnlyとして公開します。
変更は`SetPlayerData` / `SetSettings` / `SetCurrentChapter`を使用します。
変更時に`OnDataChanged`が通知され、画面の`OnModelChanged(Model)`イベントが呼ばれます。
Widget構築時にも一度呼ぶため、初期値が表示されます。Tickによる毎フレーム取得は不要です。

任意の表示例:

1. WBP_GameHUDのGraphで`Event On Model Changed`を追加する。
2. Modelから`PlayerData`を取得しBreakする。
3. PlayerNameやPlayerLevelを任意のTextへSetTextする。
4. SettingsやCurrentChapterを画像、色、進行表示などへ反映する。

以下の名前を使ったTextBlockはC++が自動更新します。すべて任意です。

| TextBlock名 | 表示 |
| --- | --- |
| `Text_CurrentScreen` | 現在の画面ID |
| `Text_PlayerName` | プレイヤー名 |
| `Text_PlayerLevel` | プレイヤーレベル |
| `Text_CurrentChapter` | 現在の章 |

UIで新しい設定値を入力するなら、入力イベントからSetterを呼びます。
「変更判断」と「変更後の表示」を同じOnModelChangedイベントへ混ぜないでください。Setterを呼び続けると通知が再帰します。
Settingsは値を保持するだけで、実際の音量制御はまだありません。

## ボタンとFocus

Editor導入手順の名前を使うとC++のNativeConstructがOnClickedを自動接続します。
各ButtonのIs Focusableを有効にします。
初期FocusはConfirmのNo、PauseのResume、TitleのStart、GameのGameClear、EndingのFinishです。
OpeningなどButtonなしの画面だけ、FocusableなWidget自身を使います。
必要ならBlueprintで`GetInitialFocusTarget`をoverrideし、画面内のFocusableな部品を返してください。

## 参照元との差

- 管理グラフをC++へ移し、4つのControllerを1つのC++Controllerへ統合。
- Enum / StructをC++型に変更。BPの旧型を併用しない。
- Endingの戻り先をOpeningからMainTitleへ変更。
- 未実装のMap / Widgetはログで明示し、バイナリのダミーを置かない。
- 環境依存のModelContextProtocol / AllToolsets / Terminalプラグインをプロジェクト定義へ含めない。
- 保存予定データは引き続きメモリー上のみ。

## API参照

- [UUserWidget](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/UMG/UUserWidget)
- [OnPreviewKeyDown](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/UMG/UUserWidget/OnPreviewKeyDown)
- [UEngine / OnTravelFailure](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/UEngine)

UE 5.8での最終的なAPI適合性はUHTとC++ビルドで確認してください。
