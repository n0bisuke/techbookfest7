
= ちょっとだけセキュアな認証システムをLINE Thingsでやってみた


LINE Thingsの自動通信機能を使って、Bluetoothの電波が届くエリアに入ったら認証コードを飛ばして、Amazon Connectから認証コードを教えてくれるシステムを作ってみました。
ペアリング済みの携帯にのみ認証コードが送られてくる仕組みなので、セキュリティもバッチリです！


== 仕組み


今回はM5StackやM5StickCを使います。LINE Thingsでペアリング済みの携帯がBluetoothエリアに入ると、自動通信機能を使いWebhook送信をします。飛ばし先はAPI Gatewayで発行されたURLです。
AWS Lambdaで実際のプログラム処理を行い、Amazon Connectに認証コードの通知がいきます。



//image[g100][今回作るアーキテクチャ]{
//}



== 準備


各種準備をこれから行っていきます。必要なものは以下の通りです。

 * Amazon Connect電話番号の発行
 * Amazon Connect問い合わせフロー作成
 * AWS Lambda関数を作成する
 * API Gatewayの設定
 * LINE Bot
 * LIFFの設定
 * LINE Things 自動通信機能サービスUUID発行
 * Vue.jsの設定
 * 外部公開をする
 * M5Stack（M5StickC）に書き込む
 * LINE Thingsのペアリング設定



それでは、順番に用意していきましょう。


=== Amazon Connect 電話番号の発行

==== リソースを作成する


AWSにアクセスし、お持ちのAWSアカウントでログインします。
@<href>{https://aws.amazon.com/jp/,https://aws.amazon.com/jp/}



コンソール画面が開いたら、サービス検索から【Amazon Connect】と入力して検索します。出てきた［Amazon Connect］をクリックします。



//image[g101][AmazonConnect]{
//}




［今すぐ始める］ボタンをクリックします。



//image[g102][今すぐ始めるをクリック]{
//}




東京リージョンになっているか確認してください。なっていない方は右上から「アジアパシフィック（東京）」を選択します。



//image[g103][アジアパシフィック（東京）を選択]{
//}




アクセスURLに今回使用するための名前を決めます。［handson-ac-{あなたのお名前}］という感じで他の人と被らないような名前にします。



//image[g104][リソース名を決める]{
//}




ステップ２の管理者の作成は後でできるので、「これをスキップ」を選択してから、［次のステップ］ボタンをクリックします。



//image[g105][これをスキップを選択し、次のステップをクリック]{
//}




ステップ３のテレフォニーオプションもそのまま何もせず［次のステップ］ボタンをクリックします。



//image[g106][次のステップをクリック]{
//}




ステップ４のデータストレージもそのまま［次のステップ］ボタンをクリックします。



//image[g107][次のステップをクリック]{
//}




ステップ５で最終確認をして問題なければ［インスタンスの作成］ボタンをクリックします。



//image[g108][インスタンスの作成をクリック]{
//}




1〜2分ほど作成に時間がかかります。



//image[g109][1〜2分待ちます]{
//}




成功すれば、［今すぐ始める］ボタンをクリックします。



//image[g110][今すぐ始めるをクリック]{
//}



==== 電話番号を取得する


Amazon Connect専用の電話番号を手に入れます。
別のタブでAmazon Connect用の画面が表示されるので、そこに表示されている［今すぐ始める］ボタンをクリックします。



日本語になっていない場合は右上から「日本語」を選択してください。



//image[g111][今すぐ始める]{
//}




電話番号の取得で、国はJapan +81を選択します。



//image[g112][+81を選択]{
//}




次のタイプは2種類あります。お好きな方を選んでください。

//table[tbl1][電話番号タイプ]{
タイプ	特徴
-----------------
Direct Dial	050番号が取得できる。発信者が通話料負担する。
Toll Free	0800番号が取得できる。発信者の通話料無料（フリーダイヤル）開発者が負担する。
//}


//image[g113][電話番号タイプを選択]{
//}




最後にお好きな番号を選択して、［次へ］ボタンをクリックします。



//image[g114][次へをクリック]{
//}




［Continue］ボタンをクリックします。これで電話番号が発行されました。



//image[g115][Continueをクリック]{
//}



==== 発行した電話番号を確認する


取得した電話番号を確認します。
左メニューからルーティングの電話番号をクリックします。



//image[g116][ルーティングの電話番号をクリック]{
//}




取得した電話番号が表示されています。別の番号を増やしたい場合は右側にある［電話番号の追加］ボタン押して追加することもできます。この電話番号は後ほど使うので、メモしておきましょう。



//image[g117][電話番号をメモしておく]{
//}



=== Amazon Connect問い合わせフロー作成する


問い合わせフローを作成していきましょう。M5Stackから発行された認証コードを言ってもらうためのフローを作ります。GUI画面でブロックを繋げるだけで簡単に構築することができます。


==== 問い合わせフローを作成する


左側メニューのルーティングから「問い合わせフロー」をクリックします。



//image[g200][問い合わせフローをクリック]{
//}




［問い合わせフローの作成］をクリックします。



//image[g201][問い合わせフローの作成をクリック]{
//}




名前を「M5-Security-AmazonConnect」と入力します。



//image[g202][名前を入力する]{
//}




設定カテゴリにある「音声の設定」ブロックをドラッグアンドドロップして、ドロップしたブロックをクリックします。



//image[g203][音声の設定ブロックをドラッグアンドドロップする]{
//}




言語は「日本語」でお好きな音声を選択してください。



//image[g204][日本語を選び音声を選択]{
//}




エントリポイントと音声の設定ブロックを繋げます。



//image[g205][音声の設定ブロックを繋げる]{
//}




設定カテゴリにある「問い合わせ属性の設定」をドラッグアンドドロップします。



//image[g206][問い合わせ属性の設定をドラッグアンドドロップ]{
//}




属性の設定を行います。「属性を使用する」を選択してください。

//table[tbl2][属性の設定]{
キー	値
-----------------
宛先キー	message
タイプ	ユーザー定義
属性	message
//}


//image[g207][属性の設定を行う]{
//}




ブロックを繋げます。



//image[g208][ブロックを繋げる]{
//}




操作カテゴリにある「プロンプトの再生」をドラッグアンドドロップします。



//image[g209][プロンプトの作成をドラッグアンドドロップ]{
//}




属性の設定を行います。「テキスト読み上げ機能(アドホック)」を選択してください。

//table[tbl3][属性の設定]{
項目	値
-----------------
テキスト読み上げ機能(アドホック)	動的に入力する
タイプ	ユーザー定義
属性	message
//}


//image[g210][属性の設定を行う]{
//}




ブロックを繋げます。



//image[g211][ブロックを繋げる]{
//}




ブランチカテゴリにある「ループ」をドラッグアンドドロップします。
ループ回数はお好きな数を指定してください。



//image[g212][ループ設定する]{
//}




ブロックを繋げます。



//image[g213][ブロックを繋げる]{
//}




ループとプロンプトの再生を繋げます。



//image[g214][ループとプロンプトのブロックを繋げる]{
//}




終了 / 転送カテゴリにある「切断/ハングアップ」をドラッグアンドドロップします。



//image[g215][切断/ハングアップをドラッグアンドドロップ]{
//}




まだ繋いでいない部分を全て「切断/ハングアップ」に繋ぎます。



//image[g216][繋いでいない部分を繋げる]{
//}




右上の①［保存］と②「公開」ボタンを順番にクリックします。



//image[g217][保存と公開をクリックする]{
//}



==== IDをメモしておく


問い合わせフローの名前の下に「追加のフロー情報の表示」という項目があるので、それを展開します。展開するとARNの情報が表示されるのでinstanceのIDとconstact-flowのIDをそれぞれメモしておきます。



//image[g218][instanceのIDとconstact-flowのID]{
//}



=== AWS Lambda関数を作成する


LINE Thingsから送られてくる認証コードを受け取って、Amazon Connectに認証コードを発話するための処理を行います。


==== Lambda関数を作成する


サービスから「Lambda」を検索して、出てきたものをクリックします。



//image[g400][Lambdaを検索する]{
//}




Lambdaから新規で関数を作成します。［関数の作成］ボタンをクリックします。



//image[g401][関数の作成をクリック]{
//}




関数は以下の通り入力して、［関数の作成］ボタンをクリックします。

//table[tbl4][Lambda関数の設定]{
項目	値
-----------------
①関数名	M5-Security-AmazonConnect
②ランタイム	Node.js 10.x
③実行ロール	新しいロールを作成
④ロール名	M5-Security-AmazonConnect-Role
⑤ポリシーテンプレート	基本的なLambda@Edgeのアクセス権限
//}


//image[g402][関数の設定項目を埋めて関数の作成をクリックする]{
//}



==== Amazon Connectアクセス権限を追加する


Lambda関数からAmazon Connectから電話をかけるための権限を付与します。
M5-Security-AmazonConnect-Roleロールを表示をクリックします。



//image[g403][M5-Security-AmazonConnect-Roleロールをクリック]{
//}




［インラインポリシーの追加］をクリックします。



//image[g404][インラインポリシーの追加をクリック]{
//}




サービスを展開して、検索窓に「Connect」と入れて検索します。出てきた［Connect］をクリックします。



//image[g405][Connectで検索する]{
//}




アクションのアクセスレベルにある「書き込み」部分を展開して、その中にある@<tt>{StartOutboundVoiceContact}のチェックを入れます。



//image[g406][StartOutboundVoiceContactのチェックを入れる]{
//}




すべてのリソースを選択して、右下の［ポリシーの確認］ボタンをクリックします。



//image[g407][ポリシーの確認をクリック]{
//}




ポリシー名を入力します。@<tt>{M5-Security-AmazonConnect-Policy}としました。右下の［ポリシーの作成］ボタンをクリックします。



//image[g408][ポリシー名を入力]{
//}




Lambda画面に戻り、画面更新するとAmazon Connectの権限が追加されます。



//image[g409][権限が追加された]{
//}



==== プログラムを書き込む


index.jsを開き、下記プログラムをコピペしてください。
LINE Thingsからリクエストが飛んでくるので、bodyから対象値を取得します。


//emlist[][javascript]{
const Util = require('./util.js');

exports.handler = async (event) => {
    const response = {
        statusCode: 200,
        body: {"result":"completed!"},
    };

    const body = JSON.parse(event.body);
    const thingsData = body.events[0].things.result;

    if (thingsData.bleNotificationPayload) {
        // LINE Thingsから飛んでくるデータを取得
        const blePayload = thingsData.bleNotificationPayload;
        var buffer1 = Buffer.from(blePayload, 'base64');
        var m5Data = buffer1.toString('ascii');  //Base64をデコード

        const sendMessage = `認証コードは「${m5Data}」です。`;

        // Amazon Connect送信
        await Util.callMessageAction(sendMessage);

    }
    return response;
};
//}


続いて、新規ファイルを作成します。



//image[g410][新規ファイルを作成する]{
//}




下記コードをコピペしてください。


//emlist[][javascript]{
'use strict';
const AWS = require('aws-sdk');
var connect = new AWS.Connect();

// 電話をかける処理
module.exports.callMessageAction = async function callMessageAction(message) {
    return new Promise(((resolve, reject) => {

        // Attributesに発話する内容を設定
        var params = {
            Attributes: {"message": message},
            InstanceId: process.env.INSTANCEID,
            ContactFlowId: process.env.CONTACTFLOWID,
            DestinationPhoneNumber: process.env.PHONENUMBER,
            SourcePhoneNumber: process.env.SOURCEPHONENUMBER
        };

        // 電話をかける
        connect.startOutboundVoiceContact(params, function(err, data) {
            if (err) {
                console.log(err);
                reject();
            } else {
                resolve(data);
            }
        });
    }));
};
//}


保存する際はファイル名を「util.js」にしてください。



//image[g411][ファイル名を「util.js」にする]{
//}



==== 環境変数を設定する


環境変数を設定していきます。これまでにメモした内容を反映させていきましょう。

//table[tbl5][環境変数の設定]{
項目	値
-----------------
INSTANCEID	メモしたinstanceのID
CONTACTFLOWID	メモしたcontact-flowのID
PHONENUMBER	ご自身の携帯電話番号 ※+81を先頭につけて数字のみにします
                例)090-1234-5678 👉+819012345678
SOURCEPHONENUMBER	Amazon Connectで取得した電話番号 ※+81を先頭につけて数字のみにします
//}


//image[g412][環境変数を設定する]{
//}



==== API Gatewayを設定する


LINE ThingsからLambdaをキックするためにエンドポイントURLを発行します。
［トリガーを追加］をクリックします。



//image[g413][トリガーの追加をクリック]{
//}




トリガーの設定は下記を指定します。最後に［追加］ボタンをクリックします。

//table[tbl6][トリガーの設定]{
項目	値
-----------------
トリガー	API Gateway
API	新規APIの作成
セキュリティ	オープン
//}


//image[g414][トリガーの設定を行う]{
//}




APIエンドポイントのURLをメモしておきましょう



//image[g415][エンドポイントURLをメモ]{
//}




右上の保存ボタンをクリックします。



//image[g416][保存ボタンをクリック]{
//}



=== LINE Botを作ろう！


LINE Thingsの連携にはまず、LINE Botを作る必要があります。


==== チャネルの作成


下記にアクセスしてログインしてください。
@<href>{https://developers.line.biz/ja/,https://developers.line.biz/ja/}



プロバイダーがまだ無い方は作成お願いします。新規チャネルを作成します。



//image[g500][新規チャネルを作成をクリック]{
//}




Messaging APIをクリック



//image[g501][Messaging APIをクリック]{
//}




下記設定項目を埋めます。埋めたら［入力内容を確認する］ボタンをクリックします。

//table[tbl7][Botの設定]{
項目	値
-----------------
①アプリ名	M5-Security
②アプリ説明	M5-Security
③大業種	個人
③小業種	個人（その他）
④メールアドレス	ご自身のメールアドレス
//}


//image[g502][設定項目を埋めて、入力内容を確認するボタンをクリック]{
//}




［同意する］をクリック



//image[g503][同意するをクリック]{
//}




2つのチェックを入れてから［作成］をクリックします。



//image[g504][2つチェックを入れて作成をクリック]{
//}




作成したチャネルをクリックして、アクセストークンの発行とエンドポイントの設定を行います。


==== アクセストークンを発行する


このチャネルにアクセスするためのトークンを発行します。［再発行］をクリックします。



//image[g505][再発行をクリック]{
//}




そのまま[再発行]をクリックします。



//image[g506][再発行をクリック]{
//}




発行されたアクセストークンをメモしておきましょう。



//image[g507][アクセストークンをメモしておく]{
//}



==== Webhookを設定


Webhookのアクセス先URLを指定します。編集ボタンをクリックします。



//image[g508][編集ボタンをクリック]{
//}




メモしたAPI GatewayのURLを貼り付けます。「https://」は既にあるので、省略して貼り付けます。［更新］ボタンをクリックします。



//image[g509][API GatewayのURLを設定]{
//}




Webhook送信を利用するに変えます



//image[g510][利用するに変える]{
//}




利用するを選択して［更新］ボタンをクリックします。
稀に利用するに変わらないことがあるので、画面をリロードして確認してください。



//image[g511][利用するになっているか確認する]{
//}



==== LINE Botと友だちになる


下にスクロールするとQRコードがあるので、LINEアプリからQRを読み取って、必ずBotと友だちになっておいてください。ペアリングする際に友だちになっていないとうまく連携できません。



//image[g512][Botと友だちになっておく]{
//}



==== LIFFの設定をする


LIFFアプリを設定します。LIFFタブをクリックして、［LIFFアプリを作成］ボタンをクリックします。



//image[g513][LIFFアプリを作成をクリック]{
//}




LIFFの設定を行います。@<tt>{BLE feature}は必ず「ON」にしてください。

//table[tbl8][LIFFの設定]{
項目	値
-----------------
①名前	M5-Security
②サイズ	Full <br/>※どのサイズでもいい
③エンドポイントURL	https://www.google.com/
                  ※とりあえず、どのURLでもいい（後で設定するため）
④オプション	必ずONにする
//}


//image[g514][LIFFの設定を行う]{
//}



=== LINE Things 自動通信機能の設定


LINE Thingsを使うためにサービスUUIDを発行します。また、自動通信機能を使うためのシナリオというものも設定していきます。


==== サービスUUIDを発行する


@<href>{https://twitter.com/n0bisuke,のびすけさん}が作ったツールをありがたく使います。
下記にアクセスしてください。



@<href>{https://n0bisuke.github.io/linethingsgen/,https://n0bisuke.github.io/linethingsgen/}



左側メニューのSettingをクリックして、生成したLINE Botのアクセストークンを貼り付けます。
［保存］ボタンをクリックします。



//image[g600][LINE Botのアクセストークンを貼り付ける]{
//}




左側メニューのCreate Productをクリックして、プルダウンメニューから先程作成したLIFFアプリを指定します。
トライアルプロダクトの名前を入力して［作成］ボタンをクリックします。

//table[tbl9][LIFFアプリの設定]{
項目	値
-----------------
②LIFFアプリ	M5-Security
③トライアルプロダクトの名前	M5-Security
//}


//image[g601][トライアルプロダクトを設定する]{
//}




左側メニューのCreate Scenarioをクリックして、プルダウンメニューから先程作成した@<tt>{M5-Security}を選択。
そのままの状態で［シナリオセットの登録/更新］ボタンをクリックします。



//image[g602][シナリオセットの登録/更新ボタンをクリック]{
//}




発行された@<tt>{serviceUuid}をメモしておきます。



//image[g603][serviceUuidをメモする]{
//}



==== LINEアプリのLINE Thingsを有効化


LINE Thingsを使うために、下記QRコードを読み取ってください。するとLINEアプリの設定画面に@<tt>{LINE Things}の項目が増えます。



//image[g604][LINE Thingsを有効化する]{
//}




設定ページにLINE Thingsの項目が増えます



//image[g605][LINE Thingsの項目が増える]{
//}



=== LIFFアプリを構築する


LIFFのアクセス先をVue.jsで作成していきます。Vueの環境がない方は設定をお願いします。


==== Vue.jsでプロジェクトを作成する


Vue.jsでプロジェクトを作成します。下記コマンドを入力してください。


//emlist[][shell]{
$ vue create m5_security
//}


次の設問はマニュアルを指定します


//emlist[][shell]{
? Please pick a preset: (Use arrow keys)
   default (babel, eslint) 
❯  Manually select features 
//}


次の設問は@<tt>{Router}を選択します。カーソルを合わせて、スペースで選択してください。


//emlist[][shell]{
? Check the features needed for your project: 
 ◉ Babel
 ◯ TypeScript
 ◯ Progressive Web App (PWA) Support
❯◉ Router
 ◯ Vuex
 ◯ CSS Pre-processors
 ◉ Linter / Formatter
 ◯ Unit Testing
 ◯ E2E Testing
//}


次の設問はそのままエンターで良いです。


//emlist[][shell]{
? Use history mode for router? (Requires proper server setup for index fallback 
in production) (Y/n) Yes
//}


次の設問もそのままエンター。


//emlist[][shell]{
? Pick a linter / formatter config: (Use arrow keys)
❯ ESLint with error prevention only 
  ESLint + Airbnb config 
  ESLint + Standard config 
  ESLint + Prettier 
//}


次の設問もそのままエンター。


//emlist[][shell]{
? Pick additional lint features: (Press <space> to select, <a> to toggle all, <i
> to invert selection)
❯◉ Lint on save
 ◯ Lint and fix on commit
//}


次の設問もそのままエンター。


//emlist[][shell]{
? Where do you prefer placing config for Babel, PostCSS, ESLint, etc.? (Use arro
w keys)
❯ In dedicated config files 
  In package.json 
//}


次の設問はNoを選択


//emlist[][shell]{
? Save this as a preset for future projects? (y/N) No
//}


プロジェクトが作成できたら、できたフォルダに移動して、起動するか確認してみてください。
起動確認できたら、Ctrl + Cで終了してください。


//emlist[][shell]{
$ cd m5_security
$ npm run serve
//}

==== Vuetifyを追加


UIを小綺麗に見せるためにVuetifyをインストールします。下記コマンドを実行します。


//emlist[][shell]{
$ vue add vuetify
//}


次の設問はそのままエンター。


//emlist[][shell]{
? Choose a preset: (Use arrow keys)
❯ Default (recommended) 
  Prototype (rapid development) 
  Configure (advanced) 
//}

==== LIFFのSDKを入れる


publicフォルダのindex.htmlファイルに下記SDKを追記します。


//emlist[][html:抜粋]{
<script src="https://d.line-scdn.net/liff/1.0/sdk.js"></script>
//}


9行目あたりに追記します。


//emlist[][html]{
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width,initial-scale=1.0">
    <link rel="icon" href="<%= BASE_URL %>favicon.ico">
    <title>m5_security</title>
    <script src="https://d.line-scdn.net/liff/1.0/sdk.js"></script>
    <link rel="stylesheet" href="https://fonts.googleapis.com/css?family=Roboto:100,300,400,500,700,900">
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/@mdi/font@latest/css/materialdesignicons.min.css">
  </head>
  <body>
    <noscript>
      <strong>We're sorry but m5_security doesn't work properly without JavaScript enabled. Please enable it to continue.</strong>
    </noscript>
    <div id="app"></div>
    <!-- built files will be auto injected -->
  </body>
</html>
//}

==== 認証コード送信画面を作成する


続いて、実際に受け渡し部分のプログラムを実装していきます。
42行目にある@<tt>{USER_SERVICE_UUID}は発行したサービスUUIDを指定してください。


//emlist[][javascript]{
<template>
  <v-app id="inspire">
    <v-container>
      <v-layout
        text-xs-center
        wrap
      >
        <v-flex mb-4>
          <h1 class="display-2 font-weight-bold mb-3">
            認証画面
          </h1>
        </v-flex>

        <v-flex xs12>
          <v-form>
            <v-container>
              <v-layout row wrap>
                <v-text-field
                  v-model="code"
                  @keyup.enter="say"
                  @keypress="setCanSubmit"
                  outline
                  label="認証コードを入力してください"
                ></v-text-field>
              </v-layout>
            </v-container>
          </v-form>  
        </v-flex>

        <v-flex xs12>
          {{this.bleStatus}}
        </v-flex>
      </v-layout>
    </v-container>
  </v-app>
</template>

<script>
  export default {
    data () {
      return {
        USER_SERVICE_UUID: '＜あなたのサービスUUID＞',
        LED_CHARACTERISTIC_UUID: 'E9062E71-9E62-4BC6-B0D3-35CDCD9B027B', /* トライアルは固定値 */
        bleConnect: false,
        canSubmit: false,
        bleStatus: '',
        state: false,
        characteristic: '',
        code: '',
        user: {
          image: '',
          userId: ''
        }        
      }
    },
    methods: {
      setCanSubmit () {
        this.canSubmit = true
      },
      say () {
        this.sendData()
      },
      sendData () {
        this.bleStatus = `送信:${this.code}`

        let ch_array = this.code.split("");
        for(let i = 0; i < 16; i = i + 1){
          ch_array[i] = (new TextEncoder('ascii')).encode(ch_array[i]);
        }

        this.characteristic.writeValue(new Uint8Array(ch_array)
        ).catch(error => {
          this.bleStatus = error.message
        })
      },
      // BLEが接続できる状態になるまでリトライ
      liffCheckAvailablityAndDo: async function (callbackIfAvailable) {
        try {
          const isAvailable = await liff.bluetooth.getAvailability();
          if (isAvailable) {
            callbackIfAvailable()
          } else {
            // リトライ
            this.bleStatus = `Bluetoothをオンにしてください。`
            setTimeout(() => this.liffCheckAvailablityAndDo(callbackIfAvailable), 10000)
          }
        } catch (error) {
          alert('Bluetoothをオンにしてください。')
        }
      },
      // サービスとキャラクタリスティックにアクセス
      liffRequestDevice: async function () {
        const device = await liff.bluetooth.requestDevice()
        await device.gatt.connect()
        const service = await device.gatt.getPrimaryService(this.USER_SERVICE_UUID)
        service.getCharacteristic(this.LED_CHARACTERISTIC_UUID).then(characteristic => {
          this.characteristic = characteristic
          this.bleConnect = true
          this.bleStatus = `デバイスに接続しました！`
        }).catch(error => {
          this.bleConnect = true
          this.bleStatus = `デバイス接続に失敗=${error.message}`
        })
      },
      initializeLiff: async function(){
        await liff.initPlugins(['bluetooth']);
        this.liffCheckAvailablityAndDo(() => this.liffRequestDevice())

      }
    },
    mounted: function () {
      liff.init(
          () => this.initializeLiff()
      )
    }
  }
</script>
//}

==== 外部公開設定を行う


LIFFアプリで外部に公開する必要があるので、Vueのプロジェクト直下に@<tt>{vue.config.js}という名前のファイルを作成します。


//emlist[][javascript]{
module.exports = {
    devServer: {
        host: '0.0.0.0',
        disableHostCheck: true
    }
}
//}

==== 実行する


それでは、ローカル環境で動かしてみましょう。下記コマンドを実行して、サーバーを起動してみます。


//emlist[][shell]{
$ npm run serve
//}


サーバーがうまく起動するとこのような表示になります。ポート番号は起動する毎に変わることがあります。
下記の例だと36413のポートが開いています。URLにアクセスして、画面が表示されることを確認してください。


//emlist[][shell]{
 DONE  Compiled successfully in 18169ms                                                                                                                                                       01:18:59


  App running at:
  - Local:   http://localhost:36413/ 
  - Network: http://172.xxx.xxx.xxx:36413/

  Note that the development build is not optimized.
  To create a production build, run npm run build.
//}


localhostにアクセスすると認証コード画面が表示されます。



//image[g606][localhostにアクセス]{
//}



=== 外部公開をする


LIFF画面にVueで作ったサイトを表示してほしいので、外部公開をします。@<tt>{serveo.net}というサービスを使うとローカル環境で動いているサイトが全世界に公開されます。


==== serveo.netに公開する


下記コマンドを実行するとローカルで動いているサイトが外部公開することができます。
npm run serveした際に出ている@<tt>{http://localhost:XXXXX/}のポート番号を一致させる必要があります。
XXXXXの部分はそれぞれのポート番号を指定してください。この例だと@<tt>{36413}です。


//emlist[][shell]{
ssh -o ServerAliveInterval=120 -R 80:localhost:XXXXX serveo.net
//}


実行すると@<tt>{https}でアクセスできるURLが発行されます。発行されたURLはメモしておきましょう。


//emlist{
Forwarding HTTP traffic from https://xxxxx.serveo.net
Press g to start a GUI session and ctrl-c to quit.
//}

==== LIFFの編集をする


LIFFのエンドポイントURLを今回発行された@<tt>{serveo.net}のURLに変更します。
LIFF画面の［編集］ボタンをクリックします。



//image[g607][LIFFの編集ボタンをクリック]{
//}




LIFFのエンドポイントURLを変更します。



//image[g608][エンドポイントURL変更]{
//}



=== M5Stackに書き込む


ArduinoでM5Stackにプログラムを書き込んでいきます。M5Stackの詳しい環境構築については下記URLをご確認ください。



@<href>{https://docs.m5stack.com/#/en/quick_start/m5core/m5stack_core_quick_start,https://docs.m5stack.com/#/en/quickstart/m5core/m5stackcorequickstart}


==== M5Stackにプログラムを書き込む


Arduino IDEを開いて下記コードを入力します。11行目のサービスUUIDは生成した値を入力してください。


//emlist[][c]{
#include <M5Stack.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Device Name: Maximum 30 bytes
#define DEVICE_NAME "M5-Security"

// あなたのサービスUUIDを貼り付けてください
#define USER_SERVICE_UUID "＜あなたのサービスUUID＞"
// Notify UUID: トライアル版は値が固定される
#define NOTIFY_CHARACTERISTIC_UUID "62FBD229-6EDD-4D1A-B554-5C4E1BB29169"
// PSDI Service UUID: トライアル版は値が固定される
#define PSDI_SERVICE_UUID "E625601E-9E55-4597-A598-76018A0D293D"
// LIFFからのデータ UUID: トライアル版は値が固定される
#define WRITE_CHARACTERISTIC_UUID "E9062E71-9E62-4BC6-B0D3-35CDCD9B027B"
// PSDI CHARACTERISTIC UUID: トライアル版は値が固定される
#define PSDI_CHARACTERISTIC_UUID "26E2B12B-85F0-4F3F-9FDD-91D114270E6E"

BLEServer* thingsServer;
BLESecurity* thingsSecurity;
BLEService* userService;
BLEService* psdiService;
BLECharacteristic* psdiCharacteristic;
BLECharacteristic* notifyCharacteristic;
BLECharacteristic* writeCharacteristic;

bool deviceConnected = false;
bool oldDeviceConnected = false;
bool sendFlg = false;
bool unlockFlg = false;

// 認証コード
int randNumber;

class serverCallbacks: public BLEServerCallbacks {

  // デバイス接続
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;

    // 一度認証されないとコードは生成しない
    if (unlockFlg) {
      sendFlg = false;
      unlockFlg = false;
    }
  };

  // デバイス未接続
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;

    if (unlockFlg) {
      sendFlg = false;
      unlockFlg = false;
    }
  }
};

// LIFFから送信されるデータ
class writeCallback: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *bleWriteCharacteristic) {
    // LIFFから来るデータを取得
    std::string value = bleWriteCharacteristic->getValue();
    int myNum = atoi(value.c_str());

    // 認証コードと一致しているか確認
    if (randNumber == myNum) {
      M5.Lcd.fillScreen(GREEN);
      M5.Lcd.setCursor(0, 30);
      M5.Lcd.print("Unlock!");
      unlockFlg = true;    
    }    
  }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("");
  BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT_NO_MITM);

  // Security Settings
  BLESecurity *thingsSecurity = new BLESecurity();
  thingsSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);
  thingsSecurity->setCapability(ESP_IO_CAP_NONE);
  thingsSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

  setupServices();
  startAdvertising();

  // put your setup code here, to run once:
  M5.begin();  
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);

}

void loop() {
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // Wait for BLE Stack to be ready
    thingsServer->startAdvertising(); // Restart advertising
    oldDeviceConnected = deviceConnected;
    Serial.println("Restart!");
  }

  // Connection
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }

  if (!sendFlg && deviceConnected){
    sendFlg = true;
    delay(5000);

    // 認証コード生成
    randNumber = random(1000, 10000);
    char newValue[16];
    sprintf(newValue, "%d", randNumber);

    // 認証コードをWebhookURLに送信
    notifyCharacteristic->setValue(newValue);
    notifyCharacteristic->notify();
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 30);
    M5.Lcd.print("Send!!");
    delay(5000);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 30);
    M5.Lcd.print("Connected");  /*表示クリア*/

    delay(50);

  } else if (!deviceConnected) {
    M5.Lcd.setCursor(0, 30);
    M5.Lcd.print("Not Connected");

  }

  M5.update();

}

// サービス初期化
void setupServices(void) {
  // Create BLE Server
  thingsServer = BLEDevice::createServer();
  thingsServer->setCallbacks(new serverCallbacks());

  // Setup User Service
  userService = thingsServer->createService(USER_SERVICE_UUID);

  // LIFFからのデータ受け取り設定
  writeCharacteristic = userService->createCharacteristic(WRITE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  writeCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  writeCharacteristic->setCallbacks(new writeCallback());

  // Notifyセットアップ
  notifyCharacteristic = userService->createCharacteristic(NOTIFY_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  notifyCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  BLE2902* ble9202 = new BLE2902();
  ble9202->setNotifications(true);
  ble9202->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  notifyCharacteristic->addDescriptor(ble9202);

  // Setup PSDI Service
  psdiService = thingsServer->createService(PSDI_SERVICE_UUID);
  psdiCharacteristic = psdiService->createCharacteristic(PSDI_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
  psdiCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);

  // Set PSDI (Product Specific Device ID) value
  uint64_t macAddress = ESP.getEfuseMac();
  psdiCharacteristic->setValue((uint8_t*) &macAddress, sizeof(macAddress));

  // Start BLE Services
  userService->start();
  psdiService->start();
}

void startAdvertising(void) {
  // Start Advertising
  BLEAdvertisementData scanResponseData = BLEAdvertisementData();
  scanResponseData.setFlags(0x06); // GENERAL_DISC_MODE 0x02 | BR_EDR_NOT_SUPPORTED 0x04
  scanResponseData.setName(DEVICE_NAME);

  thingsServer->getAdvertising()->addServiceUUID(userService->getUUID());
  thingsServer->getAdvertising()->setScanResponseData(scanResponseData);
  thingsServer->getAdvertising()->start();
}
//}


書き込むボードの設定は以下の通り

//table[tbl10][ボードの設定]{
項目	値
-----------------
ボード	M5Stack-Core-ESP32
Upload Speed	921600
シリアルポート	/dev/cu.SLAB_USBtoUART
//}


//image[g609][ボードの設定]{
//}




LINEアプリを開いて、@<tt>{M5-Security}をタップします。



［今すぐ利用］をタップします。



//image[g611][今すぐ利用をタップ]{
//}


