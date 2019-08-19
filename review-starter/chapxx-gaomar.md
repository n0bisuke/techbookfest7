# ちょっとだけセキュアな認証システムをLINE Thingsでやってみた

LINE Thingsの自動通信機能を使って、Bluetoothの電波が届くエリアに入ったら認証コードを飛ばして、Amazon Connectから認証コードを教えてくれるシステムを作ってみました。
ペアリング済みの携帯にのみ認証コードが送られてくる仕組みなので、セキュリティもバッチリです！

## 仕組み

今回はM5StackやM5StickCを使います。LINE Thingsでペアリング済みの携帯がBluetoothエリアに入ると、自動通信機能を使いWebhook送信をします。飛ばし先はAPI Gatewayで発行されたURLです。
AWS Lambdaで実際のプログラム処理を行い、Amazon ConnectとLINE Notifyに認証コードの通知がいきます。

![アーキテクチャ](images/chapxx-gaomar/g100.png)

## 準備

各種準備をこれから行っていきます。必要なものは以下の通りです。

- Amazon Connect電話番号の発行
- Amazon Connect問い合わせフロー作成
- LINE Notifyアクセスキー
- AWS Lambda関数を作成する
- API Gatewayの設定
- LINE Bot
- LIFFの設定
- LINE Things 自動通信機能サービスUUID発行
- Vue.jsの設定
- Netlifyにデプロイ
- M5Stack（M5StickC）に書き込む
- LINE Thingsのペアリング設定

それでは、順番に用意していきましょう。

### Amazon Connect 電話番号の発行

#### リソースを作成する

AWSにアクセスし、お持ちのAWSアカウントでログインします。
[https://aws.amazon.com/jp/](https://aws.amazon.com/jp/)

コンソール画面が開いたら、サービス検索から【Amazon Connect】と入力して検索します。出てきた［Amazon Connect］をクリックします。

![AmazonConnect](images/chapxx-gaomar/g101.png)

［今すぐ始める］ボタンをクリックします。

![今すぐ始めるをクリック](images/chapxx-gaomar/g102.png)

東京リージョンになっているか確認してください。なっていない方は右上から「アジアパシフィック（東京）」を選択します。

![アジアパシフィック（東京）を選択](images/chapxx-gaomar/g103.png)

アクセスURLに今回使用するための名前を決めます。［handson-ac-{あなたのお名前}］という感じで他の人と被らないような名前にします。

![リソース名を決める](images/chapxx-gaomar/g104.png)

ステップ２の管理者の作成は後でできるので、「これをスキップ」を選択してから、［次のステップ］ボタンをクリックします。

![これをスキップを選択し、次のステップをクリック](images/chapxx-gaomar/g105.png)

ステップ３のテレフォニーオプションもそのまま何もせず［次のステップ］ボタンをクリックします。

![次のステップをクリック](images/chapxx-gaomar/g106.png)

ステップ４のデータストレージもそのまま［次のステップ］ボタンをクリックします。

![次のステップをクリック](images/chapxx-gaomar/g107.png)

ステップ５で最終確認をして問題なければ［インスタンスの作成］ボタンをクリックします。

![インスタンスの作成をクリック](images/chapxx-gaomar/g108.png)

1〜2分ほど作成に時間がかかります。

![1〜2分待ちます](images/chapxx-gaomar/g109.png)

成功すれば、［今すぐ始める］ボタンをクリックします。

![今すぐ始めるをクリック](images/chapxx-gaomar/g110.png)

#### 電話番号を取得する
Amazon Connect専用の電話番号を手に入れます。
別のタブでAmazon Connect用の画面が表示されるので、そこに表示されている［今すぐ始める］ボタンをクリックします。

日本語になっていない場合は右上から「日本語」を選択してください。

![今すぐ始める](images/chapxx-gaomar/g111.png)

電話番号の取得で、国はJapan +81を選択します。

![+81を選択](images/chapxx-gaomar/g112.png)

次のタイプは2種類あります。お好きな方を選んでください。

|タイプ|特徴|
|:--|:--|
|Direct Dial|050番号が取得できる。発信者が通話料負担する。|
|Toll Free|0800番号が取得できる。発信者の通話料無料（フリーダイヤル）開発者が負担する。|

![電話番号タイプを選択](images/chapxx-gaomar/g113.png)

最後にお好きな番号を選択して、［次へ］ボタンをクリックします。

![次へをクリック](images/chapxx-gaomar/g114.png)

［Continue］ボタンをクリックします。これで電話番号が発行されました。

![Continueをクリック](images/chapxx-gaomar/g115.png)

#### 発行した電話番号を確認する
取得した電話番号を確認します。
左メニューからルーティングの電話番号をクリックします。

![ルーティングの電話番号をクリック](images/chapxx-gaomar/g116.png)

取得した電話番号が表示されています。別の番号を増やしたい場合は右側にある［電話番号の追加］ボタン押して追加することもできます。この電話番号は後ほど使うので、メモしておきましょう。

![電話番号をメモしておく](images/chapxx-gaomar/g117.png)

### Amazon Connect問い合わせフロー作成する

問い合わせフローを作成していきましょう。M5Stackから発行された認証コードを言ってもらうためのフローを作ります。GUI画面でブロックを繋げるだけで簡単に構築することができます。

#### 問い合わせフローを作成する

左側メニューのルーティングから「問い合わせフロー」をクリックします。

![問い合わせフローをクリック](images/chapxx-gaomar/g200.png)

［問い合わせフローの作成］をクリックします。

![問い合わせフローの作成をクリック](images/chapxx-gaomar/g201.png)

名前を「M5-Security-AmazonConnect」と入力します。

![名前を入力する](images/chapxx-gaomar/g202.png)

設定カテゴリにある「音声の設定」ブロックをドラッグアンドドロップして、ドロップしたブロックをクリックします。

![音声の設定ブロックをドラッグアンドドロップする](images/chapxx-gaomar/g203.png)

言語は「日本語」でお好きな音声を選択してください。

![日本語を選び音声を選択](images/chapxx-gaomar/g204.png)

エントリポイントと音声の設定ブロックを繋げます。

![音声の設定ブロックを繋げる](images/chapxx-gaomar/g205.png)

設定カテゴリにある「問い合わせ属性の設定」をドラッグアンドドロップします。

![問い合わせ属性の設定をドラッグアンドドロップ](images/chapxx-gaomar/g206.png)

属性の設定を行います。「属性を使用する」を選択してください。

|キー|値|
|:--|:--|
|宛先キー|message|
|タイプ|ユーザー定義|
|属性|message|

![属性の設定を行う](images/chapxx-gaomar/g207.png)

ブロックを繋げます。

![ブロックを繋げる](images/chapxx-gaomar/g208.png)

操作カテゴリにある「プロンプトの再生」をドラッグアンドドロップします。

![プロンプトの作成をドラッグアンドドロップ](images/chapxx-gaomar/g209.png)

属性の設定を行います。「テキスト読み上げ機能(アドホック)」を選択してください。

|項目|値|
|:--|:--|
|テキスト読み上げ機能(アドホック)|動的に入力する|
|タイプ|ユーザー定義|
|属性|message|

![属性の設定を行う](images/chapxx-gaomar/g210.png)

ブロックを繋げます。

![ブロックを繋げる](images/chapxx-gaomar/g211.png)

ブランチカテゴリにある「ループ」をドラッグアンドドロップします。
ループ回数はお好きな数を指定してください。

![ループ設定する](images/chapxx-gaomar/g212.png)

ブロックを繋げます。

![ブロックを繋げる](images/chapxx-gaomar/g213.png)

ループとプロンプトの再生を繋げます。

![ループとプロンプトのブロックを繋げる](images/chapxx-gaomar/g214.png)

終了 / 転送カテゴリにある「切断/ハングアップ」をドラッグアンドドロップします。

![切断/ハングアップをドラッグアンドドロップ](images/chapxx-gaomar/g215.png)

まだ繋いでいない部分を全て「切断/ハングアップ」に繋ぎます。

![繋いでいない部分を繋げる](images/chapxx-gaomar/g216.png)

右上の①［保存］と②「公開」ボタンを順番にクリックします。

![保存と公開をクリックする](images/chapxx-gaomar/g217.png)

#### IDをメモしておく

問い合わせフローの名前の下に「追加のフロー情報の表示」という項目があるので、それを展開します。展開するとARNの情報が表示されるのでinstanceのIDとconstact-flowのIDをそれぞれメモしておきます。

![instanceのIDとconstact-flowのID](images/chapxx-gaomar/g218.png)

### LINE Notifyの設定

こちらからトークンを発行します。飛ばしたい先のトークルームを指定します。発行されたトークンは後ほど使うのでメモしておきましょう。
[https://notify-bot.line.me/my/](https://notify-bot.line.me/my/)

![トークンを発行するボタンをクリック](images/chapxx-gaomar/g300.png)

### AWS Lambda関数を作成する
LINE Thingsから送られてくる認証コードを受け取って、Amazon Connectに認証コードを発話するための処理を行います。

#### Lambda関数を作成する
サービスから「Lambda」を検索して、出てきたものをクリックします。

![Lambdaを検索する](images/chapxx-gaomar/g400.png)

Lambdaから新規で関数を作成します。［関数の作成］ボタンをクリックします。

![関数の作成をクリック](images/chapxx-gaomar/g401.png)

関数は以下の通り入力して、［関数の作成］ボタンをクリックします。

|項目|値|
|:--|:--|
|①関数名|M5-Security-AmazonConnect|
|②ランタイム|Node.js 10.x|
|③実行ロール|新しいロールを作成|
|④ロール名|M5-Security-AmazonConnect-Role|
|⑤ポリシーテンプレート|基本的なLambda@Edgeのアクセス権限|

![関数の設定項目を埋めて関数の作成をクリックする](images/chapxx-gaomar/g402.png)

#### Amazon Connectアクセス権限を追加する
Lambda関数からAmazon Connectから電話をかけるための権限を付与します。
M5-Security-AmazonConnect-Roleロールを表示をクリックします。

![M5-Security-AmazonConnect-Roleロールをクリック](images/chapxx-gaomar/g403.png)

［インラインポリシーの追加］をクリックします。

![インラインポリシーの追加をクリック](images/chapxx-gaomar/g404.png)

サービスを展開して、検索窓に「Connect」と入れて検索します。出てきた［Connect］をクリックします。

![Connectで検索する](images/chapxx-gaomar/g405.png)

アクションのアクセスレベルにある「書き込み」部分を展開して、その中にある`StartOutboundVoiceContact`のチェックを入れます。

![StartOutboundVoiceContactのチェックを入れる](images/chapxx-gaomar/g406.png)

すべてのリソースを選択して、右下の［ポリシーの確認］ボタンをクリックします。

![ポリシーの確認をクリック](images/chapxx-gaomar/g407.png)

ポリシー名を入力します。`M5-Security-AmazonConnect-Policy`としました。右下の［ポリシーの作成］ボタンをクリックします。

![ポリシー名を入力](images/chapxx-gaomar/g408.png)

Lambda画面に戻り、画面更新するとAmazon Connectの権限が追加されます。

![権限が追加された](images/chapxx-gaomar/g409.png)

#### プログラムを書き込む

index.jsを開き、下記プログラムをコピペしてください。
LINE Thingsからリクエストが飛んでくるので、bodyから対象値を取得します。

```javascript
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
```

続いて、新規ファイルを作成します。

![新規ファイルを作成する](images/chapxx-gaomar/g410.png)

下記コードをコピペしてください。

```javascript
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
```

保存する際はファイル名を「util.js」にしてください。

![ファイル名を「util.js」にする](images/chapxx-gaomar/g411.png)

#### 環境変数を設定する
環境変数を設定していきます。これまでにメモした内容を反映させていきましょう。

|項目|値|
|:--|:--|
|INSTANCEID|1-2でメモしたinstanceのID|
|CONTACTFLOWID|1-2でメモしたcontact-flowのID|
|PHONENUMBER|ご自身の携帯電話番号 ※+81を先頭につけて数字のみにします<br/>例)090-1234-5678 👉+819012345678|
|SOURCEPHONENUMBER|Amazon Connectで取得した電話番号 ※+81を先頭につけて数字のみにします|
|NOTIFY_TOKEN|発行したLINE Notifyのトークンを貼り付ける|

![環境変数を設定する](images/chapxx-gaomar/g412.png)

#### API Gatewayを設定する
LINE ThingsからLambdaをキックするためにエンドポイントURLを発行します。
［トリガーを追加］をクリックします。

![トリガーの追加をクリック](images/chapxx-gaomar/g413.png)

トリガーの設定は下記を指定します。最後に［追加］ボタンをクリックします。

|項目|値|
|:--|:--|
|トリガー|API Gateway|
|API|新規APIの作成|
|セキュリティ|オープン|

![トリガーの設定を行う](images/chapxx-gaomar/g414.png)

APIエンドポイントのURLをメモしておきましょう

![エンドポイントURLをメモ](images/chapxx-gaomar/g415.png)

右上の保存ボタンをクリックします。

![保存ボタンをクリック](images/chapxx-gaomar/g416.png)

### LINE Botを作ろう！
LINE Thingsの連携にはまず、LINE Botを作る必要があります。

#### チャネルの作成
下記にアクセスしてログインしてください。
[https://developers.line.biz/ja/](https://developers.line.biz/ja/)

プロバイダーがまだ無い方は作成お願いします。新規チャネルを作成します。

![新規チャネルを作成をクリック](images/chapxx-gaomar/g500.png)

Messaging APIをクリック

![Messaging APIをクリック](images/chapxx-gaomar/g501.png)

下記設定項目を埋めます。埋めたら［入力内容を確認する］ボタンをクリックします。

|項目|値|
|:--|:--|
|①アプリ名|M5-Security|
|②アプリ説明|M5-Security|
|③大業種|個人|
|③小業種|個人（その他）|
|④メールアドレス|ご自身のメールアドレス|

![設定項目を埋めて、入力内容を確認するボタンをクリック](images/chapxx-gaomar/g502.png)

［同意する］をクリック

![同意するをクリック](images/chapxx-gaomar/g503.png)

2つのチェックを入れてから［作成］をクリックします。

![2つチェックを入れて作成をクリック](images/chapxx-gaomar/g504.png)

作成したチャネルをクリックして、アクセストークンの発行とエンドポイントの設定を行います。

#### アクセストークンを発行する
このチャネルにアクセスするためのトークンを発行します。［再発行］をクリックします。

![再発行をクリック](images/chapxx-gaomar/g505.png)

そのまま[再発行]をクリックします。

![再発行をクリック](images/chapxx-gaomar/g506.png)

発行されたアクセストークンをメモしておきましょう。

![アクセストークンをメモしておく](images/chapxx-gaomar/g507.png)

#### Webhookを設定
Webhookのアクセス先URLを指定します。編集ボタンをクリックします。

![編集ボタンをクリック](images/chapxx-gaomar/g508.png)

メモしたAPI GatewayのURLを貼り付けます。「https://」は既にあるので、省略して貼り付けます。［更新］ボタンをクリックします。

![API GatewayのURLを設定](images/chapxx-gaomar/g509.png)

Webhook送信を利用するに変えます

![利用するに変える](images/chapxx-gaomar/g510.png)

利用するを選択して［更新］ボタンをクリックします。
稀に利用するに変わらないことがあるので、画面をリロードして確認してください。

![利用するになっているか確認する](images/chapxx-gaomar/g511.png)

#### LINE Botと友だちになる
下にスクロールするとQRコードがあるので、LINEアプリからQRを読み取って、必ずBotと友だちになっておいてください。ペアリングする際に友だちになっていないとうまく連携できません。

![Botと友だちになっておく](images/chapxx-gaomar/g512.png)

#### LIFFの設定をする
LIFFアプリを設定します。LIFFタブをクリックして、［LIFFアプリを作成］ボタンをクリックします。

![LIFFアプリを作成をクリック](images/chapxx-gaomar/g513.png)


LIFFの設定を行います。`BLE feature`は必ず「ON」にしてください。

|項目|値|
|:--|:--|
|①名前|M5-Security|
|②サイズ|Full <br/>※どのサイズでもいい|
|③エンドポイントURL|https://www.google.com/ <br/>※とりあえず、どのURLでもいい（後で設定するため）|
|④オプション|必ずONにする|

![LIFFの設定を行う](images/chapxx-gaomar/g514.png)

### LINE Things 自動通信機能サービスUUID発行
