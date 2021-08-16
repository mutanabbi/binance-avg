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
