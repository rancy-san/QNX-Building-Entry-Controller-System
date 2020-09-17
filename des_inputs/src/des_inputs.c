#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include "../../des.h"
#include <sys/netmgr.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]) {

	// PID of controller (from argv)
	pid_t pid;
	// connection ID status to controller
	int coid;
	// message send return status
	int msgid;
	// message object to store messages
	Message message;
	// person object
	Person person;

	// store argv PID (display PID)
	sscanf(argv[1], "%d", &pid);

	// Establish a connection between a process and a channel
	// int ConnectAttach( uint32_t nd, pid_t pid, int chid, unsigned index, int flags );
	// nd, is the node descriptor of the node (LOCAL) on which the process that owns the channel is running
	// pid, is the process ID of the owner of the channel (DES_DISPLAY from argv[1]). If pid is zero, the calling process is assumed.
	// chid, is the channel ID, returned by ChannelCreate() (DES_DISPLAY), of the channel to connect to the process.
	// index, is the lowest acceptable connection ID.
	// flags, executes function to start new processes based on flag value
	coid = ConnectAttach(ND_LOCAL_NODE, pid, 1, _NTO_SIDE_CHANNEL, 0);

	// print COID
	// exit DES if connection did not attach
	if(coid == -1) {
		printf("(DES_INPUT) Connection ID: %d\nEXITING", coid);
		return EXIT_FAILURE;
	}
	else
		printf("(DES_INPUT) Connection ID: %d\n", coid);
	// valid CHID, continue

	for(;;) {
		// display menu message
		displayMenuMessage();
		// get the side the person is entering from
		scanf("%s", person.event);
		if(
				(strcmp(person.event, inMessage[LEFT_SCAN]) == 0)
				||
				(strcmp(person.event, inMessage[RIGHT_SCAN]) == 0)
		) {
			printf(outMessage[START_STATE]);
			scanf("%d", person.id);
		}

		// can only work if person is already moving through one side
		if(
				strcmp(person.event, inMessage[WEIGHT_SCAN]) == 0
				&&
				(person.side == left || person.side == right)
		) {
			printf(outMessage[WEIGHT_STATE]);
			scanf("%d", person.weight);
		}

		msgid = MsgSend(coid, &person, sizeof(person), &message, sizeof(message));
		// print MSGID
		// exit DES if message status failed
		if(msgid == -1) {
			printf("(DES_INPUT) Message ID: %d\nEXITING", msgid);
			return EXIT_FAILURE;
		}
		else
			printf("(DES_INPUT) Message ID: %d\n", msgid);
	}

	return EXIT_SUCCESS;
}

void displayMenuMessage() {
	char c;
	printf("Enter an event type ...\n");
	printf("LS = LEFT SCAN\n");
	printf("RS = RIGHT SCAN\n");
	printf("WS = WEIGHT SCAN\n");
	printf("LO = LEFT OPEN\n");
	printf("RO = RIGHT OPEN\n");
	printf("LC = LEFT CLOSED\n");
	printf("RC = RIGHT CLOSED\n");
	printf("GRU = GUARD RIGHT UNLOCKED\n");
	printf("GLU = GUARD LEFT UNLOCKED\n");
	printf("GLL = GUARD LEFT LOCK\n");
	printf("GRL = GUARD RIGHT LOCK\n");

	scanf("%c", &c);
}
