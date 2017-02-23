#ifndef __MPUSH__
#define __MPUSH__

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

int Cpush_connect(unsigned int* clientHandle,struct connectOpt* pOpt);
int Cpush_disconnect(unsigned int clientHandle);
int Cpush_publish(unsigned int clientHandle, unsigned char* topic, unsigned char* msg, unsigned char qos, unsigned char retain);
int Cpush_unsubscribing(unsigned int clientHandle, unsigned char* topic);
int Cpush_subscribing(unsigned int clientHandle, unsigned char* topic, unsigned char qos, int (*cb_fn)(struct Message*));
int Cpush_checkTime(unsigned int clientHandle, unsigned int timeMs);




#endif