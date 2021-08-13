## tclhttpd websocket servers for testing

Here is some library code to spin up a test websocket server using [tclhttpd](https://core.tcl-lang.org/tclhttpd/index) and the [websocket](https://core.tcl-lang.org/tcllib/doc/trunk/embedded/md/tcllib/files/modules/websocket/websocket.md#3) package.

To use it, startup tclhttpd with *-library ./library -docRoot ./data* options. I.e., if you have a script called *tclhttpd* that starts tclhttpd from source or you use a starpack:

```
$ cd tclwebsocket/test/tclhttpd
$ tclhttpd -library ./library -docRoot ./data
```

## Servers provided on urls

* /echo - a simple echo server that reacts on text messages and echos back every input. Additionally, the input is written to stdout
* /cmd - a command server. some commands are implemented, see cmdserver.tcl. Used for the tests
* /ping - a ping server with the sole purpose to send pings and receive pongs after connection

## Usage in automated tests

For the automated tests the tclhttpd server is started in a worker thread from the run.tcl script. Therefore the Thread package is required.
Furthermore it needs to know where your tclhttpd installation is, to resolve the bin/httpd.tcl file underneath. Therefore it uses the environment variable TCLHTTPD_HOME, which must be set to the corresponding directory. For example:

```
export TCLHTTPD_HOME=~/tclhttpd
```

on unix or

```
$env:TCLHTTPD_HOME='C:/Users/myself/tclhttpd'
```

on windows powershell. It is of course also possible to set this permanently via ~/.profile resp. the environment settings dialog in windows.
