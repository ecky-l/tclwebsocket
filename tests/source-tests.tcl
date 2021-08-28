#
# Simplified script to source tests from an interactive tclsh
# For debugging and development purposes.
#
# Example: tclsh
#
# % pwd
# .../tclwebsocket/build/win
# % set argv {-load Debug/tclwebsocket.tcl -match *client-io-buffer-small* -testdir ../../tests}
# % source ../../source-tests.tcl
# ...
# output
# ...
# %

load [dict get $argv -load]
package require tcltest
namespace import tcltest::*

set PWD [file normalize [file dirname [info script]]]
#set ::tcltest::testsDirectory $PWD

set chan $::tcltest::outputChannel
#tcltest::match *[dict get $argv -tests]*
foreach file [lsort [::tcltest::getMatchingFiles]] {
    set tail [file tail $file]
    puts $chan $tail
    if {[catch {source $file} msg]} {
	puts $chan $msg
    }
}

