# Asynchronous-Non-blocking-Server-Cpp
Server written in C/C++ that creates multithreaded connections with clients and handles the requests in an asynchronous non-blocking style.

# Build & Execution
``` 
mkdir build
cmake .
make
./server
  ```


# Specification
The goal is to implement a server that can efficiently and quickly handle multiple client connections. Each client sends messages in a Protobuf format. Possible messages types and message structure is described in the Protobuf [scheme](/proto/measurements.proto). Client sends a request, whic can be either "postWord" or "getCount".
- **postWords**: message contains an array of bytes containing words to store on the server.
- **getCount**: does not carry any information from the client. Instead, it is an indicator for the server to stop adding the words, reset the internal data structure and respond to the client who send the request with a number of unique words from that structure.

# Implementation
For the implementation was chosen a Boost library, asio, that allows asynchronous read and write operations.

_[Asio](https://www.boost.org/doc/libs/1_75_0/doc/html/boost_asio.html) is a cross-platform C++ library for network and low-level I/O programming that provides developers with a consistent asynchronous model using a modern C++ approach._ 

Each client connection is handled in a different thread asynchronously. **Concurrent unordered set** data structure was chosen to store the incoming data and prevent conflicts. The set is implemented in oneAPI:

_[oneapi::tbb::concurrent_unordered_set](https://spec.oneapi.io/versions/latest/elements/oneTBB/source/containers/concurrent_unordered_set_cls.html)  is a class template that represents an unordered sequence of unique elements. It supports concurrent insertion, lookup, and traversal, but does not support concurrent erasure._

The data coming from clients is also archived with Gzip, so the implementation uses GZIPInputStream to decompress the data.

Note: The implementation relies that each time a client sends a message, there are usually 4 bytes before the message itself representing the size of the message in the number of bytes. The same principle applies to the message that goes back to the client.



