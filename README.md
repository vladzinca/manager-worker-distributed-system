# Manager Worker Distributed System

## 🌐 What is it?

This C application creates a manager-worker topology and uses it to perform simple computations in a distributed way, in this case the multiplication of all elements of an array by 5.

To do this, it uses two types of processes: manager processes, of which there are 4, which are tasked with  splitting the array fairly and eventually reassembling it, and worker processes, which do the per se multiplications.

They communicate with one another through an intricate sequence of MPI messages.

It took me an entire day to code during January 2023 and it serves as a culmination of both my understanding of distributed computing, as well as my ability to code a complex application from scratch in one single day.

## 🔧 How to run it?

### Method 1)

1.  Clone this repository.
2.  Run the automated testing script using `./checker.sh` inside the `checker/` directory (I did not make the script myself). It will run the tests present in `tests/` and print the results in the console.
3.  Enjoy!

### Method 2)

1.  Clone this repository.
2.  Create four files inside `src/` called `cluster0.txt`, `cluster1.txt`, `cluster2.txt` and `cluster3.txt`, which should contain the number of worker processes assigned to said manager (the managers are numbered 0 through 3), and then, one number per line, numbers from `4` to `P - 1` naming the assigned worker processes, where `P` is the total number of processes in the topology. If you want, you can use examples from `checker/tests/`.
3.  Compile the code found in `src/` using `make` and run it using `mpirun --oversubscribe -np P ./tema3 N`, where `P` is the total number of processes in the topology, and `N` is the dimension of the array whose elements will be multiplied. Note that it may be necessary to install `mpirun` if it's not already installed on your system.
4.  The program will output the result to the console as one line `Rezultat: <array>`, along many other lines which will be detailed further.
5.  Enjoy!

## 💍 What does it do?

To understand the application functionality, it is necessary to first understand the topology of the processes that form the app.

It consists of 4 manager processes labeled 0 to 3, which are connected through a ring-like network: there are undirected connections between managers 0 and 3, 3 and 2, 2 and 1, 1 and 0. The last connection, between 1 and 0, is considered unreliable, so all the traffic between 0 and 1 will actually flow `0 -> 3 -> 2 -> 1` and vice versa.

Every manager process is also connected to a number of worker processes, and there aren't worker processes connected to more than 1 manager.

![topology](https://user-images.githubusercontent.com/74200913/215343900-ac04b288-e1e0-4c27-89ff-f532d11afd6f.png)

In the beginning, each manager reads from its `cluster.txt` file to find out which processes are its workers, then sends and receives MPI messages to and from the other managers to find out the entire topology.

When a manager has received the entire topology, it prints it to the console and sends it all its workers, who finally "find out" who their manager is, and they also print it.

Afterwards, manager 0 creates an array of `N` elements starting from `N - 1` and decreasing to `0`, and it splits it fairly among itself and the other managers, taking into account the number of worker each manager has at its disposal.

Then, it splits its part of the array evenly among its workers, sends each worker its part and sends the rest of the array to the other managers, who do the same. Every worker multiplies each section of the array and sends it back to their manager.

Each manager than reassembles its part of the array and sends it to manager 0 either directly or through the use of other managers. Manager 0 will reassemble the array in its final form and print the result.

Every sent and received message is also logged in the console `M(a,b)`, to make sure that processes which are not connected do not "talk" to one another.

## 💬 How does it work?

To make it simpler to understand the workflow of the app, I have labeled every MPI_Send and MPI_Recv instruction with a comment `operation i`, where i is any number between 0 and 43. You can use `CTRL+F` to search for `operation 0`, for example, and once you understand those messages continue with `operation 1`, and so on.

For those that are in a rush, it can be summarized in two parts, (I) the transmission of the topology and the (II) distributed computation part.

(I) works as follows: `1 -> 2 -> 3 -> 0 -> 3 -> 2 -> 1`.

1.  Every manager "finds out" its workers.
2.  1 sends its workers to 2.
3.  2 sends the workers of 1 and 2 to 3.
4.  3 sends the workers of 1, 2 and 3 to 0.
5.  0 sends its workers back to 3.
6.  3 sends the workers of 0 and 3 back to 2.
7.  2 sends the workers of 0, 2 and 3 back to 1.
8.  Every manager prints the topology and sends it to its workers.
9.  Every worker received the topology and prints it.

(II) works as follows: `0 -> 3 -> 2 -> 1 -> 2 -> 3 -> 0`.

1.  0 splits the array in 4 parts proportional to the number of workers of each manager.
2.  0 sends the parts of 1, 2 and 3 to 3.
3.  3 sends the parts of 1 and 2 to 2.
4.  2 sends 1 its part.
5.  Every manager splits its part of the array equally among its workers.
6.  Every worker multiplies its part and sends the result back to its manager.
7.  1 sends its result back to 2.
8.  2 sends the result of 1 and 2 back to 3.
9.  3 sends the result of 1, 2 and 3 back to 0.
10.  0 reassembles the results received from 3 and its workers and prints the final array.

## 🏋️ What challenges have I overcome?

The main challenge was making sure that the array is split as fairly as possible, especially in those cases where the numbers of elements in the array `N` is not divisible by the number of worker processes available in the topology `P - 4`.

To do that, manager 0 computes locally the optimal number of elements each manager should transform, such that the difference between the elements multiplied by the "busiest" worker and the elements multiplied by the "laziest" worker is at most 1.

Another challenge was making sure the workers send back their resulting array to their corresponding manager. This is because each worker virtually executes the same code, and there is no way to separate the workers of one manager and those of another without knowing the topology before running the code.

The solution was sending the modified array to `status.MPI_SOURCE`, since every worker gets inside `status` the rank of the process that send them the array, which is precisely the rank of their manager.

## 🤔 Did you know?

The app sends a lot of MPI messages that have to be accounted for and received dilligently.

For example, for the topology present in `checker\tests\test1\`, there are as many as 135 messages sent and received before the result can be printed.
