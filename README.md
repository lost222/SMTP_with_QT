# SMTP_with_QT

NKU Computer Network 1

## 实验亮点

1. 能够实现任意大小图片接收

   得益于TCP流控制特性， 如果要发送的信息大于TCP接收窗口， TCP会抑制发送端。 保证数据都能按序到达。具体到我们的SMTP上， 主要就是邮件代理发送图片的BASE64编码时候的处理。

   我们这边的处理也很简单。原理上

   >C : DATA
   >
   >S : 354 End data with <CR><LF>.<CR><LF>\r\n
   >
   >C : data part 1 
   >
   >C : data part 2

   服务器只要什么也别发， 一直接收，直到接收到

   >C : . \r\n

   才停止接收，返回

   >S : 250 OK

   具体代码是这么实现的

   ~~~c++
   // DATA ： 不遇到 . 不能停下
   if (strncmp(request, "DATA", 4) == 0) {
       isdata = true;
       return reply_code[8]; //354 End data with <CR><LF>.<CR><LF>\r\n
   }
   
   if (this->isdata){
           this->data += request;
   //		int i=0;
   		std::size_t rfound = data.rfind(".\r\n");
   		if(rfound == std::string::npos){return "Nsend";}  // 关键
   		this->save_mail();
   		isdata = false;
           islogin = 0;
   		return reply_code[6]; //250 OK
       }
   ~~~

2. 全局只有一个send。 

   send功能只在监听线程里调用。 整个响应逻辑是这样

   >监听进程接收邮件客户代理命令
   >
   >命令作为字符串输入SMTP_server类， 经过处理 ---  涉及一点读写文件 和 信号传递
   >
   >SMTP_server类返回字符串， 作为输出 --- 例外是 "Nsend"字符串不send
   >
   >监听进程send返回的字符串

   这么写的目的有三个：

   1. 便于调试。 调试的时候完全可以将网络模块分离， 使用cin和cout调试SMTP_server的功能。实际上也是这么做的。当然后来也使用了telnet 简单调试 --- telnet会在你调用send之外的时候发一些信息。

   2. 头文件包含问题比较干净。有一个问题：

      `winsock2.h`和 `windows.h` 必须先包含前者。 这是由于`winsock.h` 和前两个头文件兼容遗留问题。减少包含这两个头文件的文件，能够快速安全运行。

   3. 
      考虑实现完整的SMTP功能的话， 完全可以把 `SMTP_server `类实现为比较标准的有限自动机。实际上关于data和login的部分还是参考了有限自动机的思路     

## 实验目标

1. 实现一个SMTP服务器，实现从用户代理接收邮件，将之储存，并且以一定格式显示。

2. 展现SMTP服务器与用户代理的交互过程

3. 实现多用户支持

## SMTP 小结

###  命令-响应式协议

SMTP是一个简单的命令-响应式协议。 邮件用户代理向邮件服务器发起TCP请求。 TCP连接之后邮件服务器基于用户代理的输入做出响应的动作， 回复合适的响应。 响应格式类似FTP ，数字在前描述在后。 数字机器看， 描述给用户。一个典型的SMTP用户代理和邮件服务器行为描述如下

```c
S: ``220` `www.example.com ESMTP Postfix
C: HELO mydomain.com
S: ``250` `Hello mydomain.com
C: MAIL FROM: <sender@mydomain.com>
S: ``250` `Ok
C: RCPT TO: <friend@example.com>
S: ``250` `Ok
C: DATA
S: ``354` `End data ``with` `<CR><LF>.<CR><LF>
C: Subject: test message
C: From:``""``< sender@mydomain.com>
C: To:``""``< friend@example.com>
C:
C: Hello,
C: This ``is` `a test.
C: Goodbye.
C: .
S: ``250` `Ok: queued ``as` `12345
C: quit
S: ``221` `Bye
```

### 历史遗留和MIME

 First defined by [RFC](https://en.wikipedia.org/wiki/Request_for_Comments_(identifier)) [821](https://tools.ietf.org/html/rfc821) in 1982, it was updated in 2008 with [Extended SMTP](https://en.wikipedia.org/wiki/Extended_SMTP) additions by [RFC](https://en.wikipedia.org/wiki/Request_for_Comments_(identifier)) [5321](https://tools.ietf.org/html/rfc5321), which is the protocol in widespread use today.

SMTP协议最早定义于RFC 821 , 1982年。产生之初只考虑了英语的传输问题。到现在看来有很多历史遗留问题。

* 所有报文（massage）的主体部分只能用简单的7位acsii码表示， 多国语言支持是没有的。
* 所有报文（massage）的主体部分只能用简单的7位acsii码表示，初衷是在网络带宽不足（1980s）时没人想用电子邮件发大的二进制文件。时至今日，这个限制要求我们把大二进制文件先编码为ACSII码， 接收到后又解码。

MIME就是相应的扩展协议。 MIME中定义了多种文件的编码方式， 本次我们只使用base64算法。

### base64 简述

三编四的编码算法。 将三个BYTE 编码为四个ASCII数。

![](C:\Users\杨起\Documents\计算机网络\bs64.png)

**特殊地方**

不够三个的话， 在末尾添加1 或者 2个 =

![](C:\Users\杨起\Documents\计算机网络\b642.png) 

实际问题：

邮件代理发过来的字母不一定在base64字母表里。这个问题其实经常出现。因为每一次邮件代理发的命令都是有

~~~c++
bool is_not_b64_member(char &c){
    if (isalnum(c) || c=='+' || c=='/' || c=='='){return false;}
    return true;
}

my_base64::my_base64(std::string &input_s) {
    this->to_decode = input_s;
    // 如果不在base64使用的编码字符集合里，直接去除这个字符
    to_decode.erase(remove_if(to_decode.begin(), to_decode.end(), is_not_b64_member), to_decode.end());
    std::cout<<"Length : "<<to_decode.length()<<std::endl;
    int i=0;
    // 四位对齐， 如果字符串不是四的整倍数，直接丢弃余数
    if (to_decode.length() % 4 != 0){ 
        i = to_decode.length() % 4;
        for(int j=0; j<i;j++){
            to_decode.erase(to_decode.end() - 1);
        }
    }
}
~~~







## 实现细节

### WINSOCK初探

WINSOCK提供了一套C风格的Win32API, 和JAVA SOCKET 编程比起来有更大的自由度.当然使用也相对复杂.

#### 创建一个客户端SOCKET的流程

1. 创建一个SOCKET

   ~~~c
   //初始化
   iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
   //获得地址信息
   iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
   //创建监听SOCKET
   ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
   ~~~

   中间有几个变量比较重要

   `  struct addrinfo hints;`

   该变量储存SOCKET的配置信息.我们通过初始化hint获得我们想要的流式TCP SOCKET

   addrinfo结构体如下

   ~~~c
   typedef struct addrinfo {
     int             ai_flags;   // 精细控制信息
     int             ai_family;  //地址协议族  IPV4 or IPV6
     int             ai_socktype;  //socket类型, 单工 or 双工
     int             ai_protocol;  //TCP or UDP
     size_t          ai_addrlen; // 地址长度
     char            *ai_canonname;  //主机规范名称
     struct sockaddr  *ai_addr;  // 通信IP地址
     struct addrinfo  *ai_next;  //链表中下一项
   } ADDRINFOA, *PADDRINFOA;
   ~~~



   函数`getaddrinfo`

   ```c
   int WSAAPI getaddrinfo(
     __in_opt  PCSTR pNodeName,
     __in_opt  PCSTR pServiceName,
     __in_opt  const ADDRINFOA *pHints,
     __out     PADDRINFOA *ppResult
   );
   ```

   简单说将结构体 hint中的IP地址赋值给result只指向的结构体, 我们以此指定我们想要的Listen Socket

2. 绑定监听端口,开始监听

   ~~~c++
   //绑定端口
   iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
   //开始监听
   iResult = listen(ListenSocket, SOMAXCONN);
   ~~~

3. 处理链接 

   ~~~c++
    // 由监听SOCKEZT产生新的SOCKET,accept在WINDOWS下和UNIX下稍有不同
    ClientSocket = accept(ListenSocket, NULL, NULL);
    //一般而言要loop等待接听,我们这里直接关闭监听端口
    closesocket(ListenSocket);
   
   ~~~

4. 至此我们得到了一个TCP链接 `ClientSocket`我们以这个SOCKET 进行 SMTP和邮件代理的交互.

### 一些TRICK

#### 1. 全局变量根路径

	将项目的根路径写在`my_base64.h`中.具体而言

```c
const std::string PROJECT_PATH = "C:\\code\\s_SMTP\\s_SMTP\\";
```

	有这个需求是因为osfstream不能使用相对路径打开文件
	
	WINDFOWS 下有函数可以实现需求,但是返回的是.exe所在文件夹,即debug文件夹.权宜之计,将项目文件夹写在代码中

#### 2 . recvbuf大小比声明的大一

~~~c
iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
if (iResult <= 0) { continue;}
recvbuf[iResult] = '\0';
~~~

技巧,利用`iResualt`变量返回的是从Socket获得的字符串长度,刚好将recvbuf字符串的末位置为'\0', 方便后来将它作为字符串处理.

但是在recvbuf全满的时候会数组越界,为了避免这个情况.实际上申请的大小比使用的大一.



## 工程结构

1. `mainwindow`类 包括 `mainwindow.h` `mainwindow.cpp` ,QT的主进程。 做图形界面初始化。 监听处理各种事件
2. `my_base64.h`&& `my_base64.cpp` 包括my_base64类和 SMTP_server类。 一个核心解码算法， 一个决定SMTP服务器的行为（思路是有限自动机）。

3. `listenthread.h` && `listenthread.cpp` 监听线程， 负责监听本机的25端口， 产生新的socket和邮件用户代理通信。

4. `form.h` && `form.cpp` ， 一个新的页面，主要是处理图片显示。解决图片和邮件内容一起显示的话图片缩放不好定义问题。