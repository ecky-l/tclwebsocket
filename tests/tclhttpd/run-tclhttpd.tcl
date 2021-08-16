## run-tclhttpd.tcl (created by Tloona here)
package require Thread

global PWD
set PWD [file dirname [info script]]


proc start-tclhttpd-thread {} {
    global PWD env
    if {![info exists env(TCLHTTPD_HOME)]} {
        throw {START_TCLHTTPD THREAD} "Cannot start tclhttpd, the TCLHTTPD_HOME variable is not set!"
    }

    append script [list set tclHttpdLib [file join $PWD library]] \n
    append script [list set docRoot [file join $PWD data]] \n
    append script {
        set argv [list -host 127.0.0.1 -port 8015 -library $tclHttpdLib -docRoot $docRoot]
        source $env(TCLHTTPD_HOME)/bin/httpd.tcl
    }
    set tid [thread::create $script]
    puts "tclhttp starting..."
    after 2000
    set i 0
    for {} {[catch {socket localhost 8015} sock] && $i < 5} {incr i} {
        puts "tclhttp still starting..."
        after 1000
    }
    if {$i >= 5} {
        throw {START_TCLHTTPD THREAD} "Cannot start tclhttpd!"
    }
    close $sock
    return $tid
}

proc stop-tclhttpd-thread {tid} {
    thread::send $tid {set forever now}
}

proc start-tclhttpd {} {
    global PWD tcl_platform
    switch -- $tcl_platform(platform) {
        windows {
            set fh [open "|tclhttpd -library [file join $PWD library] -docRoot [file join $PWD data]"]
        }
        unix {
            set fh [open "|tclhttpd -library [file join $PWD library] -docRoot [file join $PWD data]"]
        }
    }

    puts "tclhttp starting..."
    after 2000
    while {[catch {socket localhost 8015} sock]} {
        puts "tclhttp still starting..."
        after 1000
    }
    close $sock
    return $fh
}

proc stop-tclhttpd {} {
    global tcl_platform
    puts "tclhttp stopping..."
    switch -- $tcl_platform(platform) {
        windows {
            exec powershell -Command "kill (Get-Process | Where-Object { \$_.MainWindowTitle -eq 'tclhttpd' }).Id"
        }
        unix {
            exec /bin/bash -c "kill \$(ps -ef | grep tclhttpd | grep -v grep | awk '{print \$2}')"
        }
    }
}

if {[info exists argv] && [llength $argv] > 0} {
    set stst [lindex $argv 0]
    switch -- $stst {
        start - stop {
            [set stst]-tclhttpd
        }
    }
}
