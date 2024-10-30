これはBNO055にESP32を接続し、micro-rosを使用してros2上にimuのセンサーデータをpublishするプログラムです。
ESP32に書き込み、Agentを起動するコマンドを接続先のPCで実行すると起動することができます。
詳しくはmicro-rosの起動方法のリンクを参照してください。とても助かりました。
環境:
ROS2 Humble 
Ubuntu 22.04
ESP32 WROOM-DA Module

参考:
micro-rosの起動方法
https://qiita.com/ousagi_sama/items/b4eb3d9c6b337cbe1b05

IMUデータの使い方
https://qiita.com/hirekatsu0523/items/434bc03ab40ae4b05a05
