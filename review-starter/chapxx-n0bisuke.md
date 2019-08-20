# たぶん最速！LINE Things ハンズオン in Node学園祭2018 #nodefest

Node学園祭2018で初心者向けのハンズオンやっております。

## 今日のハンズオンについて

今日は全体参加者的に人数が少ないことが見込まれるので、参加者の人たちの様子みながら適宜資料更新していきます。

## この資料について

![](https://i.gyazo.com/eb4eb1e94a09bbdeee0546ca76eb3fb5.png)

* この資料をハンズオンなどに活用してNode.js普及を促進させてもらえたら幸いです。
	* 使うときにコメント欄にコメント貰えると嬉しいです！
	* ハンズオン進めてみた感想などもコメント貰えると嬉しいです！

## 今日の流れ

全体を通して、Node.jsを触るキッカケが作れればいいなと思っています。
NodeSchoolで基礎をやりつつ、LINE BOTやLINE Thingsは「Node.jsやJSで具体的にこんなの作れるよ」というコンテンツ例として採用しているイメージです。

ずっとNodeSchoolやってたい！って人はそのままNodeSchoolやっててもOKです。

全3部です。スポットだけ混ざってもやれると思います。

## 1部: 「Node School」でNode.jsの基礎を学ぼう

[Node School](https://nodeschool.io/)はNode.jsの学習ワークショップツールです。

![](https://i.gyazo.com/462b366edd0e379ca5d281916dd8841b.png)

色々なレッスンがあります。

* javascripting: JSの勉強

```
npm install -g javascripting
```

* learnyounode

```
npm install -g learnyounode
```

**進められる人は自分の好きなレッスンをインストールして進めてみましょう。**

ん？情報少なくね？これだけだと無理！って人は

ちゃんとく先生の[NodeSchoolでNode.jsの学習を始めよう！インストール手順から基本操作までステップ別解説](https://dotstud.io/blog/node-school-before-learn/)を見ながら進めてみましょう。

## 2部: LINE BOT ハンズオン

Node.jsでLINE BOTを作ってみます。コピペでいけるので初心者の方でもお気軽です。

ちなみに資料は昨年のハンズオンの資料を使います。（少しアップデートしました）

> [1時間でLINE BOTを作るハンズオン (資料+レポート) in Node学園祭2017 #nodefest](https://qiita.com/n0bisuke/items/ceaa09ef8898bee8369d)

早く終わった人は@kenakamuさんの続きの資料があります。

> [LINE BOOT AWARDS 公式夏期講習 ～2018 年 7 月時点での LINE Bot の機能を振り返る～](https://qiita.com/kenakamu/items/b2580f2546b925832045)

* リッチメニュー
* URL スキーム ( カメラ/GPSなど)
* Flex Message
* LINE Pay
* LIFF

## 3部: LINE Things ハンズオン

LINE BOTを作った続きになります。以下の二つの資料を進めていきます。

**LINE ThingsはWeb Bluetoothに似た仕様で、ブラウザ(LIFF)のJavaScriptからBluetoothを制御できる技術です。**

今回はJavaScriptを書いてデバイスと連携する部分を作ってみます。

* 前半資料: [さっそくLINE Thingsを触ってみた（ESP32でリモートLチカ） #linedevday](https://qiita.com/n0bisuke/items/b0541a32f01007f938a4)
* 後半資料: [LINE ThingsのLIFFアプリ(とデバイス)を自前で作る](https://qiita.com/n0bisuke/items/0c09ae5da43b551e98b1)

今日はM5Stackを5台用意しています。

<img src="https://qiita-image-store.s3.amazonaws.com/0/35387/0a88059d-d896-bb7b-4250-ca72543fb14b.png" width="200">



