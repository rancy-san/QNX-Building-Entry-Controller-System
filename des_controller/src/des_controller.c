#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include "../../des.h"
#include <sys/netmgr.h>
#include <string.h>

// global variable used in all functions for MsgSend();
// store channel id for connection status
int coid;
// person obj/struct
Person person;
// response struct for all functions
Message message;
// from example "Tutorial State Machines With C Callbacks", store function call
StateFunc statefunc;

// store process ID to connect process ID and channel
// needs to get PID passed from argv
int main(int argc, char* argv[]) {

	// PID of display (from argv)
	pid_t pid;
	// get process ID to be displayed
	const pid_t ppid = getpid();
	// indicates who we should reply to
	int rcvid;
	// channel id
	int chid;
	// FSM state starts at  start_state
	statefunc = start_state;

	// store argv PID (display PID)
	sscanf(argv[1], "%d", &pid);

	// display PID
	printf("(DES_CONTROLLER) Process ID: %d\n", ppid);

	// create a channel
	chid = ChannelCreate(0);
	// check if channel has been created
	if(chid == -1) {
		printf("(DES_CONTROLLER) Channel ID: %d\nEXITING\n", chid);
		// exit immediately if channel creation has failed
		exit(EXIT_FAILURE);
	} else
		printf("(DES_CONTROLLER) Channel ID: %d\n", chid);
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
		printf("(DES_CONTROLLER) Connection ID: %d\nEXITING\n", coid);
		return EXIT_FAILURE;
	}
	else
		printf("(DES_CONTROLLER) Connection ID: %d\n", coid);
	// valid CHID, continue

	for(;;) {
		// store receive identifier of message received used for reply (lib: neutrino.h)
		// int MsgReceive( int chid, void * msg, int bytes, struct _msg_info * info );
		// chid is chid, msg is the person struct as info, bytes is the size of buffer msg, info is BULL or pointer to additional information for the message
		rcvid = MsgReceive(chid, &person, sizeof(person), NULL);
		if(rcvid == -1) {
			printf("(DES_CONTROLLER) Receive ID: %d\nEXITING\n", rcvid);
			return EXIT_FAILURE;
		}
		else
			printf("(DES_CONTROLLER) Receive ID: %d\n", rcvid);
		printf("RCVID FROM DES_CONTROLLER: %d\n", rcvid);
		// exit anywhere from DES on EXIT
		if(person.event == inMessage[EXIT]) {
			exit_state();
		}

		// cleaner code by having state function to return a pointer to the next state function
		statefunc = (StateFunc)(*statefunc)();
		// sleep for a second
		sleep(1);

		// reply with msg
		// rcvid, The receive ID that MsgReceive*() returned when you received the message.
		// status, The status to use when unblocking the MsgSend*() call in the rcvid thread.
		//msg, A pointer to a buffer that contains the message that you want to reply with.
		//size, The size of the message, in bytes.
		MsgReply(rcvid, 1, &message, sizeof(message));
	}

	// shut down DES on successful exit
	// destroy channel successfully
	ChannelDestroy(coid);
	// detach connection after destroying channel
	ConnectDetach(coid);

	return EXIT_SUCCESS;
}

void exit_state() {

	// set exit state on person
	person.state = EXIT_STATE;

	// exit immediately
	exit(EXIT_SUCCESS);

}

// FSM start state sets person is neither on the left or right of the door because the state is unknown
// begin changing state to left or right scan, then set function state to either left or right
void *start_state() {

	// store message id error/success state
	int msgid;

	// reset state to default because no person needs to go through the door
	person.side = none;
	person.weight = 0;
	person.id = 0;

	// if statement split for left and right to initially account for the side the person is entering from
	// if person's event type is the same as left scan event type
	if(strcmp(person.event, inMessage[LEFT_SCAN]) == 0) {
		// set entering side to left
		person.side = left;
		// update person state
		person.state = SCAN_STATE;

		// try sending message
		msgid = MsgSend(coid, &person, sizeof(person), &message, sizeof(message));

		// if message send was unsuccessful exit DES
		if(msgid == -1) {
			printf("(DES_CONTROLLER) [start_state(), left] Message Send ID: %d\nEXITING\n", msgid);
			exit(EXIT_FAILURE);
		}
		// return left_scan as the next function that should be executed after start_state
		return scan_state;
	} else if(strcmp(person.event, inMessage[RIGHT_SCAN]) == 0) { // if person's event type is the same as right scan event type
		// set entering side to left
		person.side = right;
		// update person state
		person.state = SCAN_STATE;

		// try sending message
		msgid = MsgSend(coid, &person, sizeof(person), &message, sizeof(message));

		// if message send was unsuccessful exit DES
		if(msgid == -1) {
			printf("(DES_CONTROLLER) [start_state(), right] Message Send ID: %d\nEXITING\n", msgid);
			exit(EXIT_FAILURE);
		}
		// return left_scan as the next function that should be executed after start_state
		return scan_state;
	}
	// returns this function if condition above not met
	return start_state;
}

// FSM scan state from either left or right side stored in the person object
void *scan_state() {
	// store message id error/success state
	int msgid;

	// left door or right door can only be unlocked if person is coming from the left side or right side respectively
	if(
			(strcmp(person.event, inMessage[GUARD_LEFT_UNLOCK]) == 0 && person.side == left)
			||
			(strcmp(person.event, inMessage[GUARD_RIGHT_UNLOCK]) == 0 && person.side == right)
	) {
		// update person state to unlock state
		person.state = UNLOCK_STATE;
		// try sending message
		msgid = MsgSend(coid, &person, sizeof(person), &message, sizeof(message));

		// if message send was unsuccessful exit DES
		if(msgid == -1) {
			printf("(DES_CONTROLLER) [scan_state()] Message Send ID: %d\nEXITING\n", msgid);
			exit(EXIT_FAILURE);
		}
		// function proceeds to unlocking the door
		return unlock_state;
	}
	// returns this function if condition above not met
	return scan_state;
}

void *unlock_state() {
	// store message id error/success state
	int msgid;

	// unlock door based on the side the person is entering from
	if(
			(strcmp(person.event, inMessage[LEFT_OPEN]) == 0 && person.side == left)
			||
			(strcmp(person.event, inMessage[RIGHT_OPEN]) == 0 && person.side == right)
	) {
		person.state = OPEN_STATE;
		// try sending message
		msgid = MsgSend(coid, &person, sizeof(person), &message, sizeof(message));

		// if message send was unsuccessful exit DES
		if(msgid == -1) {
			printf("(DES_CONTROLLER) [unlock_state()] Message Send ID: %d\nEXITING\n", msgid);
			exit(EXIT_FAILURE);
		}

		// function proceeds to opening the door
		return open_state;
	}
	// returns this function if condition above not met
	return unlock_state;
}

// FSM open door state opens either door depending on the side the person is passing through
void *open_state() {
	// store message id error/success state
	int msgid;
	// display output message
	printf(outMessage[OPEN_STATE]);
	// prevent looping and wrong state after gru in FSM. weight should be 0 if passing by as 1 person through 1 side
	if(person.weight == 0) {
		// open the door based on the side the person is coming in from
		if(
				(strcmp(person.event, inMessage[LEFT_OPEN]) == 0 && person.side == left)
				||
				(strcmp(person.event, inMessage[RIGHT_OPEN]) == 0 && person.side == right)
		) {
			person.state = WEIGHT_STATE;
			// try sending message
			msgid = MsgSend(coid, &person, sizeof(person), &message, sizeof(message));
			// if message send was unsuccessful exit DES
			if(msgid == -1) {
				printf("(DES_CONTROLLER) [open_state()] Message Send ID: %d\nEXITING\n", msgid);
				exit(EXIT_FAILURE);
			}
			// function proceeds to opening the door
			return weight_state;
		}
	}
	// this part of the FSM happens after person has been weighted, so the end doors need to open
	else if(
			// open right door when coming in from the left, or vice versa, both after weight scan already happened
			(strcmp(person.event, inMessage[RIGHT_OPEN] == 0 && person.side == left))
			||
			(strcmp(person.event, inMessage[LEFT_OPEN] == 0 && person.side == right))
	) {
		person.state = CLOSED_STATE;
		// try sending message
		msgid = MsgSend(coid, &person, sizeof(person), &message, sizeof(message));
		// if message send was unsuccessful exit DES
		if(msgid == -1) {
			printf("(DES_CONTROLLER) [open_state()] Message Send ID: %d\nEXITING\n", msgid);
			exit(EXIT_FAILURE);
		}
		return closed_state;
	}

	// returns this function if condition above not met
	return open_state;
}

// FSM weight state, door doesn't lock until person has weight etnered
void *weight_state() {
	// store message id error/success state
	int msgid;
	// arbitrary weight scan side
	if(
			strcmp(person.event, inMessage[WEIGHT_SCAN]) == 0
			&&
			(person.side == left || person.side == right)
	) {
		person.state = CLOSED_STATE;
		// try sending message
		msgid = MsgSend(coid, &person, sizeof(person), &message, sizeof(message));
		// if message send was unsuccessful exit DES
		if(msgid == -1) {
			printf("(DES_CONTROLLER) [weight_state()] Message Send ID: %d\nEXITING\n", msgid);
			exit(EXIT_FAILURE);
		}
		// function proceeds to opening the door
		return closed_state;
	}

	// returns this function if condition above not met
	return weight_state;
}

void *closed_state() {
	// store message id error/success state
	int msgid;

	// display output message
	printf(outMessage[CLOSED_STATE]);

	if(
			(strcmp(person.event, inMessage[LEFT_CLOSED]) == 0 && person.side == left)
			||
			(strcmp(person.event, inMessage[RIGHT_CLOSED]) == 0 && person.side == right)
	) {
		person.state = LOCK_STATE;
		// try sending message
		msgid = MsgSend(coid, &person, sizeof(person), &message, sizeof(message));
		// if message send was unsuccessful exit DES
		if(msgid == -1) {
			printf("(DES_CONTROLLER) [closed_state()] Message Send ID: %d\nEXITING\n", msgid);
			exit(EXIT_FAILURE);
		}
		// function proceeds to opening the door
		return lock_state;
	}

	// returns this function if condition above not met
	return closed_state;
}

void *lock_state() {
	// store message id error/success state
	int msgid;
	// display output message
	printf(outMessage[LOCK_STATE]);
	// lock first door on first time locking from left or right side
	if(
			(strcmp(person.event, inMessage[GUARD_LEFT_LOCK]) == 0 && person.side == left)
			||
			(strcmp(person.event, inMessage[GUARD_RIGHT_LOCK]) == 0 && person.side == right)
	) {
		person.state = UNLOCK_STATE;
		// try sending message
		msgid = MsgSend(coid, &person, sizeof(person), &message, sizeof(message));
		// if message send was unsuccessful exit DES
		if(msgid == -1) {
			printf("(DES_CONTROLLER) [lock_state()] Message Send ID: %d\nEXITING\n", msgid);
			exit(EXIT_FAILURE);
		}
		// function proceeds to opening the door
		return unlock_state;
	}
	// lock second door on second time locking from left or right side
	else if(
			(strcmp(person.event, inMessage[GUARD_RIGHT_LOCK]) == 0 && person.side == left)
			||
			(strcmp(person.event, inMessage[GUARD_LEFT_LOCK]) == 0 && person.side == right)
	){
		// no exit in FSM since it is a real time system, restart at start state
		person.state = START_STATE;
		// try sending message
		msgid = MsgSend(coid, &person, sizeof(person), &message, sizeof(message));
		// if message send was unsuccessful exit DES
		if(msgid == -1) {
			printf("(DES_CONTROLLER) [lock_state()] Message Send ID: %d\nEXITING\n", msgid);
			exit(EXIT_FAILURE);
		}

		return start_state;
	}
	// returns this function if condition above not met
	return lock_state;
}

