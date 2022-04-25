Litmus是一套用于测试内存一致性模型的工具集，常常用于验证程序内存一致性等相关问题。
要运行Litmus测试需要安装herdtools7工具。下面在Ubuntu 20.04主机上安装该工具。

$ sudo apt install opam
$ opam init
$ opam update
$ opam install herdtools7
$ eval $(opam config env)

Run:
$ herd7 -cat aarch64.cat example_18_12_asm.litmus

详情请看这篇文章：
https://mp.weixin.qq.com/s/dSD88LSml1QYy9Btg8lp3w
https://mp.weixin.qq.com/s/yJQGfBjQzbs5VJAgssLxyQ
