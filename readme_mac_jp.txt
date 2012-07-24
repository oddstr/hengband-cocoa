変愚蛮怒 2.0.0 Mac OS X フロントエンドについて


このフロントエンドは vanilla Angband 3.3.0 から移植作業中のものです。
セーブファイル・ハイスコア等が破損する可能性もありますので、
ご使用は自己責任でお願いします。


1. コンパイル済アプリケーションについて

~/Documents/Hengband 以下に保存ディレクトリを勝手に作ります。
（現時点では、変更不可）
~/Library/Preferences/jp.sourceforge.hengband.Hengband.plist
にGUI関連の初期設定が保存されます。
変愚蛮怒の lib/ は Hengband.app/Contents/Resources 以下にあります。
Finder で右クリック→パッケージの内容を表示して下さい。


2. ビルド方法について

build/ 以下のファイルを変愚蛮怒の最新版ソースツリーに設置します。
src/cocoa/ （ディレクトリ全体）
src/main-cocoa.m
src/makefile.osx
lib/pref/pref-mac.prf （上書き）

ターミナルで src/ に cd し、
make -f makefile.osx
するとツリートップにビルドされます。（要 Xcode）


3. 未サポートの機能・既知の問題

・IMEによる日本語入力（ペーストは可能）
・テキストのコピー

・グラフィックタイル
・サウンド
・ムービー再生
・中継

・保存ディレクトリの変更
・小さい文字でアンチエイリアスが見づらい
・文字の間隔が不適切（特にヒラギノやDFPフォント）
・64bit モードでの動作が不安定
　（ビルド済および makefile は 32bit に制限しています）


ビルド・動作の確認は OSX Lion 10.7.4, Xcode 4.3.3 で行なっています。
メンテナの皆さん、いつもお疲れ様です。

2012/7/24
＠の溜まり場II プログラム関連スレ の445
