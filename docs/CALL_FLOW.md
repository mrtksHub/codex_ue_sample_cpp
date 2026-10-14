# コールフローと処理フロー

この図は登録済みC++ソースの静的な読み取りに基づきます。UEでの実行トレースではありません。
MapとWidget BlueprintはEditorで作成する必要があります。

## ソースと略号

| 略号 | ソース | クラスと役割 |
| --- | --- | --- |
| GI | [SampleGameInstance.cpp](../Source/ScreenFlow/Private/SampleGameInstance.cpp) | USampleGameInstance: 状態、遷移、共通データ |
| PC | [SamplePlayerController.cpp](../Source/ScreenFlow/Private/SamplePlayerController.cpp) | ASamplePlayerController: 入力、画面生成、Pauseと確認 |
| W | [SampleScreenWidget.cpp](../Source/ScreenFlow/Private/SampleScreenWidget.cpp) | USampleScreenWidget: ボタン接続、表示更新 |
| GM | [SampleGameMode.cpp](../Source/ScreenFlow/Private/SampleGameMode.cpp) | ASampleGameModeと4子クラス: 画面構成 |
| 型 | [SampleTypes.h](../Source/ScreenFlow/Public/SampleTypes.h) | 画面・確認Enum、データStruct |
| BP | Editorで作成するWBP_* | 見た目とOnModelChangedの表示処理 |
| UE | Unreal Engine | Level遷移、ライフサイクル、Widget構築 |

図のPC / GI / Wは上表のソースを表します。UE経由の呼び出しはC++からの直接呼び出しと区別します。
OpenLevelは遷移を要求します。呼び出した場所から次のBeginPlayへ直ちに関数呼び出しが続くわけではありません。

## 1. 起動と画面構築

GameInstanceはInitでTravelFailure通知を購読し、Shutdownで解除します。
画面とWidgetの組み合わせはGMの各コンストラクターで指定済みです。
SetupInputComponentでEsc / AnyKeyを接続します。以下の図はLevel開始後の画面構築の順序です。

```mermaid
sequenceDiagram
    participant UE as Unreal Engine
    participant PC as SamplePlayerController.cpp
    participant GI as SampleGameInstance.cpp
    participant W as SampleScreenWidget.cpp
    participant BP as Widget Blueprint
    UE->>PC: BeginPlay()
    PC->>PC: Model()でGI取得、GameMode取得
    PC->>GI: NotifyScreenReady(Mode.Screen)
    GI->>GI: MapPackage()と現在Mapを照合
    GI->>GI: CurrentScreen更新、遷移ロック解除
    GI->>GI: OnDataChanged.Broadcast()
    GI-->>PC: true
    PC->>UE: SetGamePaused(false)
    PC->>GI: OnDataChangedへSyncFlowAvailabilityを登録
    PC->>PC: CreateView(ScreenWidgetClass, 0)
    PC->>UE: LoadSynchronous / CreateWidget / AddToViewport
    UE->>W: NativeConstruct()
    W->>W: ボタン接続とOnDataChanged購読
    W->>W: RefreshFromModel()
    W->>BP: OnModelChanged(Model)
    PC->>PC: FocusTopWidget()
```

NotifyScreenReadyの通知時点では、新しい画面のWはまだ購読していません。
そのためNativeConstructでRefreshFromModelを明示的に呼び、初期値を表示します。
この一度の初期更新が、最初の表示を成立させます。

## 2. Startを押した場合のコールフロー

```mermaid
sequenceDiagram
    participant BP as WBP_MainTitle
    participant W as SampleScreenWidget.cpp
    participant PC as SamplePlayerController.cpp
    participant GI as SampleGameInstance.cpp
    participant UE as Unreal Engine
    Note over W,PC: NativeConstructでBtn_StartをStartGameへ接続済み
    BP->>PC: Btn_Start.OnClicked → StartGame()
    PC->>PC: CanUseScreen(MainTitle)
    PC->>PC: TravelTo(Game)
    PC->>GI: GoToScreen(Game)
    GI->>GI: 遷移ロックと許可経路を確認
    GI->>GI: MapPackage(Game)、Map存在確認
    GI->>GI: PendingScreen=Game、遷移ロック取得
    GI->>UE: SetGamePaused(false)
    GI->>UE: OpenLevel(L_Game)
    GI-->>PC: true（要求受理）
    PC->>PC: SyncFlowAvailability() → FocusTopWidget()
    Note over PC: 遷移前のUIを操作不可にする
    Note over PC,UE: 以降はUEが遷移を進める。直接の関数呼び出しではない
    UE->>PC: 旧ControllerのEndPlay()
    PC->>PC: 通知解除、CleanupUI()
    UE->>PC: 新ControllerのBeginPlay()
    PC->>GI: NotifyScreenReady(Game)
    GI->>GI: CurrentScreen=Game、ロック解除、通知
    PC->>PC: CreateView(WBP_GameHUD, 0)
    PC->>UE: Widget生成とViewport追加
    UE->>W: NativeConstruct() → RefreshFromModel()
    PC->>PC: FocusTopWidget()
```

OnClickedの接続先はPCです。クリックのたびにWの中継関数を呼ぶ構造ではありません。
Wは構築時に接続を準備します。新旧PCは同じクラスですが別インスタンスです。

## 3. 画面遷移の処理フロー

```mermaid
flowchart TD
    A["PC: StartGame / ClearGame / FinishEnding"] --> B{"PC: CanUseScreenで操作可能か"}
    B -->|いいえ| R["要求を実行しない"]
    B -->|はい| C["PC: TravelTo(Target)"]
    C --> D["GI: GoToScreen(Target)"]
    D --> E{"Worldがあり、遷移中ではないか"}
    E -->|いいえ| R
    E -->|はい| F{"許可経路か、Mapが存在するか"}
    F -->|いいえ| R
    F -->|はい| G["GI: PendingScreen設定、遷移ロック取得"]
    G --> H["GI: Pause解除、OpenLevel"]
    H --> I["PC: SyncFlowAvailabilityでUIを無効化"]
    I --> J{"UE: Level遷移の結果"}
    J -->|成功| K["旧PC: EndPlay → CleanupUI"]
    K --> L["新PC: BeginPlay → GI: NotifyScreenReady"]
    L --> M{"Mapと画面IDが一致するか"}
    M -->|はい| N["GI: CurrentScreen更新、ロック解除"]
    N --> O["PC: CreateView → FocusTopWidget"]
    M -->|いいえ| X["GI: エラーログ、BeginPlayの初期化を中止"]
    J -->|TravelFailure通知| P["GI: HandleTravelFailure → ロック解除と通知"]
    P --> Q["PC: SyncFlowAvailability → UIと元のPauseを復元"]
```

Map/GameMode不一致は設定エラーです。自動修復する処理はありません。
TravelFailureは当該GameInstanceのWorldからの通知を処理します。全てのロード障害に対する復帰を保証するものではありません。
GoToScreenの許可経路チェックと、PCのUI操作可否チェックは別です。通常のUI操作はPCのコマンドから呼びます。

## 4. Escの処理フロー

UIにFocusがある場合はWのNativeOnPreviewKeyDownからPCのHandleUIKeyを呼びます。
Focusがない場合はPCのSetupInputComponentで登録したEscバインドがHandleEscapeを呼びます。
UIで処理済みのキーはFReply::Handledで消費し、リピートによる再操作も抑えます。

```mermaid
flowchart TD
    A["Esc入力"] --> B["PC: HandleEscape()"]
    B --> C{"遷移中、Modelなし、画面なし"}
    C -->|はい| Z["処理終了"]
    C -->|いいえ| D{"ConfirmWidgetがあるか"}
    D -->|はい| E["PC: ConfirmNo → FocusTopWidget"]
    E --> Z
    D -->|いいえ| F{"PauseWidgetがあるか"}
    F -->|はい| G["PC: ResumeGame → Pause解除とFocus復帰"]
    G --> Z
    F -->|いいえ| H{"CurrentScreen"}
    H -->|Game| I["PC: OpenPauseMenu → CreateView、Pause開始"]
    H -->|MainTitle| J["PC: RequestQuitGame → OpenConfirm"]
    H -->|Opening| K["PC: HandleAnyKey → TravelTo(MainTitle)"]
    H -->|Ending| Z
    I --> Z
    J --> Z
    K --> Z
```

いずれか1つの操作をしたらreturnします。Confirmから同じEscでGameまで戻りません。

## 5. Pauseからタイトルへ戻るコールフロー

以下はPauseが開いている状態からです。各ボタンはWのNativeConstructでPCの対応関数へ接続済みです。

```mermaid
sequenceDiagram
    participant BP as Widget Blueprint
    participant PC as SamplePlayerController.cpp
    participant W as SampleScreenWidget.cpp
    participant GI as SampleGameInstance.cpp
    BP->>PC: Btn_ReturnToTitle → RequestReturnToTitle()
    PC->>PC: OpenConfirm(ReturnToTitle)
    PC->>PC: CreateView(ConfirmWidgetClass, 200)
    Note over PC,W: UE経由でNativeConstructが実行される
    PC->>W: SetConfirmMessage()
    PC->>PC: FocusTopWidget()
    Note over PC: 元のPauseを維持し、下層UIを無効化
    alt NoまたはEsc
        BP->>PC: ConfirmNo()
        PC->>PC: Confirmを削除、PendingConfirm解除
        PC->>PC: FocusTopWidget()でPauseへ
    else Yes
        BP->>PC: ConfirmYes()
        PC->>PC: TravelTo(MainTitle)
        PC->>GI: GoToScreen(MainTitle)
        GI->>GI: 検査、ロック取得、Pause解除、OpenLevel
        GI-->>PC: 要求の受理結果
        Note over PC: 受理時は遷移、拒否時はConfirmとPauseを維持
    end
```

終了確認のYesはConfirmYesのQuitGame分岐です。
Pause解除 → ConfirmNo → 必要ならResumeGame → UKismetSystemLibrary::QuitGameの順に実行します。
No / EscではQuitGameを呼びません。

## 6. 管理データからBlueprint表示へのコールフロー

```mermaid
sequenceDiagram
    participant Caller as データを変更する処理
    participant GI as SampleGameInstance.cpp
    participant PC as SamplePlayerController.cpp
    participant W as SampleScreenWidget.cpp
    participant BP as Widget Blueprint
    Caller->>GI: SetCurrentChapter(3)
    GI->>GI: 値の補正、CurrentChapter更新
    GI->>GI: OnDataChanged.Broadcast()
    par 登録済みPCの購読処理
        GI->>PC: SyncFlowAvailability()
        PC->>PC: FocusTopWidget()
    and 登録済みWidgetの購読処理
        GI->>W: RefreshFromModel()
        W->>W: 名前付きTextBlockをSetText
        W->>BP: OnModelChanged(Model)
        BP->>BP: 任意の画像・文字・色を更新
    end
```

parは同じ通知に複数の購読先があることを示します。別スレッドで並列実行する意味ではありません。
購読先の実行順に依存した表示処理は作らないでください。
SetPlayerData / SetSettingsも同じ通知経路です。
OnModelChangedは表示を更新するためのイベントです。この中で同じSetterを呼び続けると通知が再帰します。

## 7. ボタンと呼び出し先

接続コードはすべてWのNativeConstructにあります。PCのコマンドが管理処理の入口です。

| Widget | Button | PCの関数 | 結果 |
| --- | --- | --- | --- |
| WBP_MainTitle | Btn_Start | StartGame | Gameへ |
| WBP_MainTitle | Btn_ExitGame | RequestQuitGame | 終了確認 |
| WBP_GameHUD | Btn_GameClear | ClearGame | Endingへ |
| WBP_Ending | Btn_Finish | FinishEnding | MainTitleへ |
| WBP_PauseMenu | Btn_Resume | ResumeGame | 再開 |
| WBP_PauseMenu | Btn_ReturnToTitle | RequestReturnToTitle | タイトル復帰確認 |
| WBP_PauseMenu | Btn_ExitGame | RequestQuitGame | 終了確認 |
| WBP_ConfirmDialog | Btn_Yes | ConfirmYes | 確認内容を実行 |
| WBP_ConfirmDialog | Btn_No | ConfirmNo | 確認だけ閉じる |

Opening → MainTitleの入口はHandleAnyKeyです。
ゲーム開始、クリア、Ending終了はそれぞれStartGame / ClearGame / FinishEndingですが、
TravelTo以降の遷移手順は共通です。

## 読む順序

1. PCのStartGame / CanUseScreen / TravelTo。
2. GIのGoToScreen / MapPackage / NotifyScreenReady。
3. PCのBeginPlay / CreateView / EndPlay / CleanupUI。
4. WのNativeConstruct / RefreshFromModel。
5. PCのHandleEscape / OpenPauseMenu / OpenConfirm / ConfirmYes / ConfirmNo。

この順で読むと、操作、遷移、画面構築、表示更新、補助UIの順に理解できます。
