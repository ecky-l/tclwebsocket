## kraken-ws.tcl (created by Tloona here)
package require websocket 1.5.0

websocket::loglevel warn

Url_PrefixInstall /echo [list ::ws::connect /echo ::ws::echo::handle]

namespace eval ::ws {
    namespace eval echo {}
}

proc ::ws::echo::handle {sock type message} {
    try {
        switch -glob -- $type {
            connect {
            }
            text {
                puts "Received $message on sock $sock"
                websocket::send $sock text [string trimr $message]
            }
            disconnect - close {
                puts "bye $sock"
            }
        }
    } trap {} {err status} {
        puts "error occured during ws handle: $status"
    }
}


