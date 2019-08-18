
= LINE Pay での決済とLINE Things に対応したドリンクバーのプロトタイプを開発する


最近は『○○Pay』 がたくさん登場し、大規模なポイントバックキャンペーンが展開されたり、一部のキャッシュレス決済サービスがサービス開始早々にセキュリティ問題が発覚してサービス終了になったりと、良い意味でも悪い意味でも話題になったこともあり、一般消費者にもキャッシュレス決済が認知されてきました。



キャッシュレス決済が一般に認知され、決済のAPI が公開されているのであれば、我々個人開発者も自分のサービスに決済機能を付けて、一儲けとまでは行かなくても運用費＋お小遣いくらいは稼げるのではないか、ということでAPI が公開されているLINE Pay API と、LINE アプリからハードウェアを操作できるLINE Things、LINE Bot などを開発できるLINE Messaging API を使ったプロトタイプ「LINE Things Drink Bar」を開発しました。


== LINE Things Drink Bar とは


「LINE Things Drink Bar」は、LINE Pay での決済機能と、LINE アプリからハードウェアを操作できるIoT プラットフォームであるLINE Things に対応した自動販売機のプロトタイプです。
いわゆるカップ式自動販売機で、商品（ジュース）選択から支払い（LINE Pay での決済）、ハードウェアへのジュース抽出の指示、購入後の抽選に至る全てを日常で頻繁に使うLINE アプリ一つで操作することができます。



ここまでの話だけでは自動販売機の操作をLINE アプリで出来るようにしただけではないか、と思われるかもしれませんが、LINE 対応することでユーザー個々を識別できる（LINE の各API ではユーザー固有の識別子 @<b>{userId} が利用できる）ます。
つまり、ユーザー単位で、いつ・どの商品を、どの自動販売機で購入した（または購入しなかった）かなどが把握できることになります。従来の自動販売機では取得できなかったユーザー単位の情報が取得できることで、ユーザー個別に販売促進（おすすめ）なども行えるようになります。



LINE Pay とLINE Things などを組み合わせることで、これまでネットサービス以外では把握が難しかったユーザー単位での利用状況が、自動販売機だけでなく色んな場面で把握できるようになるため、ユーザーの利用状況を踏まえたサービスを展開することも可能になります。



なお、今回掲載する内容はLINE Pay API を使った実装の解説を主眼にしたプロトタイプです。
実際のサービスとしてリリースするには、商品の在庫管理や認証や認可などに関する考慮・実装が必要となりますので、ご承知おきください。



//image[LineThingsDrinkBarOverview][LINE Things Drink Bar]{
//}



== システム構成


ここまでは事務的な作業でしたが、ココから本筋の開発に関する内容です。



システム構成は下図のようになっています。
LINE Messaging API やLINE Pay API との連携はheroku 上に構築したサーバーを介して行います。
LINE アプリ上で実行されるウェブアプリケーションはLINE Front-end Framework（以下、LIFF）として実行されます。LIFF としてウェブアプリケーションが実行されることで、操作するLINE ユーザーを一意に識別できる @<b>{userId} や表示名などを取得できるようになります。



//image[SystemOverview][LINE Things Drink Bar システム構成図]{
//}




フロントサイドはVue.js 、サーバーサイドはウェブアプリケーション フレームワークのFlask で実装します。
商品情報の取得や注文情報の登録はサーバーサイドで提供するAPI をフロントからaxios モジュールを利用して実行するという構成になっています。


=== サーバーサイドの構成
 * heroku
 ** Docker
 ** Python
 *** Flask
 *** SQL Alchemy
 *** Requests
 ** Heroku Postgres
 *** PostgreSQL をheroku アプリケーションから利用できるheroku のアドオン
 **** 無料枠あり
 ** QuotaGuard Static
 *** 固定IPで外部アクセスするためのプロキシ を利用できるheroku のアドオン
 **** 無料枠あり
 **** LINE Pay API アクセス時に固定IPアドレスが必要なため
 **** なお、LINE Pay API Sandbox 環境では固定IP は必須ではない
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


=== ソースコード


LINE Things Drink Bar で使用しているソースコードは、本書では紙面の都合によりすべて掲載できませんので、Github のリポジトリで公開しています。
本書と合わせて確認してください。

 * LINE Things Drink Bar のソースコード
 ** https://github.com/line-developer-community/techbookfest7


== LINE Pay APIを使った決済の流れ


LINE Pay での決済には3つの登場人物が存在します。一つ目は @<strong>{サービスプロバイダー} です。これは有償で商品またはサービスを提供する事業主（おそらくあなた）で、実質的に何らかのアプリとなります。本書ではLINE Things Drink Bar が相当します。
二つ目は その商品またはサービスを購入する @<strong>{ユーザー} です。
そして三つ目は @<strong>{LINE Pay}です。サービスプロバイダーはLINE PayのAPIに、ユーザーはLINE Payのアプリにアクセスして下記の流れで決済をおこなうことになります。



//image[LINEPayAPIOverview][LINE Pay API 概要図]{
//}


 * SP = サービスプロバイダー



以下では決済の処理内容について説明します。


=== 決済予約


サービスプロバイダーは商品、金額など決済情報を決済予約のAPI（Reserve API）に送信し、決済URLを取得します。


=== ユーザーによる承認


取得した決済URLをユーザーに提供し、ユーザーが決済URLに進みます。LINE Payが起動して商品と金額が表示され、ユーザーはその情報を確認の上、決済承認をおこないます。


=== 決済実行


ユーザーが承認すると、任意のURLへのリダイレクトまたは任意のURLへのPOSTリクエストにてサービスプロバイダーに通知されます。その時点で決済を実行できる状態となっていますので、あとは決済実行のAPI（Confirm API）にアクセスすれば決済完了となります。


== LINE Things Drink Bar での購入・決済の流れ


ユーザーはLINE アプリで次のような操作をしてドリンクを購入します。

 1. 自動販売機の近くでLINE アプリを起動する
 1. 購入する商品（ドリンク）を選択する
 1. 注文情報を登録し決済予約を行う
 1. LINE Pay 決済画面で購入内容を確認して決済を実行する
 1. 決済完了後にドリンク抽出画面を確認する
 1. 抽選ボタンを押下して購入後のお楽しみ抽選を行う



<<<<<<<< LINE Things Drink Bar 画面遷移図 >>>>>>>>
//image[LineThingsDrinkBarFlow][LINE Things Drink Bar での購入・決済の流れ]{
//}



== LINE Pay 導入


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



@<href>{https://pay.line.me/jp/intro/faq?locale=ja_JP&sequences=14,個人事業主も加入できますか？：よくある質問＠LINE Pay}



加盟店申請とは関係ないですが、青色申告をしていれば、していない場合（白色申告）に比べて所得の控除額が大きく、減価償却の特例が受けられる（30万円未満の固定資産を一括償却できる。上限あり）など税金面での優遇がありますので、副業などをしている方は青色申告しておくのをお勧めします。


==== 青色申告するには


本記事はIT技術記事なので細かいことは書きませんが、開業届と青色申告承認申請書のフォーマットに沿って記載して、所管の税務署に提出するだけです。
申告書類を簡単に作成できるサービスもたくさんあるので、検索してみてください。


==== LINE Pay 加盟店申請に必要な書類など


いよいよLINE Pay 加盟店申請ですが、個人で申請する際に必要な書類などがあるので揃えておきましょう。

 * 開業届または確定申告書の控え
 * 本人確認書類（運転免許証など）
 ** 運転免許証の場合、「運転免許証：変更事項があれば裏面含みます」と記載されていますが、裏面に記載がなくても裏表の提出が求められたので注意してください。
 * Web サイト



特別なものはありませんね。それではいよいよ加盟店申請です。



//image[LINEPayAppForMenber][LINE Pay 加盟店申請ページ]{
//}




@<href>{https://pay.line.me/jp/intro?locale=ja_JP,LINE Pay 加盟店申請ページ}ページ（リンク先の右上赤枠部分）にアクセスしてください。


==== 加盟店申請


個人でLINE Pay API を使う加盟店申請を行うのであれば、申請画面で以下のように選択して申請を進めます。

 * 事業種別：個人事業主
 * 支払い方法：オンライン



//image[LINEPayAppForMember][LINE Pay 加盟店申請]{
//}




あとは、規約を読んで同意し、必要書類をスキャンしてPDF 形式などにして、スキャンしたファイルを申請ページから必要事項の記入とともにアップロードするだけです。


==== 審査と登録完了通知


申請書類の不備などが二度ありましたが、約2週間で審査完了しました。審査がスムーズなのも時間がない個人としては嬉しいところです。



審査が完了すると、下のようなメールが届きます。
//image[ReviewCompleted][加盟店審査完了メール]{
//}



=== LINE Pay Sandboxの申請と設定


実際に決済するには加盟店登録が必要ですが、開発して動作を確認するフェーズであればSandbox が利用できます。こちらは下記のURLから申請すると払い出されるLINE Pay API用のアカウントで、誰でもすぐに利用できます。



https://pay.line.me/jp/developers/techsupport/sandbox/creation?locale=ja_JP



アカウントが払い出されたらLINE Payコンソールの決済連動管理 > 連動キー管理からChannel ID とChannel Secret Key を確認します。これらの値はLINE Pay のAPI コールに必要になります。
//image[LinkKey][連動キー管理]{
//}



== 開発

=== サーバー（heroku）の準備

==== heroku CLI のインストール


@<href>{https://jp.heroku.com/,heroku} に登録し、ターミナルなどのCLI から操作できるheroku CLI をインストールします。
Mac で開発する場合はbrew を使ってインストールできます。


//emlist[][bash]{
$ brew tap heroku/brew && brew install heroku
//}

==== アプリケーションの作成


ここからheroku CLI を使ってアプリケーションの登録などをしていきます。



まずはログイン。ログインコマンドを入力すると、ブラウザでheroku のログインページが表示されるのでこちらからログインします。


//emlist[][bash]{
$ heroku login
heroku: Press any key to open up the browser to login or q to exit: 
Opening browser to https://cli-auth.heroku.com/auth/browser/xxxxxxxxx
Logging in... done
Logged in as xxxx@xxxx.com
//}


続けてアプリケーションを作成します。

 * YOUR@<b>{APP}NAME にはheroku 全体で一意となるアプリケーション名を指定します。


//emlist[][bash]{
$ heroku create {YOUR_APP_NAME}
Creating ⬢ {YOUR_APP_NAME}... done
https://{YOUR_APP_NAME}.herokuapp.com/ | https://git.heroku.com/{YOUR_APP_NAME}.git
//}

==== データベースの登録


heroku CLI でHeroku Postgres のデータベース登録します。


//emlist[][bash]{
$ heroku addons:create heroku-postgresql:hobby-dev -a {YOUR_APP_NAME}
Creating heroku-postgresql:hobby-dev on ⬢ {YOUR_APP_NAME}... free
Database has been created and is available
 ! This database is empty. If upgrading, you can transfer
 ! data from another database with pg:copy
Created postgresql-reticulated-xxxxx as DATABASE_URL
Use heroku addons:docs heroku-postgresql to view documentation
//}

==== 固定IPでのアクセス用プロキシの登録


heroku CLI でQuotaGuard Static の固定IPでのアクセス用プロキシを設定します。


//emlist[][bash]{
$ heroku addons:create quotaguardstatic:starter -a {YOUR_APP_NAME}
Creating quotaguardstatic:starter on ⬢ {YOUR_APP_NAME}... free
Your static IPs are [xx.xxx.xx.xx, xxx.xxx.xxx.xxx]
Created quotaguardstatic-clean-xxxxx as QUOTAGUARDSTATIC_URL
Use heroku addons:docs quotaguardstatic to view documentation
//}


環境変数「QUOTAGUARDSTATIC_URL」にプロキシURL が自動で設定されます。


=== コンテナの準備


heroku にはDocker を使ってコンテナでウェブアプリケーションをデプロイします。


==== Dockerfile


コンテナのビルド時に実行する処理をDockerfile で設定します。

 * Python からPostgreSQL を使うために関連のモジュールをインストール
 * 「requirements.txt」に記載したサーバーサイドのPython プログラムで使うライブラリをインストール
 * プログラムのコードもコンテナ内にコピー
 * ウェブアプリケーションを起動


//emlist{
FROM python:3.6-alpine

RUN apk update && apk add postgresql-dev gcc python3-dev musl-dev

# make working dir
RUN mkdir -p /app

# Install dependencies
ADD requirements.txt /tmp
RUN pip install --no-cache-dir -q -r /tmp/requirements.txt

# Add app code
ADD ./app /app
WORKDIR /app

# Run the app.  CMD is required to run on Heroku
ENV FLASK_APP /app/main.py
CMD flask run -h 0.0.0.0 -p $PORT --debugger --reload
//}

==== requirements.txt


Python プログラムで使うライブラリはrequirements.txt に定義します。



ウェブアプリケーション フレームワークに「Flask」、データベース（PostgreSQL）へのアクセス用に「psycopg2」、ORマッパーとして「SQLAlchemy」を使っています。
また、LINE Pay API や、Flask で構築するウェブアプリケーションのAPI への接続には「requests」を利用します。


//emlist{
flask
SQLAlchemy
flask_sqlalchemy
Jinja2
psycopg2
requests
line-bot-sdk
//}

=== 商品選択画面


ユーザーが操作して商品を選択する画面です。



商品選択画面をはじめフロント側はVue.js で作成しています。今回の解説では画面数も少ないのでコンポーネント化はせずに、HTML ファイルベースで開発します。また、この画面をLIFF として登録しておきます。



この画面で出来ることは以下のとおりです。

 * ドリンク一覧を表示する
 * 選択した商品を基に注文情報を登録する
 * LINE Pay 決済画面へ遷移する
 * LINE Things Drink Bar を制御する


=== ドリンク一覧の表示


LINE アプリから商品選択画面のLIFF URL を選択するとFlask の下記URL にGET リクエストが送られます。



このリクエストを受けたFlask のプログラム（main.py）がテンプレートエンジン（Jinja2）で商品選択画面のHTML ファイル（drink_bar.html）を返します。


//emlist[][python]{
@app.route('/drink_bar', methods=['GET'])
def get_drink_bar():
    app.logger.info('handler get_drink_bar called!')
    return render_template(
        'drink_bar.html',
    )
//}

==== HTML ファイル（drink_bar.html）


商品選択画面のHTML ファイルです。
ドリンク一覧や、選択したドリンクの詳細と決済へ進むボタン、ドリンクの抽出状況などを表示します。
それぞれの表示は処理のステータスを示す @<b>{flow_status} の状態に応じて切り替えています。



下記では、ドリンク一覧を表示する部分について抜粋しています。


//emlist[][html]{
<!--    No order - Item list     -->
<v-container fluid v-else-if="flow_status === 'INITIAL'">
    <v-layout>
        <v-flex xs12>
            <v-toolbar color="green darken-2" dark>
                <v-toolbar-title>LINE Things Drink Bar</v-toolbar-title>
            </v-toolbar>
            <!-- items -->
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


商品選択画面で表示する商品情報をサーバーサイドから取得します。
画面の初期化時にaxios を利用して、サーバーサイドで提供される商品情報取得APIを実行します。



下記では、商品情報取得API の実行部分（drink_bar.js）について抜粋しています。


//emlist[][javascript]{
getItems: async function() {
    console.log('function getItems called!')
    // Item 取得
    this.api_loading = true
    const api_url = '/api/items'
    const response = await axios.get(api_url).catch(error => {
        console.error('API getItems failed...')
        console.error(error)
        this.api_result = null
        this.api_loading = false
    })
    console.log('API response: ', response)
    this.api_result = response.data
    this.items = this.api_result.items
    this.api_loading = false
},
//}


サーバーサイドで提供される商品情報取得API を実行し、取得した商品情報をVue.js の表示用変数に割り当てます。



なお、LIFF 開発でデバッグなどに利用できる便利なツール「vConsole」があります。導入方法などは下記URL の記事を参照してください。

 * https://qiita.com/sumihiro3/items/9f4f1adb5d8883d9ceeb


==== 商品情報取得API


商品選択画面のスクリプト（drink_bar.js）から呼び出される商品情報取得API です。
サーバーサイドではSQLAlchemy を使ってデータベースから商品情報を取得して返しています。


//emlist[][python]{
@app.route('/api/items', methods=['GET'])
def get_items():
    app.logger.info('handler get_items called!')
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
    # return items
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
axios を利用して、サーバーサイドで提供される注文情報登録API を実行します。



下記では、注文情報登録API の実行部分（drink_bar.js）について抜粋しています。


//emlist[][javascript]{
orderItem: async function(item_id) {
    console.log('function orderItem called!')
    // 注文登録
    this.api_loading = true
    let order_item_ids = [item_id]
    // Order!
    const params = {
        user_id: this.line_user_id,
        order_items: order_item_ids
    }
    const url = '/api/purchase_order'
    const response = await axios.post(url, params).catch(function (err) {
        this.api_loading = false
        console.error('API POST PurchaseOrder failed', err)
        this.flow_status = 'INITIAL'
        throw err
    })
    console.log('Response: ', response)
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


商品選択画面のスクリプト（drink_bar.js）から呼び出される注文情報登録API です。


//emlist[][python]{
@app.route('/api/purchase_order', methods=['POST'])
def post_purchase_order():
    app.logger.info('handler post_purchase_order called!')
    app.logger.debug('Request json: %s', request.json)
    request_dict = request.json
    user_id = request_dict.get('user_id', None)
    user = User.query.filter(User.id == user_id).first()
    # ユーザーが登録されていなければ新規登録
    if user is None:
        user = add_user(user_id)
    order_items = request_dict.get('order_items', [])
    order_item_list = Item.query.filter(Item.id.in_(order_items))
    app.logger.debug('order_item_list: %s', order_item_list)
    # order !
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
なお、LINE Pay での決済では注文IDが重複不可のため、一意なIDを生成しておく必要があります。


//emlist[][python]{
def add_purchase_order(user, order_items):
    app.logger.info('add_purchase_order called!')
    # 一意な注文IDを生成する
    order_id = uuid.uuid4().hex
    timestamp = int(dt.now().timestamp())
    details = []
    amount = 0
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
    const url = '/pay/reserve'
    console.log('Payment URL:', url)
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
    // redirect to payment_url
    window.location.href = payment_url
    this.api_loading = false
},
//}


サーバーサイドで提供される決済予約API を実行し、サーバーサイドから返されるLINE Pay の決済実行URL に遷移します。


==== 注文情報登録API


商品選択画面のスクリプト（drink_bar.js）から呼び出される決済予約API です。


//emlist[][python]{
@app.route("/pay/reserve", methods=['POST'])
def handle_pay_reserve():
    app.logger.info('handler handle_pay_reserve called!')
    app.logger.debug('Request json: %s', request.json)
    request_dict = request.json
    user_id = request_dict.get('user_id', None)
    order_id = request_dict.get('order_id', None)
    # 注文情報とユーザー情報をデータベースから取得する
    order = PurchaseOrder.query.filter(PurchaseOrder.id == order_id).first()
    app.logger.debug('PurchaseOrder: %s', order)
    user = User.query.filter(User.id == user_id).first()
    app.logger.debug('User: %s', user)
    ordered_item = Item.query.filter(Item.id == order.details[0].item_id).first()
    app.logger.debug('Ordered Item: %s', ordered_item)
    # LINE Pay の決済予約API を実行してtransactionId を取得する
    response = pay.reserve_payment(order, product_image_url=ordered_item.image_url)
    app.logger.debug('Response: %s', json_util.dump_json_with_pretty_format(response))
    app.logger.debug('returnCode: %s', response["returnCode"])
    app.logger.debug('returnMessage: %s', response["returnMessage"])
    transaction_id = response["info"]["transactionId"]
    app.logger.debug('transaction_id: %s', transaction_id)
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
    logger.info('Method %s.reserve_payment called!!', self.__class__.__name__)
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
    # リクエスト送信
    response = requests.post(
        line_pay_endpoint,
        json_util.dump_json(body).encode('utf-8'),
        headers=self.__headers,
        proxies=self.__proxies
    )
    return response.json()
//}


API のパラメータについては、LINE Pay 技術ドキュメントをご確認ください。
- https://pay.line.me/jp/developers/documentation/download/tech?locale=ja_JP


==== LINE Pay の決済承認画面


決済予約処理が正常に完了すると、下図のようにLINE Pay の決済承認画面が表示されます。ここはLINE Pay プラットフォーム側の画面となります。
注文情報（注文名や決済する金額）、指定した画像と決済承認ボタンが表示されています。



//image[PayByLinePay][決済承認画面]{
//}




「PAY NOW」と表示された決済承認ボタンを押下すると決済承認処理が行われ、決済承認完了画面が表示されます。この後、開発したサービス（LINE Things Drink Bar）のサーバーへ処理が返ってきます。



//image[PayCompleted][決済承認完了]{
//}



=== 決済実行処理の実行


LINE Pay 側からサーバーサイドの決済実行処理が呼び出されますので、決済実行API を呼び出して決済を完了させます。
決済実行API が正常に完了すれば、注文情報のステータスを決済完了に更新します。


//emlist[][python]{
@app.route("/pay/confirm", methods=['GET'])
def handle_pay_confirm():
    app.logger.info('handler handle_pay_confirm called!')
    transaction_id = request.args.get('transactionId')
    order = PurchaseOrder.query.filter_by(transaction_id=transaction_id).one_or_none()
    if order is None:
        raise Exception("Error: transaction_id not found.")
    # run confirm API
    response = pay.confirm_payments(order)
    app.logger.debug('returnCode: %s', response["returnCode"])
    app.logger.debug('returnMessage: %s', response["returnMessage"])

    order.status = PurchaseOrderStatus.PAYMENT_COMPLETED.value
    db.session.commit()
    db.session.close()
    return render_template(
        'drink_bar.html',
        message='Payment successfully completed.',
        transaction_id=transaction_id
    )
//}

==== LINE Pay API 決済実行


LINE Pay API を呼び出す部分です。
API で設定された各種パラメータを設定して決済実行APIのURL（/v2/payments/{transaction_id}/confirm）へPOSTリクエストを送ります。



API のパラメータについては、LINE Pay 技術ドキュメントをご確認ください。
- https://pay.line.me/jp/developers/documentation/download/tech?locale=ja_JP


//emlist[][python]{
    def confirm_payments(self, purchase_order):
        line_pay_url = self.__line_pay_url
        line_pay_endpoint = f'{line_pay_url}/v2/payments/{purchase_order.transaction_id}/confirm'
        body = {
            'amount': purchase_order.amount,
            'currency': purchase_order.currency,
        }
        # リクエスト送信
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




ドリンク抽出後はお楽しみ抽選が実行され、確率で当たり／はずれが決定されます。



//image[FukubikiDone][抽選完了]{
//}




以上でLINE Pay 決済に関わる部分の実装が完了します。
本書では決済完了後のLINE Things でのDrink Bar の制御部分や、購入後のお楽しみ抽選に関するコードの解説は行いませんが、前述の通りソースコードをGithub のリポジトリで公開していますので、そちらを御覧ください。


=== heroku へのデプロイ


実装が完了したので、heroku にLINE Things Drink Bar のプログラムをデプロイします。


==== 環境変数の設定


デプロイの前にLINE Things Drink Bar で使用する環境変数をheroku に登録しておきます。


===== LINE Bot のChannel Secretとアクセストークン


これらの情報はLINE Developers の画面から取得できます。


//emlist[][bash]{
$ heroku config:set LINEBOT_CHANNEL_ACCESS_TOKEN=xxxxxxxxxxxxxxxxxxx -a {YOUR_APP_NAME}
$ heroku config:set LINEBOT_CHANNEL_SECRET=xxxxxxxxxx -a {YOUR_APP_NAME}
//}

===== LINE Pay API のChannel ID など


これらの情報はLINE Pay API Sandbox の画面から取得できます。



LINE@<b>{PAY}CONFIRM_URL は、LINE Pay での決済承認後、自分のサーバーに返ってくるURLを指定します。


//emlist[][bash]{
$ heroku config:set LINE_PAY_URL=https://sandbox-api-pay.line.me -a {YOUR_APP_NAME}
$ heroku config:set LINE_PAY_CHANNEL_ID=xxxxxxxxx -a {YOUR_APP_NAME}
$ heroku config:set LINE_PAY_CHANNEL_SECRET=xxxxxxxxxxxx -a {YOUR_APP_NAME}
$ heroku config:set LINE_PAY_CONFIRM_URL=https://{YOUR_APP_NAME}/pay/confirm -a {YOUR_APP_NAME}
//}

===== psycopg2 でのアクセスで使用するデータベスURL


SQL Alchemy でのデータベース接続情報はheroku のHeroku Postgres 設定画面などで確認できます。


//emlist[][bash]{
$ heroku config:set POSTGRESQL_DATABASE_URL=postgres+psycopg2://{DB_USER_ID}:{DB_USER_PASSWORD}@{DB_SERVER_URL}:5432/{DB_NAME} -a {YOUR_APP_NAME}
//}

===== 固定IPアクセス用のプロキシURL


固定IPアクセス用のプロキシURL に関する環境変数は、QuotaGuard Static アドオン登録時に登録されますので追加設定は不要です。


==== コンテナのデプロイ


いよいよLINE Things Drink Bar のプログラムをheroku にリリースします。



heroku のコンテナサービスにログインして、コンテナイメージをpush、heroku アプリケーションとしてリリースします。


//emlist[][bash]{
// login to heroku container
$ heroku container:login
Login Succeeded

// push container image to heroku container
$ heroku container:push -a {YOUR_APP_NAME} web
Your image has been successfully pushed. You can now release it with the 'container:release' command.

// release application
$ heroku container:release -a {YOUR_APP_NAME} web
Releasing images web to {YOUR_APP_NAME}
//}


これでデプロイ完了です！


== 最後に


解説が長くなりましたが、基本的な決済機能の組み込みは以上です。
サービスとして公開するにはキャンセル処理なども実装する必要がありますが、他APIの呼び出しについても同様にAPI ドキュメントに沿ってパラメータを指定するだけです。



また、今回はLINE Bot への組み込みで解説しましたがWeb ページへの組み込みもほぼ同じです。
単純にLINE Pay での決済機能を利用するだけでなく、多くのユーザーが利用するLINE の特性を活かしてLINE Things などを組み合わせたWeb の世界以外とも連携したサービスを開発してはいかがでしょうか。



なお、LINE Things や、LIFF などの設定は本章では触れていませんが、他の章でLINE API Expert のみなさんが解説していますので、そちらを参考にしていただければと思います。

