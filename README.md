shanxun-linux
====
A shanxun client for linux users in Python.

Requriements
----
- pppoeconf
```sh
sudo pppoeconf # You can press enter to skip some config
```
- Python 2.7
- A username ever logged on shanxun offical client for Mac

Usage
----
```sh
sudo python shanxun.py your_username your_password
```

Show ppp log
```sh
plog
```

Set ppp as the default route.
```sh
route # Query route table and find the ppp device
sudo route add default dev ppp0 # Example 
```
To stop the connection
```sh
poff dsl-provider
```

License
-------
shanxun-linux is licensed under the [MIT](#) license. 
