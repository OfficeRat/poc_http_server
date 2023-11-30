# POC HTTP Server

This is a simple HTTP server written in C that handles GET requests. It serves static files from the local directory. The server is configured to listen on a specified port (default is 8080) and supports customization of the port through a command-line argument.

## Usage

Compile the code using a C compiler, for example:

```bash
gcc poc_webserver.c -o poc_webserver
```

Run the compiled binary with optional port specification:

```bash
./poc_webserver -p <port>
```

By default, the server will use port 8080 if no port is specified.

## Features

* Listens for incoming connections on the specified port.
* Handles GET requests and serves corresponding files from the local directory.
* Logs incoming requests with client IP, port, requested file, and timestamp.

## Dependencies

* This code uses standard C libraries and should work on most Unix-like systems.

## Options

*  `-p <port>`: Specify the port on which the server should listen.

## Example

```bash
./poc_webserver -p 47891
```
This will start the server on port 47891.