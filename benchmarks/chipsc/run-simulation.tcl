#proc printCmd { args } {
#  puts [join $args { }]
#}

set xprPath [lindex $argv 0]
set topLevelModule [lindex $argv 1]
set postImpl [lindex $argv 2]
#TODO refactor: get_fileset sim_1

puts $xprPath

set defines [list]
foreach arg $argv {
  if [string match *.vivadobugworkaround $arg] {
    lappend defines [string trimright $arg .vivadobugworkaround]
  } else {
    lappend defines $arg
  }
}

open_project $xprPath

set_property top $topLevelModule [get_fileset sim_1]

puts $argv
set_property verilog_define [join [lrange $defines 2 end] { }] [get_fileset sim_1]

set_property -name {xsim.simulate.runtime} -value {1000ms} -objects [get_filesets sim_1]

#TODO if (boolean in tcl?)
if $postImpl {
  launch_simulation -mode post-implementation -type functional
} else {
  launch_simulation 
}

#restart
#run 10ms
