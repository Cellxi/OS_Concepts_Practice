This repo includes examples and my answers to chapter exercise of Operating System Concepts(Tenth Edition).Details may differ from the original text in the book due to differences of version.They've been tested on Linux 6.1.0.

The book: https://archive.org/details/operatingsystemconcepts10th

## Chapter 1 Introduction

## Chapter 2 Operating-System Structures

#### Programming Problems

CopyFile(CF) : Copy files with POSIX API

#### Projects

Simple Module : An easiest kernel module from the textbook.

Hello Module : A module creating the hello proc in *virtual file system*(VFS), which ouputs hello message when read by users. Also from the textbook.

Task1 : A modified Simple Module to fetch tick rate and jiffies.

Task2 : Merge Task1 and Hello Module to create proc files in VFS, printing jiffies and the module running time when they're read by users.

## Chapter 3 Processes

#### Examples

3.7.1 : An example of producer-consumer model, using shared memory for *inter process communication*(IPC) from the textbook.

3.7.4 : An example of IPC through pipes from the textbook.

#### Practice Exercise

LineA : The program of practice 3.1 from the textbook.

LineA_pipe : Implements IPC through pipes to pass a value across processes.

LineA_shm : Implements IPC through shared memory to pass the value.

XYproblem : A modified-with-switch version of practice 3.16 from the textbook.

XYproblem : Implements IPC through shared memory in XYproblem.

lineJ : The program of practice 3.12 from the textbook.

lineJ_run : An example of wrong arguments causing execlp() failure.

#### Programming Problems

zombie : Generates a zombie process by omitting wait() and delaying parent exiting.

tryclock : Have a try on clock_gettime() to replace gettimeofday() in the textbook.

tcpipe : Times how long a command runs.

tcommand : Pass the timespec struct through shared memory.

Collatz : Gets an integer between 1 ~ 2^63 from shell and tries to verify that it fits the conjecture. The child process handles the calculating and the parent process outputs. They communicate by both pipes and share memory.

#### UNIX SHELL

This project follows the guide from Project 1 in 3.15 Programming Projects. It implements an interface to receive commands from users and execute them in child processes. It also supports I/O redirection from/to files and pipes as a form of IPC between a pair of commands.
