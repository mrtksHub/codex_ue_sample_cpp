# UE 5.8 Editor 導入とBlueprint接続

本リポジトリはC++コードのみ。4 Mapと6 Widget Blueprintはまだ含まれていません。
元のBlueprint版と同じ名前・レイアウトのアセットを新規作成するか、元のDesignerをMigrateしてください。

## 1. ビルド

1. `codex_ue_sample_cpp.uproject` を右クリック → Generate Visual Studio project files。
2. VS2022、C++によるゲーム開発、対応Windows SDK、UE 5.8を用意。
3. Development Editor / Win64で `codex_ue_sample_cppEditor` をビルド（Runtime moduleはScreenFlow）。
4. UE Editorを起動。C++ UCLASSの変更後はEditorを閉じて通常ビルド。

## 2. 4 MapとGameMode

Content/Mapsに4つのLevelを用意し、World Settings → GameMode Overrideを次のように設定。

| Map | GameMode Override | Engineが生成するPlayerController |
| --- | --- | --- |
| L_Opening | OpeningGameMode | OpeningController |
| L_MainTitle | MainTitleGameMode | TitleController |
| L_Game | GameGameMode | GameController |
| L_Ending | EndingGameMode | EndingController |

Default PawnはNone。Level Blueprintに画面遷移を追加しない。
Project SettingsのGameInstanceは `SampleGameInstance`。DefaultEngine.iniに指定済み。

## 3. Widget Blueprint（親クラスと接続）

Content/UI配下に以下6 WBPを用意。Designerのボタンと文字部品は元の名前を使うこと。
ボタンの `Is Focusable` を有効にしてCompile/Saveしてください。

| Widget | 親クラス | Blueprint Graph の接続 |
| --- | --- | --- |
| UI/Opening/WBP_Opening | UserWidget | ボタン遷移は不要。Any KeyはOpeningControllerが処理 |
| UI/MainTitle/WBP_MainTitle | UserWidget | Btn_Start.OnClicked → GetGameInstance → Cast SampleGameInstance → GoToScreen(Game)。Btn_ExitGame.OnClicked → GetOwningPlayer → Cast TitleController → RequestExitGame |
| UI/Game/WBP_GameHUD | UserWidget | Btn_GameClear.OnClicked → GetGameInstance → Cast SampleGameInstance → GoToScreen(Ending) |
| UI/Ending/WBP_Ending | UserWidget | Btn_Finish.OnClicked → GetGameInstance → Cast SampleGameInstance → GoToScreen(Opening) |
| UI/Common/WBP_PauseMenu | UserWidget | Btn_Resume → ResumeGame、Btn_ReturnToTitle → RequestReturnToTitle、Btn_ExitGame → RequestQuitGame（いずれもGetOwningPlayer → Cast GameController） |
| UI/Common/WBP_ConfirmDialog | SampleConfirmDialogWidget | Btn_Yes.OnClicked → ConfirmYes、Btn_No.OnClicked → ConfirmNo |

ConfirmのTextBlockは `Text_Message` と命名（Is Variable有効）。
Confirmの親クラスに `SampleConfirmDialogWidget` を選ぶと `OnConfirmed` / `OnCancelled` が使えます。
元のWBP_ConfirmDialogをMigrateして再利用する場合は、親変更時に **同名のBP独自Dispatcherを除去して、親のDispatcherを使う** ようにしてください。
元のWBPに残る古い `Cast To BP_GameInstance` や旧 `E_ScreenState` 型は新しい `SampleGameInstance` / `ESampleScreen` に差し替えます。
見た目のDesignerは変更不要です。

**C++は4つのControllerのBeginPlayからWidgetを生成します。WBP側でCreateWidget/AddToViewportを重複実行しないでください。**
WidgetのEvent OnClickedは従来どおりWBPに記述し、C++はその呼び先を提供します。

## 4. Focusとキー

- Title通常時: Btn_Start / Confirm時: Btn_No。
- Game通常時: Btn_GameClear / Pause時: Btn_Resume / Confirm時: Btn_No。
- Ending: Btn_Finish。すべてFocusableにする。
- EscはTitleControllerとGameControllerが処理し、Pause中も入力可能。
- PIEはEscがEditor Stop Playに奪われる場合があるためStandaloneで確認する。

## 5. 実行と保存

1. L_Openingを起動しAny Key → MainTitleを確認。
2. Start → Game、Game Clear → Ending、Finish → Openingを確認。
3. Title ExitのYes/No/Esc、Game Pause・Resume・Confirmを確認。
4. PIE、Standalone、Windowsパッケージで検証。
5. Content配下のMapとWBPをSaveしてGitへcommit。

操作ごとの受け入れ条件は [VALIDATION.md](VALIDATION.md)。