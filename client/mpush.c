#include <stdio.h>
#include "MQTTClient.h"
#include "mpush.h"
// #include "linux/MQTTLinux.h"


enum {
	CPUSH_RET_OK = 0,
	CPUSH_RET_FAIL = -1,
	CPUSH_RET_ERROR_PARA = -2,
	CPUSH_RET_ERROR_CONNECTFAIL = -3,
}CPUSH_RET;


/*static*/
#define MOSQ_MQTT_BUFFER_MAX_SIZE (1024*8)
#define MOSQ_MQTT_ID_MAX_LENGTH 23

int Cpush_connect(unsigned int* clientHandle,struct connectOpt* pOpt)
{
	int ret = CPUSH_RET_OK;
	*clientHandle = 0;
	unsigned char* buf = 0;
	unsigned char* readbuf = 0;
	Network* network = NULL;
	Client* pClient = NULL;

	if (!pOpt) {
		ret = CPUSH_RET_ERROR_PARA;
		goto FAIL;
	}

	/*check client id*/
	if ((!pOpt->clientid)|| (strlen((const char*)pOpt->clientid) > MOSQ_MQTT_ID_MAX_LENGTH)){
		ret = CPUSH_RET_ERROR_PARA;
		goto FAIL;
	}

	/*connect to host*/
	// Network n;
	network = (Network*)calloc(1,sizeof(Network));
	if (!network) {
		ret = CPUSH_RET_FAIL;
		goto FAIL;
	}
	NewNetwork(network);
	if (ConnectNetwork(network, (char*)pOpt->host, pOpt->port) < 0) {
		printf("connect to %s:%d error!\n", pOpt->host, pOpt->port);
		ret = CPUSH_RET_ERROR_CONNECTFAIL;
		goto FAIL;
	}

	/*init mqtt client*/
	pClient = (Client*)calloc(1,sizeof(Client));
	if (!pClient) {
		ret = CPUSH_RET_FAIL;
		goto FAIL;
	}
	buf = (unsigned char*)calloc(MOSQ_MQTT_BUFFER_MAX_SIZE,sizeof(char));
	if (!buf) {
		ret = CPUSH_RET_FAIL;
		goto FAIL;
	}
	readbuf = (unsigned char*)calloc(MOSQ_MQTT_BUFFER_MAX_SIZE,sizeof(char));
	if (!readbuf) {
		ret = CPUSH_RET_FAIL;
		goto FAIL;
	}
	MQTTClient(pClient, network, 1000, buf, MOSQ_MQTT_BUFFER_MAX_SIZE, readbuf, MOSQ_MQTT_BUFFER_MAX_SIZE);

	/*connect to mqtt server*/
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;       
	data.willFlag = 0;
	data.MQTTVersion = 4;
	data.cleansession = 1;
	data.clientID.cstring = pOpt->clientid;
	if (pOpt->username) {
		data.username.cstring = pOpt->username;		
	}
	if (pOpt->password) {
		data.password.cstring = pOpt->password;		
	}
	data.keepAliveInterval = 10;
	printf("Connecting to %s %d\n", pOpt->host, pOpt->port);
	int rc = MQTTConnect(pClient, &data);
	printf("Connected %d\n", rc);
	if (SUCCESS != rc) {
		printf("Cpush_connect fail! ret:%d", rc);
		ret = CPUSH_RET_FAIL;
		goto FAIL;
	}
	printf("Cpush_connect OK!");
	ret = CPUSH_RET_OK;
	*clientHandle = (unsigned int)pClient;
	return CPUSH_RET_OK;

FAIL:
	if (0 != network->my_socket) {network->disconnect(network);}
	if (network) {free(network);}
	if (buf) {free(buf);}
	if (readbuf) {free(readbuf);}
	if (pClient) {free(pClient);}
	return ret;
}

int Cpush_disconnect(unsigned int clientHandle)
{
	if (0 == clientHandle) {
		return CPUSH_RET_ERROR_PARA;
	}

	Client* client = (Client*)clientHandle; 
	if (SUCCESS != MQTTDisconnect(client)) {
		return CPUSH_RET_FAIL;
	}

	if (0 != client->ipstack->my_socket) {client->ipstack->disconnect(client->ipstack);}
	if (client->ipstack) {free(client->ipstack);}
	if (client->buf) {free(client->buf);}
	if (client->readbuf) {free(client->readbuf);}
	if (client) {free(client);}
	return CPUSH_RET_OK;
}

int Cpush_publish(unsigned int clientHandle, unsigned char* topic, unsigned char* msg, unsigned char qos, unsigned char retain)
{
	if ((0 == clientHandle)||(NULL == topic)||(NULL == msg)) {
		return CPUSH_RET_ERROR_PARA;
	}

	if (qos > 2) {
		return CPUSH_RET_ERROR_PARA;
	}
	if (retain > 1) {
		return CPUSH_RET_ERROR_PARA;
	}

	Client* client = (Client*)clientHandle;
	MQTTMessage mqttMsg;
	if (0 == qos) {
		mqttMsg.qos = QOS0;
	} else if (1 == qos) {
		mqttMsg.qos = QOS1;
	} else if (2 == qos) {
		mqttMsg.qos =QOS2;
	}
	mqttMsg.retained = retain;
	mqttMsg.dup = 0;
	mqttMsg.id = 0;
	mqttMsg.payload = msg;
	mqttMsg.payloadlen = strlen((const char*)msg)+1;
	int rc = MQTTPublish(client, (const char*)topic, &mqttMsg);
	if (FAILURE == rc) {
		printf("Cpush_pushlish fail! ret:%d", rc);
		return CPUSH_RET_FAIL;
	}

	printf("Cpush_pushlish %s OK!", msg);
	return CPUSH_RET_OK;
}

int Cpush_subscribing(unsigned int clientHandle, unsigned char* topic, unsigned char qos, int (*cb_fn)(struct Message*))
{
	if ((0 == clientHandle)||(NULL == topic)||(qos>2)) {
		return CPUSH_RET_ERROR_PARA;
	}

	Client* client = (Client*)clientHandle;
	int rc = MQTTSubscribe(client,(const char*)topic,qos,cb_fn);
	if (FAILURE == rc) {
		printf("Cpush_subscribing fail! ret:%d", rc);
		return CPUSH_RET_FAIL;
	}

	printf("Cpush_subscribing OK!");
	return CPUSH_RET_OK;
}

int Cpush_unsubscribing(unsigned int clientHandle, unsigned char* topic)
{
	if ((0 == clientHandle)||(NULL == topic)) {
		return CPUSH_RET_ERROR_PARA;
	}

	Client* client = (Client*)clientHandle;
	int rc = MQTTUnsubscribe(client,(const char*)topic);
	if (FAILURE == rc) {
		printf("Cpush_unsubscribing fail! ret:%d", rc);
		return CPUSH_RET_FAIL;
	}

	printf("Cpush_unsubscribing OK!");
	return CPUSH_RET_OK;
}

int Cpush_checkTime(unsigned int clientHandle, unsigned int timeMs)
{
	Client* c = (Client*)clientHandle;
	MQTTYield(c, timeMs);
	return CPUSH_RET_OK;
}

/*
#define DEFAULT_ID_PREFIX "CPUSH"
static int client_id_generate(const char* id_prefix, char* pId)
{
	int len = 0;
	char hostname[256];
	char* prefix = NULL;

	hostname[0] = '\0';
	gethostname(hostname, 256);
	hostname[255] = '\0';

	if (id_prefix) {
		prefix = id_prefix;
	}
	else {
		prefix = DEFAULT_ID_PREFIX;
	}

	len = strlen(prefix) + strlen("/-")+ strlen(hostname) + 6 ;
	pId = malloc(len);
	if(!pId){
		printf(stderr, "Error: Out of memory.\n");
		return -1;
	}

	snprintf(pId, len, "%s/%s-%d", prefix, hostname, getpid());			
	if(strlen(pId) > MOSQ_MQTT_ID_MAX_LENGTH){
		pId[MOSQ_MQTT_ID_MAX_LENGTH] = '\0';
	}

	return 0;
}*/

