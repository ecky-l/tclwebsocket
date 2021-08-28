## kraken-ws.tcl (created by Tloona here)
package require websocket 1.5.0
package require sha256 1.0.4

websocket::loglevel warn

Url_PrefixInstall /echo [list ::ws::connect /echo ::ws::echo::handle]

namespace eval ::ws {
    namespace eval echo {}
}

interp alias {} ::ws::echo::puts {} ::ws::puts


proc ::ws::echo::handle {sock type message} {
    puts wwwwww,$type
    try {
        switch -glob -- $type {
            connect {
            }
            text {
                puts "Received $message on sock $sock"
                websocket::send $sock text [string trimr $message]
            }
            binary {
                puts "Received binary data with size [string length $message] on sock $sock"
                websocket::send $sock text [sha2::sha256 -hex -- $message]
            }
            disconnect {
                puts "disconnected $sock"
            }
            close {
                puts "closed $sock"
            }
        }
    } trap {} {err status} {
        puts "error occured during ws handle: $status"
    }
}


