## pingserver.tcl (created by Tloona here)

Url_PrefixInstall /test/ping [list ::ws::connect /test/ping ::ws::test::ping::handle]

namespace eval ::ws {
    namespace eval test {
        namespace eval ping {
            variable NPingPongs
            array set NPingPongs {}
        }
    }
}

interp alias {} ::ws::test::ping::puts {} ::ws::puts


proc ::ws::test::ping::handle {sock type message} {
    variable NPingPongs

    switch -glob -- $type {
        connect {
            #puts "connected $sock"
            set NPingPongs($sock) 0
            websocket::send $sock ping
        }
        text {
        }
        disconnect {
            #puts "disconnected $sock"
        }
        close {
            #puts "closed $sock"
        }
        pong {
            incr NPingPongs($sock)
            puts "Got $NPingPongs($sock) pongs from $sock"
            if {$NPingPongs($sock) < 5} {
                after 1000
                websocket::send $sock ping
            } else {
                websocket::send $sock text done
            }
        }
    }
}
