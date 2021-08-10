## run.tcl (created by Tloona here)
# all.tcl --
#
# This file contains a top-level script to run all of the Tcl
# tests.  Execute it by invoking "source all.test" when running tcltest
# in this directory.
#
# Copyright (c) 1998-2000 by Scriptics Corporation.
# All rights reserved.

package require tcltest
namespace import ::tcltest::*

if {[llength $argv] != 0} {
    if {[dict exists $argv -files]} {
        tcltest::matchFiles *[dict get $argv -files].test
    }
    if {[dict exists $argv -tests]} {
        tcltest::match *[dict get $argv -tests]*
    }
}

set ::tcltest::testSingleFile false
set ::tcltest::testsDirectory [file dir [info script]]

tcltest::configure -verbose {body pass line error}

# We need to ensure that the testsDirectory is absolute
if {[catch {::tcltest::normalizePath ::tcltest::testsDirectory}]} {
    # The version of tcltest we have here does not support
    # 'normalizePath', so we have to do this on our own.

    set oldpwd [pwd]
    catch {cd $::tcltest::testsDirectory}
    set ::tcltest::testsDirectory [pwd]
    cd $oldpwd
}

set chan $::tcltest::outputChannel

puts $chan "Tests running in interp:       [info nameofexecutable]"
puts $chan "Tests running with pwd:        [pwd]"
puts $chan "Tests running in working dir:  $::tcltest::testsDirectory"
if {[llength $::tcltest::skip] > 0} {
    puts $chan "Skipping tests that match:            $::tcltest::skip"
}
if {[llength $::tcltest::match] > 0} {
    puts $chan "Only running tests that match:        $::tcltest::match"
}

if {[llength $::tcltest::skipFiles] > 0} {
    puts $chan "Skipping test files that match:       $::tcltest::skipFiles"
}
if {[llength $::tcltest::matchFiles] > 0} {
    puts $chan "Only sourcing test files that match:  $::tcltest::matchFiles"
}

set timeCmd {clock format [clock seconds]}
puts $chan "Tests began at [eval $timeCmd]"

# run tclhttpd
source [file join [file dirname [info script]] tclhttpd run-tclhttpd.tcl]
start-tclhttpd

# source each of the specified tests
foreach file [lsort [::tcltest::getMatchingFiles]] {
    set tail [file tail $file]
    puts $chan $tail
    if {[catch {source $file} msg]} {
	puts $chan $msg
    }
}

proc tcltest::cleanupTestsHook {} {
    variable numTests
    set ::exitCode [expr {$numTests(Failed) > 0}]
}

stop-tclhttpd

# cleanup
puts $chan "\nTests ended at [eval $timeCmd]"
::tcltest::cleanupTests 1

exit $exitCode


