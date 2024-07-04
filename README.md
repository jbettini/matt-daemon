# Matt-Daemon

## Overview
Matt-Daemon is a project aimed at creating a daemon process in C++20 that acts as a TCP server. It features file lock systems and multi-threaded client management, supporting up to 3 clients simultaneously. The daemon logs all commands to a file and requires root privileges to run.

## Features
-   Implements a daemon process in C++20.
-   TCP server functionality.
-   File lock system to manage access to resources.
-   Multi-threaded client management, supporting up to 3 clients.
-   Command logging to a file.
-   Requires root privileges to run.

## Prerequisites

-   A Unix-like operating system.
-   GCC with C++20 support.
-   Basic knowledge of C++, daemon processes, and networking.

## Installation
```
git clone https://github.com/jbettini/Matt-Daemon.git
cd Matt-Daemon
sudo make
```

## Usage

Run the compiled daemon executable as root:
```
sudo ./matt-daemon
```

Connect to the daemon using netcat:
```
nc localhost 4242
```

Disconnect from the daemon:
```
$> Quit
Qutting
```
