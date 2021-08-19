# A websocket channel for Tcl


```
% package require tclwebsocket
0.0.1
% set sock [websocket localhost 8015 /echo]
websocket0
% fconfigure $sock -buffering line
% puts $sock "hello world"
% gets $sock
hello world
% close $sock
```

## Description

Tclwebsocket is a channel driver for the Tcl programming language. It provides a new channel type for the communication with websocket clients and servers (servers tbd), but hides the complex details of http upgrade, ssl handshake etc. from the view of Tcl programmers. These details are handled by the [libwebsockets](https://libwebsockets.org/) library, which is actively developed and supported by a big community for some time now.

Once a channel is created and connected with the *websocket* command, it can be used like any other channel in Tcl. You can *fconfigure* common options for it, like *-buffering* or *-blocking*, plus specific options like *-keepalive* and *-transmission* (tbd). You can *puts* data on it to send it to the remote and *gets* data from it, which was sent by the remote. You can create *-async* websockets like sockets with *socket -async* and configure readable/writable handler procs via *fileevent* (this feature is also tbd). You can *close* it when you're done with it.

## But there is the tcllib websocket package already?

Once a websocket connection is established, it works for the user more or less like a normal socket. So there is no reason why it shouldn't feel like that ;-). The websocket package works as well (most of the time), but it has a different interface. And it depends on various other packages, http and tls in particular, which must fit together, be available and loaded extra.

Tclwebsocket is well suited if you like the standard way of channel communication in Tcl, or if you want to switch frequently between a normal socket connection and websocket connection. This is particularly easy with tclwebsocket, since both work alike.

## Features

There should be frequent releases even if not all features are implemented. Here is a list of the total feature set that are to be implemented in total. The ones that are already implemented, are marked with a checkmark &#x1F5F9;. Features that not yet implemented, but are on the list, are marked with a not-checked-mark &#x237B;. All other features are not planned yet. However, the list is probably not complete and subject to change in the future.

* client websocket
  	* &#x2713; connect to websocket servers normal and ssl
	* &#x2713; *text* data IO
	* &#x2713; blocking and non-blocking IO
	* &#x237B; *binary* data IO (*fconfigure -transmission binary*)
	* &#x237B; *-async* Option and file events
	* &#x237B; ssl cert client authentication
	
* server websocket
	- &#x237B; create server websocket with handler proc
	- &#x237B; *text* data IO servers
	- &#x237B; blocking and non-blocking IO
	- &#x237B; *binary* data IO (*fconfigure -transmission binary*)

* general client and server
	- &#x237B; keepalive ping/pong on driver layer
	- &#x237B; automatic reconnect on driver layer
	
## Installation

There are bre built packages for Windows x64, Linux x64 and macOS. They can be downloaded [here](https://github.com/ecky-l/tclwebsocket/releases) from the releases page. A development build is generated for each push to master. To install the package, extract the archive and drop the resulting directory somewhere on your *auto\_path*.

Installation can also be performed after a build from sources, which is described below.

## Building tclwebsocket

This is a [cmake](https://cmake.org/) C++ project. To build it you will need cmake and a normal C++ compiler with support for C++-17. All recent C++ compilers should do that. Furthermore you need [openssl](https://www.openssl.org/) and [Tcl](https://www.tcl-lang.org/) installed for dependencies, together with their devel files (headers and linkable libs). [Libwebsockets](https://libwebsockets.org/) comes as a git submodule with the project, it is compiled with tclwebsocket and statically linked. See the CMakeLists.txt file for a description why a shared library is not really an option.

The steps described below can also be followed in the github actions based [workflow](https://github.com/ecky-l/tclwebsocket/blob/master/.github/workflows/tclwebsocket-master.yml) for the master build.

### Install the dependencies

The dependencies can usually be easily installed on a recent linux system:

* centos/redhat: `sudo dnf install tcl-devel openssl-devel cmake gcc-g++`

* debian/ubuntu: `sudo apt install tcl-dev libssl-dev cmake g++`
* macOS using macports: `sudo port install openssl cmake tcl-devel`
* Microsoft Windows with [chocolatey](https://chocolatey.org/): `choco install -y openssl cmake magicsplat-tcl-tk`

On windows, the magicsplat Tck/Tk is the easiest option to get a recent and complete Tcl/Tk installation with many precompiled extension packages. But you should make sure that it is installed in *C:/Program Files/Tcl* instead of the default user local directory. This can be achieved by adding the command line options `ALLUSERS=1 APPLICATIONFOLDER=C:\Progra~1\Tcl` to the msi install, or choosing the "Install for all users" option from the advanced dialog in the Installer GUI, if you go this way (click on advanced at the bottom of the dialog). Furthermore you need the [Microsoft Visual Studio](https://visualstudio.microsoft.com/vs/community/) with the "C++ workload". The free community edition is sufficient. The "C++ workload" contains the C++ Platform SDK and stuff, it can be installed from the *Visual Studio Installer* by clicking on *modify*.

Given the setup was successful and everything is in place, the following steps should be easy. Except on Windows they can be performed at a bash/zsh terminal. On windows, they must be performed at the *Developer PowerShell for VS &lt;version&gt;*, which can be found in the windows start menu.

### Get the sources

```
git clone https://github.com/ecky-l/tclwebsocket.git
cd tclwebsocket
git submodule update --init --recursive
```

Generate a make file or IDE project (on windows, it will generate a MSVC solution which can be opened in Visual Studio):

```
mkdir -p build
cd build
cmake ..
```

### Build the project

* Linux and macOS:

```
make
```

* Windows: 

```
msbuild .\tclwebsocket.sln /property:Configuration=Release
```

Afterwards you have *tclwebsocket.so*, resp. *tclwebsocket.dylib*, resp. Release/tclwebsocket.dll

### Run the tests

The client tests require an actual websocket server on the other side. This is in most cases a tclhttpd server with websocket servers implemented with the websocket package from tcllib. The server needs to be started asynchronously for automated tests and the most portable way to do this is via a thread. So there are a few more dependencies for running the tests:

* tclhttpd (fossil sources from https://core.tcl-lang.org/tclhttpd)
* [Tcl Thread extension](https://sourceforge.net/projects/tcl/files/Thread%20Extension/)
* Tcllib (fossil sources from https://core.tcl-lang.org/tcllib)

On Linux and macOS, the Thread and Tcllib extensions can usually be installed via the package manager (`dnf install tcllib`, `port install tcllib`, etc.). On Windows, they are part of Magicsplat Tcl/Tk. The tclhttpd webserver must be downloaded from the fossil repo using fossil. There are no recent releases yet available, but 3.5.3 is necessary.

The tests must find the tclhttpd installation, and to do this, they look for the environment variable `TCLHTTPD_HOME`. Set this environment variable to the place where your tclhttpd installation lives, this can be the checked out fossil repo sources:

Linux / macOS bash or zsh:

```
export TCLHTTPD_HOME=path/to/tclhttpd
```

Windows PowerShell

```
$env:TCLHTTPD_HOME = 'C:/path/to/tclhttpd'
```

Then you can execute the tests from the build directory above:


Linux / macOS:

```
ctest --verbose
```

Windows Powershell:

```
ctest -C Release --verbose
```

The tclhttpd websocket servers write a lot of stuff to stdout, including some errors logging which usually can be ignored. Important are the last lines  of a test run, which should look similar to this:

```
100% tests passed, 0 tests failed out of 2
```

Then the package can be installed.

### Install the package

This is as easy as `make install` on Linux/macOS and `cmake --install .` on Windows, both from the build directory.
