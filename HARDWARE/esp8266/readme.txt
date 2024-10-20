AT+CWMODE=1         Station模式
AT+RST              重启
AT+CWJAP = "ssid","password"        连接网络
AT+CIPMUX=0         单路连接
AT+CIPSTART = "TCP","api.seniverse.com",80

AT+CIPMODE=1

AT+CIPSEND      向服务器发送数据

GET https://api.seniverse.com/v3/weather/now.json?key=smtq3n0ixdggurox&location=zhengzhou&language=zh-Hans&unit=c