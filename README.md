# RelayIM
A simple chat program that lets you create, join and send messages in chat rooms. 

## Tech Stack
- Networking Code via Winsock2 over TCP
- Server using Windows Console
- Client using Qt6 Framework
- All C++

## Interesting Problems Solved
- Qt C++ QListView does not have a bottom to top flow available. Solved by creating a "virtual scroll" widget, using QScrollArea. 
	- Physical QMessage objects are created and deleted as needed, only for messages that _would_ appear in the QScrollArea's viewport. This helps us limit performance cost of potentially hundreds or thousands of messages needing to be processed by Qt's UI thread. QMessage objects are manually positioned on QChatModel, and message positions are precalculated from which we can perform binary search and quickly find the correct messages to display.
- Creating a custom binary packet protocol
	- Type-safe, futureproof wire protocol with built-in validation via magic number and version number.
- Server Architecture for handling multiple clients
	- In the SOCKET layer of networking, each client has two threads: one for receiving, one for sending. When the receiving thread receives a packet in full, it them places the packet along with it's peer id into a work-queue on the server. This work-queue runs on a separate thread, woken by a condition_variable, and processes the packet, updating server state as appropriate. This multi-threaded architecture allows the server to listen and send networkdata concurrrently to every client.
- Thread-Safe Qt Integration
	- Models in Qt require them to be processed on the Qt thread, but the network runs on background threads. To get around this, I used Qt's Signal/Slot system with queued blocking connections to copy data over to main Qt thread, and from there emit signals are are UI safe.