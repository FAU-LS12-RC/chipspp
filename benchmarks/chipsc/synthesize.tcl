set xprPath [lindex $argv 0]

open_project $xprPath
reset_run impl_1
launch_runs impl_1 -jobs 4
wait_on_run impl_1
#open_run impl_1
