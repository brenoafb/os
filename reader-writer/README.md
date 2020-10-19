# Reader-Writer

These two programs present a mechanism for communication between two processes.
A writer serves information to a reader, while notifying the message size
in each turn.

Upon running, the writer will setup the IPC channels and wait for a notification
from the reader program.
After this, the reader can be executed.
Upon starting, the reader will try to establish access to the IPC channels and
will attempt to notify the writer.

After this initial setup is done, the writer will send a string to the reader
using signals (for notifying that a message has been sent), shared memory
(for sharing the message size), and message queues (through which the message
is sent).

## Example

```
$ ./writer & ./reader
[1] 3726359
Writer: initializing message queue
Writer: initializing shared memory
Writer: attaching shared memory
Writer: waiting for PID message
Reader: initializing
Reader: sending PID message (PID=3726360)
Reader: PID message sent, waiting for ACK
Writer: received PID 3726360, sending ACK
Writer: sending message
Reader: received ACK, waiting for message
Writer: message sent, sending termination
Reader: received message "Hello from writer!"
Writer: termination sent, finalizing
Reader: received termination
Reader: finished reception
[1]  + done       ./writer
```

