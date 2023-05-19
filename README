# HTTP Client

**Description**: This project aims to provide hands-on experience with socket programming and a better understanding of application-level protocols by implementing the well-known HTTP protocol. The HTTP client constructs an HTTP request based on user input, sends the request to a web server, receives the server's reply, and displays the reply message on the screen. The client supports IPv4 connections and communicates over the default HTTP port (port 80).

## Background

HTTP (Hyper Text Transfer Protocol) is a protocol used for communication between web clients and servers. The client initiates a conversation by opening a connection to the server and sending an HTTP request. The server responds with an HTTP response. An HTTP request consists of a header and an optional body. The header contains information such as the request method, request URI, and protocol version.

## Usage

To use the HTTP client, run the `client.c` program with the following command line usage:

client [–p n <text>] [–r n <pr1=value1 pr2=value2 …>] <URL>


The program accepts two options, `-p` and `-r`, along with a required argument `<URL>`. The options and the URL can be provided in any order. The `-p` option specifies a POST request instead of the default GET request and requires an additional argument `<text>` for the request body. The `-r` option allows specifying additional request parameters in the format `<pr1=value1 pr2=value2 ...>`.

##Implementation Details

The `client.c` program parses the provided URL and constructs an HTTP request based on the command line options. It establishes a connection with the server, sends the HTTP request, receives the server's HTTP response, and displays the response on the screen.

Before sending the request, the program prints the constructed request to stdout, including its length. After receiving the response and printing it, the program displays the total number of received response bytes.


