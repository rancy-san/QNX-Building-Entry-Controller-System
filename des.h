/*
 * Filename:	des.h
 */

#ifndef DES_H_
#define DES_H_

// from example "Tutorial State Machines With C Callbacks"
typedef void *(*StateFunc)();

// possible entry side for DES
#define NUM_SIDES 3
typedef enum {
	left,
	right,
	none
} Side;

// FSM states
#define NUM_STATES 8
typedef enum {
	START_STATE = 0,
	SCAN_STATE = 1,
	UNLOCK_STATE = 2,
	OPEN_STATE = 3,
	WEIGHT_STATE = 4,
	CLOSED_STATE = 5,
	LOCK_STATE = 6,
	EXIT_STATE = 7
} State;

#define NUM_INPUTS 12
typedef enum {
	LEFT_SCAN = 0,
	RIGHT_SCAN = 1,
	WEIGHT_SCAN = 2,
	LEFT_OPEN = 3,
	RIGHT_OPEN = 4,
	LEFT_CLOSED = 5,
	RIGHT_CLOSED = 6,
	GUARD_RIGHT_UNLOCK = 7,
	GUARD_RIGHT_LOCK = 8,
	GUARD_LEFT_LOCK = 9,
	GUARD_LEFT_UNLOCK = 10,
	EXIT = 11
} Input;

#define NUM_OUTPUTS 8
typedef enum {
	START_MSG = 0,
	SCAN_MSG = 1,
	UNLOCK_MSG = 2,
	LOCK_MSG = 3,
	OPEN_MSG = 4,
	WEIGHT_MSG = 5,
	CLOSED_MSG = 6,
	EXIT_MSG = 7
} Output;

const char *outMessage[NUM_OUTPUTS] = {
	"(START) Waiting for enter ID scan:",
	"(SCAN) Scanning card.",
	"(UNLOCK) Unlocking door.",
	"(LOCK) Locking door."
	"(OPEN) Opening door.",
	"(WEIGHT) Waiting for object weight:",
	"(CLOSE) Closing door.",
	"(EXIT) Exiting DES."
};

const char *inMessage[NUM_INPUTS] = {
	"LS",
	"RS",
	"WS",
	"LO",
	"RO",
	"LC",
	"RC",
	"GRU",
	"GRL",
	"GLL",
	"GLU",
	"EXIT"
};

// object holding person's details
typedef struct person {
	int id;
	int state;
	char event[1000];
	int weight;
	Side side;
} Person;

typedef struct message {
	char msg[50];
} Message;

// start DES
void *start_state();
// scan the ID
void *scan_state();
// unlock the door
void *unlock_state();
// open the door
void *open_state();
// weight the object
void *weight_state();
// close the door
void *closed_state();
// lock the door
void *lock_state();
// exits out of the program at any time
void exit_state();
// display menu
void displayMenuMessage();

#endif /* DES_H_ */
