## run-tclhttpd.tcl (created by Tloona here)

global PWD
set PWD [file dirname [info script]]


proc start-tclhttpd {} {
    global PWD tcl_platform
    switch -- $tcl_platform(platform) {
        windows {
            open "|tclhttpd -library [file join $PWD library] -docRoot [file join $PWD data]"
        }
    }

    puts "tclhttp starting..."
    after 2000
    while {[catch {socket localhost 8015} sock]} {
        puts "tclhttp still starting..."
        after 1000
    }
    close $sock
}

proc stop-tclhttpd {} {
    global tcl_platform
    puts "tclhttp stopping..."
    switch -- $tcl_platform(platform) {
        windows {
            exec powershell -Command "kill (Get-Process | Where-Object { \$_.MainWindowTitle -eq 'tclhttpd' }).Id"
        }
    }
}

if {[llength $argv] > 0} {
    set stst [lindex $argv 0]
    switch -- $stst {
        start - stop {
            [set stst]-tclhttpd
        }
    }
}
