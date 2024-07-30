Multi-threaded Web Server and Closed-loop Load Testing
==============

Simple and thoroughly commented `Multi-threaded Web Server` and `Load-generator`, both written in C. Analysing the results of the load generator through graphs is done in Python. Created by Darshit Gandhi as a university exercise.


Components and their features
--------
* Multi-threaded Web Server:
  * The web server uses the `master-worker thread pool architecture` to handle multiple clients concurrently. It uses a pool of reusable worker threads.
  * The main server creates a `pool of worker threads` at the start. Whenever a new client connection is accepted, the server places the accepted client file descriptor in a queue/array shared with the workers. Each worker thread fetches a client from this queue and serves it as long as the client is connected. Once the client finishes and terminates, the worker thread returns to the queue to get the next client to handle. This way, the same pool of worker threads can serve multiple clients.
  * The main server thread and the worker threads use `mutex locks` to access this queue of accepted client file descriptors without race conditions.
 
* Closed Loop Load-generator:
  * This will rapidly fire requests at the server to `measure the capacity of the web server`.
  * Used `Valgrind` to fix any memory leaks from the web server before running the load test to avoid server crashes due to memory errors.
  * This will act as a `closed-loop load generator`, i.e., the load is generated from a certain number of concurrent emulated users.
  * Each thread of the load generator will emulate an HTTP user/client by sending an HTTP request to the server, waiting for a response from the server, and firing the next request after the think time.
  * The number of concurrent users/threads, think-time between requests, and the duration of the load test can be changed accordingly.
  * After all the load generator threads run for the specified duration, the load generator must compute (across all its threads) and display the following performance metrics before terminating:
    * `Average throughput of the server`: The average number of HTTP requests per second successfully processed by the server for the duration of the load test.
    * `Average response time of the server`: The average amount of time taken to get a response from the server for any request, as measured at the load generator.
  * The python script then generates plots of the average throughput and response time of the server as a function of the load level.
 

Source Code Structure
--------
* `server/http_server_with_thread_pool.c` : Main source file containing the code to start the web server.
* `server/serverHelperFunctions.c` : Contains the various helper functions that the main source file of the web server will call.
* `server/interuptHandler.c` : Contains code to handle interupts.
* `server/serverVariables.c` : Contains the web server's global variables.
* `server/htmlFiles/` : Contains dummy HTML files to demonstrate how the multi-threaded web server works.

* `load_generator/load_generator.c` : Main source file containing the code for the load testing of the web server.
* `load_generator/load_generator_script.py` : Python script to plot the desired graph by using the load_generator executable.
* `load_generator/loadGenHelperFunctions.c` : Contains the various helper functions that the main source file of the load-tester will call.
* `load_generator/loadGenVariables.c` : Contains the load tester's global variables.


Compilation and Usage
--------
The recommended operating system for running this program is Ubuntu.

1. To start the web server at some port number (say 8001), go to the `server/` path and run the following commands in order:
```
make  
```
```
./server 8001
```
After starting this, you can go to `127.0.0.1:8000` to check if your web server is running correctly or not.

2. To run the load tester, go to the `load_generator/` path and run the following commands in order:
```
make
```
```
python3 load_generator_script.py 8001     
```
(8001 is the port number on which the web server is running.)

This will run the load_generator.c and will plot the desired graph.

3. Clean Object files:
```
make clean
```
