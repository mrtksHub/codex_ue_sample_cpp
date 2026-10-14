# codex_ue_sample_cpp

Unreal Engine 5.8 の [codex_ue_sample](https://github.com/mrtksHub/codex_ue_sample) を **設計変更せずにC++へ移す**ためのプロジェクトです。
管理処理はC++、Widgetの外観・ボタンイベントはWidget Blueprintに残します。

## 参照元と同じ構成

- 画面: **Opening → MainTitle → Game → Ending → Opening**
- 4 Map、4 GameMode、4 PlayerController、1 GameInstance、6 Widget Blueprint。
- Controllerは共通化せず、4画面に対応するC++クラスを各々 APlayerController から直接派生させます。
- 元のBlueprint版と同じく、GameModeが画面固有のControllerを選択します。
- `GoToScreen`は遷移中フラグのみを確認し、遷移先は4画面のEnumから選びます。
- 元のPause、Confirm、Esc優先順位、Focus、カーソル表示を維持します。
- `CurrentChapter` / `PlayerData` / `Settings` / `SaveData` は元のBP_GameInstanceと同じメモリ上のデータです。

## C++とBlueprintの境界

| 処理 | C++ | Blueprint |
| --- | --- | --- |
| GameInstance画面状態・GoToScreen・NotifyScreenReady | USampleGameInstance | プロパティ参照と関数呼び出し |
| GameModeのController選択 | ASampleGameModeBaseと4子クラス | なし |
| BeginPlay、Widget生成、カーソルとFocus | 4つのController | Widgetのデザイン |
| Title/Game/Endingボタン | 呼び先のC++関数 | WBP内のOnClickedグラフ |
| Confirm結果のディスパッチ | USampleConfirmDialogWidgetで型を定義 | WBP_ConfirmDialogでYes/Noの発火 |
| Pause・Confirm・Esc | ATitleController / AGameController | PauseMenuのボタンから呼び出し |

## 現在の状態

**GitHub上のC++ソースを更新した段階で、動作確認は未実施です。**

- このリポジトリにはまだ `.uasset` / `.umap` がありません。
- Editorで4つのMapと6つのWBPを作るか、元のDesignerを移す必要があります。
- UE 5.8 UHT/C++ビルド、PIE、Standalone、Packagingは未確認です。
- 元のBlueprintの詳細なノード接続は設計書を基準に移植しました。UE Editor内で最終照合してください。

導入手順: [docs/EDITOR_SETUP.md](docs/EDITOR_SETUP.md)

クラスと処理対応: [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)

確認項目: [docs/VALIDATION.md](docs/VALIDATION.md)

## License

MIT License。原著作権表示は [LICENSE](LICENSE) を参照してください。
Unreal Engineや第三者アセットにはそれぞれのライセンスが適用されます。