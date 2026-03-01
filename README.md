前端页面：![alt text]({64FCA1F0-BFA0-40F6-8ED4-9C03A34D792A}.png)
服务器终端：![alt text]({BCCE9B7B-E58E-4026-805F-6ACC361F1484}.png)
项目整体思路：1.前端调用用css、HTML、js，其中用js写一个login登录函数 浏览器调用fetch函数发送http请求给服务器
             2.服务端按照socket➡️bind➡️listen➡️accept➡️fork➡️signal➡️handle_login()接收浏览器的请求且展示在终端 并且按照http格式返回到浏览器页

