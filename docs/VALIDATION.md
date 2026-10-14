# 検証状況と受け入れ確認

## 作成環境で確認したこと

- プロジェクト定義のJSON構文とRuntime Module / Target / APIマクロ名の整合。
- 設定、C++の4 Map名、設計書のMap名の一致。
- 6 Widgetパスと導入手順の一致。
- ボタン名とC++接続先、Blueprint公開コマンドの対応。
- 差分の空白エラー、生成物や秘密設定を含めていないこと。

**上記は静的な確認です。UHT・C++コンパイル・Editor動作を保証するものではありません。**

## 未実施

- UE 5.8のUHTとC++ビルド。
- Map / Widget Blueprintの作成、CompileとSave。
- PIE / Standaloneでの動作。
- Windowsパッケージの作成と実行。

## Editor環境での受け入れ確認

| 操作・条件 | 期待する結果 |
| --- | --- |
| Openingでキー / マウスクリック | MainTitleへ1回だけ進む |
| Start → Game Clear → Finish | Game → Ending → MainTitleの順。Openingには戻らない |
| Start / Game Clear / Finishを連打 | 遷移の多重実行なし |
| MainTitleでExit Game / Esc | Confirmが1つだけ開く |
| TitleのConfirmでNo / Esc | Titleへ戻る |
| GameでEsc | Pauseが開き、ゲームが停止 |
| PauseでResume / Esc | Pauseが閉じ、ゲームが再開 |
| PauseでReturn to Title | Confirmが開く |
| Pause上のConfirmでNo / Esc | Confirmだけが閉じ、Pauseを維持 |
| Return to TitleのYes | Pauseを解除しMainTitleへ |
| Title / PauseのExit Game → Yes | Standalone / Packageで終了 |
| Confirm中に背景のボタンをクリック | 下層の操作は実行されない |
| Escを押し続ける | リピートで複数階層を戻らない |
| Game → Title → Gameを繰り返す | 古いWidgetやPause状態が残らない |
| 各Mapを直接PIEで開始 | Mapに対応するCurrentScreenで起動 |
| MapとGameModeが不一致 | Output Logへエラー。誤った画面進行をしない |
| 遷移先Mapを一時的に別名へ変更 | 遷移要求を拒否し元の画面を維持 |
| TravelFailure通知 | 遷移ロック解除、元の画面とPause操作を復元 |
| GameでSetPlayerData / SetCurrentChapterを実行 | 初期表示に加え、変更した値が通知で表示される |
| Controller / WidgetのFocus | Non-Focusable widgetの警告が出ない |
| Windowsパッケージ | 4 Map、6 WBPがCookされ、一周とPause/終了が動く |

初期Game画面には動くゲームオブジェクトがありません。停止の確認は`Is Game Paused`で行い、
必要なら一時的な回転ActorなどでTickの停止も確認してください。
Setterの確認は一時的なテスト用ActorやBlueprint呼び出しから行い、OnModelChanged内ではSetterを呼ばないでください。
実施後、この文書にUE版、ビルド構成、結果と未解決項目を追記してください。
