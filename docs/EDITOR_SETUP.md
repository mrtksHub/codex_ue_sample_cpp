# Unreal Editorでの導入手順

本リポジトリはC++ソースの土台です。以下のMapとWidgetをEditorで作成・保存して初めて画面遷移を実行できます。
元のプロジェクトに追加するのではなく、独立したプロジェクトとして開いてください。

## 1. C++ビルド

1. リポジトリをcloneする。
2. UE 5.8、Visual Studio 2022の「C++によるゲーム開発」と対応Windows SDKを用意する。
3. `codex_ue_sample_cpp.uproject`を右クリックし、Generate Visual Studio project filesを実行する。
4. 生成されたSolutionを開き、Development Editor / Win64で`codex_ue_sample_cpp`をビルドする。Editor Targetは`codex_ue_sample_cppEditor`、Runtime Moduleは`ScreenFlow`。
5. `.uproject`を開く。

初回はStartup Mapが未作成なので、読み込みエラーが出る場合があります。Map作成前は正常に遊べません。
初回ビルドとUCLASSの変更時はEditorを閉じて通常ビルドしてください。
Engineの配置に応じてコマンドでビルドする場合:

```powershell
& '<UE5.8の場所>\Engine\Build\BatchFiles\Build.bat' codex_ue_sample_cppEditor Win64 Development '<clone先>\codex_ue_sample_cpp.uproject' -WaitMutex
```

## 2. 4つのMap

「ファイル → 新規レベル」で空のLevelを作り、`Content/Maps`へ以下の名前で保存します。
各Mapの「ワールド設定 → GameMode Override」を設定してください。

| Map | GameMode Override |
| --- | --- |
| `L_Opening` | `SampleOpeningGameMode` |
| `L_MainTitle` | `SampleTitleGameMode` |
| `L_Game` | `SampleGameGameMode` |
| `L_Ending` | `SampleEndingGameMode` |

DefaultPawnはC++でNoneです。プレイヤーやカメラの配置は必要ありません。Level Blueprintも空のままです。
GameInstanceはDefaultEngine.iniにより`SampleGameInstance`になります。
INIのGameModeMapPrefixesも設定済みですが、World SettingsのOverrideが優先されるため、明示的に確認してください。

## 3. 6つのWidget Blueprint

Content Browserで「ユーザーインターフェース → ウィジェットブループリント」を作成し、親クラスとして`SampleScreenWidget`を選びます。
作成時に選べない場合は、Widgetを開き「クラス設定 → 親クラス」を`SampleScreenWidget`へ変更します。

| 保存先（/Gameから） | 必要な部品名 |
| --- | --- |
| `UI/Opening/WBP_Opening` | 任意のText。例: CODEX UE SAMPLE CPP / Press Any Key |
| `UI/MainTitle/WBP_MainTitle` | Button: `Btn_Start`, `Btn_ExitGame` |
| `UI/Game/WBP_GameHUD` | Button: `Btn_GameClear` |
| `UI/Ending/WBP_Ending` | Button: `Btn_Finish` |
| `UI/Common/WBP_PauseMenu` | Button: `Btn_Resume`, `Btn_ReturnToTitle`, `Btn_ExitGame` |
| `UI/Common/WBP_ConfirmDialog` | TextBlock: `Text_Message`、Button: `Btn_Yes`, `Btn_No` |

名前は大文字・小文字も含めて一致させてください。すべてC++のBindWidgetOptionalで参照します。
Optionalなので、名前の間違いではCompileエラーにならずボタンが働かない場合があります。
各Buttonを「変数」にし、「Is Focusable」を有効にしてください。
部品の型はButton / TextBlockです。似た別クラスに置き換えないでください。

レイアウトは自由です。まずCanvas PanelへTextとButtonを置いた簡単な画面で一周させてください。
Openingは全画面に広げ、親WidgetのIs Focusableを有効のままにします。
PauseとConfirmは全画面の半透明背景と中央Panelを作ると操作しやすくなります。
C++が下の画面を無効化するので、背景クリックが下のボタンへ届いても実行されません。

**ボタンのOnClickedに遷移グラフを追加する必要はありません。C++が自動接続します。**
すべてCompileしてSaveします。各GameModeは表のパスを既定値として読み込みます。

## 4. 管理情報の表示確認

WBP_GameHUDに任意で`Text_CurrentScreen`、`Text_PlayerName`、`Text_PlayerLevel`、`Text_CurrentChapter`を作ります。
Gameの開始時にGame、空文字、1、0が表示されます。
Graphの`Event On Model Changed`からModelの値を使えば、名前に依存しない任意の画面部品にも反映できます。
詳細は[ARCHITECTURE.md](ARCHITECTURE.md)を参照してください。

## 5. 起動と確認

1. L_Openingを開く。
2. Standalone Gameで起動する。
3. Opening → MainTitle → Game → Ending → MainTitleを確認する。
4. GameのPause、確認のYes / No / Escを確認する。
5. [VALIDATION.md](VALIDATION.md)の残りを実施する。
6. Map、WidgetをSaveし、`Content/`配下の実アセットをGitへcommitする。

PIEではEscがEditorの「Stop Play」に奪われる設定があります。Escの確認はStandaloneを基本にしてください。
PIEで確認する場合はEditor PreferencesのキーボードショートカットでStop PlayのEscを変更してください。

## 元のUIの見た目を利用する場合

安全な初期導入は、新しい6つのWBPへ元のDesignerの部品や画像を移す方法です。
元のWBPをMigrateして再利用する場合、参照から旧BP_GameInstanceやControllerも一緒に移ることがあります。
親をSampleScreenWidgetへ変更し、旧OnClicked、ConstructのUI生成・入力設定、Pause、確認Dispatcherと遷移グラフを削除してください。
旧Enum / Structは新しいC++型とは別型なので、参照も差し替えてください。
元のMapを移す場合、World Settingsの旧BP GameMode Overrideも必ず変更してください。
新しいC++経路と旧Blueprint経路を同時に残さないでください。

## トラブルの確認

- C++クラスが見えない: Editor Targetのビルド成功、Moduleのロード、Editorの再起動を確認。
- 画面が空: Output Logの`ScreenFlow: missing Widget Blueprint`、保存先、親クラスを確認。
- 遷移しない: Map名、保存済みか、Output Log、GameInstanceとGameModeを確認。
- Buttonが働かない: 表の部品名とButton型、親クラスを確認。
- Escで複数階層を戻る: 旧BP入力グラフや独自OnKeyDownが残っていないか確認。
- PackageでMap / Widgetがない: Save All、MapsToCookとDirectoriesToAlwaysCookを確認。
