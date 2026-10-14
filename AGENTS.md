# 作業ルール

- 最初にREADME.md、docs/ARCHITECTURE.md、docs/EDITOR_SETUP.mdを読む。
- UE 5.8。C++が画面遷移・共通状態・入力・Pause・確認結果の判断を管理する。
- Widget Blueprintはレイアウト・見た目・アニメーション・管理データの表示を担当する。
- 基本ループはOpening → MainTitle → Game → Ending → MainTitle。EndingからOpeningへ戻さない。
- 元のcodex_ue_sampleは変更しない。本リポジトリは独立したプロジェクト。
- .uasset / .umapはUnreal Editorで作成・保存する。空ファイルや独自バイナリで代用しない。
- UE EditorやMCPが接続できない場合、アセット作成・ビルド・PIE確認を実施済みと報告しない。
- UIの管理データはBlueprintReadOnly。変更はC++の関数経由で行う。
- 既存Blueprint版の遷移・Pause・入力グラフを残したままC++へ二重接続しない。
- 1回のEscはConfirmを閉じる、Pauseを閉じる、Pauseを開く、のいずれか1段階のみ。
- レベル遷移の前にPause解除、失敗時は遷移ロック解除と元のUI復帰を維持する。
- Saved / Intermediate / DerivedDataCache / Binariesや生成されたIDEファイルをコミットしない。
- Publicリポジトリ。秘密情報、認証情報、環境固有のプラグイン設定を含めない。
- 変更後は実施した検証と未実施の検証を分けて記録する。
