## Building Entry Controller System

### Requirements

QNX Operating System
Momentics IDE

### Use
QNX must be online/turned on.
Hook the QNX IP address to Momentics IDE
Compile code in Momentics IDE
Run program in Momentics, or run program in QNX.

### Details

This real-time program consists of designing and building a set of programs to simulate a building entry controller system and provide a testing harness. The system is similar to a two door system with card scanners on each door, a scale that weighs the person after they have entered the space between the two doors, and a human guard that uses a switch to open each of the doors. This program uses QNX Momentics, and QNX SDP as the Client and Server.

1. People enter the entry control space from the left to enter the building and enter the controlspace from the right to exit the building.
2. Each person requesting to enter or exit the building scans their unique personal id cardcontaining an integer person_id number. The scanner sends a message to the controller withthe input information (which door opening is being requested, and which person_id is beingused.; (e.g., “left 12345”)
3. Only 1 person at a time should be able to be inside the lock.
4. Assume that the door is self-closing (but not self-locking), and that an event (see below underStep 4.a.) will be sent to the controller when the status of the door changes.
Usage:
a. The first prompt should be:
Enter the event type (ls= left scan, rs= right scan, ws= weight scale, lo =left open,
ro=right open, lc = left closed, rc = right closed , gru = guard right unlock, grl = guard
right lock, gll=guard left lock, glu = guard left unlock)
b. If the event is the lo, ro, lc, rc, glu, gll, gru, or grl, no further prompt is required.
c. If the event is ls or rs, prompt for the person_id.
Enter the person_id
d. If the event is ws, prompt for the weight.
Enter the weight
Page 2 of 2
Once the input is finished, send a message to the des_controller program to provide the input “event”, and loop back to prompt again.
This scenario represents a Person entering the building:
ls
12345
glu
lo
ws
123
lc
gll
gru
ro
rc
grl
exit Example Startup: #./des_display &
The display is running as process_id 77234. [status update : initial startup] #./des_controller 77234 &
The controller is running as process_id 77235. #./des_inputs 77235 Enter the device (ls= left scan, rs= right scan, ws= weight scale, lo =left open, ro=right open, lc = left closed, rc = right closed , gru = guard right unlock, grl = guard right lock, gll=guard left lock,
glu = guard left unlock)