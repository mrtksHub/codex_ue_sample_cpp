# codex_ue_sample_cpp

[codex_ue_sample](https://github.com/mrtksHub/codex_ue_sample)を参考にした、Unreal Engine 5.8の独立したC++サンプルです。
**ゲーム管理はC++、画面の作り込みはWidget Blueprint**で行います。

画面構成: **Opening → MainTitle → Game → Ending → MainTitle**

参照元はEndingからOpeningへ戻ります。本プロジェクトはタイトルへ戻る仕様です。

## 現在の状態

**C++基盤とEditor導入手順を作成した段階です。まだ完成したUE実行サンプルではありません。**

- C++ソース、プロジェクト定義、設定を登録しています。
- `.uasset` / `.umap` はまだ含まれていません。Editorで4 Mapと6 Widget Blueprintを作成してください。
- この作業環境にはUnreal Engine / Unreal Editorがないため、UHT / C++ビルド・PIE・Standalone・パッケージ確認は未実施です。
- 汎用の仮ゲーム画面を対象とします。テトリスなどの個別ゲーム処理はありません。

## 担当の分離

| 領域 | 担当 |
| --- | --- |
| 現在画面、遷移の許可、連打防止、共通データ | `USampleGameInstance` |
| 画面とWidgetクラスの対応、Pawnなしの設定 | `ASampleGameMode`と4つのC++子クラス |
| 入力、UIの生成・破棄、Pause、終了確認、Focus | `ASamplePlayerController` |
| ボタンの自動接続、データ変更の通知、キーの消費 | `USampleScreenWidget` |
| 背景、文字、画像、ボタン配置、アニメーション | 6つのWidget Blueprint |

元のBP_GameInstanceやControllerの管理グラフはコピーしません。
Widgetのボタンは名前によってC++から自動接続されるため、通常はOnClickedグラフも不要です。

## 最初に行うこと

1. このリポジトリをWindowsのUE 5.8環境へcloneする。
2. Visual StudioのUE用C++開発環境とWindows SDKを用意する。
3. `.uproject`からプロジェクトファイルを生成し、`codex_ue_sample_cppEditor`をDevelopment Editor / Win64でビルドする。
4. [Editor導入手順](docs/EDITOR_SETUP.md)に従いMapとWidget Blueprintを作成する。
5. [動作確認項目](docs/VALIDATION.md)に従ってStandaloneで確認する。

詳細な責務とBlueprint接続方法: [設計](docs/ARCHITECTURE.md)

## 主な操作

- Opening: キーまたは画面クリック → MainTitle
- MainTitle: Start → Game、Exit Game / Esc → 終了確認
- Game: Game Clear → Ending、Esc → Pause Menu
- Pause Menu: Resume、Return to Title、Exit Game
- Confirm Dialog: Yesで実行、No / Escでキャンセル
- Ending: Finish → MainTitle

## License

MIT License。参照元の著作権表記を[LICENSE](LICENSE)に保持しています。
Unreal EngineおよびEpic Gamesや第三者が提供するアセットには、それぞれの利用条件が適用されます。
