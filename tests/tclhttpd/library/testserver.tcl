## testserver.tcl (created by Tloona here)
package require websocket 1.5.0

websocket::loglevel warn

Url_PrefixInstall /test/text [list ::ws::connect /test/text ::ws::test::text::handle]

namespace eval ::ws {
    namespace eval test {
        namespace eval text {
            namespace eval Cmd {}
        }
    }
}


proc ::ws::test::text::handle {sock type message} {
    try {
        switch -glob -- $type {
            connect {
            }
            text {
                ProcessMessage $sock $message
                puts "Received $message on sock $sock"
                websocket::send $sock text $message
            }
            disconnect {
                puts "bye $sock"
            }
            ping {
                # ignore
            }
        }
    } trap {} {err status} {
        puts "error occured during ws handle: $status"
    }
}

proc ::ws::test::text::ProcessMessage {sock message} {
    set cmd [string tol [string trim [lindex [split $message :] 0]]]
    set args [lmap a [string trim [lindex [split $message :] 1]] {string trim $a}]
    ::ws::test::text::Cmd::[set cmd] $sock {*}$args
}

proc ::ws::test::text::Cmd::remoteclose {sock args} {
    websocket::close $sock
}


