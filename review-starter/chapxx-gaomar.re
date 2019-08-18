
= ちょっとだけセキュアな認証システムをLINE Thingsでやってみた


LINE Thingsの自動通信機能を使って、Bluetoothの電波が届くエリアに入ったら認証コードを飛ばして、Amazon Connectから認証コードを教えてくれるシステムを作ってみました。
ペアリング済みの携帯にのみ認証コードが送られてくる仕組みなので、セキュリティもバッチリです！


== 仕組み


今回はM5StackやM5StickCを使います。LINE Thingsでペアリング済みの携帯がBluetoothエリアに入ると、自動通信機能を使いWebhook送信をします。飛ばし先はAPI Gatewayで発行されたURLです。
AWS Lambdaで実際のプログラム処理を行い、Amazon ConnectとLINE Notifyに認証コードの通知がいきます。



//image[g100][アーキテクチャ]{
//}



== 準備


各種準備をこれから行っていきます。必要なものは以下の通りです。

 * Amazon Connect電話番号の発行
 * Amazon Connect問い合わせフロー作成
 * LINE Notifyアクセスキー
 * AWS Lambda関数を作成する
 * API Gatewayの設定
 * LINE Bot
 * LIFFの設定
 * LINE Things 自動通信機能サービスUUID発行
 * Vue.jsの設定
 * Netlifyにデプロイ
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

//table[tbl1][aa]{
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

//table[tbl2][tbl2]{
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

//table[tbl3][tbl3]{
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



=== LINE Notifyの設定


こちらからトークンを発行します。飛ばしたい先のトークルームを指定します。発行されたトークンは後ほど使うのでメモしておきましょう。
@<href>{https://notify-bot.line.me/my/,https://notify-bot.line.me/my/}



//image[g300][トークンを発行するボタンをクリック]{
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

//table[tbl4][tbl4]{
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
const Util = require('util.js');

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

        // LINE Notify送信
        await Util.postNotify(sendMessage);

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

// あなたのLINE Notifyトークンを設定してください
const headers = {
    Authorization: `Bearer ${process.env.NOTIFY_TOKEN}`,
    'Content-Type': 'application/x-www-form-urlencoded'
};

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

// LINE Notifyへ送信
module.exports.postNotify = async function postNotify(postMessage) {
    // オプションを定義
    const jsonData =
    {
      'message': postMessage
    };

    const options = {
      url: 'https://notify-api.line.me/api/notify',
      method: 'POST',
      headers: headers,
      form: jsonData
    };

    return new Promise(function (resolve, reject) {
      request(options, function (error, response, body) {
        if (!error) {
          console.log(body.name);
          resolve(true);
        } else {
          console.log('error: ' + response + body);
          resolve(true);
        }
      });
    });
};
//}


保存する際はファイル名を「util.js」にしてください。



//image[g411][ファイル名を「util.js」にする]{
//}



==== 環境変数を設定する


環境変数を設定していきます。これまでにメモした内容を反映させていきましょう。

//table[tbl5][tbl5]{
項目	値
-----------------
INSTANCEID	1-2でメモしたinstanceのID
CONTACTFLOWID	1-2でメモしたcontact-flowのID
PHONENUMBER	ご自身の携帯電話番号 ※+81を先頭につけて数字のみにします<br/>例)090-1234-5678 👉+819012345678
SOURCEPHONENUMBER	Amazon Connectで取得した電話番号 ※+81を先頭につけて数字のみにします
NOTIFY_TOKEN	発行したLINE Notifyのトークンを貼り付ける
//}


//image[g412][環境変数を設定する]{
//}



==== API Gatewayを設定する


LINE ThingsからLambdaをキックするためにエンドポイントURLを発行します。
［トリガーを追加］をクリックします。



//image[g413][トリガーの追加をクリック]{
//}




トリガーの設定は下記を指定します。最後に［追加］ボタンをクリックします。

//table[tbl6][tbl6]{
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

//table[tbl7][tbl7]{
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


