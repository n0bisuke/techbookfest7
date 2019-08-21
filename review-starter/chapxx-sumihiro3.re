
= LINE Pay 決済とLINE Things に対応したドリンクバーを開発する


最近はキャッシュレス決済サービス『○○Pay』 が数多く登場し、大規模なポイントバックキャンペーンが展開されたり、一部のサービスが開始早々にセキュリティ問題が発覚してサービス終了を迎えたりと、良い意味でも悪い意味でも話題になったこともあり、一般消費者にもキャッシュレス決済が認知されてきました。



キャッシュレス決済が一般に認知され、決済のAPI が公開されているのであれば、我々個人開発者も自分のサービスに決済機能を付けて、一儲けとまでは行かなくても運用費＋お小遣いくらいは稼げそうです。



また、キャッシュレス決済サービスの一つ「LINE Pay」は、数あるサービスの中でもAPI が告解されているだけでなく、LINE 自体の国内での月間アクティブユーザー数が8000万人以上、日間・月間アクティブ率が86%と圧倒的なシェアを占めており、周りでもほとんどの人が利用している最早インフラとも言えるプラットフォームであるため、このLINE プラットフォームを利用することで個人開発サービスでも多くの利用者、ひいてはサービス利用料などの決済件数も見込めそうです。



今回は、LINE Pay API と、LINE アプリからハードウェアを操作できるLINE Things、LINE Bot などを開発できるLINE Messaging API を利用したプロトタイプ「LINE Things Drink Bar」を開発しました。


== LINE Things Drink Bar とは


「LINE Things Drink Bar」は、LINE Pay での決済機能と、LINE アプリからハードウェアを操作できるIoT プラットフォームであるLINE Things に対応したジュースの自動販売機のプロトタイプです。いわゆるカップ式自動販売機です。
商品（ジュース）の選択から支払い（LINE Pay での決済）、ハードウェアへのジュース抽出の指示、購入後の抽選に至る全てをLINE アプリ一つで操作できます。



ここまでの話では自動販売機の操作をLINE アプリで出来るようにしただけではないか、と思われるかもしれませんが、LINE に対応することでユーザー個々を識別（LINE の各API ではユーザー固有の識別子 @<b>{userId} が利用）できます。
つまり、ユーザー単位で、いつ・どの商品を、どの自動販売機で購入した（または購入しなかった）かなどが把握できます。さらにLINE Things に対応することで、従来の自動販売機などオフラインのみのサービスでは取得できなかったユーザー単位の情報も取得できるようになります。



LINE Pay とLINE Things などを組み合わせることで、これまでオンラインでのサービス以外では把握が難しかったユーザー単位での利用状況が、自動販売機だけでなく様々なオフラインの場面でも把握できるようになるため、オンラインとオンライン両方のユーザー行動履歴を掛け合わせたレコメンドなど、これまでにないサービスを展開することも可能になります。



本章ではLINE Pay API を使った部分の実装を主眼に解説します。
なお、今回はプロトタイプの実装となります。実際のサービスとしてリリースするには、商品の在庫管理や認証や認可などに関する考慮・実装が必要となりますので、ご承知おきください。



//image[LineThingsDrinkBarOverviewAtMFT2019][LINE Things Drink Barの外観]{
//}




「LINE Things Drink Bar」は、LINE Things とLINE Pay のプロトタイプとして、メイカーズバザール大阪  2019や、Maker Faire Tokyo 2019 などにおいて、LINE 社のブースにて展示・デモ稼働させていただきました。


== システム構成


システム構成は下図のようになっています。
LINE Messaging API やLINE Pay API との連携は、クラウド・アプリケーション・プラットフォームであるHeroku 上に構築したサーバーを介して行います。



LINE アプリ上で実行されるウェブアプリケーションはLINE Front-end Framework（以下、LIFF）として実行されます。ウェブアプリケーションがLIFF として実行されることで、サービスを利用するLINE ユーザーを一意に識別できる @<b>{userId} や表示名などを取得できるようになります。



//image[SystemOverview][LINE Things Drink Bar システム構成図]{
//}




ウェブアプリケーションのフロントサイドはVue.js 、サーバーサイドはウェブアプリケーション フレームワークのFlask で実装します。
商品情報の取得や注文情報の登録はサーバーサイドで提供するAPI を、フロントサイドから実行するという構成になっています。


=== サーバーサイドの構成
 * Heroku
 ** Docker
 *** 実装したウェブアプリケーションをDocker イメージでHeroku にデプロイする
 ** Heroku Postgres
 *** PostgreSQL をHeroku アプリケーションから利用できるHeroku のアドオン
 **** 無料枠あり
 ** QuotaGuard Static
 *** 固定IPで外部アクセスするためのプロキシを利用できるHeroku のアドオン
 **** 無料枠あり
 **** LINE Pay API アクセス時に固定IPアドレスが必要なため
 **** なお、LINE Pay API Sandbox 環境では固定IP は必須ではない
 * Python
 ** Flask
 ** SQL Alchemy
 ** Requests
 * LINE Messaging API
 ** LINE Messaging API 用SDK のPython版
 * LINE Pay API
 ** LINE Pay API のSandbox 環境


=== フロントサイドの構成
 * Vue.js
 ** Vuetify
 *** Vue.js でマテリアルデザインのコンポーネントを簡単に使えるコンポーネントフレームワーク
 * LIFF (LINE Front-end Framework)
 ** LINE アプリ内で動作するウェブアプリのプラットフォーム
 * LINE Things
 ** LINE アプリを介して、チャネルとBluetooth® Low Energy 対応デバイスを連携し、操作を可能にするIoT プラットフォーム


=== ハードウェアの構成
 * LINE Things development board
 ** Nordic Semiconductor nRF52832 を搭載したLINE Things 実験用ボード
 * モータードライバー
 * エアーポンプ
 ** ドリンクの抽出用
 * PET ボトル
 * 電源
 ** モバイルバッテリーLINE Things development board 用
 ** 9V 電池エアーポンプ用



//image[LineThingsDrinkBarInside][LINE Things Drink Bar ハードウェア部分]{
//}



=== ソースコード


LINE Things Drink Bar で使用しているソースコードは、本章では紙面の都合によりすべて掲載できませんので、Github のリポジトリで公開しています。
本章と合わせて確認してください。

 * LINE Things Drink Bar のソースコード
 ** https://github.com/sumihiro3/LineThingsDrinkBar


== LINE Pay APIを使った決済の流れ


LINE Pay での決済には3つの登場人物が存在します。一つ目は @<strong>{サービスプロバイダー} です。これは有償で商品を販売またはサービスを提供する事業主（おそらくあなた）で、実質的に何らかのアプリとなります。本書ではLINE Things Drink Bar が相当します。



二つ目は その商品またはサービスを購入する @<strong>{ユーザー} です。



そして三つ目は @<strong>{LINE Pay}です。サービスプロバイダーはLINE PayのAPIに、ユーザーはLINE Payのアプリにアクセスして下記の流れで決済をおこなうことになります。



//image[LINEPayAPIOverview][LINE Pay API 概要図]{
//}


 * SP = サービスプロバイダー



以下では決済の処理内容について説明します。


=== 決済予約


サービスプロバイダーは商品、金額など決済情報を決済予約のAPI（Reserve API） に送信し、決済URLを取得します。


=== ユーザーによる承認


取得した決済URL をユーザーに提供し、ユーザーは決済URL に進みます。LINE Payが起動して商品と金額が表示され、ユーザーはその情報を確認の上、決済承認をおこないます。


=== 決済実行


ユーザーが承認すると、任意のURL へのリダイレクトまたは任意のURL へのPOST リクエストにてサービスプロバイダーに通知されます。その時点で決済を実行できる状態となっているので、あとは決済実行のAPI（Confirm API）に アクセスすれば決済完了となります。


== LINE Things Drink Bar での購入・決済の流れ


ユーザーはLINE アプリで次のような操作をしてドリンクを購入します。

 1. ユーザーは、自動販売機の近くでLINE アプリを起動する
 1. ユーザーは、購入する商品（ドリンク）を選択する
 1. LINE Things Drink Bar は、注文情報を登録し決済予約を行う
 1. ユーザーは、LINE Pay 決済画面で購入内容を確認して決済を実行する
 1. LINE Things Drink Bar は、決済完了後にドリンクを抽出する
 1. ユーザーは、抽選ボタンを押下して購入後のお楽しみ抽選を行う



//image[LineThingsDrinkBarFlow][LINE Things Drink Bar での購入・決済の流れ]{
//}



== LINE Pay の導入


ここからはLINE Pay を開発するサービスで導入するための作業について説明します。
作業の流れは以下のとおりです。

 1. 加盟店申請
 1. LINE Pay Sandbox での開発
 1. 本番設定＆リリース



今回は、これらのうち「1. 加盟店申請」と「2. LINE Pay Sandbox での開発」を中心に説明します。


=== LINE Pay の加盟店申請

==== 個人でも加盟店申請できる


LINE Pay の加盟店になるには法人でなくとも大丈夫です。個人で加盟店申請するには、個人事業主として開業届を出して青色申告をしてあればOKです。
LINE Pay 公式の「よくある質問」にも、個人事業主でも加入できると書いてあります。

 * 個人事業主も加入できますか？：よくある質問＠LINE Pay
 ** https://pay.line.me/jp/intro/faq?locale=ja_JP&sequences=14



加盟店申請とは関係ないですが、青色申告をしていれば、していない場合（白色申告）に比べて所得の控除額が大きく、減価償却の特例が受けられる（30万円未満の固定資産を一括償却できる。上限あり）など税金面での優遇がありますので、副業などをしている方は青色申告しておくのをお勧めします。


==== 青色申告するには


本章では詳細な申告方法については触れませんが、開業届と青色申告承認申請書のフォーマットに沿って記載して、所管の税務署に提出するだけです。
申告書類を簡単に作成できるサービスもたくさんあるので、検索してみてください。


==== LINE Pay 加盟店申請に必要な書類など


いよいよLINE Pay 加盟店申請ですが、個人で申請する際に必要な書類などがあるので揃えておきましょう。

 * 開業届または確定申告書の控え
 * 本人確認書類（運転免許証など）
 ** 運転免許証の場合、「運転免許証：変更事項があれば裏面含みます」と記載されていますが、裏面に記載がなくても裏表の提出が求められたので注意してください
 * Web サイト



特別なものはありませんね。それではいよいよ加盟店申請です。



//image[LINEPayAppForMenber][LINE Pay 加盟店申請ページ]{
//}




LINE Pay 加盟店申請ページ上部の「申請」リンクをクリックして、加盟店申請フォームを開いてください。

 * LINE Pay 加盟店申請ページ
 ** https://pay.line.me/jp/intro?locale=ja_JP


==== 加盟店申請


個人でLINE Pay API を使う加盟店申請を行うのであれば、加盟店申請フォームで以下のように選択して申請を進めます。

 * 事業種別：個人事業主
 * 支払い方法：オンライン



//image[LINEPayAppForMember][加盟店申請フォーム]{
//}




あとは、規約を読んで同意し、必要書類をスキャンしてPDF ファイルを作成し、スキャンしたファイルを加盟店申請フォームから必要事項の記入とともにアップロードするだけです。


==== 審査と登録完了通知


申請書類の不備などが二度ありましたが、約2週間で審査完了しました。審査がスムーズなのも時間がない個人としては嬉しいところです。



審査が完了すると、下のようなメールが届きます。
//image[ReviewCompleted][加盟店審査完了メール]{
//}



=== LINE Pay Sandboxの申請と設定


実際に決済するには加盟店登録が必要ですが、開発して動作を確認するフェーズであればSandbox が利用できます。こちらは下記のURLから申請すると、Sandbox 用のLINE Pay API アカウントが払い出されますので、誰でもすぐに利用できます。

 * LINE Pay Sandboxの申請
 ** https://pay.line.me/jp/developers/techsupport/sandbox/creation?locale=ja_JP



アカウントが払い出されたらLINE Pay コンソールの決済連動管理 > 連動キー管理からChannel ID とChannel Secret Key を確認します。これらの値はLINE Pay のAPI コールに必要になります。
//image[LinkKey][連動キー管理]{
//}



== 開発

=== サーバー（Heroku）の準備


ここまでは事務的な作業でしたが、ここから本筋の開発に関する内容です。


==== Heroku CLI のインストール


@<href>{https://jp.Heroku.com/,Heroku} へのアプリケーションやアドオンの登録・設定をターミナルなどのCLI から操作できるHeroku CLI をインストールします。
Mac で開発する場合はbrew を使ってインストールできます。


//emlist[][bash]{
$ brew tap Heroku/brew && brew install Heroku
//}

==== アプリケーションの作成


ここからHeroku CLI を使ってアプリケーションの登録などをしていきます。



まずはログイン。ログインコマンドを入力すると、ブラウザでHeroku のログインページが表示されるのでログインします。


//emlist[][bash]{
$ Heroku login
Heroku: Press any key to open up the browser to login or q to exit: 
Opening browser to https://cli-auth.Heroku.com/auth/browser/xxxxxxxxx
Logging in... done
Logged in as xxxx@xxxx.com
//}


続けてアプリケーションを作成します。

 * 「YOUR@<b>{APP}NAME」 にはHeroku 全体で一意となるアプリケーション名を指定します。


//emlist[][bash]{
$ Heroku create {YOUR_APP_NAME}
Creating {YOUR_APP_NAME}... done
https://{YOUR_APP_NAME}.Herokuapp.com/ | https://git.Heroku.com/{YOUR_APP_NAME}.git
//}

==== データベースの登録


Heroku CLI でHeroku Postgres アドオンのデータベース登録します。


//emlist[][bash]{
$ Heroku addons:create Heroku-postgresql:hobby-dev -a {YOUR_APP_NAME}
Creating Heroku-postgresql:hobby-dev on {YOUR_APP_NAME}... free
Database has been created and is available
 ! This database is empty. If upgrading, you can transfer
 ! data from another database with pg:copy
Created postgresql-reticulated-xxxxx as DATABASE_URL
Use Heroku addons:docs Heroku-postgresql to view documentation
//}

==== 固定IPでのアクセス用プロキシの登録


Heroku CLI でQuotaGuard Static アドオンの固定IPでのアクセス用プロキシを設定します。


//emlist[][bash]{
$ Heroku addons:create quotaguardstatic:starter -a {YOUR_APP_NAME}
Creating quotaguardstatic:starter on {YOUR_APP_NAME}... free
Your static IPs are [xx.xxx.xx.xx, xxx.xxx.xxx.xxx]
Created quotaguardstatic-clean-xxxxx as QUOTAGUARDSTATIC_URL
Use Heroku addons:docs quotaguardstatic to view documentation
//}


設定が完了すると環境変数「QUOTAGUARDSTATIC_URL」にプロキシURL が自動で設定されます。


=== コンテナの準備


Heroku にはDocker を使ってコンテナでウェブアプリケーションをデプロイします。


==== Dockerfile


コンテナのビルド時に実行する処理をDockerfile で設定します。

 * Python 3.6 のイメージを使用
 * Python からPostgreSQL を使うために関連のモジュールをインストール
 * 「requirements.txt」に記載したサーバーサイドのPython プログラムで使うライブラリをインストール
 * プログラムのコード類をコンテナ内にコピー
 * ウェブアプリケーションを起動


//emlist{
FROM python:3.6-alpine

RUN apk update && apk add postgresql-dev gcc python3-dev musl-dev

# 作業ディレクトリの作成
RUN mkdir -p /app

# 依存ライブラリのインストール
ADD requirements.txt /tmp
RUN pip install --no-cache-dir -q -r /tmp/requirements.txt

# アプリケーションコード類をイメージ内にコピー
ADD ./app /app
WORKDIR /app

# Flask の起動 
# CMD is required to run on Heroku
ENV FLASK_APP /app/main.py
CMD flask run -h 0.0.0.0 -p $PORT --debugger --reload
//}

==== requirements.txt


Python プログラムで使うライブラリはrequirements.txt に定義します。



ウェブアプリケーション フレームワークに「Flask」、データベース（PostgreSQL）へのアクセス用に「psycopg2」、ORマッパーとして「SQLAlchemy」を使っています。また、LINE Pay API への接続には「requests」を利用します。


//emlist{
flask
SQLAlchemy
flask_sqlalchemy
Jinja2
psycopg2
requests
line-bot-sdk
//}

=== ドリンクバー画面


商品を選択するなど「LINE Things Drink Bar」のメインとなる画面です。



ドリンクバー画面をはじめフロント側はVue.js で作成しています。今回の解説では画面数も少ないのでコンポーネント化はせずに、HTML ファイルベースで開発します。また、この画面をLIFF として登録しておきます。



この画面で出来ることは以下のとおりです。

 * ドリンク一覧を表示する
 * 選択した商品を基に注文情報を登録する
 * LINE Pay 決済画面へ遷移する
 * LINE Things Drink Bar を制御する


=== ドリンク一覧の表示


LINE アプリのリッチメニューからドリンクバー画面にアクセスするとを選択すると、Flask の下記関数にGET リクエストが送られます。



このリクエストを受けたプログラム（main.py）がテンプレートエンジン（Jinja2）でドリンクバー画面のHTML ファイル（drink_bar.html）を返します。


//emlist[][python]{
@app.route('/drink_bar', methods=['GET'])
def get_drink_bar():
    # ドリンクバー画面を表示
    return render_template(
        'drink_bar.html',
    )
//}

==== HTML ファイル（drink_bar.html）


ドリンクバー画面のHTML ファイルです。
ドリンク一覧や、選択したドリンクの詳細と決済へ進むボタン、ドリンクの抽出状況などを表示します。
それぞれの表示は処理のステータスを示す @<b>{flow_status} の状態に応じて切り替えています。



下記では、ドリンク一覧を表示する部分を抜粋しています。


//emlist[][html]{
<!--    No order - Item list     -->
<v-container fluid v-else-if="flow_status === 'INITIAL'">
    <v-layout>
        <v-flex xs12>
            <v-toolbar color="green darken-2" dark>
                <v-toolbar-title>LINE Things Drink Bar</v-toolbar-title>
            </v-toolbar>
            <!-- 商品一覧 -->
            <v-card>
                <v-container
                        fluid
                        grid-list-lg
                >
                    <v-layout row wrap>
                        <v-flex xs12 v-for="(item, index) in items">
                            <v-card color="brown lighten-5" class="black--text">
                                <v-layout pt-1>
                                    <v-flex xs5>
                                        <v-img
                                                :src="item.image_url"
                                                height="125px"
                                                contain
                                        ></v-img>
                                    </v-flex>
                                    <v-flex xs7>
                                    <!-- 商品情報 -->
                                        <v-card-title primary-title>
                                            <div>
                                                <div><h4 class="pb-1">[[ item.name ]]</h4></div>
                                                <div><h6 class="pb-2">[[ item.description ]]</h6></div>
                                                <div><h4 class="green--text">[[ item.unit_price ]] 円</h4></div>
                                            </div>
                                        </v-card-title>
                                    </v-flex>
                                </v-layout>
                                <v-divider light></v-divider>
                                <v-card-actions class="pa-3">
                                    <v-btn
                                            @click="orderItem(item.id)"
                                            class="white--text"
                                            color="brown" block>
                                        Order
                                    </v-btn>
                                </v-card-actions>
                            </v-card>
                        </v-flex>
                    </v-layout>
                </v-container>
            </v-card>
        </v-flex>
    </v-layout>
</v-container>
//}

==== 商品情報の取得処理


ドリンクバー画面で表示する商品情報をサーバーサイドから取得します。
画面の初期化時にaxios を利用して、サーバーサイドで提供される商品情報取得APIを実行します。



下記では、商品情報取得API の実行部分（drink_bar.js）を抜粋しています。


//emlist[][javascript]{
getItems: async function() {
    console.log('function getItems called!')
    this.api_loading = true
    const api_url = '/api/items'
    // 商品情報取得API 実行
    const response = await axios.get(api_url).catch(error => {
        console.error('API getItems failed...')
        console.error(error)
        this.api_result = null
        this.api_loading = false
    })
    console.log('API response: ', response)
    // API 実行結果をVue.js の表示用変数へ
    this.api_result = response.data
    this.items = this.api_result.items
    this.api_loading = false
},
//}


サーバーサイドで提供される商品情報取得API を実行し、取得した商品情報をVue.js の表示用変数に割り当てます。



なお、LIFF 開発でデバッグなどに利用できる便利なツール「vConsole」があります。導入方法などは下記URL の記事を参照してください。

 * vConsole を使ってLIFF(LINE Front-end Framework) の開発を加速する
 ** https://qiita.com/sumihiro3/items/9f4f1adb5d8883d9ceeb


==== 商品情報取得API


ドリンクバー画面から呼び出される商品情報取得API です。（main.py）
サーバーサイドではSQLAlchemy を使ってデータベースから商品情報を取得して返しています。


//emlist[][python]{
@app.route('/api/items', methods=['GET'])
def get_items():
    # DB から商品情報を取得
    item_list = Item.query.filter(Item.active == True).all()
    app.logger.debug(item_list)
    items = []
    for i in item_list:
        item = {
            'id': i.id,
            'name': i.name,
            'description': i.description,
            'unit_price': i.unit_price,
            'stock': i.stock,
            'image_url': i.image_url
        }
        app.logger.debug(item)
        items.append(item)
    # 販売可能な商品一覧を返す
    app.logger.debug(items)
    return jsonify({
        'items': items
    })
//}

==== 画面表示


フロントサイドから正常に商品情報取得API を実行できると、下図のようなドリンク一覧が表示されます。



//image[ItemList][ドリンク一覧]{
//}



=== 注文情報の登録


ドリンク一覧で選択したドリンクを基に注文情報を登録します。


==== 注文情報の登録処理


選択したドリンクを基にサーバーサイドへ注文情報の登録をリクエストします。
サーバーサイドで提供される注文情報登録API を実行します。



下記では、注文情報登録API の実行部分（drink_bar.js）について抜粋しています。


//emlist[][javascript]{
orderItem: async function(item_id) {
    console.log('function orderItem called!')
    this.api_loading = true
    // 注文情報
    let order_item_ids = [item_id]
    const params = {
        user_id: this.line_user_id,
        order_items: order_item_ids
    }
    const url = '/api/purchase_order'
    // 注文情報登録API 実行
    const response = await axios.post(url, params).catch(function (err) {
        this.api_loading = false
        console.error('API POST PurchaseOrder failed', err)
        this.flow_status = 'INITIAL'
        throw err
    })
    console.log('Response: ', response)
    // API 実行結果をVue.js の表示用変数へ
    this.api_result = response.data
    this.order.id = this.api_result.order_id
    this.order.title = this.api_result.order_title
    this.order.amount = this.api_result.order_amount
    this.order.slot = this.api_result.order_item_slot
    this.order.ordered_item_image_url = this.api_result.ordered_item_image_url
    this.flow_status = 'ORDERED'
    this.api_loading = false
},
//}


サーバーサイドで提供される注文情報登録API を実行し、取得した注文情報をVue.js の表示用変数に割り当てます。


==== 注文情報登録API


ドリンクバー画面から呼び出される注文情報登録API です。（main.py）


//emlist[][python]{
@app.route('/api/purchase_order', methods=['POST'])
def post_purchase_order():
    app.logger.info('handler post_purchase_order called!')
    request_dict = request.json
    user_id = request_dict.get('user_id', None)
    user = User.query.filter(User.id == user_id).first()
    # ユーザーが登録されていなければ新規登録
    if user is None:
        user = add_user(user_id)
    order_items = request_dict.get('order_items', [])
    order_item_list = Item.query.filter(Item.id.in_(order_items))
    app.logger.debug('order_item_list: %s', order_item_list)
    # // API 実行結果をVue.js の表示用変数へ
    order = add_purchase_order(user, order_item_list)
    ordered_item = Item.query.filter(Item.id == order.details[0].item_id).first()
    # return
    return jsonify({
        'order_id': order.id,
        'order_title': order.title,
        'order_amount': order.amount,
        'order_item_slot': ordered_item.slot,
        'ordered_item_image_url': ordered_item.image_url
    })
//}


データベースに注文情報を登録する部分はadd@<b>{purchase}order 関数で行います。
なお、LINE Pay での決済では注文IDは重複不可のため、一意となるIDを生成しておく必要があります。


//emlist[][python]{
def add_purchase_order(user, order_items):
    # 一意な注文IDを生成する
    order_id = uuid.uuid4().hex
    timestamp = int(dt.now().timestamp())
    details = []
    amount = 0
    # 注文情報を生成
    for item in order_items:
        detail = PurchaseOrderDetail()
        detail.id = order_id + '-' + item.id
        detail.unit_price = item.unit_price
        detail.quantity = 1
        detail.amount = item.unit_price * detail.quantity
        detail.item = item
        detail.created_timestamp = timestamp
        db.session.add(detail)
        details.append(detail)
        amount = amount + detail.amount
    # 注文情報をDBに登録する
    order_title = details[0].item.name
    if len(details) > 1:
        order_title = '{} 他'.format(order_title)
    order = PurchaseOrder(order_id, order_title, amount)
    order.user_id = user.id
    order.details.extend(details)
    db.session.add(order)
    db.session.commit()
    return order
//}

==== 注文情報の表示


フロントサイドから正常に注文情報登録API を実行できると、下図のような注文情報が表示されます。



//image[PurchaseOrder][注文情報]{
//}



=== 決済予約の実行


注文を確定し、LINE Pay の決済予約を実行します。


==== 決済予約の実行処理


ユーザーが注文情報を確認するとLINE Pay での決済に移ります。注文情報の下部に表示される”PAY BY LINE PAY”ボタンを押下すると、サーバーサイドへ決済予約処理をリクエストします。
axios を利用して、サーバーサイドで提供される決済予約API を実行します。



下記では、決済予約API の実行部分（drink_bar.js）について抜粋しています。


//emlist[][javascript]{
payReserve: async function() {
    console.log('function pay_reserve called!')
    // 決済予約
    this.api_loading = true
    const params = {
        user_id: this.line_user_id,
        order_id: this.order.id
    }
    // 決済予約API の実行
    const url = '/pay/reserve'
    const response = await axios.post(url, params).catch(function (err) {
        this.api_loading = false
        console.error('API POST PayReserve failed', err)
        this.flow_status = 'INITIAL'
        throw err
    })
    console.log('Response: ', response)
    this.api_result = response.data
    const payment_url = this.api_result.payment_url
    this.flow_status = 'PAYING'
    // LINE Pay の決済画面へ移動
    window.location.href = payment_url
    this.api_loading = false
},
//}


サーバーサイドで提供される決済予約API を実行し、決済予約API から返されるLINE Pay の決済実行URL に遷移します。


==== 注文情報登録API


ドリンクバー画面から呼び出される決済予約API です。（main.py）


//emlist[][python]{
@app.route("/pay/reserve", methods=['POST'])
def handle_pay_reserve():
    request_dict = request.json
    user_id = request_dict.get('user_id', None)
    order_id = request_dict.get('order_id', None)
    # 注文情報とユーザー情報をデータベースから取得する
    order = PurchaseOrder.query.filter(PurchaseOrder.id == order_id).first()
    user = User.query.filter(User.id == user_id).first()
    ordered_item = Item.query.filter(Item.id == order.details[0].item_id).first()
    app.logger.debug('Ordered Item: %s', ordered_item)
    # LINE Pay の決済予約API を実行してtransactionId を取得する
    response = pay.reserve_payment(order, product_image_url=ordered_item.image_url)
    transaction_id = response["info"]["transactionId"]
    # 取得したtransactionId を注文情報に設定してデータベースを更新する
    order.user_id = user.id
    order.transaction_id = transaction_id
    db.session.commit()
    db.session.close()
    payment_url = response["info"]["paymentUrl"]["web"]
    # LINE Pay の決済実行URL をフロントに返す
    return jsonify({
        'payment_url': payment_url
    })
//}


リクエストされた注文情報を基にLINE Pay の決済予約処理を実行します。
LINE Pay の決済予約処理はLinePay クラス（line@<b>{pay.py）のreserve}payment メソッドで行います。
商品、金額など決済情報をLINE Pay の決済予約API（Reserve API）に送信し、決済URLを取得します。


//emlist[][python]{
def reserve_payment(
        self,
        purchase_order,
        product_image_url=LINE_PAY_BOT_LOGO_URL,
        mid=None,
        one_time_key=None,
        delivery_place_phone=None,
        pay_type='NORMAL',
        lang_cd=None,
        capture=True,
        extras_add_friends=None,
        gmextras_branch_name=None):
    # LINE Pay の決済予約処理を実行
    line_pay_url = self.__line_pay_url
    line_pay_endpoint = f'{line_pay_url}/v2/payments/request'
    order_id = purchase_order.id
    body = {
        'productName': purchase_order.title,
        'amount': purchase_order.amount,
        'currency': purchase_order.currency,
        'confirmUrl': self.__confirm_url,
        'confirmUrlType': self.__confirm_url_type,
        'checkConfirmUrlBrowser': self.__check_confirm_url_browser,
        'orderId': order_id,
        'payType': pay_type,
        'capture': capture,
    }
    if product_image_url is not None:
        body['productImageUrl'] = product_image_url
    if mid is not None:
        body['mid'] = mid
    if one_time_key is not None:
        body['oneTimeKey'] = one_time_key
    if self.__cancel_url is not None:
        body['cancelUrl'] = self.__cancel_url
    if delivery_place_phone is not None:
        body['deliveryPlacePhone'] = delivery_place_phone
    if lang_cd is not None:
        body['langCd'] = lang_cd
    if extras_add_friends is not None:
        body['extras.addFriends'] = extras_add_friends
    if gmextras_branch_name is not None:
        body['gmextras.branchName'] = gmextras_branch_name
    # API 実行
    response = requests.post(
        line_pay_endpoint,
        json_util.dump_json(body).encode('utf-8'),
        headers=self.__headers,
        proxies=self.__proxies
    )
    return response.json()
//}


LINE Pay の決済予約API のパラメータについては、LINE Pay 技術ドキュメントをご確認ください。

 * LINE Pay技術連動ガイド
 ** https://pay.line.me/jp/developers/documentation/download/tech?locale=ja_JP


==== LINE Pay の決済承認画面


決済予約処理が正常に完了すると、下図のようにLINE Pay の決済承認画面が表示されます。この画面はLINE Pay プラットフォーム側の画面となります。
注文情報（注文名や決済する金額）、指定した画像と決済承認ボタンが表示されています。



//image[PayByLinePay][決済承認画面]{
//}




「PAY NOW」と表示された決済承認ボタンを押下すると決済承認処理が行われ、決済承認完了画面が表示されます。この後、開発したサービス（LINE Things Drink Bar）のサーバーへ処理が返ります。



//image[PayCompleted][決済承認完了]{
//}



=== 決済実行処理の実行


LINE Pay 側からサーバーサイドの決済実行処理が呼び出されますので、LINE Pay の決済実行API を呼び出して決済を完了させます。
決済実行API が正常に完了すれば、注文情報のステータスを決済完了に更新します。


//emlist[][python]{
@app.route("/pay/confirm", methods=['GET'])
def handle_pay_confirm():
    # 決済承認完了後、LINE Pay 側から実行される 
    transaction_id = request.args.get('transactionId')
    order = PurchaseOrder.query.filter_by(transaction_id=transaction_id).one_or_none()
    if order is None:
        raise Exception("Error: transaction_id not found.")
    # LINE Pay の決済実行API を実行
    response = pay.confirm_payments(order)
    app.logger.debug('returnCode: %s', response["returnCode"])
    app.logger.debug('returnMessage: %s', response["returnMessage"])
    # 注文情報の決済ステータスを完了にする
    order.status = PurchaseOrderStatus.PAYMENT_COMPLETED.value
    db.session.commit()
    db.session.close()
    # ドリンクバー画面を表示
    return render_template(
        'drink_bar.html',
        message='Payment successfully completed.',
        transaction_id=transaction_id
    )
//}

==== LINE Pay API 決済実行


LINE Pay の決済実行API を呼び出す部分です。
API の各種パラメータを設定して決済実行APIのURL（/v2/payments/{transaction_id}/confirm）へPOSTリクエストを送ります。



API のパラメータについては、LINE Pay 技術ドキュメントをご確認ください。

 * LINE Pay技術連動ガイド
 ** https://pay.line.me/jp/developers/documentation/download/tech?locale=ja_JP


//emlist[][python]{
    def confirm_payments(self, purchase_order):
        # LINE Pay の決済実行処理を実行
        line_pay_url = self.__line_pay_url
        line_pay_endpoint = f'{line_pay_url}/v2/payments/{purchase_order.transaction_id}/confirm'
        body = {
            'amount': purchase_order.amount,
            'currency': purchase_order.currency,
        }
        # 決済実行API を実行
        response = requests.post(
            line_pay_endpoint,
            json_util.dump_json(body).encode('utf-8'),
            headers=self.__headers,
            proxies=self.__proxies
        )
        return response.json()
//}


決済確認が完了するとLINE Things Drink Bar に対してドリンクを抽出する操作を行います。



//image[DispensingDrink][ドリンク抽出]{
//}




ドリンク抽出後はお楽しみ抽選が実行され、確率で当たりか、はずれが決定されます。



//image[FukubikiDone][抽選完了]{
//}




以上でLINE Pay 決済に関わる部分の実装が完了します。
本章では決済完了後のLINE Things でのDrink Bar の制御部分や、購入後のお楽しみ抽選に関するコードの解説は行いませんが、前述の通りソースコードをGithub のリポジトリで公開していますので、そちらをご覧ください。


== Heroku へのデプロイ


実装が完了したので、Heroku にLINE Things Drink Bar のプログラムをデプロイします。


=== 環境変数の設定


デプロイの前にLINE Things Drink Bar で使用する環境変数をHeroku に登録しておきます。


==== LINE Bot のChannel Secretとアクセストークン


これらの情報はLINE Developers の画面から取得できます。


//emlist[][bash]{
$ Heroku config:set LINEBOT_CHANNEL_ACCESS_TOKEN=xxxxxxxxxxxxxxxxxxx -a {YOUR_APP_NAME}
$ Heroku config:set LINEBOT_CHANNEL_SECRET=xxxxxxxxxx -a {YOUR_APP_NAME}
//}

==== LINE Pay API のChannel ID など


これらの情報はLINE Pay API Sandbox の画面から取得できます。



LINE@<b>{PAY}CONFIRM_URL は、LINE Pay での決済承認後、自分のサーバーに返ってくるURLを指定します。


//emlist[][bash]{
$ Heroku config:set LINE_PAY_URL=https://sandbox-api-pay.line.me -a {YOUR_APP_NAME}
$ Heroku config:set LINE_PAY_CHANNEL_ID=xxxxxxxxx -a {YOUR_APP_NAME}
$ Heroku config:set LINE_PAY_CHANNEL_SECRET=xxxxxxxxxxxx -a {YOUR_APP_NAME}
$ Heroku config:set LINE_PAY_CONFIRM_URL=https://{YOUR_APP_NAME}/pay/confirm -a {YOUR_APP_NAME}
//}

==== psycopg2 でのアクセスで使用するデータベスURL


SQL Alchemy でのデータベース接続情報はHeroku のHeroku Postgres 設定画面などで確認できます。


//emlist[][bash]{
$ Heroku config:set POSTGRESQL_DATABASE_URL=postgres+psycopg2://{DB_USER_ID}:{DB_USER_PASSWORD}@{DB_SERVER_URL}:5432/{DB_NAME} -a {YOUR_APP_NAME}
//}

==== 固定IPアクセス用のプロキシURL


固定IPアクセス用のプロキシURL に関する環境変数は、QuotaGuard Static アドオン登録時に登録されますので追加設定は不要です。


=== コンテナのデプロイ


いよいよLINE Things Drink Bar のプログラムをHeroku にリリースします。



Heroku のコンテナサービスにログインして、コンテナイメージをpush、Heroku アプリケーションとしてリリースします。


//emlist[][bash]{
# Heroku container にログイン
$ Heroku container:login
Login Succeeded

# Heroku container にDocker イメージをPush
$ Heroku container:push -a {YOUR_APP_NAME} web
Your image has been successfully pushed. You can now release it with the 'container:release' command.

# ウェブアプリケーションをリリース
$ Heroku container:release -a {YOUR_APP_NAME} web
Releasing images web to {YOUR_APP_NAME}
//}


これでデプロイ完了です！


== 最後に


基本的なLINE Pay 決済機能の組み込みは以上です。
サービスとして公開するにはキャンセル処理なども実装する必要がありますが、他APIの呼び出しについても同様にAPI ドキュメントに沿ってパラメータを指定するだけです。



また、今回はLINE Bot への組み込みで解説しましたがウェブアプリケーションへの組み込みもほぼ同じです。



LINE Pay での決済機能を利用するのも良いですが、多くのユーザーが利用するLINE の特性を活かしてLINE Messaging API や、LINE Things などを組み合わせたWeb の世界以外とも連携したサービスを開発してはいかがでしょうか。



なお、LINE Things や、LIFF などの基本的な説明や、設定手順については本章では触れていませんが、他の章でLINE API Expert のみなさんが解説していますので、そちらを参考にしてください。

