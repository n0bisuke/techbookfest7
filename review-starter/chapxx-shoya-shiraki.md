# LIFF 活用事例 & 開発 Tips の紹介

## はじめに
この章では
「LIFF を使うことでどんなことが実現できるのか」を知ってもらい  
「実現(開発)するために使える手段」と「実際に困ったことの解決策」を合わせて紹介することで  
「LIFF の良さを伝え、LIFF を使った開発事例が増える」ことを目的としています。

LIFF 自体の説明や導入方法は、LINE Developers のドキュメントや Web 上の記事が充実しているので最低限とします。

### 対象

「これから LIFF 使った開発をやってみたい人」から既にふれたことがあり「LIFF の開発で困っている人」まで  
「LIFF を使うことで LINE ボットの可能性を広げたい人」を対象とします。

### 目次
 - LIFF とは
 - 活用事例
    - 入力内容の確認
    - オートログイン
    - ボットユーザーと紐づける属性収集
    - LINE Thingsとの連携
 - 開発Tips
   - キャッシュ問題
   - liff-cli
 - LIFFであれやりたい・これやりたい
   - カメラ起動
      - `<input type="file" accept="image/*" capture="camera">`
      - iOSは問題なし, Androidは画像ギャラリーからの選択のみ
   - 複数画像の同時アップロード
   - ファイル受信
      - Messaging APIにもあるが、https://developers.line.biz/ja/reference/messaging-api/#wh-file LIFFでやったほうがステップ少なくできる
 - Q&A事例

## LIFF とは

LINE Front-end Framework の略称で LINE 内で動作するウェブアプリのプラットフォームです。

[LINE Front-end Frameworkの概要](https://developers.line.biz/ja/docs/liff/overview/)

LIFF がはじめてで、まず動かしてみたい・動作イメージをつかみたい方は[はじめてのLIFF #LINE_API](https://dev.classmethod.jp/etc/liff-tutorial/)の記事がおすすめです。

## LIFF 活用事例

LIFF が活用されている LINE ボットを紹介していきます。
「ボットの概要」「LIFF 活用ポイント」「もっと LIFF を活用するには」の3点で紹介していきます。

### AJINOMOTO
#### 概要
味の素が提供している LINE ボットで  
キャンペーン情報の配信・おすすめ献立の閲覧・フリーワードでのレシピの検索ができます。

<img src="images/chapxx-shoya-shiraki/test.jpg" width="20%">

![味の素QRコード](images/chapxx-shoya-shiraki/qr-ajinomoto.png)

#### LIFF 活用ポイント
「検索条件を追加する」をタップしたときに LIFF で設定画面が開きます。

LIFF で開いたページではレシピ検索の検索条件を設定でき  
「5分以内にレンジで作れる和風な主菜」といった条件を  
ユーザーはいちど設定すれば2回目以降は同じ設定での検索ができます。

毎日の晩ご飯を考えるときに使ってみよう となり
LIFF ユーザーが継続してボットを利用するハードルを下げています。

#### もっと LIFF を活用するには
検索された結果をタップすると、外部ブラウザによって詳細が開きます。

これを LIFF で開くようにしてお気に入りボタンを付けることで  
「今日調べた味噌汁のレシピが美味しかったのでまた作りたい」 
の実現ができることで、よりユーザーが使いたくなるボットになると思います。

### EPARKグルメ
#### 概要
EPARKグルメが提供している LINE ボットで、飲食店の検索・予約ができます。

<img src="images/chapxx-shoya-shiraki/test.jpg" width="20%">

![EPARKグルメQRコード](images/chapxx-shoya-shiraki/qr-epark.png)

#### LIFF 活用ポイント
「近くのお店を探す」をタップしたときに LIFF で地図が開きます。
地図上から位置情報を送信すると EPARK を使って予約できる近くのお店が表示され  
詳細の確認、電話予約、お気に入り登録ができます。

LINE ボットで近くのお店を検索して、そのまま(電話で)予約できるのは
2次会のお店探しや近くで晩ご飯を済ませたいときに使えるのではないでしょうか。

#### もっと LIFF を活用するには

LIFF を使うとWebの予約画面でユーザーの情報が取得できるようになるので  
検索だけでなく予約まで LINE ボット上で完結できると便利になると思います。

### アイカサ〜傘シェア〜
#### 概要

アイカサが提供している LINE ボットで、傘のシェアリングサービスを提供しています。

#### LIFF 活用ポイント
#### もっと LIFF を活用するには


### 福岡市
#### 概要

福岡市が提供している LINE ボットで、防災・ゴミの日・イベントなどの情報が配信されます。

<img src="images/chapxx-shoya-shiraki/test.jpg" width="20%">

![EPARKグルメQRコード](images/chapxx-shoya-shiraki/qr-fukuoka.png)

#### LIFF 活用ポイント
#### もっと LIFF を活用するには

## 開発 Tips
### キャッシュ問題への対策
### liff-cli の活用
### vConsole の活用

## Q & A 事例
### カメラ起動
### 複数ファイルの同時アップロード
### ファイル受信
