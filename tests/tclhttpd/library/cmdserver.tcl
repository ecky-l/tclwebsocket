## testserver.tcl (created by Tloona here)
package require websocket 1.5.0

websocket::loglevel warn

Url_PrefixInstall /cmd [list ::ws::connect /cmd ::ws::cmd::handle]

namespace eval ::ws {
    namespace eval cmd {
        namespace eval Cmd {}
    }
}

interp alias {} ::ws::cmd::puts {} ::ws::puts

proc ::ws::cmd::handle {sock type message} {
    try {
        switch -glob -- $type {
            connect {
            }
            text {
                puts "Received $message on sock $sock"
                ProcessMessage $sock $message
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

proc ::ws::cmd::ProcessMessage {sock message} {
    set cmd [string tol [string trim [lindex [split $message :] 0]]]
    set args [lmap a [lindex [split $message :] 1] {string trim $a}]
    ::ws::cmd::Cmd::[set cmd] $sock {*}$args
}

proc ::ws::cmd::Cmd::remoteclose {sock args} {
    websocket::close $sock
}

proc ::ws::cmd::Cmd::echoafter {sock tm text} {
    after $tm
    websocket::send $sock text $text
}


