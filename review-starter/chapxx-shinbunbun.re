
= AWSLambdaを使ってサーバレスでLINEログインを実装してみよう！

== LINEログインとは


LINEログインとは、ウェブアプリやネイティブアプリに組み込むことで、ユーザーがLINEアカウントを使って簡単にログインが出来るようになるサービスです。OAuth2.0とOpenID Connectに基づいています。
LINEログインで取得したアクセストークンを使用してSocial API V2.1を叩くことで、ユーザープロフィールやLINE公式アカウントとの友達関係の取得をすることが出来ます。


== LINEログイン&Social API V2.1で出来ること
 * ユーザーがLINEに登録しているメールアドレスの取得（申請が必要）
 * ユーザーがLINEに登録しているプロフィール（アカウント名、プロフィール画像、ステータスメッセージ）の取得
 * userId（「プロバイダ×ユーザー」固有のID）の取得
 * LINE公式アカウントとの友達関係の取得


== アーキテクチャ


//image[67][アーキテクチャ]{
//}



== 前提条件
 * Node.jsがインストールされている(npmが使える)
 * AWSアカウントを持っている
 * LINEDevelopersのアカウントとプロバイダを作成済み



※まだ作成したことがない方は以下の記事を参考に作成をしてください。
https://qiita.com/shinbunbun_/items/b1d82f5bd85786a7f06e


== LINEログインのチャネルを作成しよう！


LINEDevelopers(https://developers.line.biz/ja/) にアクセスして、「LINEログイン」を開きます。



//image[1][図.1]{
//}




「今すぐ始めよう」を押して下さい。



//image[2][図.2]{
//}




プロバイダを選択して下さい。



//image[3][図.3]{
//}




必要事項を入力していきます。「アプリ名」、「アプリ説明」、「メールアドレス」は適当なものを入力してください。（アプリ名に「LINE」をいうワードは使えません）「アプリタイプ」は「WEBで使う」を選択します。



//image[5][図.4]{
//}




全て入力し終わったら「入力内容を確認する」を押してください



//image[4][図.5]{
//}




確認画面が出てくるので、「LINE Developers Agreementの内容に同意します」にチェックを入れて「作成」を押してください



//image[6][図.6]{
//}




これでチャネル作成完了です！<br>
※このページは後ほど使用するため、閉じないでそのままにしておいて下さい。


== フロントエンドの設定をしよう！

=== S3バケットを作成しよう！


※AWSのアカウントを持っていない方は事前に作成をしてください



AWSマネジメントコンソール(https://console.aws.amazon.com/console/home) にログインします



「サービスを検索する」の検索欄に「S3」と入力すると「S3」という項目が出てくるので、それを選択します。



//image[7][図.7]{
//}




「バケットを作成する」を選択します



//image[8][図.8]{
//}




バケット名を入力して「次へ」を選択します



//image[9][図.9]{
//}




特に何も変更せず「次へ」を押します



//image[10][図.10]{
//}




「パブリックアクセスをすべてブロック」のチェックボックスを外して、「次へ」を押します。（このチェックを外すことによってバケット内のファイルに誰でもアクセスできるようになります）



//image[11][図.11]{
//}




「バケットを作成」を選択します



//image[12][図.12]{
//}




これでバケットの作成は完了です！


=== S3バケットにファイルをアップロードしよう！


バケットの作成が完了したら早速コードをアップしていきます



@<href>{,こちらのリポジトリ}のコードをgit cloneなりZIPでダウンロードするなりして任意のフォルダに落として下さい



/code/frontのフォルダをまとめてドラックアンドドロップでS3にアップします



//image[13][図.13]{
//}




「次へ」を選択します



//image[14][図.14]{
//}




「このオブジェクトにパブリック読み込みアクセス権限を付与しない（推奨）」を押すとプルダウンメニューが出てくるので、「このオブジェクトにパブリック読み込みアクセス権限を付与する」を選択して、「アップロード」を押します。



//image[15][図.15]{
//}




これでコードのアップロードは完了です！



左上にバケット名が表示されています。後ほど使用するのでメモ帳等にコピペしておいて下さい。



//image[48][図.16]{
//}



== バックエンドの設定をしよう

=== Lambda関数を作成しよう！


@<href>{https://console.aws.amazon.com/console/home,AWSマネジメントコンソール}を開き、「サービスを検索する」の検索欄に「Lambda」と入力すると「Lambda」という項目が出てくるので、それを選択します。



//image[16][図.17]{
//}




「関数の作成」を選択します



//image[17][図.18]{
//}




関数名は適当なものを入力し、ランタイムは「Node.js 10.X」を選択して下さい。実行ロールに関しては、以前作ったものなどがある場合はそちらを使用しても結構です。今回はCloudWatch Logs以外の権限は特に必要ありません。必要事項の入力が終わったら「関数の作成」を選択して下さい。



//image[18][図.19]{
//}




これでLambda関数の作成は完了です！


=== モジュールをインストールしよう！


Lambda側のコードは/code/back/index.jsです。コードを見ていただければわかる通り色々なモジュールを使用しているので、それらを先にローカルでインストールしておきます。



ターミナル（Windowsの場合はcmdまたはPowerShell ）を開き、/code/back/index.jsがあるフォルダに移動して下さい。



以下のコマンドを入力してpackage.jsonを生成します。


//emlist{
npm init -y
//}


//image[19][図.20]{
//}




以下のコマンドを入力してモジュールをインストールします。<br>
※cryptoは標準モジュールなのでインストールする必要はありません。


//emlist{
npm i axios uuid qs jsonwebtoken
//}


//image[20][図.21]{
//}



=== Lambdaにコードをアップロードしよう！


ZIP形式でLambdaにコードをアップロードしていきます。



/code/backフォルダに、元からあったindex.jsと先ほど作成したpackage.json・package-lock.json・node_modulesがあるはずなので、それらを纏めてZIPに圧縮します。<br>
※以下の画像はMacの場合



//image[21][図.22]{
//}




先程作成したLambda関数を開いて少し下の方にスクロールすると「コードをインラインで編集」という項目が出てくるので、それをクリックして下さい。そうするとプルダウンメニューが出てくるので、「.zipファイルをアップロード」を選択して下さい。



//image[22][図.23]{
//}




「アップロード」を選択して下さい。



//image[23][図.24]{
//}




先程圧縮したzipファイルを選択してアップロードして下さい。



アップロードが完了したら右上の「保存」を押して下さい



//image[24][図.25]{
//}




画面左側のファイル一覧から、関数名のフォルダ（このサンプルでは「line-login-test」）直下にindex.jsがあることを確認して下さい。



//image[26][図.26]{
//}




以下のようにフォルダがネストしている場合は圧縮方法が間違っている可能性があるので確認して下さい。



//image[25][図.27]{
//}



=== API GateWayの設定をしよう！

==== APIの作成をしよう！


外からLambda関数にアクセスできるようにするために、APIGatewayでAPIの作成をします。



「トリガーを追加」を押します



//image[28][図.28]{
//}




「トリガーの選択」を押すとプルダウンメニューが出てくるので「API Gateway」を選択します。



//image[30][図.29]{
//}




「新規APIの作成」でセキュリティは「オープン」を選択し、「追加」を押して下さい。



//image[31][図.30]{
//}




これでAPIの作成は完了です！



API名（このサンプルでは「line-login-test-API」）を押して下さい



//image[33][図.31]{
//}



==== APIのリソースを作成しよう！


今の状態では「/」にしかアクセス出来ないので、今回使用する「/authorize」、「/callback」にアクセス出来るようにリソースの作成をしていきます。


===== 「/authorize」のリソースを作成しよう！


まずは「/authorize」から作成していきます。



「/」を押して下さい



//image[38][図.32]{
//}




「アクション」を押すとプルダウンメニューが出てくるので、「リソースの作成」を押して下さい



//image[37][図.33]{
//}




リソース名に「authorize」と入力して「リソースの作成」を押して下さい



//image[39][図.34]{
//}




「アクション」を押すとプルダウンメニューが出てくるので「メソッドの作成」を選択して下さい。



//image[35][図.35]{
//}




ボタンが出てくるので、それを押して下さい



//image[36][図.36]{
//}




プルダウンメニューが出てくるので「GET」を選択してチェックマークを押します



//image[58][図.37]{
//}




右側にセットアップ画面が出てくるので、「Lambda プロキシ統合の使用」にチェックマークを入れて「Lambda 関数」に先程作成したLambdaの関数名を入力し、右下の「保存」ボタンを押して下さい。



//image[41][図.38]{
//}




下図のような画面が出てくるので「OK」を選択します。



//image[42][図.39]{
//}




これで「/authorize」のリソース作成は完了です！


===== 「/callback」のリソース作成をしよう！


次に「/callback」のリソースを作成していきます！



「/」を押して下さい



//image[43][図.41]{
//}




「アクション」を押すとプルダウンメニューが出てくるので、「リソースの作成」を押して下さい



//image[37][図.42]{
//}




リソース名に「callback」と入力して「リソースの作成」を押して下さい



//image[44][図.43]{
//}




「アクション」を押すとプルダウンメニューが出てくるので「メソッドの作成」を選択して下さい。



//image[35][図.44]{
//}




ボタンが出てくるので、それを押して下さい



//image[36][図.45]{
//}




プルダウンメニューが出てくるので「GET」を選択してチェックマークを押します



//image[58][図.46]{
//}




右側にセットアップ画面が出てくるので、「Lambda プロキシ統合の使用」にチェックマークを入れて「Lambda 関数」に先程作成したLambdaの関数名を入力し、右下の「保存」ボタンを押して下さい。



//image[41][図.47]{
//}




下図のような画面が出てくるので「OK」を選択します。



//image[42][図.48]{
//}



==== APIをデプロイしよう！


APIの設定が全て完了したのでデプロイしていきます。



「アクション」を押して「APIのデプロイ」を選択します。



//image[45][図.49]{
//}




default（API作成の際にステージ名を指定した方はそのステージ名）を選択し、「デプロイ」ボタンを押して下さい。



//image[46][図.50]{
//}




デプロイが完了するとapiのエンドポイントが表示されます。後ほど使用するので、これをメモ帳等にコピペしておいて下さい。



//image[47][図.51]{
//}




これでAPIの設定が完了しました！


=== Lambdaの環境変数を設定しよう！


Channel ID、clientSecret、redirectUri、frontUriを環境変数に登録していきます



先程LINEログインのチャネルを作成したページを開き、先程作成したチャネルを選択します。



//image[29][図.52]{
//}




下の方にスクロールするとChannel IDとChannel Secretが出てくるので、それらをメモ帳等にコピペします。



//image[27][図.53]{
//}




先程作成したLambda関数を開いて下の方にスクロールすると「環境変数」という項目が出てくるので、以下のようにChannel IDとChannel Secret、redirectUri、frontUriを入力して下さい。



redirectUriは、先程メモしたAPIエンドポイントに「/callback」をくっ付けたものになります。（https://~/.amazonaws.com/default/callback という形）



frontUriはS3バケットのエンドポイントで、「https://バケット名.s3-リージョン名.amazonaws.com」という形になります。例えば、バケット名が「example」でリージョンが「東京」の場合は「https://example.s3-ap-northeast-1.amazonaws.com」になります。



リージョンの一覧は以下のページから確認できます。<br>
https://docs.aws.amazon.com/ja_jp/AWSEC2/latest/UserGuide/using-regions-availability-zones.html



//image[59][図.54]{
//}




入力が完了したら右上の「保存」ボタンを押して下さい



//image[24][図.55]{
//}



== LINEDevelopersでリダイレクト設定をしよう！


先程作成したLINEログインのチャネルのページを開きます



//image[60][図.56]{
//}




「アプリ設定」を選択します



//image[61][図.57]{
//}




Callback URLの編集ボタンを押します



//image[62][図.58]{
//}




先程Lambdaの環境変数に登録したredirectUri（APIエンドポイントに「/callback」をくっ付けたもの）を入力して、「更新」ボタンを押します。



//image[63][図.59]{
//}



== S3にアップしたファイルの修正をしよう！


先程フロントのファイルをS3にアップしましたが、ログインページのリダイレクト先にLambdaのAPIエンドポイントを設定する必要があるため修正が必要です。


=== ローカルでファイルを書き換えよう！


gitから落としたファイルを開き、/code/front/login/login.jsをVSCode等のエディタで開きます。



以下のようなコードが表示されるので、3行目の{LambdaURL}をLambdaのAPIエンドポイント（先程Lambdaの環境変数に「redirectUri」として登録したもの）に書き換えます



//image[50][図.60]{
//}




以下のようなコードになります



//image[51][図.61]{
//}



=== S3にアップロードしよう！


先程ファイルをアップロードしたS3のページを開き、loginフォルダを開きます。



//image[52][図.62]{
//}




先程コードを書き換えたindex.jsをドラッグ&ドロップしてアップロードします。



//image[55][図.63]{
//}




以下のような画面になるので「次へ」を選択します。



//image[56][図.64]{
//}




「このオブジェクトにパブリック読み取りアクセス権限を付与しない (推奨)」を押すとプルダウンメニューが出てくるので、「このオブジェクトにパブリック読み取りアクセス権限を付与する」を選択して「アップロード」を押します。



//image[57][図.65]{
//}




これで修正完了です！


== 動かしてみよう！


ログインページ（ https://バケット名.s3-リージョン名.amazonaws.com/login/login.html ）にアクセスします。先程Lambdaの環境変数にfrontUriとして登録したものに「/login/login.html」をくっつけたものになります。



//image[49][図.66]{
//}




ログインボタンを押すと以下のようなログインフォームへリダイレクトするので、LINEDevelpersアカウントと紐づいているLINEアカウントでログインをして下さい。



//image[64][図.67]{
//}




以下のような画面が出てくるので「許可する」を押してください。



//image[65][図.68]{
//}




自分がLINEに登録しているプロフィールが表示されれば成功です！



//image[66][図.69]{
//}



== コード解説

=== バックエンド

//emlist[][js]{
//モジュールを読み込み
const axios = require("axios");
const uuidv4 = require('uuid/v4');
const random = require('crypto');
const qs = require('qs');
const jsonwebtoken = require('jsonwebtoken');
//環境変数を取得
const channelId = process.env.channelId;
const channelSecret = process.env.channelSecret;
const redirectUri = process.env.redirectUri;
const frontUri = process.env.frontUri;

//Lambda関数が呼び出された時に呼び出される関数
exports.handler = async (event, context, callback) => {
    //パスを取得
    const path = event.path;
    //レスポンスを定義
    let res;
    //パスによって条件分岐
    switch (path) {
        case '/authorize':
            //authorizeFuncを呼び出し
            res = authorizeFunc();
            break;
        case '/callback':
            //callbackFuncを呼び出し
            res = await callbackFunc(event);
            break;
        default:
        // code
    }

    //レスポンスを返す
    callback(null, res);
};
//}


2~6行目  :必要なモジュールの読み込みを行なっています。<br>
8~11行目 :環境変数を取得し、変数に格納しています。<br>
14行目   :Lambda関数が呼び出された時に最初に呼び出される関数です。<br>
16行目   :このLambda関数は「/authorize」・「/callback」の二種類のパスから呼び出せるため、どちらのから呼び出されたか判別するためにパスを取得します。<br>
18行目   :レスポンスを定義します。<br>
20~29行目:パスで条件分岐して、「/authorize」の場合はauthorizeFunc、「/callback」の場合はcallbackFuncを呼び出しています。<br>
32行目   :レスポンスを返します。


//emlist[][js]{
const authorizeFunc = () => {
    //レスポンスのインスタンスを生成
    const response = new Response();
    //CSRF防止用の文字列を生成
    const state = random.randomBytes(16).toString('hex');
    //リプレイアタックを防止するための文字列を生成
    const nonce = uuidv4();

    //認可リクエストを送信するためのレスポンスを生成
    response.statusCode = 302;
    response.headers.Location = `https://access.line.me/oauth2/v2.1/authorize?response_type=code&client_id=${channelId}&redirect_uri=${redirectUri}&state=${state}&scope=openid%20profile&nonce=${nonce}`;
    response.multiValueHeaders = { "Set-Cookie": [`state=${state};HttpOnly`, `nonce=${nonce};HttpOnly`] };
    response.body = JSON.stringify({ "status": "succeed" });

    return response;
};
//}


35~50行目がauthorizeFuncになっています。



37行目:レスポンスのインスタンスを生成しています。<br>
39行目:CSRF防止用の文字列を生成しています。<br>
41行目:リプレイアタック防止用の文字列を生成しています。<br>
44行目:リダイレクトするためステータスコード302を指定しています。<br>
45行目:リダイレクトするURLを指定しています。<br>
46行目:stateとnonceをCookieにセットしています。フロント側からこの値を使用することはないため、HttpOnlyの属性を付与しています。multiValueHeadersとは、Lambdaプロキシ統合において複数値をもつヘッダーをセットする時に使用するものです。これを渡すとAPIGatewayが勝手にheadersとマージしてくれます。<br>
47行目:bodyがないとエラーを吐かれてしまうので適当なものを作っておきます。<br>
49行目:レスポンスを返します。


//emlist[][js]{
const callbackFunc = async (event) => {
    //レスポンスのインスタンスを生成
    const response = new Response();
    //パラメータを取得
    const params = event.queryStringParameters;

    //パラメータに"error"という項目が含まれていた場合
    if (params.error) {
        //エラーを出力
        console.log(`error: ${params.error}`);
        //エラーページにリダイレクトさせるレスポンスを生成
        response.statusCode = 302;
        response.headers.Location = `${frontUri}/error/error.html`;
        response.body = JSON.stringify({ status: "error" });
        return response;
    }

    //パラメータから認可コードを取得
    const code = params.code;
    //パラメータからstate（認可リクエスト時に送信したCSRF防止用の文字列）を取得
    const callbackState = params.state;
    //リクエストヘッダからCookieを取得
    const cookie = event.headers.cookie.split("; ");
    //Cookieからstateを取得
    const cookieState = cookie[0].slice(6);
    //コールバックで送られてきたstateとCookieから取得したstateが同じものか確認する
    //違うものだった場合はCSRF攻撃を受けている可能性があるため、エラーページへリダイレクトして再ログインを要求する
    if (callbackState !== cookieState) {
        //stateを出力
        console.log(`callbackState: ${callbackState}, cookieState: ${cookieState}`);
        response.statusCode = 302;
        response.headers.Location = `${frontUri}/error/error.html`;
        response.body = JSON.stringify({ status: "error" });
        return response;
    }
    //レスポンスボディを定義
    let resBody;

    try {
        //アクセストークン発行のapiを叩く
        const res = await axios.post("https://api.line.me/oauth2/v2.1/token", qs.stringify({
            "grant_type": "authorization_code",
            "code": code,
            "redirect_uri": `${redirectUri}`,
            "client_id": channelId,
            "client_secret": channelSecret
        }));
        resBody = res.data;
        //idtokenをデコードする
        const idToken = jsonwebtoken.verify(resBody.id_token, channelSecret);
        //idtokenに含まれているnonceとCookieから取得したnonceが同じものか確認する
        //違う場合はリプレイアタックを受けている可能性があるため、エラーページへリダイレクトして再ログインを要求する
        if (idToken.nonce !== cookie[1].slice(6)) {
            console.log(`idToken.nonce: ${idToken.nonce}, cookie.nonce: ${cookie.nonce}`);
            response.statusCode = 302;
            response.headers.Location = `${frontUri}/error/error.html`;
            response.body = JSON.stringify({ status: "error" });
            return response;
        }
    }
    //リクエストでエラーが発生した場合
    catch (error) {
        //エラーを出力後、エラーページへリダイレクト
        console.log(`requestError: ${error.response}`);
        response.statusCode = 302;
        response.headers.Location = `${frontUri}/error/error.html`;
        response.body = JSON.stringify({ status: "error" });
        return response;
    }

    //レスポンスボディからアクセストークンを取り出す
    const accessToken = resBody.access_token;

    //アクセストークンを使ってプロフィール取得のapiを叩く
    try {
        const res = await axios.post("https://api.line.me/v2/profile", {}, { "headers": { "Authorization": `Bearer ${accessToken}` } });
        resBody = res.data;
    }
    //エラーの場合
    catch (error) {
        //エラーを出力
        console.log(error.response);
    }

    //プロフィール表示ページへリダイレクトさせるためのレスポンスを生成
    response.statusCode = 302;
    response.headers.Location = `${frontUri}/profile/profile.html?userId=${resBody.userId}&displayName=${encodeURIComponent(resBody.displayName)}&pictureUrl=${resBody.pictureUrl}&statusMessage=${encodeURIComponent(resBody.statusMessage)}`;
    //Cookieに保存していたstateとnonceはもういらないので削除する
    response.multiValueHeaders = { "Set-Cookie": [`state=;HttpOnly`, `nonce=;HttpOnly`] };
    response.body = JSON.stringify({ status: "succeed" });
    return response;
};
//}


52~144行目がcallbackFuncになっています。



54行目     :レスポンスのインスタンスを生成しています。
56行目     :リクエストパラメータを取得しています。
59~67行目  :認可に失敗した場合に以下のようなリクエストが飛んでくるので、パラメータに「error」という項目があった場合はそのエラー内容を出力(61行目)してからエラーページへリダイレクトします。


//emlist{
https://example.com/callback?error=access_denied&error_description=The+resource+owner+denied+the+request.&state=0987poi
//}


70行目     : リクエストパラメータから認可コードを取得しています。
72行目     :リクエストパラメータに含まれるstateを取得しています。
74行目     :Cookieを取得しています。Cookieは以下のような形で送られてくるので、区切り文字「; 」で分割しています。


//emlist{
state=4ed77097249d0c5bddb6205483993372; nonce=fd0cb395-fb16-4392-b627-5bbbbdf82ede
//}


76行目     :splitで分割された文字列は配列になるため、このような形でstate部分を切り出します。
79~86行目  :リクエストパラメータに含まれていたstateとCookieに保存していたstateが同じものか確認し、違うものだった場合はCSRF攻撃を受けている可能性があるためエラーページへリダイレクトします。
88行目     :レスポンスボディを定義します。
92~98行目  :アクセストークン発行のAPIを叩いています。
99行目     :取得したデータをresBodyに格納しています。
101行目    :idtokenがJWTになっているのでデコードします。
104~110行目:デコードしたidtokenに含まれているnonceとCookieに保存していたnonceが同じものか確認し、違うものだった場合はリプレイアタックを受けている可能性があるため、エラーページへリダイレクトします。
113~120行目:リクエストでエラーが発生した場合は、エラーを出力後エラーページへリダイレクトします。
123行目    :レスポンスボディからアクセストークンを取り出しています。
126~134行目:プロフィール取得のapiを叩いています。
137~142行目:取得したプロフィールをGETパラメータにセットして、プロフィールページへリダイレクトしています。
140行目    :stateとnonceはもう必要ないため削除しています。


//emlist[][js]{
//レスポンスのクラスを生成
class Response {
    constructor() {
        this.statusCode = "";
        this.headers = {};
        this.multiValueHeaders = {};
        this.body = {};
    }
}
//}


146~153行目:レスポンスのクラスを作成しています。


=== フロント


フロントのコードは今回はあまり重要ではないため、各ファイルの役割だけ簡単に解説します。


==== login.html


画面中央にログインボタンが表示されるようになっており、ログインボタンを押すとAPIエンドポイント/autorizeにリダイレクトするようになっています。


==== profile.html


リクエストパラメータからプロフィールを取得し、表示しています。


==== error.html


「認証でエラーが発生しました。ログインページへリダイレクトします。」とアラートしてログインページにリダイレクトするようになっています。

