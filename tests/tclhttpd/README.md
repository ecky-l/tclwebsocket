## tclhttpd websocket servers for testing

Here is some library code to spin up a test websocket server using [tclhttpd](https://core.tcl-lang.org/tclhttpd/index) and the [websocket](https://core.tcl-lang.org/tcllib/doc/trunk/embedded/md/tcllib/files/modules/websocket/websocket.md#3) package.

To use it, startup tclhttpd with *-library ./library -docRoot ./data* options. I.e., if you have a script called *tclhttpd* that starts tclhttpd from source or you use a starpack:

```
$ cd tclwebsocket/test/tclhttpd
$ tclhttpd -library ./library -docRoot ./data
```

## Servers provided on urls

* /echo - a simple echo server that reacts on text messages and echos back every input. Additionally, the input is written to stdout
