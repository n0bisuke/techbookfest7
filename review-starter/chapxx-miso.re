
= TypeScript × VSCode × Clova開発

== はじめに


本章では昨今波に乗りまくっているTypeScriptでClovaスキルを超効率爆速開発できるようになるため、JavaScriptで書かれたClovaスキルのソースをTypeScriptに移植しながらTypeScriptを学習していきます。@<br>{}
ngrokでローカルにサーバーをたてテスト画面から動作確認をするため、クラウド環境や実機は必要ありません。  


=== 本章の対象者


本章の対象者は以下のような方です。  

 * TypeScriptが気になってるけどまだ触ったことない
 * tsconfig開いてそっ閉じしてしまった
 * Clovaスキル開発をもっと効率的に行いたい



またClovaスキル開発経験者を前提としているため、未経験の方はまずTODO:Mr.MO氏の章を読んでみてください。  


=== 本章の必要環境


以下の環境が必要となります。  

 * Visual Studio Code
 * Git
 * Node.js v10以上


== Step1. TypeScriptについて


まずはTypeScriptについて特徴、メリットについて説明していきます。  


=== TypeScriptとは


TypeScriptの特徴をあげると以下のような特徴があります。  

 * Microsoftが開発したオープンソースのプログラミング言語
 * 省略可能な静的型付け
 * クラスベースのオブジェクト指向
 * JavaScriptと互換性を持つスーパーセット
 * TypeScriptのソースからJavaScriptのソースを生成する


=== TypeScriptのメリット


ではTypeScriptを使うとどんなメリットがあるかというと主に以下のようなメリットがあります。  

 * 型
 * 入力補完
 * 最新のECMAScript構文を使える
 * JavaScript完全互換



これらについて詳細を説明していきます。


=== 型

==== 古えの型


CやJavaのような古えの静的型付け言語では基本的にはメモリを確保するために型が使われていました。@<br>{}
例えばJavaでは数値型だけでもたくさんあります。  

 * byte (8bit)
 * short (16bit)
 * int (32bit)
 * long (64bit)
 * float (32bit浮動小数型)
 * double (64bit浮動小数型)


==== ナウい型


対してJavaScriptやTypeScriptの型はナウでヤングな型になっています。@<br>{}
数値ならnumberのみ、文字列ならstringのみであり、型のメモリへの影響はありません。  



つまり、古えの型はコンピューターのために存在していたものであり、ナウい型はソースを書く人間のために存在しているのです。  


==== JavaScriptの型


JavaScriptでは表面的には型はありませんが、内部的には型が存在しています。@<br>{}
以下のような@<tt>{typeof xxx}で返ってくる値が内部的な型です。  

 * プリミティブ型
 ** boolean (真偽値)
 ** string (文字列)
 ** number (数値)
 ** undefined
 ** null
 ** ]
 *  *
 * オブジェクト型
 ** object (オブジェクト、配列)
 ** Function (関数)


==== TypeScriptの型


TypeScriptの型はJavaScriptの型に加え、なんでも入るany、unknownの他、リテラル型、タプル型、ユニオン型と様々な型があります。@<br>{}
それらを組み合わせたりクラス作ったりと自由自在に柔軟に型を定義することができます。  



//image[type][TypeScriptの様々な型]{
//}



==== 型の関係


型の固さ関係を簡単に図にすると以下のようになります。@<br>{}
色が濃くなるほど固く（厳格に）なっていきます。  



//image[typelist][型の関係 ※簡略化のため一部の型のみ抜粋しています]{
//}



==== anyとunknown

===== any


どんな型でも入れられる型（JavaScirptと同じ状態）です。@<br>{}
ただし型チェックや入力補完といった恩恵が得られなくなってしまいます。@<br>{}
これらの恩恵がなくてもいいから、とりあえず書き進めたい時などに使います。  


===== unknown


anyでは危なっかしいという事でver3.0で追加された型です。@<br>{}
any同様どんな型でも入りますが、使う時は何の型か分かる状況でないと使えません。@<br>{}
何の型か分かる状況というのは下図のように型アサーションで型を明示するか、ifとtypeofによりなんの型か分かりきっている状況です。@<br>{}
安全に書き進めたいならanyではなくunknownを使用すると良いでしょう。  



//image[any-unknown][unknown型の例]{
//}



==== 代入警告


型への誤った代入時に警告してくれる機能です。@<br>{}
これによりコード書いてる時に気づけ、不毛なデバッグを回避できます。  



//image[type-alert][代入警告の例]{
//}



==== 型推論


分かりきってる型を勝手に推論してくれる機能です。@<br>{}
下図では変数numに対して型定義していませんが、代入した値が数値型であるため、TypeScriptがnumは数値型だと推論してくれています。@<br>{}
ifとtypeofにより型が絞られた状態でも同様に型推論を行ってくれます。  



//image[type-inference][型推論の例]{
//}



=== 入力補完


型が分かっているオブジェクトのメソッドやプロパティを補完してくれる機能です。@<br>{}
情報がツールチップで見れるのでドキュメントを確認せずに済みます。@<br>{}
ライブラリや誰かが作ったAPIでも引数や戻り値が分かります。  



//image[completion][入力補完の例]{
//}



=== 最新のECMAScript構文を使える


最新ECMAScriptで書いたTypeScriptのソースから任意のバージョンのJavaScriptのソースへトランスパイルされるので、FaaS等デプロイ先のNode環境が古くても最新の構文が使えます。@<br>{}
ただしサポートされるのはあくまで構文です。@<br>{}
追加メソッドも使いたい場合は別途ポリフィルを入れる必要があります。  


=== JavaScript完全互換


TypeScriptはJavaScriptを完全に含みます。@<br>{}
つまり@<tt>{tsconfig.json}でstrictをオフにしてしまえば拡張子を.tsに変えるだけで動きます。  


=== 読んでおいたほうがいいもの


公式ドキュメントとTypeScript Deep Diveは目を通しておくことをおすすめします。  



公式ドキュメント: http://www.typescriptlang.org/docs/tutorial.html@<br>{}
日本語訳: http://js.studio-kingdom.com/typescript/  



TypeScript Deep Dive: https://typescript-jp.gitbook.io/deep-dive/  


== Step2. JavaScriptサンプルスキルの構築


この節ではTypeScriptへの移植元となるJavaScriptのスキルを構築します。@<br>{}
ngrokでローカルにサーバーをたて、テスト画面で動作確認します。  


=== サンプルスキル


Clova Developer Centerのチュートリアルにサンプルダイスというスキルがあります。@<br>{}
このスキルのNode.js SDK対応版をGitHubリポジトリに公開しています。@<br>{}
これを@<tt>{git clone}しローカルに構築します。  


//cmd[]{
git clone https://github.com/miso-develop/typescript-clova-handson
//}

=== スキル作成


Clova Developer Center(https://clova-developers.line.biz/cek/#/list)を開き、スキルを作成します。@<br>{}
なお作成したスキルはテスト画面からしか起動しないため、スキル名や呼び出し名はなんでもいいです。  


=== 対話モデル構築


ビルトインスロットタイプから@<tt>{CLOVA.NUMBER}を有効化します。@<br>{}
続いてカスタムインテントを@<tt>{ThrowDiceIntent}という名前で作成し、@<tt>{git clone}したディレクトリにある@<tt>{ThrowDiceIntent.tsv}をアップロードして保存します。@<br>{}
最後にビルドボタンをクリックします。  


=== VSCodeの起動


cloneしたリポジトリにある@<tt>{step.code-workspace}をVSCodeでを開きます。  


=== ngrokのインストールと実行


ngrokとはローカルで実行したプログラムを外部からhttpアクセスするためのトンネリングアプリケーションです。@<br>{}
一般的にスマートスピーカースキルではAWS LambdaといったFaaSにプログラムをデプロイしますが、ngrokを使うことでデプロイの手間を省き開発を迅速に進めることができます。@<br>{}
VSCodeでターミナルを起動したのち以下のコマンドを実行し、ngorkのインストール、実行を行います。@<br>{}
なお、ngrokは常に起動し続けている必要があるのでターミナルを落とさないよう気をつけてください。  


//cmd[]{
npm i -g ngrok
ngrok http 3000
//}

=== ngrokドメインのコピー


@<tt>{Forwarding}のhttpsドメインのURLをコピーします。@<br>{}
※httpドメインのURLではありません  


//cmd[]{
ngrok by @inconshreveable                                       (Ctrl+C to quit)

Session Status                online
Account                       Miso Tanaka (Plan: Free)
Version                       2.2.8
Region                        United States (us)
Web Interface                 http://127.0.0.1:4040
Forwarding                    http://xxxxxxxx.ngrok.io -> localhost:3000
Forwarding                    https://xxxxxxxx.ngrok.io -> localhost:3000
//}

=== エンドポイント設定


ブラウザに戻り開発設定画面を開きます。@<br>{}
ExtensionサーバーのURLにngrokのURLを入力し、末尾に@<tt>{/sample-dice}を追記します。@<br>{}
最後に保存ボタンをクリックします。  


=== npmパッケージインストールとNode実行


CEK SDKやexpressといったスキルに必要なモジュールのインストールとプログラムの実行を行います。@<br>{}
VSCodeで新しいターミナルを開き以下のコマンドを実行します。  


//cmd[]{
npm i
node index.js
//}


ターミナルに@<tt>{start!}と出たら正常に実行できています。  


=== テスト実行


続いてスキルのテストを行いましょう。@<br>{}
ブラウザに戻ってテスト画面を開きます。@<br>{}
「対話モデルテスト」をクリックしてシナリオテストに切り替え、「〇〇を起動して」ボタンをクリックしてください。@<br>{}
スキルが起動したら適当な数字を入力して「送信」ボタンをクリックします。@<br>{}
下図のように結果が返ってくればOKです。  



//image[test][スキルのテスト]{
//}



=== ソースについて


ソースについて少し解説します。@<br>{}
VSCodeで@<tt>{index.js}を開いてみてください。@<br>{}
以下のような構成になっています。  

 * 定数、ロジック、ハンドラーでブロックを分けている
 * @<tt>{Guide}や@<tt>{Cancel}といったプリセットのインテントは基本@<tt>{Fallback}に投げている
 * まともな実装は@<tt>{throwDiceHandler}ぐらい
 ** Clovaに伝えた数字分サイコロを降って、結果を話す
 ** 発話は@<tt>{setSpeechList}で、サイコロの数、サイコロを振る音、結果の合計値を順に話していく
 ** このあたりの内部処理はLogic部の3つの関数で処理している


== Step3. TypeScript環境構築


この節ではTypeScriptの環境を構築していきます。@<br>{}
ディレクトリ構成や関連パッケージ、@<tt>{tsconfig.json}について説明していきます。  


=== ディレクトリ作成 & ソース移動（リネーム）


@<tt>{src/}にTypeScriptのソース（@<tt>{.ts}）を格納し、@<tt>{dist/}にコンパイルされたJavaScriptのソースを出力するようにします。@<br>{}
以下のコマンドを実行し、ディレクトリ作成とファイルリネームを行います。@<br>{}
※@<tt>{mv}時に拡張子を@<tt>{js}から@<tt>{ts}に変える  


//cmd[]{
mkdir src dist
mv index.js src/index.ts
//}

=== typescript & typesyncインストール


以下のコマンドを実行し、TypeScript関連パッケージをインストールします。  


//cmd[]{
npm i -g typescript typesync
//}

=== 型定義インストール


@<tt>{package.json}を開いたうえで以下のコマンドを実行してみてください。  


//cmd[]{
typesync
npm i
//}


@<tt>{package.json}の@<tt>{devDependencies}に@<tt>{@types/...}というパッケージが追加されます。  



//image[at-types][typesync実行後のpackage.json]{
//}



=== 型定義と@<tt>{@types}について

==== 型定義とは


世の既存ライブラリのほとんどはJavaScriptで作られています。@<br>{}
つまりそれらは型をもちません。@<br>{}
なのでせっかく型チェックや入力補完あるのに使えません。@<br>{}
そこでライブラリに対応した型定義だけを用意して読み込めば、ライブラリ自体のソースにも影響ないよね、ということで作られたのが型定義ファイルです。@<br>{}
@<tt>{xxx.d.ts}というファイル名のものです。  


==== @<tt>{@types}とは


昔は@<tt>{tsd}という巨大型定義リポジトリを落としたり、@<tt>{typings}といった型定義専用パッケージマネージャみたいなものがありました。@<br>{}
しかし、今は@<tt>{@types}というnpmパッケージ群から@<tt>{npm i -D @types/xxx}という感じで簡単に型定義をインストールできるようになりました。@<br>{}
使ってるライブラリ自体に型定義がなかったらひとまず@<tt>{npm i -D @types/xxx}としてみてください。  


=== tsconfigの作成


以下のコマンドでTypeScriptの初期化を行い@<tt>{tsconfig.json}を作成します。@<br>{}
@<tt>{tsconfig.json}が作成されたらVSCodeで開いてみてください。  


//cmd[]{
tsc --init
//}

=== tsconfigの編集


@<tt>{tsconfig}を少し編集しディレクトリ構成を設定します。@<br>{}
以下の部分以外はデフォルト値のままで大丈夫です。  


==== アウトプットディレクトリの指定

//emlist[tsconfig.json（15行目あたり）]{
- //"outDir": "./",
+ "outDir": "./dist",
//}

==== インプットディレクトリの指定

//emlist[tsconfig.json（62行目あたり）]{
- }
+ },
+ "include": ["./src/**/*"]
//}

=== tsconfigについて


tsconfigのパラメーターについてスキル開発にあたり主だった部分を抜粋して説明します。  


==== Basic Options
 * target: コンパイル先ESバージョンを指定
 * module: モジュール管理方法を指定
 * lib: ECMAScriptの上位バージョンだったりDOMだったり標準APIを有効化
 * sourceMap: Source Mapファイルを生成するか
 ** Source Mapファイルとは
 *** コンパイルされたJavaScriptのどの行がTypeScriptのどの行にあたるかをデバッガーに伝えるためのファイルのこと
 *** @<tt>{xxx.js.map}みたいな名前で出力される
 * outDir: コンパイルされたjsの出力先ディレクトリを指定


==== Strict Type-Checking Options
 * strict: 以下の全てのチェックを有効化する
 * noImplicitAny: 暗黙のanyを許可しない
 * strictNullChecks: 厳密なnullチェック
 * strictFunctionTypes: 関数の引数の厳密に型チェック
 * strictBindCallApply: bind, call, applyで厳密に型チェック
 * strictPropertyInitialization: クラスのプロパティ初期化をチェック
 * noImplicitThis: thisの型を指定しないとエラー
 * alwaysStrict: "use strict"を有効化


==== Additional Checks
 * noUnusedLocals: 未使用のローカル変数があるとエラー
 * noUnusedParameters: 未使用のパラメーターがあるとエラー
 * noImplicitReturns: 関数内に通らないコードがあるとエラー
 * noFallthroughCasesInSwitch: switch文でbreakがないとエラー


=== index.ts確認


ここで@<tt>{index.ts}を開いてみましょう。@<br>{}
VSCodeで問題パネルを開いてみてください。@<br>{}
下図のようにいくつかエラーが出ています。@<br>{}
これらは@<tt>{tsconfig.json}が作成されたことによりTypeScriptのチェックがVSCode上で動き検出されたエラーです。  



//image[error][TypeScriptエラー]{
//}



=== tsconfig.jsonのstrictを無効化


@<tt>{tsconfig.json}にてstrictをコメントアウトしてみましょう。  


//emlist[tsconfig.json（26行目あたり）]{
- "strict": true,
+ // "strict": true,
//}


再度@<tt>{index.ts}のエラーをチェックしてみてください。@<br>{}
すべてのエラーが消えてることでしょう。@<br>{}
strictをオフにしたことによりすべての型チェックが無効化され、JavaScriptとほぼ同様の状態となりました。  


=== コンパイル


以下のコマンドを実行し、コンパイルを行います。  


//cmd[]{
tsc
//}


@<tt>{dist/}配下に@<tt>{index.js}が生成されているので開いてみましょう。@<br>{}
今回@<tt>{tsconfig.json}のtargetにてes5が指定された状態でコンパイルされているため、ES5形式のJavaScriptソースが出力されています。  


=== Node実行 & テスト


この状態でプログラムを実行してみましょう。  


//cmd[]{
node dist/index.js
//}


ブラウザに戻り@<tt>{テスト画面}からスキルをテストしてみてください。@<br>{}
JavaScriptStep2のときと同じ動きになります。  



このようにJavaScriptのソースをそのまま持ってきても、@<tt>{tsconfig.json}のstrictを無効化しておけばコンパイル実行してソースを動かすことができます。@<br>{}
この状態でもJavaScriptよりは若干チェックが厳しいのですが、TypeScriptのうまみが少ない状態です。  



以下のコードを.ts、.js両方のファイルに書いてみてエラーの出具合を比較してみてください。  


//emlist{
"aaa"++ // JavaScriptだったらNaNで通るけど、TypeScriptだとエラー
//}

== Step4. JavaScript -> TypeScript移植


この節ではJavaScriptのソースを実際にTypeScriptへ書き換えつつ、TypeScriptのメリットを体感していきます。  


=== requireをimportに
 * @<tt>{@line/clova-cek-sdk-nodejs}


//emlist{
- const { Client, SpeechBuilder } = require("@line/clova-cek-sdk-nodejs")
+ import { Client, SpeechBuilder } from "@line/clova-cek-sdk-nodejs"
//}
 * @<tt>{express}, @<tt>{body-parser}


//emlist{
- const express = require("express")
- const bodyParser = require("body-parser")
+ import express from "express"
+ import bodyParser from "body-parser"
//}

=== @<tt>{app.post(ENDPOINT, bodyParser.json(), clovaSkillHandler)}でエラー発生
 * エラー内容確認
 ** @<tt>{RequestHandler}型にする必要があるっぽい


//emlist{
Type 'Function' is missing the following properties from type '(RequestHandler | ErrorRequestHandler)[]'
//}
 * @<tt>{RequestHandler}がどこにあるか探す
 ** @<tt>{const app = express()}の@<tt>{express}を@<tt>{ctrl + クリック}
 *** @<tt>{express}の型定義が開くので@<tt>{RequestHandler}を検索してみる


=== ソースを修正
 * @<tt>{clovaSkillHandler}に型アサーションをつける


//emlist{
- app.post(ENDPOINT, bodyParser.json(), clovaSkillHandler)
+ app.post(ENDPOINT, bodyParser.json(), clovaSkillHandler as express.RequestHandler)
//}
 * SDKが@<tt>{handle()}メソッドで@<tt>{Function}型を返してるのも良くない
 ** @<href>{https://github.com/line/clova-cek-sdk-nodejs/blob/master/src/client.ts#L77,GitHubの最新ソース}を見ると@<tt>{(req: any, res: any) => void}という型だと分かる
 * jsで動いてた=問題ない事がわかりきってるのであれば@<tt>{as any}で解決もあり


=== 問題パネルからエラーを確認
 * 問題パネル開く
 ** Win: @<tt>{ctrl + shift + m}
 ** Mac: @<tt>{⌘ + ⇧ + m}
 * 問題は関数の引数の暗黙的any
 ** diceCount
 ** throwResult
 ** responseHelper


=== @<tt>{diceCount}の型


変数名から分かる通り数値型。
なので@<tt>{number}型でアノテーションをつける。


//emlist{
// 26行目
- const getThrowResultMessage = (diceCount) => {
+ const getThrowResultMessage = (diceCount: number) => {

...

// 35行目
- const throwDice = (diceCount) => {
+ const throwDice = (diceCount: number) => {
//}

=== @<tt>{responseHelper}の型


よくわかんないから@<tt>{any}、、、としておきたいけど、これよく使うのでちゃんと型を定義しておきたい


=== 型定義から読み解いてみる


@<tt>{onLaunchRequest}までたどり、関数名にマウスオーバーして表示されるツールチップを見てみる

 * カーソルあてて@<tt>{ctrl (⌘) + k -> ctrl (⌘) + i}でもツールチップ開けます



//image[tooltip][image]{
//}


 * また@<tt>{Function}…


=== 正解の型
 * @<href>{https://github.com/line/clova-cek-sdk-nodejs/blob/master/src/client.ts#L36,GitHubの最新ソース}をみると@<tt>{(ctx: Context) => void}
 ** ここの@<tt>{ctx}が@<tt>{responseHelper}に相当する
 ** なので正解の型は@<tt>{Context}


=== @<tt>{Context}アノテーションをつける


@<tt>{responseHelper}に@<tt>{Context}アノテーションをつける


//emlist{
- const launchHandler = async (responseHelper) => {
+ const launchHandler = async (responseHelper: Context) => {
//}


@<tt>{Context}型が無いよと怒られる…



//image[context-error][image]{
//}



=== @<tt>{Context}型を探す
 * SDKを開いてみる
 ** 1行目のimport文から@<tt>{@line/clova-cek-sdk-nodejs}を@<tt>{ctrl + クリック}
 *** SDKの型定義（@<tt>{index.d.ts}）が開かれる



//image[sdk-d-ts][image]{
//}



=== @<tt>{Context}のインポート

//emlist{
- import { Client, SpeechBuilder } from "@line/clova-cek-sdk-nodejs"
+ import { Client, Context, SpeechBuilder } from "@line/clova-cek-sdk-nodejs"
//}

=== 入力補完を試してみる
 * launchHandler（70行目らへん）の関数内で@<tt>{responseHelper.}と入力しサジェスチョンを表示
 ** Win: @<tt>{ctrl + space} / Mac: @<tt>{⌘ + space}
 * @<tt>{setSimpleSpeech}を選択
 ** 選択後、@<tt>{()}の入力を忘れずに



//image[suggestion][image]{
//}



=== ついで@<tt>{SpeechBuilder}でも試してみる
 * @<tt>{SpeechBuilder.}と入力しサジェスチョンを表示
 ** @<tt>{createSpeechText}を選択
 *** 選択後、@<tt>{()}の入力を忘れずに
 * @<tt>{createSpeechText}にカーソルを合わせツールチップを表示
 ** Win: @<tt>{ctrl + k -> ctrl + i}
 ** Mac: @<tt>{⌘ + k -> ⌘ + i}
 * ツールチップに表示される引数を確認し実装
 ** 第2引数の@<tt>{lang?}の@<tt>{?}はオプショナル（省略可）をあらわす


=== 全ての@<tt>{responseHelper}の修正
 * VSCodeのマルチカーソルを使うとラク
 ** エラーの出てる@<tt>{responseHelper}をダブルクリック
 ** @<tt>{shift (⇧) + end}で行末尾まで選択
 ** @<tt>{ctrl (⌘) + d}で修正対象を選択していく
 ** 一括修正！
 ** @<tt>{esc}でマルチカーソル解除


=== 今度は83行目の@<tt>{const throwResultMessage = getThrowResultMessage(diceCount)}でエラー
 * @<tt>{getThrowResultMessage}の引数の型は@<tt>{number}だから@<tt>{string}は入らないよ！というエラー
 * @<tt>{const diceCount = responseHelper.getSlots().number || 1}を見てみる
 ** 一見@<tt>{number}が返ってきそうだけど…


=== @<tt>{responseHelper.getSlots()}からは@<tt>{string | null}が返ってくる！
 * 数字を言って伝えたつもりでも、Clovaに届くのはJSONデータ
 ** パースされると@<tt>{1}（数値型）ではなく@<tt>{"1"}（文字列型）になる
 *** わりとハマりやすいポイント



//image[slot][image]{
//}



=== TypeScriptの大きなメリットである型のおかげで安全！
 * JavaScriptだとエラーが発生しない
 ** 気付かずハマって半日消費していたかもしれない…！
 * TypeScriptなら型チェックにより、こうやって未然にエラー検出してくれる！


=== 型エラー対処


数値型として型アサーションをつける


//emlist{
- const diceCount = responseHelper.getSlots().number || 1
+ const diceCount = (responseHelper.getSlots().number || 1) as number
//}

=== @<tt>{throwResult}の型
 * ソースを追っかけてみる
 ** @<tt>{throwDice}の戻り値と一緒
 *** 関数名のツールチップを見てみると…


=== 型を切り出して宣言してみる
 * 正解の型は@<tt>{{midText: string, sum: number, diceCount: number\}}
 ** このまま型アノテーションつけられなくもないけど、見にくい…
 *** 専用の型を宣言しちゃおう！


//emlist{
+ type ThrowResult = {midText: string, sum: number, diceCount: number}
//}
 * 型名はパスカルケース（先頭大文字）


=== @<tt>{ThrowResult}型アノテーションをつける


@<tt>{resultText()}の引数


//emlist{
- const resultText = (throwResult) => {
+ const resultText = (throwResult: ThrowResult) => {
//}


@<tt>{throwDice()}の戻り値にもつけてみる


//emlist{
- const throwDice = (diceCount: number) => {
+ const throwDice = (diceCount: number): ThrowResult => {
//}
 * この状態だと@<tt>{return}のオブジェクトの構成を変えるとエラー


=== @<tt>{interface}でも型宣言できる

//emlist{
- type ThrowResult = {midText: string, sum: number, diceCount: number}
+ interface ThrowResult {midText: string, sum: number, diceCount: number}
//}
 * どっちを使えばいいの？
 ** 7/10のTypeScript Meetup2でも話題になってた
 *** @<strong>{typeのほうが多機能なのでこっち使っとくのが無難}
 *** @<tt>{interface}はそれでしか使えない機能を使うときだけ
 *** @<tt>{typescript-eslint}は@<tt>{interface}を推してくる
 **** オフろう -> @<href>{https://github.com/typescript-eslint/typescript-eslint/blob/master/packages/eslint-plugin/docs/rules/prefer-interface.md,interface-over-type-literal}


=== コンパイル & Node実行 & テスト
 * コンパイル & Node実行


//cmd[]{
tsc
node dist/index.js
//}
 * テスト
 ** ブラウザに戻り@<tt>{テスト画面}から同様にチェック


=== 【演習】ガイドインテントを実際に実装してみよう


fallbackに飛ばされている実装を@<tt>{GUIDE_MESSAGE}を返すよう修正してみてください
（@<tt>{responseHelper}、@<tt>{SpeechBuilder}の入力補完を活かして！）


//emlist{
/**
 * ガイドインテント
 */
const guideHandler = async (responseHelper: Context) => {
    fallbackHandler(responseHelper)
}
//}

== Step5. 関連ツールについて(lint, tsc -w, nodemon, debug)

=== lint

==== typescript-eslint vs tslint
 * 去年までは@<tt>{tslint}が主流だった
 * だけど今年1月にTypeScriptチームがESLintチームと協力して@<tt>{typescript-eslint}を開発していくと宣言
 * @<strong>{2019年中にtslintは非推奨になるのでtypescript-eslintを使っていくのが正解}


==== typescript-eslintの導入
 * インストール
 ** ESLint本体に加え、TypeScript用プラグインとパーサーが必要


//cmd[]{
npm i -g eslint@5.16.0 @typescript-eslint/eslint-plugin @typescript-eslint/parser
//}
 * VSCodeでESLint拡張機能インストール
 ** 拡張機能パネルを開いて@<tt>{eslint}で検索して有効化
 *** Win: @<tt>{ctrl + shift + x} / Mac: @<tt>{⌘ + ⇧ + x}



//image[vscode-eslint][image]{
//}



==== eslintrc生成
 * @<tt>{init}コマンドを打って以下の設定で進める
 ** @<tt>{Where does your code run?}では@<tt>{Browser}のチェックを外す


//cmd[]{
$ eslint --init

? How would you like to use ESLint? To check syntax, find problems, and enforce code style
? What type of modules does your project use? JavaScript modules (import/export)
? Which framework does your project use? None of these
? Where does your code run? Node
? How would you like to define a style for your project? Answer questions about your style
? What format do you want your config file to be in? JSON
? What style of indentation do you use? Tabs
? What quotes do you use for strings? Double
? What line endings do you use? Unix
? Do you require semicolons? Yes
? What format do you want your config file to be in? JSON
//}

==== TypeScript対応
 * @<tt>{.eslintrc.json}を修正


//emlist{
-   "extends": "eslint:recommended",
+   "extends": [ "plugin:@typescript-eslint/recommended" ],
    "globals": {
        "Atomics": "readonly",
        "SharedArrayBuffer": "readonly"
    },
-   "parserOptions": {
-       "ecmaVersion": 2018,
-       "sourceType": "module"
-   },
+   "parser": "@typescript-eslint/parser",
+   "parserOptions": { "project": "./tsconfig.json" },
+   "plugins": [ "@typescript-eslint" ],
//}

==== TypeScript用ルールの指定
 * @<tt>{rules}配下に@<tt>{@typescript-eslint/...}と追記していく
 ** 注意点として本家ESlintと一部重複するルールがあり、それを使う場合本家ESLint側でルールを無効化する必要がある


//emlist{
"rules": {
    "no-unused-vars": "off", // 本家ESLint側では無効化
    "@typescript-eslint/no-unused-vars": "error"
}
//}
 * @<href>{https://github.com/typescript-eslint/typescript-eslint/tree/master/packages/eslint-plugin#supported-rules,@typescript-eslintルール一覧}
 * @<href>{https://github.com/typescript-eslint/typescript-eslint/blob/master/packages/eslint-plugin/ROADMAP.md,TSLintとの対照表}
 ** TSLintから移行するときはここを参照


=== tsc -w

==== tsc -w
 * @<tt>{-w | --watch}
 ** ソースを監視して変更があれば自動でコンパイル実行するオプション
 ** @<tt>{tsc}単体で打つと若干時間かかるけど@<tt>{-w}だと起動以降はコンパイルちょっぱや


//cmd[]{
tsc -w
//}

=== nodemon

==== nodemon
 * @<href>{https://nodemon.io/,nodemon}とは？
 ** @<strong>{ソース変更を検知してプロセスを自動で再起動してくれるツール}
 * インストール & 実行
 ** 一度実行しておけばソースが変更される度に勝手に再実行してくれる
 ** @<tt>{-w}オプションで特定のディレクトリのみ監視できる


//cmd[]{
npm i -g nodemon
nodemon -w dist dist/index.js
//}

==== 開発tips
 * ローカル開発するときは@<tt>{ngrok http 3000}, @<tt>{tsc -w}, @<tt>{nodemon -w dist dist/index.js}を同時に動かしておくとすごくラク
 ** ソース変更 -> 勝手にコンパイル -> 勝手にnode再起動 -> しあわせ
 * 私は一発起動バッチ使ってます（Windows）


//emlist[bat]{
@echo off
start ngrok http 3000
start tsc -w
start nodemon -w dist dist/index.js
//}

=== VSCodeでのデバッグ

==== TypeScriptのSource Map有効化
 * @<tt>{tsconfig.json}にてSource Mapを有効にする


//emlist{
- // "sourceMap": true,                     /* Generates corresponding '.map' file. */
+ "sourceMap": true,                     /* Generates corresponding '.map' file. */
//}
 * コンパイルして@<tt>{index.js.map}が生成されることを確認


//cmd[]{
tsc
ls dist/index.js.map
//}

==== デバッグ設定の初期化
 * デバッグパネルを開く
 ** Win: @<tt>{ctrl + shift + d}
 ** Mac: @<tt>{⌘ + ⇧ + d}
 * 歯車アイコンを押す
 ** コマンドパレットが開くので@<tt>{Node.js}を選択



//image[debug-init][image]{
//}


 * @<tt>{.vscode/launch.json}が作成され開かれる


==== @<tt>{nodemon}に対応するよう@<tt>{launch.json}を修正

//emlist{
    "configurations": [
        {
            "type": "node",
-           "request": "launch",
-           "name": "Launch Program",
-           "program": "${file}"
+           "request": "attach",
+           "name": "Node: Nodemon",
+           "processId": "${command:PickProcess}",
+           "restart": true,
+           "protocol": "inspector"
        }
    ]
//}

==== ブレークポイントを置く
 * ブレークポイントとは
 ** ソースがその行を通る時に処理を一時停止できる
 ** 止まった時点の変数の状態とかを確認できる
 *** @<tt>{console.log}地獄から卒業
 ** 条件付きブレークポイントやログポイントも置ける
 * 試しに@<tt>{launchHandler}の最初の行（71行目）に置いてみる
 ** 行数の左側をマウスでポチ（もしくは@<tt>{F9}）



//image[breakpoint][image]{
//}



==== デバッグの実行
 * @<tt>{--inspect}オプションをつけてnodemon実行


//cmd[]{
nodemon --inspect dist/index.js
//}
 * VSCodeで@<tt>{F5}押してデバッグモード実行
 ** コマンドパレットが開かれるので@<tt>{inspect}と打って一番上の項目を選択する



//image[vscode-debug-start][image]{
//}



==== スキル実行
 * ブラウザの@<tt>{テスト画面}にてスキル起動
 * VSCodeにてブレークポイントで停止してその時の変数状態がチェックできることを確認
 ** 再度@<tt>{F5}を押すと処理が進む



//image[vscode-debug-run][image]{
//}



==== デバッグの注意点
 * @<tt>{テスト画面}だとレスポンスがしばらく返ってこないとタイムアウトしちゃう
 ** 最終的なレスポンスのJSONがチェックできない＞＜
 * 中村さんが作ったシミュレーターを使うとタイムアウトせずに確認できる！
 ** Qiita: @<href>{https://qiita.com/kenakamu/items/cdc9eacaec8f4400019f,CEK開発用Clovaシミュレーターで開発スピードを上げる}
 ** GitHub: @<href>{https://github.com/kenakamu/clova-simulator,Clova CEK シミュレーター}


== さいごに


xxx

