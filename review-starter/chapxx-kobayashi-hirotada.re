
= Blazor と Azure Functionsで作るLIFFアプリケーション


本章では、LINE Front-end Framework（LIFF）を利用して LINE のトーク画面で操作できる Todo 管理アプリケーションを作成します。@<br>{}
LIFFとして登録する Webアプリ (LIFFアプリ) の開発には Blazor (client-side) を使います。   



Blazor (client-side) は、Microsoft社がPreview版として開発を進めている (2019年8月現在) client-side の技術を利用した Single Page Application (SPA) フレームワークです。  

 * クライアントサイドのコードを C# で記述することができます。
 * 既存の .Netライブラリの資産をそのまま活用することができます。
 * サーバー側も C# を利用することで、サーバーとクライアントで データやロジックを共有できます。



LINE Front-end Framework（LIFF）は、LINE内で動作するウェブアプリのプラットフォームです。@<br>{}
WebアプリをLIFFとして登録すると、LINEのユーザーID等の情報が取得できたりユーザーの代わりにメッセージを送信したりできるようになります。  



LINEのユーザーIDを利用することで、アプリ作成者は自前でユーザーを管理しなくてもユーザーに紐づいたサービスを提供することができます。


== 対象者


本章で対象とする読者は以下の通りです。

 * ASP.Net, ASP.NetCore でのWebアプリケーションの作成経験がある方
 * C# によるプログラミングの経験がある方
 * LINE Messaging API や LIFF (LINE Frontend Framework)に関する基礎的な知識のある方



//note[■ 参考]{



Blazor (client-side)によるSPAアプリケーション開発については、あらかじめ下記自習書のテキストに一通り目を通しておくことをお勧めします。

 * https://github.com/jsakamoto/self-learning-materials-for-blazor-jp



//}


== 作成するアプリケーションの概要


以下のフレームワーク及びサービスを利用してアプリケーションを作成します。

 * Blazor (client-side) による SPA アプリケーションとして作成します。
 * GitHubページの静的サイトとして公開します。
 * LIFFアプリとしてLINE上で動作します。
 * Todo 情報の記録・取得を行う API を Azure Functions で作成します。
 * Todo 情報の永続化には Azure Table Storage を使用します。



以下のようなTodo管理アプリケーションを作成します。

 * Todo作成フォームで新規レコードを作成作成したTodoを一覧表示します。
 * 一覧画面はTodoのステータス毎に切り替えて表示することができます。
 * 一覧画面内で個別のTodoを選んで編集することができます。
 * 一覧画面内で個別のTodoに表示されるボタンでステータスの更新が可能です。
 * Todo レコードは作成者のLINEアカウントに紐づいて保存し、自分の作成したTodoのみが表示されます。



//image[app-sample][Todo管理アプリケーション]{
//}



== 開発環境


執筆時(2019年8月)現在、Blazor の開発を行うには.NetCore 3.0のプレビュー版のSDKが必要になります。  また、開発環境に Visual Studio を使用する場合 Visual Studio 2019 のプレビュー版も必要となります。  



※ Visual Studioの代わりに VSCodeを使用することも可能ですが、ここでは VS2019 での開発を前提として記載します。

 * .NetCore SDK 3.0 preview8-013656
 ** https://dotnet.microsoft.com/download/dotnet-core/3.0
 * Visual Studio 2019 preview 2.0
 ** https://visualstudio.microsoft.com/ja/vs/preview/



また、Microsoft Azure のサブスクリプションが必要になります。



サブスクリプションをお持ちでない方は、無料でAzureクレジットが1年間付与される Visual Studio Dev Essentials の利用をお勧めします。 

 * Visual Studio Dev Essentials@<br>{}https://visualstudio.microsoft.com/ja/dev-essentials/


=== サンプルリポジトリをフォーク


本稿では、サンプルリポジトリ pierre3/liff-blazor-todo-app をフォークして作業を進めていく形式で進行します。

 * https://github.com/pierre3/liff-blazor-todo-app


==== リポジトリの内容
 * liff.blazor-todo-app
 ** docs: (GitHubページに公開するアプリの配置先)
 ** liff-app-stater: (Blazor(client-side)のみで構成されたサンプルコード)
 *** TodoBot.Client: (Blazor (client-side) のプロジェクト)
 ** lif-blazor-todo-app: (Todo管理アプリのサンプル)
 *** TodoBot.Client: (Blazor (client-side) のプロジェクト)
 *** TodoBot.Server: (Todo API のAzure Functions プロジェクト)
 *** TodoBot.Shared: (Client,Server 双方から参照されるオブジェクトを定義するプロジェクト)


== Blazor (client-side) アプリを GitHub ページに公開する


まずは 「liff-app-starter」 にある Blazor (client-side) アプリ (TodoBot.Client) をGitHubページの静的サイトとして公開するところまでやってみます。


=== GitHubの設定


フォークしたリポジトリのルートに「docs」フォルダがあると思います。
今回は、このフォルダ内にビルドしたBlazorアプリを配置して、GitHubページとして公開されるように設定します。

 * GitHubの自分のリポジトリのページへ行き、Settingsタブをクリック
 * GitHub Pages 欄の Source で「master branch/docs folder」を選択



//image[github-settings][GitHub Settings]{
//}

//image[github-pages][GitHub Pages]{
//}




これで「docs」に配置した内容が静的サイトとして公開されるようになります。



サイトが公開されると、GitHub Pages設定欄に「Your site is published at {サイトのURL}」と表示されます。URLは @<tt>{https://{アカウント名\}.github.io/{リポジトリ名\}/} の様に設定されていると思います。



例：https://pierre3.github.io/liff-blazor-todo-app/


=== TodoBot.Clientプロジェクトの確認


「liff-app-starter」 フォルダ内の 「liff-blazor-todo-app」を Visula Studio で開きます。
Blazor (client-side) のプロジェクト TodoBot.Client が1つだけあるので、この中身を確認していきます。


==== index.htmlの設定


wwwroot フォルダ内にある index.html を確認します。

 * baseタグのhrefにGitHubのリポジトリ名を設定


//emlist[][html]{
<base href="/liff-blazor-todo-app/">
//}


//note[■ 補足]{



Blazorのテンプレートからプロジェクトを作成した場合の初期値は href="/"　となっていますが、GitHubページに展開する場合、URLのリポジトリ名の部分までを base パスとする必要があります。



//}


==== TodoBot.Client の配置
 * リポジトリルートの「docs」フォルダ内のファイルは「.nojekyll」ファイルを除き全て削除しておく
 * Visual Studio のソリューションエクスプローラでTodoBot.Clientを右クリック
 * 「発行...」をクリック
 * 「新規作成」をクリックし、「フォルダー」を選択
 * 「参照..」で任意のフォルダを指定し、「発行」をクリック
 * 発行が完了したら指定した発行先フォルダを開く
 * "./TodoBot.Client/dist"　フォルダ内のファイルを全てコピー
 * リポジトリルートの「docs」フォルダに張り付ける
 * 変更された「docs」フォルダをコミットして GitHub の Master ブランチに Push



//note[■ 補足]{



GitHubページでは標準でJekyllというツールを使って静的サイトを生成するようになっていますが、「.nojekyll」ファイルを置くことでこれを無効にしています。今回の様に作成した静的サイトをそのままホストしたい場合には不要となるためです。 



//}



GitHubページのURLにアクセスしてみましょう。以下の様に表示されればOKです！   



//image[starter-site][Starter App]{
//}



== LIFFアプリの作成


次はGitHubページに公開したページをLIFFアプリとして動作するように設定します。


=== LIFFのクライアントライブラリを使用する


LIFF アプリでは、JavaScript 製の LIFFクライアントSDK を使用して LINE のユーザー情報の取得等を行います。@<br>{}
今回は、これを Blazor 向けに C# でラップしたクラスライブラリ LineDC.Liff を使用して開発を行います。  

 * LineDC.Liff
 ** GitHub: https://github.com/line-developer-community/liff-client-csharp
 ** NuGet: https://www.nuget.org/packages/LineDC.Liff/



サンプルプロジェクトには、既に NuGet Packageを参照するように設定されています。


==== index.html 内のスクリプト参照を確認


LIFFクライアント（公式SDKおよびC#ライブラリ）を使用するには、imdex.html にて以下のjsファイルを参照する必要があります。

 * wwwroot/index.html の下部に以下のスクリプト参照があることを確認
 ** LIFFクライアント公式SDKのjsファイルの参照
 *** https://d.line-scdn.net/liff/1.0/sdk.js
 ** クラスライブラリ LineDC.Liff が内部で参照するjsファイル
 *** _content/LineDC.Liff/liffInterop.js


//emlist[][html]{
<script src="https://d.line-scdn.net/liff/1.0/sdk.js"></script>
<script src="_content/LineDC.Liff/liffInterop.js"></script>
//}

==== ビルドおよび再配置を行う


前述した「TodoBot.Client の配置」と同様の手順でアプリのビルドと再配置を行います。  



//note[■ 注意]{



少々面倒ですが、実機端末のLINEアプリ上で動作確認をする際には毎回この手順を実施してください。



//}


=== GitHubページのURLをLIFFアプリとして登録する
 * LINE Developers でMessagingAPIのチャネルを作成（手順は割愛します。）
 * 作成したチャネルの「チャネル基本設定」にあるQRコードでBotを友達追加
 * チャネル設定画面で「LIFF」タブを選び「＋追加」ボタンをクリック



//image[liff-settings][LIFF 設定画面]{
//}


 * LIFF名、サイズ（FullかTallが良いです）を入力
 * エンドポイントURLにGitHubページのURLを入力後、「保存する」をクリック@<br>{}



//image[liff-add][LIFFの追加]{
//}


 * 作成したLIFFを確認。「LIFF URL」欄に表示されているURLを確認します。



//image[liff-list][LIFF URLの確認]{
//}




ついでに自動応答メッセージでLIFF URL を返すように設定します。

 * 公式アカウントマネージャを開く https://manager.line.biz/
 * 左側のメニューで「自動応答メッセージ」-「応答メッセージ」を選択
 * 「作成」ボタンをクリック
 * タイトルを入力、メッセージ入力エリアにLIFF URLを入力
 * 「キーワードを設定する」にチェックを入れ、"todo"　をキーワードに設定



//image[auto-msg][応答メッセージ]{
//}


 * BOTとのトーク画面を開き、"todo"を送信
 * 応答されたLIFF URLを開く
 * LINEの認証画面で「許可する」をクリック



うまくいくと「Hello LIFF APP!」が表示されます。@<br>{}
また、右上のボタン「Profile」を選んで自分のアカウント情報が表示されれば成功です。



//image[liff-profile][LIFF Profileの確認]{
//}



== API サーバーをAzure Functionsで作る


続いて、Todoアイテムの作成・取得・編集・削除等の機能を提供するWeb APIを Azure Functions で作成します。@<br>{}
これ以降の内容は、「liff-app-starter」ではなく、もう一つの「liff-blazor-todo-app」フォルダに入っているサンプルを使用します。


=== Todo Modelの作成


APIサーバーの作成に入る前に、まず Todo の内容を格納するクラスを定義します。@<br>{}
今回はクライアント側とAPIサーバー側共にC# で記述するため、１つのクラス定義を共有することができます。@<br>{}
新たに TodoBot.Shared というプロジェクトを作成してそこに定義することにします。

 * Visual Studioで「ファイル」-「新規作成」メニューから .Net Standardクラスライブラリのテンプレートを選択してTodoBot.Shared という名前で作成
 * Todo クラスおよび Status クラスを定義


//emlist[][cs]{
public class Todo
{
    public string Id { get; set; }         //TodoテーブルのレコードID
    public string UserId { get; set; }     //ユーザーID。LIFF SDKで取得したLINEに紐づくユーザーIDを指定
    public string Title { get; set; }      //Todoのタイトル
    public string Content { get; set; }    //Todoの内容    
    public Status Status { get; set; }     //Todoのステータス
    public DateTime DueDate { get; set; }  //Todoの期日   
}

public enum Status
{
    Ready,      //準備中
    Doing,      //実施中
    Done,       //実施済み
    Canceled    //取り消し済み
}
//}

=== Todo APIの仕様


以下の仕様でTodo操作用のAPIを作成します。

//table[tbl1][]{
メソッド	URL	関数名	機能
-----------------
POST	api/todoList	CreateTodo	Todoの作成
PUT	api/todoList/{id}	UpdateTodo	Todoの更新
GET	api/{userId}/todoList	GetTodoList	ユーザーのTodo一覧取得
GET	api/{userId}/todoList/{id}	GetTodo	指定レコードの取得
DELETE	api/{userId}/todoList/{id}	DeleteTodo	レコードの削除
//}


//note[■ 補足]{



URLに含まれる{id}にはTodo毎にAPI側で割り振られるレコードのIDを指定。{userId}にはLIFF SDKから取得したLINEに紐づくユーザーIDを指定します。



//}


=== Azure Functions プロジェクトの作成


APIサーバーとなる Azure Functions のプロジェクトを作成します。

 * Visual Studioで「ファイル」-「新規作成」メニューから Azure Functions のテンプレートを選択して以下の設定で作成
 ** プロジェクト名：TodoBot.Server
 ** Functionの種類: Function V2（.NetCore)
 ** トリガー: Http Trigger
 ** Authorization Level: Anonymous


=== Functionクラスの定義


関数を定義するクラスを作成し、そこにAPI仕様に従った関数(のひな型)を作成します。

 * テンプレートで作成された Function1.cs を TodoBotFunction.csに変更
 * APIの仕様に従って必要な関数を定義する


//emlist[][cs]{
public class TodoBotFunction
{

    [FunctionName("CreateTodo")]
    public async Task<IActionResult> CreateTodo(
        [HttpTrigger(AuthorizationLevel.Anonymous, "post", Route = "todoList")] HttpRequest req,
        ILogger log)
    {
        throw new NotImplementationException();
    }

    [FunctionName("UpdateTodo")]
    public async Task<IActionResult> UpdateTodo(
        [HttpTrigger(AuthorizationLevel.Anonymous, "put", Route = "todoList/{id}")] HttpRequest req,
        string id,
        ILogger log)
    {
         throw new NotImplementationException();
    }

    [FunctionName("GetTodoList")]
    public async Task<IActionResult> GetTodoList(
        [HttpTrigger(AuthorizationLevel.Anonymous, "get", Route = "{userId}/todoList")] HttpRequest req,
        string userId,
        ILogger log)
    {
         throw new NotImplementationException();
    }

    [FunctionName("GetTodo")]
    public async Task<IActionResult> GetTodo(
        [HttpTrigger(AuthorizationLevel.Anonymous, "get", Route = "{userId}/todoList/{id}")] HttpRequest req,
        string userId,
        string id,
        ILogger log)
    {
         throw new NotImplementationException();
    }

    [FunctionName("DeleteTodo")]
    public async Task<IActionResult> DeleteTodoAsync(
        [HttpTrigger(AuthorizationLevel.Anonymous, "delete", Route = "{userId}/todoList/{id}")] HttpRequest req,
        string userId,
        string id,
        ILogger log)
    {
        throw new NotImplementationException();
    }
}
//}

=== データアクセス処理の実装


作成したTodoBotFunctionの各関数内では、受け取ったデータをDBに保存したり、DBから取得したデータを返却したりといった処理を記述します。  


==== ITodoRepository インターフェース


データにアクセスする処理は、ITodoRepository インターフェースで定義します


//emlist[][cs]{
public interface ITodoRepository
{
    Task<string> CreateTodoAsync(Todo todo);
    Task UpdateTodoAsync(string id, Todo todo);
    Task<IList<Todo>> GetTodoListAsync(string userId);
    Task<Todo> GetTodoAsync(string userId, string id);
    Task DeleteTodoAsync(string userId, string id);
}
//}

==== Azure Table Storageへの保存


データの保存先は Azure Table Storage を使用します。@<br>{}
Azure Table Storage 用の実装を CloudTableTodoRepository クラスに記述します。


===== プレーンなオブジェクト(POCO)でTable Storage にアクセスできるようにする


通常、Azure Storage のSDKでTable を操作する場合、Modelクラスは TableEntity クラスから派生している必要があります。  


//emlist[][cs]{
class Todo: TableEntity
{
    //...
}
//}


上記の様にTodo クラスでTableEntityを継承させれば良いのですが、このクラスはクライアント側でも共用するので出来るだけプレーンな状態にしておきたいのです。  



そこで今回は、プレーンなオブジェクト(POCO：Plain Old CLR Object)のまま Table の操作が行える Nuget ライブラリ TableStorage.Abstractions.POCO (https://github.com/giometrix/TableStorage.Abstractions.POCO) を使用して実装しました。


//emlist[][cs]{
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using TableStorage.Abstractions.POCO;
using TodoBot.Shared;

public class CloudTableRepository : ITodoRepository
{
    private readonly PocoTableStore<Todo, string, string> tableStore;
    private readonly string tableName = "TodoList";

    public CloudTableRepository(string connectionString)
    {
        tableStore = new PocoTableStore<Todo, string, string>(
            tableName,
            connectionString,
            partitionProperty: todo => todo.UserId,
            rowProperty: todo => todo.Id);
        if (!tableStore.TableExists())
        {
            tableStore.CreateTable();
        }
    }

    public async Task<string> CreateTodoAsync(Todo todo)
    {
        todo.Id = Guid.NewGuid().ToString().Replace("-", "");
        await tableStore.InsertAsync(todo);
        return todo.Id;
    }

    public async Task UpdateTodoAsync(string id, Todo todo)
    {
        todo.Id = id;
        await tableStore.UpdateAsync(todo);
    }

    public async Task<IList<Todo>> GetTodoListAsync(string userId)
    {
       var query = await tableStore.GetByPartitionKeyAsync(userId);
        return query.OrderBy(todo => todo.DueDate).ToList();
    }

    public Task<Todo> GetTodoAsync(string userId, string id)
    {
        return tableStore.GetRecordAsync(userId, id);
    }

    public async Task DeleteTodoAsync(string userId, string id)
    {
        await tableStore.DeleteAsync(new Todo() { UserId = userId, Id = id });
    }
}
//}

==== ITodoRepositoryインターフェースをDIする


Azure Function V2 では依存関係の挿入(Depenency Injection)デザインパターンがサポートされています。  

 * .NET Azure Functions で依存関係の挿入を使用する@<br>{}https://docs.microsoft.com/ja-jp/azure/azure-functions/functions-dotnet-dependency-injection



これを利用して、データアクセス部分の処理(ITodoRepository)を TodoBotFunction に外部から挿入するようにしてみましょう。


===== サービスを登録する
 * Startup.cs を作成
 * 作成したcsファイルの Namespace に属性@<br>{}[assembly: FunctionsStartup(typeof(TodoBot.Server.Startup))] を追加
 * FunctionStartup クラスを継承した Startupクラスを作成
 * Configure メソッドをオーバーライドしてサービスに ITodoRepository を登録
 * ITodoRepository の実体として CloudTableTodoRepository のインスタンスを使用するように設定


//emlist[][cs]{
[assembly: FunctionsStartup(typeof(TodoBot.Server.Startup))]
namespace TodoBot.Client
{ 
    public class Startup : FunctionsStartup
    {
        public override void Configure(IFunctionsHostBuilder builder)
        {
            //Using Azure Table Storage
            builder.Services.AddTransient<ITodoRepository>(provider => 
            {
                var configuration = provider.GetRequiredService<IConfiguration>();
                var connectionString = configuration.GetValue<string>("AzureWebJobsStorage");
                return new CloudTableRepository(connectionString);
            });
        }
    }
}
//}

===== TodoBotFunction クラスで ITodoRepository を受け取る


TodoBotFunction クラスにコンストラクタを追加し、その引数から ITodoRepository を受け取るようにします。


//emlist[][cs]{
public class TodoBotFunction
{
    private readonly ITodoRepository todoRepository;
    public TodoBotFunction(ITodoRepository todoRepository)
    {
        this.todoRepository = todoRepository;
    }
}
//}


あとは、各関数内で ITodoRepository の必要なメソッドを呼ぶだけです。


//emlist[][cs]{
[FunctionName("CreateTodo")]
public async Task<IActionResult> CreateTodo(
    [HttpTrigger(AuthorizationLevel.Anonymous, "post", Route = "todoList")] HttpRequest req,
    ILogger log)
{
    var json = await req.ReadAsStringAsync();
    var todo = JsonConvert.DeserializeObject<Todo>(json);

    await todoRepository.CreateTodoAsync(todo);

    return new CreatedResult("", $"{{\"id\":\"{todo.Id}\"}}"); 
}
//}

==== リクエスト元の検証を行う


作成した Todo API はパブリックに公開されているため、URLさえ知っていれば誰でもアクセス可能となっています。@<br>{}
今回 API を利用する側のLIFFアプリは、静的サイトとなっているため、クライアント側からAjaxでリクエストを行う必要があります。そうするとAPIキーを発行してリクエスト元の検証を行うといったこともできません　(クライアント側にAPIキーを記述するしかなく、隠蔽できないため)。  



この問題をクリアするために、LIFFクライアント SDK の getAccessToken() で取得できるアクセストークンを利用します。


//emlist[][cs]{
await Liff.GetAccessTokenAsync();
var accessToken = Liff.AccessToken;
//}

===== リクエストヘッダにアクセストークンを付加


クライアント(LIFFアプリ) 側は、リクエストヘッダにLIFF APIから取得したアクセストークンを付加してリクエストを送るようにします。  

 * ヘッダ名: X-LDC-Access-Token


===== サーバー側でアクセストークンを検証


サーバー側はヘッダに付加されたアクセストークンを取得して、リクエストの有効性を検証します。

 * TodoBotFunction の各関数にアクセストークンを検証するコードを追加


//emlist[][cs]{
[FunctionName("CreateTodo")]
public async Task<IActionResult> CreateTodo(
    [HttpTrigger(AuthorizationLevel.Anonymous, "post", Route = "todoList")] HttpRequest req,
    ILogger log)
{
    if (!await lineTokenService.VerifyTokenAsync(req.Headers["X-LDC-Access-Token"]))
    {
        return new ForbidResult();
    }
    //...
}
//}


アクセストークンの検証は "https://api.line.me/oauth2/v2.1/verify" にGetでリクエストすることで行います。  



有効なアクセストークンであれば、レスポンスに scope, client@<b>{id, expires}in　といった値が返ってきます。client_id には、LIFFを登録したチャネルの channelID が格納されているので、これを確認することで正規のチャネルに登録されたLIFFアプリからのリクエストであると判断できます。    



検証を行う処理も ILineTokenService として切り出し、依存関係の挿入を使って利用するようにします。  


//emlist[][cs]{
public class LineTokenService : ILineTokenService
{
    private HttpClient httpClient = new HttpClient();
    private string clientId;
    private string url;
    public LineTokenService(string clientId, string url= "https://api.line.me/oauth2/v2.1/verify")
    {
        this.clientId = clientId;
        this.url = url;
    }

    public async Task<bool> VerifyTokenAsync(string accessToken)
    {
        if (string.IsNullOrEmpty(accessToken)) { return false; }
        var response = await httpClient.GetStringAsync($"{url}?access_token={accessToken}");
        var result = JsonConvert.DeserializeAnonymousType(response, new { scope = "", client_id = "", expires_in= 0 });
        return result?.client_id == clientId;
    }
}
//}


Startup.cs にて ILineTokenService を登録し、TodoBotFunction のコンストラクタでインスタンスを受け取るようにします。


//emlist[][cs]{
 public class Startup : FunctionsStartup
{
    public override void Configure(IFunctionsHostBuilder builder)
    {

        builder.Services.AddSingleton<ILineTokenService>(provider =>
        {
            var configuration = provider.GetRequiredService<IConfiguration>();
            var clientId = configuration.GetValue<string>("LineClientId");
            return new LineTokenService(clientId);
        });
    }
}
//}

//emlist[][cs]{
public class TodoBotFunction
{
    private readonly ITodoRepository todoRepository;
    private readonly ILineTokenService lineTokenService;
    public TodoBotFunction(ITodoRepository todoRepository, ILineTokenService lineTokinService)
    {
        this.todoRepository = todoRepository;
        this.lineTokenService = lineTokenService;
    }
}
//}


//note[■ 補足]{



なお、検証に使用するクライアントID（channelID）は Function App のアプリケーション設定にあらかじめ設定しておき、そこから取得するようにしています。



//}


=== Azure Function のデプロイ


Visual Studioからデプロイを行います。@<br>{}
また、アプリの発行と同時に Azure Functions のリソースも作成します。（作成済みのリソースを使用してもかまいません。その場合は適宜手順を読み替えてください)

 * ソリューションエクスプローラで「TodoBot.Server」を右クリック
 * 「発行...」をクリック
 * 「発行先を選択」で「Azure Functionsの従量課金制プラン」を選択
 * 「新規作成」を選択、「パッケージファイルから実行」のチェックを外して「すぐに発行」をクリック
 * App Service 新規作成画面で以下を入力後、「作成」ボタンをクリック
 ** 名前
 ** サブスクリプション
 ** リソースグループ（既存のものから選択するか、「新規..」ボタンで新規作成）
 ** 場所 (Japan East)
 ** Azure Storage（既存のものから選択するか、「新規..」ボタンで新規作成）



//image[azure-function][App Service新規作成]{
//}




続いて、Azure ポータルでCORSの設定を行います。 

 * Azure ポータルへログイン
 * 作成した Function App のページを開き、作成した Function App名を選択
 * 「Platform features」タブをクリックして 「API」欄の「CORS」をクリック
 * 「Allowed Origins」欄にGitHubページのドメイン https://{アカウント名}.github.io を追加
 * 「Save」をクリック



//image[function-cors][CORS の設定]{
//}




さらに、Azure ポータルでアプリケーション設定に検証用のクライアントID（channelID）を設定します。

 * LINE Developers の「チャネル基本設定」ページに記載されている「Channel ID」の値を取得
 * Azure ポータルへログイン
 * 作成した Function App のページを開き、作成した Function App名を選択
 * 「Platform features」タブをクリックして 「General Settings」欄の「configuration」をクリック
 * 「+ New Application Settings」をクリック
 * 「Name」に "LineClientId" を、「Value」に 取得したchannelIDを入力



//image[app-settings][アプリケーション設定]{
//}



== Blzaor  (client-side) アプリの実装


Blazor (client-side) アプリ本体 (TodoBot.Client) の実装について解説します。  



まずは、画面周りの開発を行う前に LIFF と Todo API を使用するための準備を行います。

 * LIFF クライアント SDK の初期化と利用
 * Todo API を利用した Todo データへのアクセス


=== LIFF クライアント SDK の初期化と利用


LIFFクライアントSDKのC#ライブラリの初期化と利用方法について確認します。


==== ILiffClientをDIする


ライブラリLineDC.Liff では、ILiffClient インターフェースを通して LIFF クライアントの機能を利用します。@<br>{}
ILiffClient インターフェースは、依存関係の挿入(DI)を利用して各ページで使用できるように設定します。

 * Startup.cs の ConfigureServices メソッド内で LiffClient をSingleton サービスとして登録


//emlist[][cs]{
public void ConfigureServices(IServiceCollection services)
{
    services.AddSingleton<ILiffClient, LiffClient>();
}
//}
 * 使用するページ（*.razor）に @inject ディレクティブを追加し、ILiffClient インターフェースを指定@<br>{}


//emlist[][cs]{
@inject ILiffClient Liff
//}

==== LIFFの初期化


LIFFクライアントを使用するには、始めに初期化処理を行う必要があります。LIFFの初期化には LiffClient.InitializeAzync() メソッドを使用します。@<br>{}
今回は、トップページ(Index.razor)の初期化時に実行されるメソッド OnInitializedAsync() でLIFFの初期化を行うようにしています。 
また、クライアント側から Azure Functions のTodo API を利用する際に必要となるアクセストークンもここで取得しています。  


//emlist[][cs]{
page "/"
@inject ILiffClient Liff
@inject IJSRuntime JSRuntime

@code{
    protected override async Task OnInitializedAsync()
    {
        await Liff.InitializeAsync(JSRuntime);
        await Liff.GetAccessTokenAsync();
    }    
}
//}


初期化が成功すると、LINE から取得したユーザー情報等を LiffClient のプロパティから取得できるようになります。@<br>{}
今回使用する "ユーザーID" および　"アクセストークン" には、以下の様にアクセスします。


//emlist[][cs]{
var userId = Liff.Data.Context.UserId;
var accessToken = Liff.AccessToken
//}

=== Todo API を利用した Todo データへのアクセス


Azure Functions で作成した Todo API を利用して、Todoデータを読み書きする処理を実装します。


==== ITodoClient インターフェース


APIの仕様に合わせて、以下のようなインターフェースを用意します。@<br>{}
APIサーバー側の ITodoRepository とほぼ同じですが、各メソッドの第一引数に検証用のアクセストークンを渡すようにしています。


//emlist[][cs]{
public interface ITodoClient
{
    Task CreateTodoAsync(string accessToken, Todo todo);
    Task DeleteTodoAsync(string accessToken, string userId, string id);
    Task<Todo> GetTodoAsync(string accessToken, string userId, string id);
    Task<IList<Todo>> GetTodoListAsync(string accessToken, string userId);
    Task UpdateTodoAsync(string accessToken, string id, Todo todo);
}
//}

==== API呼び出し処理の実装


実際にTodo API を利用する部分の実装を見てみます。



Blazor(client-side)では、C# 開発者にはおなじみの HttpClient クラスを使ってAjaxリクエストを送信することができます。



//note[■ 注意]{



HttpClient は Blzaor のフレームワーク側から依存関係の挿入(DI)で渡される特殊なインスタンスを使用する必要があります。  自分でインスタンス化したものは使用できないので注意が必要です。



//}



-　HttpClient で各APIのリクエストを記述
- リクエストヘッダ "X-LDC-Access-Token" にLIFF SDKから取得したアクセストークンを設定して送信


//emlist[][cs]{
public class TodoClient : ITodoClient
{
    private string baseUrl;
    private HttpClient httpClient;

    public TodoClient(HttpClient httpClient, string baseUrl)
    {
        this.baseUrl = baseUrl;
        this.httpClient = httpClient;
    }

    public async Task CreateTodoAsync(string accessToken, Todo todo)
    {
        var json = JsonConvert.SerializeObject(todo);
        var content = new StringContent(json, Encoding.UTF8, "application/json");
        await SendAsync(
            accessToken,
            HttpMethod.Post,
            $"{baseUrl}/api/todoList",
            content);
    }

    private async Task<HttpResponseMessage> SendAsync(string accessToken, HttpMethod method, string url, HttpContent content = null)
    {
        var request = new HttpRequestMessage(method, url);
        request.Headers.Add("X-LDC-Access-Token", accessToken);
        request.Content = content;
        var response = await httpClient.SendAsync(request);
        return response.EnsureSuccessStatusCode();
    }
}
//}


ITodoClient インターフェースも ILiffClient と同様に依存関係の挿入(DI)の仕組みを使って各ページから利用できるように設定します。

 * Startup.cs の ConfigureServices メソッド内で ITodoClient をSingleton サービスとして登録
 * その際、TodoClient のインスタンス生成処理を指定。コンストラクタに HttpClientのインスタンス　と Todo APIのBase URLを指定


//emlist[][cs]{
public void ConfigureServices(IServiceCollection services)
{
    services.AddSingleton<ITodoClient, TodoClient>(provider =>
　　    new TodoBotClient(provider.GetService<HttpClient>(), "https://myTodo.azurewebsites.net"));
}
//}


※ HttpClient のインスタンスは自前で生成せずに、Blazor のフレームワーク側で既にサービスとして登録されているものから GetService() で取得して指定します。

 * 使用するページ（*.razor）に @inject ディレクティブを追加し、ITodoClient インターフェースを指定@<br>{}


//emlist[][cs]{
@page "/create"
@inject ILiffClient Liff
@inject IJSRuntime JSRuntime
@inject ITodoClient TodoClient

@code
{
    async Task CreateTodoAsync(Todo todo)
    {
        todo.UserId = Liff.Data.Context.UserId;
        await TodoClient.CreateTodoAsync(Liff.AccessToken, todo);
    }
}
//}

=== Todo管理画面の実装


それでは、Blazor による画面周りの実装を見てみましょう。



Todo管理用の画面は「Pages」フォルダ内の *.razor ファイルに実装されています。

 * 「Pages」フォルダ
 ** Todo一覧画面 (Index.razor(Topページ))
 ** Todo作成画面 (Create.razor)
 ** Todo編集画面 (Edit.razor)



また、各ページ共通で使用できる部分はコンポーネントとして切り出して定義します。

 * 「Components」フォルダ
 ** Alert.razor: エラー等のアラートを表示するコンポーネント
 ** TodoEditor.razor: Todoの作成および編集時の入力フォームコンポーネント


==== Blzaor アプリ (TodoBot.Client) の開発にモックを使う


画面周りの実装を行う際には、外部サービスとの連携部分をモックに差し替えておきます。



サンプルコードでは ".\TodoList.Client\Services\Mock" フォルダ内に2つのモッククラスが用意されています。

 * MockLiffClient@<br>{}LIFFクライアントライブラリ LineDC.Liff.LiffClient のモック。ダミーのユーザー情報を返します。
 * MockTodoClient@<br>{}Todo API を利用するHttpClient (TodoClient) のモック。メモリ上のTodo一覧にTodoデータを保存します。



モックへの差し替えは、Startup.cs の ConfigureServices メソッドを以下の様に書き換えるだけでOKです。


//emlist[][cs]{
 public void ConfigureServices(IServiceCollection services)
{
    services.AddSingleton<ILiffClient, MockLiffClient>();
    services.AddSingleton<ITodoClient, MockTodoClient>();

    //services.AddSingleton<ILiffClient, LiffClient>();
    //services.AddSingleton<ITodoClient, TodoClient>(provider =>
    //    new TodoClient(provider.GetService<HttpClient>(), "https://myTodo.azurewebsites.net"));
}
//}

=== Todo作成画面


画面の実装について、Todo作成画面 (Create.razor) を例に見てみます。


==== Todo作成画面 (Create.razor)


作成する Todo の入力フォームに「TodoForm」コンポーネントを使用しています。



コンポーネントにはパラメータを渡すことができます。パラメータには関数（デリゲート）を渡すことも可能です。@<br>{}
この例では、「作成」ボタンをクリックしたら「CreateTodoAsync」メソッドが実行されます。

 * Todo : フォームの初期値となるTodoオブジェクトを設定
 * ButtonName : フォームの Submit ボタンの表示名を指定
 * ButtonClickAsync : ボタン押下時に実行する処理を指定



また、エラー時は「Alert」コンポーネントにエラーの内容を表示するようにしています。


//emlist[][cs]{
@page "/create"
@inject ILiffClient Liff
@inject IJSRuntime JSRuntime
@inject ITodoClient TodoClient

<TodoForm Todo="new Todo() { DueDate = DateTime.Now}"
            ButtonName="作成"
            ButtonClickAsync="CreateTodoAsync"></TodoForm>

<Alert Message="@ErrorMessage" AlertType="danger"></Alert>

@code{
    string ErrorMessage;
    async Task CreateTodoAsync(Todo todo)
    {
        ErrorMessage = null;
        try
        {
            todo.UserId = Liff.Data.Context.UserId;
            await TodoClient.CreateTodoAsync(Liff.AccessToken, todo);
        }
        catch (Exception e)
        {
            ErrorMessage = e.Message;
        }
    }
//}

==== Todo入力フォーム (TodoForm.razor)


Todo入力フォームの実装も見てみましょう。  



Todo作成画面から渡されたパラメータは、 Parameter 属性を付けたプロパティ(またはフィールド) に格納されます。


//emlist[][cs]{
@inject ILiffClient Liff
@inject IJSRuntime JSRuntime
@inject HttpClient httpClient
@inject IUriHelper UriHelper

<EditForm style="max-width:24rem;" Model="Todo" OnValidSubmit="OnButtonClickAsync">
    <DataAnnotationsValidator />
    <div class="form-group">
        <label for="title">タイトル</label>
        <input type="text" name="title" class="form-control" @bind="Todo.Title" />
    </div>
    <div class="form-group">
        <label for="content">詳細</label>
        <textarea class="form-control" name="content" @bind="Todo.Content" />
    </div>
    <div class="form-group">
        <label for="status">状況</label>
        <select class="form-control" name="status" @bind="Todo.Status">
            <option value="@Status.Ready">@Status.Ready.ToString()</option>
            <option value="@Status.Doing">@Status.Doing.ToString()</option>
            <option value="@Status.Done">@Status.Done.ToString()</option>
            <option value="@Status.Canceled">@Status.Canceled.ToString()</option>
        </select>
    </div>
    <div class="form-group">
        <label for="dueDate">期日</label>
        <input type="date" class="form-control" name="dueDate" @bind="Todo.DueDate" @bind:format="yyyy-MM-dd" />
    </div>
    <div class="text-danger">
        <ValidationSummary></ValidationSummary>
    </div>
    <button class="form-control btn btn-primary">@ButtonName</button>
</EditForm>

<Alert Message="@ErrorMessage" AlertType="danger"></Alert>

@code{
    [Parameter]
    public Todo Todo { get; set; }

    [Parameter]
    public string ButtonName { get; set; }

    [Parameter]
    public Func<Todo, Task> ButtonClickAsync { get; set; }

    string ErrorMessage;

    async Task OnButtonClickAsync()
    {
        ErrorMessage = null;
        try
        {
            await ButtonClickAsync?.Invoke(Todo);
            UriHelper.NavigateTo($"{UriHelper.GetBaseUri()}");
        }
        catch (Exception e)
        {
            ErrorMessage = e.Message;
        }
    }
}
//}

===== インプットデータを双方向バインドする


フォーム内の各コントロールの @bind 属性にデータを保持するオブジェクトを指定することで、入力値をオブジェクトにバインドすることができます。


//emlist[][html]{
<input type="text" name="title" class="form-control" @bind="Todo.Title" />
//}


select コントロールと enum のバインドは 以下の様に行います。選択肢としてenum の要素を option に設定します。


//emlist[][html]{
<select class="form-control" name="status" @bind="Todo.Status">
    <option value="@Status.Ready">@Status.Ready.ToString()</option>
    <option value="@Status.Doing">@Status.Doing.ToString()</option>
    <option value="@Status.Done">@Status.Done.ToString()</option>
    <option value="@Status.Canceled">@Status.Canceled.ToString()</option>
</select>
//}


また、Date コントロールを DateTime 型でバインドする場合、以下の様に Format を指定する必要があります。


//emlist[][html]{
<input type="date" class="form-control" name="dueDate" @bind="Todo.DueDate" @bind:format="yyyy-MM-dd" />
//}

===== バリデーション機能を使う


フォーム入力に EditForm コンポーネントを使用します。これによりフレームワークが提供するバリデーションの機能が使えるようになります。

 * モデルクラス (Todo) にバリデーション用の属性を追加


//emlist[][cs]{
public class Todo
{
    public string Id { get; set; }
    public string UserId { get; set; }

    [Required(ErrorMessage ="タイトルを入力してください。")]
    public string Title { get; set; }
    public string Content { get; set; }
    public Status Status { get; set; }

}
//}


//note[■ 参考]{



今回はタイトルを入力必須とするのみの実装ですが、Asp.Net MVC の開発でもおなじみの System.ComponentModel.DataAnnotations で定義されるアノテーションがそのまま使用できます。  



https://docs.microsoft.com/ja-jp/dotnet/api/system.componentmodel.dataannotations



//}

 * EditForm の Model にモデルクラス (Todo) の名前を、OnValidSubmit に Submit ボタン押下時に実行する関数の名前を指定
 * EditForm 内に DataAnnotationsValidator コンポーネントを追加
 * バリデーションエラーの出力先として ValidationSummary コンポーネントを配置


===== Submit 時の処理をカスタマイズする


フォーム内のボタンをクリックし入力値の検証が通ると、OnValidSubmit に設定された OnButtonClickAsync メソッドが呼ばれます。



OnButtonClickAsync 内では、 パラメータとして ButtonClickAsync に設定されたデリゲートを呼んでいます。@<br>{}
これにより、Submit 時に実行される処理を「Todo 作成画面」では Todo の作成、「Todo 編集画面」ではTodoの更新といった具合にカスタマイズすることが可能となります。


//emlist[][cs]{
async Task OnButtonClickAsync()
{
    ErrorMessage = null;
    try
    {
        await ButtonClickAsync?.Invoke(Todo);
        UriHelper.NavigateTo($"{UriHelper.GetBaseUri()}");
    }
    catch (Exception e)
    {
        ErrorMessage = e.Message;
    }
}
//}
