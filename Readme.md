Приложение под Linux (Ubuntu 18-20) для поиска самого быстрого websocket подключения к маркетдате на [binance.com](https://binance-docs.github.io/apidocs/spot/en/#websocket-market-streams).
Нужно найти самый быстрый IP адрес для хоста `stream.binance.com`, по потоку `@depth`, через который маркетдата приходит быстрее.
Результат работы приложения нужно выводить в консоль с заданной периодичностью, как список IP адресов и минимальной/средней/максимальной задержек, отсортированный по средней задержке.
Как бонус можно сделать сборку стакана по потоку `@depth` на лету (без запроса снапшота). Стакан можно выводить вместе со статисикой.
Достаточно подписаться на один символ, который передаётся параметром.

apt-get install ssl-dev
apt-get install libboost-dev
mkdir build
cd build
cmake ..
make -j


	> avg usd/rur
	ip			avg		min		max
	---------------------------------
	10.0.0.1    15ms	5ms		25ms
	10.0.0.2    27ms	3ms		125ms

	bid		ask
	-----------
	120		30
	110		11
	50		10
	15		11

	10.0.0.1    15ms	5ms		25ms
	10.0.0.2    27ms	3ms		125ms

```
{
  "e": "depthUpdate", // Event type
  "E": 123456789,     // Event time
  "s": "BNBBTC",      // Symbol
  "U": 157,           // First update ID in event
  "u": 160,           // Final update ID in event
  "b": [              // Bids to be updated
    [
      "0.0024",       // Price level to be updated
      "10"            // Quantity
    ]
  ],
  "a": [              // Asks to be updated
    [
      "0.0026",       // Price level to be updated
      "100"           // Quantity
    ]
  ]
}
```

```
{
  "e":"depthUpdate",
  "E":1629132299478,
  "s":"BTCUSDT",
  "U":13041864808,
  "u":13041864883,
  "b":[
    ["46364.76000000","0.11232700"],
    ["46354.66000000","0.00000000"],
    ["46349.94000000","0.00215800"],
    ["46349.93000000","1.03821000"],
    ["46337.58000000","0.00000000"],
    ["46335.36000000","0.00000000"],
    ["46335.19000000","0.16000000"],
    ["46334.62000000","1.00000000"],
    ["46329.55000000","0.80000000"],
    ["46315.65000000","0.45311500"],
    ["46290.11000000","1.00000000"],
    ["46289.80000000","0.00000000"],
    ["46250.12000000","0.00215600"],
    ["46246.08000000","0.00000000"],
    ["46150.00000000","2.67393000"],
    ["46100.00000000","5.12067200"],
    ["45798.91000000","0.00058300"],
    ["45370.00000000","0.43211400"],
    ["44020.58000000","0.04464400"],
    ["42920.07000000","0.01141100"],
    ["41722.74000000","0.00000000"]
  ],
  "a":[
    ["46364.77000000","3.24388400"],
    ["46366.08000000","0.70299000"],
    ["46366.57000000","0.00000000"],
    ["46367.37000000","0.23509200"],
    ["46367.38000000","0.05232800"],
    ["46367.63000000","0.00000000"],
    ["46368.66000000","0.23509200"],
    ["46368.67000000","0.04314600"],
    ["46368.69000000","0.00000000"],
    ["46368.79000000","0.40000000"],
    ["46368.86000000","0.00000000"],
    ["46369.41000000","0.06647000"],
    ["46369.71000000","0.23509200"],
    ["46370.61000000","0.03321900"],
    ["46370.65000000","1.23800000"],
    ["46371.02000000","0.00000000"],
    ["46371.12000000","0.00000000"],
    ["46371.52000000","0.00000000"],
    ["46372.78000000","0.17800000"],
    ["46373.08000000","0.00000000"],
    ["46375.16000000","0.00000000"],
    ["46392.22000000","0.40000000"],
    ["46417.29000000","0.30264300"],
    ["46417.32000000","0.00000000"],
    ["46457.50000000","0.06100000"],
    ["46697.99000000","0.00000000"],
    ["46728.39000000","0.48490300"],
    ["48028.00000000","0.02833900"]
  ]
}
```

wss://stream.binance.com:9443/ws/bnbbtc@depth
book snapshot:
https://api.binance.com/api/v3/depth?symbol=BNBBTC&limit=1000

- All symbols for streams are lowercase
- A single connection to stream.binance.com is only valid for 24 hours; expect to be disconnected at the 24 hour mark
- The websocket server will send a ping frame every 3 minutes. If the websocket server does not receive a pong frame back from the connection within a 10 minute period, the connection will be disconnected. Unsolicited pong frames are allowed
- The base endpoint is: wss://stream.binance.com:9443
- Raw streams are accessed at /ws/<streamName>
- While listening to the stream, each new event's U should be equal to the previous event's u+1
- The data in each event is the absolute quantity for a price level.
- If the quantity is 0, remove the price level.
- Receiving an event that removes a price level that is not in your local order book can happen and is normal.
