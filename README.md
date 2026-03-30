# RelayIM
A simple chat program that lets you create, join and send messages in one or more chat rooms.

## Why, though?
I wanted a fun project to work on that would let me learn Winsock2, Qt6, and concurrent code.

## Tech Stack
- Networking Code via Winsock2 over TCP
- Server using Windows Console
- Client using Qt6 Framework
- All C++

## Interesting Problems Solved
- Qt C++ QListView does not have a bottom to top flow available. Solved by creating a "virtual scroll" widget, using QScrollArea. 
	- Physical QMessage objects are created and deleted as needed, only for messages that _would_ appear in the QScrollArea's viewport. This helps us limit performance cost of potentially hundreds or thousands of messages needing to be processed by Qt's UI thread. QMessage objects are manually positioned on QChatModel, and message positions are precalculated from which we can perform binary search and quickly find the correct messages to display.
- Creating a custom binary packet protocol
	- Type-safe, futureproof wire protocol with built-in validation via magic number and version number. No other extra encoding layers, purely binary to trim the fat.
	- 14 Packet types, all with custom payloads, using the same header structure.
- Server Architecture for handling multiple clients
	- Network Traffic is handled in layers. We have the SOCKET layer, and the Packet layer, cleanly seperating logic out for readability.
		- The SOCKET layer handles pure binary network traffic. It doesn't care about packet contents; just a packet size at the begginning of a payload, and passes that data to the packet layer when that size is filled. Two threads are used per connected client, for receiving and sending. 
		- The packet layer actually reads the whole payload, and updates the server state based on what is received from clients. It also sends back data to clients if necessary. The reading of packets and the updating of state happens on a seperate thread, which operates as a "work queue", only waking when there are packets to process.
- Thread-Safe Qt Integration
	- Various data structures in Qt such as Models require them to be processed on the Qt thread, but the network code runs on background threads. To get around this, I used Qt's Signal/Slot system with queued blocking connections to copy data over to main Qt thread, and from there emit signals are are UI safe.
