# <center> C API for Push </center>

## Connect 连接服务器
```c
int Cpush_connect(unsigned int* clientHandle,connectOpt* pOpt);
```

+ @参数 clientHandle [out] 连接句柄。若连接成功，此参数将被赋值为当前连接的句柄；否则被赋值为0。
+ @参数 pOpt [in] 连接参数，为connectOpt结构体指针。其中clientId最大长度不能超过23byte。
+ @返回值 成功返回0；失败返回<0的值。

## Disconnect 断开服务器
```c
int Cpush_disconnect(unsigned int clientHandle);
```

+ @参数 clientHandle [in] 连接句柄。
+ @返回值 成功返回0；失败返回<0的值。


## Publish 发布消息(指定主题)
```c
int Cpush_pushlish(unsigned int clientHandle, unsigned char* topic, unsigned char* msg, unsigned char qos, bool retain);
```

+ @参数 clientHandle [in] 连接句柄。
+ @参数 topic [in] 发布消息的主题。
+ @参数 msg [in] 发布的消息内容。
+ @参数 qos [in] 发布消息的质量。0最多发一次；1至少发一次；2仅发一次。
+ @参数 retain [in] 保留标志。如果需要服务器保留这条消息则设置为true；否则为false。
+ @返回值 成功返回0；失败返回<0的值。


## Subscribing 订阅主题
```c
int Cpush_subscribing(unsigned int clientHandle, unsigned char* topic, unsigned char qos, int (*cb_fn)(MessageData*));
```

+ @参数 clientHandle [in] 连接句柄。
+ @参数 topic [in] 要订阅的主题。
+ @参数 qos [in] 要订阅的主题消息质量。0最多发一次；1至少发一次；2仅发一次。
+ @参数 cb_fn [in] 回调函数。当收所订阅主题的消息时将调用此函数。参数1为消息体，参数2为消息体长度。
+ @返回值 成功返回0；失败返回<0的值。
+ **注意：目前只支持最多订阅5个主题。**

```c
int (*cb_fn)(struct Message*)
```
接收订阅消息注册的回调函数。当所订阅的主题有消息到达时，将调用此函数。故此函数内不能有任何需要延时或者可能导致中断的操作！


## Unsubscribing 取消订阅主题
```c
int Cpush_unsubscribing(unsigned int clientHandle, unsigned char* topic);
```
+ @参数 clientHandle [in] 连接句柄。
+ @参数 topic [in] 要取消订阅的主题。
+ @返回值 成功返回0；失败返回<0的值。

## CheckTime 接收及心跳
```c
int Cpush_checkTime(unsigned int clientHandle, unsigned int timeMs);
```
+ @参数 clientHandle [in] 连接句柄。
+ @参数 timeMs [in] 心跳及接收数据检测时间间隔。
+ **注意：此函数需要在循环中调用。**


## 数据结构
```c
typedef struct Message
{
    unsigned char qos;
    char retained;
    char dup;
    unsigned short id;
    void *payload;
    char payloadlen;
    char* topinName;
};

typedef struct connectOpt
{
    char* clientid;
    char* username;
    char* password;
    char* host;
    unsigned int port;
};

```


---
