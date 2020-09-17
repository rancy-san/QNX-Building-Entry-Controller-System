#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include "../../des.h"

int main(void) {
	// indicates who we should reply to
	int rcvid;
	// Person struct/object
	Person person;
	// channel ID
	int chid;
	// get process ID to be displayed
	const pid_t ppid = getpid();
	// message object
	Message message;

	// display PID
	printf("(DES_DISPLAY) Process ID: %d\n", ppid);
	// create channel ID, and store channel ID (lib: neutrino.h)
	chid = ChannelCreate(0);
	// print CHID
	// exit DES if channel did not create
	if(chid == -1) {
		printf("(DES_DISPLAY) Channel ID: %d\nEXITING\n", chid);
		return EXIT_FAILURE;
	}
	else
		printf("(DES_DISPLAY) Channel ID: %d\n", chid);
	// valid CHID, continue


	// real-time begins. wait for a message on channel
	for(;;) {
		// store receive identifier of message received used for reply (lib: neutrino.h)
		// int MsgReceive( int chid, void * msg, int bytes, struct _msg_info * info );
		// chid is chid, msg is the person struct as info, bytes is the size of buffer msg, info is BULL or pointer to additional information for the message
		rcvid = MsgReceive(chid, &person, sizeof(person), NULL);
		// print RCVID
		printf("(DES_DISPLAY) Receive ID: %d\n", rcvid);
		// print RCVID
		// exit DES if message receive status failed
		if(rcvid == -1) {
			printf("(DES_CONTROLLER) Receive ID: %d\nEXITING\n", rcvid);
			return EXIT_FAILURE;
		}
		else {
			printf("(DES_CONTROLLER) Receive ID: %d\n", rcvid);
			if(strcmp(person.state, inMessage[LEFT_SCAN]) == 0)
				printf(outMessage[SCAN_MSG]);
			if(strcmp(person.state, inMessage[RIGHT_SCAN]) == 0)
				printf(outMessage[SCAN_MSG]);
			if(strcmp(person.state, inMessage[WEIGHT_SCAN]) == 0)
				printf(outMessage[WEIGHT_MSG]);
			if(strcmp(person.state, inMessage[LEFT_OPEN]) == 0)
				printf(outMessage[OPEN_MSG]);
			if(strcmp(person.state, inMessage[RIGHT_OPEN]) == 0)
				printf(outMessage[OPEN_MSG]);
			if(strcmp(person.state, inMessage[LEFT_CLOSED]) == 0)
				printf(outMessage[CLOSED_MSG]);
			if(strcmp(person.state, inMessage[RIGHT_CLOSED]) == 0)
				printf(outMessage[CLOSED_MSG]);
			if(strcmp(person.state, inMessage[GUARD_RIGHT_UNLOCK]) == 0)
				printf(outMessage[UNLOCK_MSG]);
			if(strcmp(person.state, inMessage[GUARD_LEFT_UNLOCK]) == 0)
				printf(outMessage[UNLOCK_MSG]);
			if(strcmp(person.state, inMessage[GUARD_RIGHT_LOCK]) == 0)
				printf(outMessage[LOCK_MSG]);
			if(strcmp(person.state, inMessage[GUARD_LEFT_LOCK]) == 0)
				printf(outMessage[LOCK_MSG]);
			if(strcmp(person.state, inMessage[EXIT]) == 0)
				printf(outMessage[EXIT_MSG]);

		}
		// valid CHID, continue

		// reply with msg
		// rcvid, The receive ID that MsgReceive*() returned when you received the message.
		// status, The status to use when unblocking the MsgSend*() call in the rcvid thread.
		//msg, A pointer to a buffer that contains the message that you want to reply with.
		//size, The size of the message, in bytes.
		MsgReply(rcvid, 1, &message, sizeof(message));
	}

	return EXIT_SUCCESS;
}
