## kraken-ws.tcl (created by Tloona here)
package require websocket 1.5.0

websocket::loglevel warn

Url_PrefixInstall /echo [list ::ws::connect /echo ::ws::echo::handle]

namespace eval ::ws {
    namespace eval echo {}
}

proc ::ws::connect {prefix handler sock suffix} {
    upvar #0 Httpd[set sock] data
    websocket::server $sock
    websocket::live $sock $prefix $handler
    set wstest [websocket::test $sock $sock $prefix $data(headerlist) $data(query)]
    if {$wstest} {
        Httpd_Suspend $sock 0
        websocket::upgrade $sock
    } else {
        Httpd_ReturnData $sock text/html "this is not a valid websocket"
    }
    puts "new socket $sock on $prefix"
    websocket::configure $sock -keepalive 0
}

proc ::ws::echo::handle {sock type message} {
    try {
        switch -glob -- $type {
            connect {
            }
            text {
                puts "Received $message"
                websocket::send $sock text $message
            }
            close {
                websocket::close $sock
            }
            ping {
                # ignore
            }
        }
    } trap {} {err status} {
        puts "error occured during ws handle: $status"
    }
}


