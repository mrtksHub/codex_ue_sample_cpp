# Blueprint等価移植: 検証記録

## 確認可能なこと

- Blueprint版の設計書（docs/BLUEPRINT_SPEC.md）とC++のクラス/関数を対照。
- 1 GameInstance、共通基底GameModeと4子GameMode、独立した4 PlayerControllerを確認。
- Ending → Openingへ戻ること、GameInstanceが遷移中フラグだけで重複を防止することをソースから確認。
- WBPのボタンイベントはBPに残し、C++の呼び先をBlueprintCallableとして公開。

## 未実施（必須）

- UE 5.8のUHTとDevelopment Editor / Win64ビルド。
- Editorで4 Mapと6 WBPを作成、または元のDesignerをMigrate。
- 実際の元Blueprint Event Graphとの行単位の接続照合。
- PIE/StandaloneのEsc優先順位、Focus、Confirm、Widget寿命を検証。
- Windows Packagingと実行確認。

## 受け入れ条件

| 操作 | 期待結果（元Blueprint版に準拠） |
| --- | --- |
| L_Opening起動 | Opening画面とカーソルが表示される |
| OpeningでAny Key | MainTitleに1回だけ遷移 |
| MainTitle Start | Gameへ遷移 |
| Game Game Clear | Endingへ遷移 |
| Ending Finish | **Openingへ戻る** |
| Openingへ戻った後Any Key | 再びMainTitleへ進める |
| ボタン連打 | GameInstanceの遷移中ロックが重複を防ぐ |
| MainTitle Exit/Esc | Confirm Dialogが1つ開く |
| TitleのConfirm No/Esc | MainTitleへ戻りExitしない |
| GameのEsc | Pause Menuが開きGamePaused=true |
| PauseのEsc/Resume | Pause Menuを閉じGamePaused=false |
| PauseのReturn to Title | Confirmを表示 |
| PauseのConfirm No/Esc | Confirmだけ閉じPauseを維持 |
| Return to Title Yes | Pause解除 → MainTitle |
| PauseのExit Game Yes | Pause解除 → QuitGame |
| Confirm表示中Esc | 下位のPauseやGameまで一度に戻らない |
| Title/Game/Ending | Focusableなボタンにフォーカスされる |
| GameInstance初期データ | GameState=Opening、CurrentChapter=0、各Structの初期値が一致 |
| Windows Packaging | 4 Map/6 WBPが含まれ同じ挙動 |

検証時にUEバージョン、日時、実測結果と不具合を追記すること。