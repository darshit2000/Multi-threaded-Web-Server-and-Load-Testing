"""
This script generates load on a server using the `load_generator` executable 
and plots the graph for average throughput and response time against the number of users.

Usage: python3 load_generator_script.py <server_port>
"""

#!/usr/bin/python3

import os
import sys
import matplotlib.pyplot as plot
import numpy as np

averageRTT = list()
averageThroughput = list()
totalUsers = list()
serverPort = int(sys.argv[1])

# Function to generate variable load on the multithreaded server
def generate_load():
    # Compile the load_generator executable
    os.system("gcc load_generator.c -o load_generator -g -lpthread")
    print("Load generating for : ")

    for i in range(100, 5500, 500):
        print(str(i) + " users...")
        # Run the load_generator executable
        os.system("taskset -c 1,2,3 ./load_generator localhost " + str(serverPort) + " "  +  str(i) + " 0.04 60 >> /dev/null")
        file = open("load_gen.log", "r")
        for line in file:
            if "Average Throughput of Server" in line:
                averageThroughput.append(float(line.split(" : ")[-1].split('\n')[0].strip()))
            elif "Average Response Time of Server" in line:
                averageRTT.append(float(line.split(" : ")[-1].split('\n')[0].strip()))
            elif "Total number of users" in line:
                totalUsers.append(float(line.split(" : ")[-1].split('\n')[0].strip()))
            
    file.close()

    print(totalUsers)
    print(averageThroughput)
    print(averageRTT)


"""
Function to plot the graph for average throughput and response time against the number of users.

Parameters:
averageThroughput (list): A list of average throughput values.
averageRTT (list): A list of average response time values.
totalUsers (list): A list of total users values.
"""
def plot_graph():
    plot.figure(figsize=(12,11))
    plot.subplot(211)
    plot.plot(totalUsers, averageThroughput, label='Throughput', marker='.', ls='-', color='red', markersize=7, mew=2, linewidth=2)
    plot.xlabel("Number of Users  -->")
    plot.ylabel("Throughput (no. of requests/sec)  -->")
    plot.title("Number of Users vs. Throughput")
    plot.grid(visible='on')
    plot.legend()
    plot.subplot(212)
    plot.plot(totalUsers, averageRTT, label='RTT', marker='.', ls='-', color='green', markersize=7, mew=2, linewidth=2)
    plot.xlabel("Number of Users  -->")
    plot.ylabel("Response Time (sec)  -->")
    plot.title("Number of Users vs. Response Time")
    plot.grid(visible='on')
    plot.legend()
    plot.show()


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 load_generator_script.py <server_port>")
        sys.exit(1)
    generate_load()
    plot_graph()
