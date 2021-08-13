## ws-common.tcl (created by Tloona here)
package require websocket 1.5.0

namespace eval ::ws {
}

proc ::ws::puts {args} {
    ::puts {*}[lrange $args 0 end-1] "    *** WS Server :: [lindex $args end] ::"
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
