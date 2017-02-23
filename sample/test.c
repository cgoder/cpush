
#include <stdio.h>
#include "../client/mpush.h"


int msgaaaaaa(struct Message* msg)
{
	printf("msg:[%d]%s\n", msg->payloadlen,msg->payload);
	return 0;
}

int main(int argc, char** argv) {

	unsigned int clientHandle = 0;
	struct connectOpt connect;
	connect.clientid = "testClient";
	connect.username = "";
	connect.password = "";
	connect.host = "192.168.6.174";
	connect.port = 1984;
	printf("connect ...\n");

	int rc = Cpush_connect(&clientHandle,&connect);
	if (rc <0)
	{
		printf("connect fail:%d\n", rc);
		return -1;
	}
	printf("connect ok:%d\n", rc);


	rc = Cpush_subscribing(clientHandle, "pubtopic", 0, &msgaaaaaa);
	if (rc <0)
	{
		printf("subscribing fail:%d\n", rc);
		return -1;
	}
	printf("subscribing ok:%d\n", rc);		


	while (1)
	{
		Cpush_checkTime(clientHandle, 1000);	

		rc = Cpush_publish(clientHandle, "substopic", "heheda...", 0, 0);
		if (rc <0)
		{
			printf("publish fail:%d\n", rc);
			break;
		}
		// printf("publish ok:%d\n", rc);
	}
	
	printf("Stopping\n");

	rc = Cpush_disconnect(clientHandle);
	if (rc <0)
	{
		printf("disconnect fail:%d\n", rc);
		return -1;
	}
	printf("disconnect ok:%d\n", rc);	




	return 0;
}