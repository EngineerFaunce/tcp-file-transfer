# TCP File Transfer using C networking

This is a simple program that sends a file in chunks using the TCP networking protocol.

### Compiling
A simple Makefile is provided and will compile the two files. Simply enter `make` into the terminal at the root of the project to compile.

### Usage
The program can work on a local machine by itself, but is aimed at being used between two host machines. To use between two machines, the
user must set the `SERVER_ADDRESS` definition to match the IP of the intended server machine. The default port used is 5000, but can be changed
by the user.

To run the program
  * Start the server on the host machine
  * Start the client on a peer machine
  
The two programs will log information into the console as they execute.
