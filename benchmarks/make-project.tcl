#puts "Length: [llength $argv]"
#foreach arg $argv {
#  puts "$arg"
#}
#puts [lindex $argv 0]
#puts [lindex $argv 1]
#puts [lindex $argv 2]

#open_project chips/chips.xpr
#save_project_as [lindex $argv 0] /tmp/foo
#remove_files [get_files -of [get_filesets {sources_1 sim_1}]]
#add_files -fileset sources_1 /tmp/blah.v

set src [lindex $argv 0]
set name [lindex $argv 1]
set dest generated-models-tests/temp/$name

#create_project -part xc7z010clg400-1 $name $dest
create_project -part xc7z020clg484-1 $name $dest
add_files -fileset sources_1 $src/src
add_files -fileset constrs_1 -copy_to $dest $src/constraints.xdc
add_files -fileset sim_1 $src/sim/tb_file.vhd
add_files -fileset sim_1 $src/sim/file_io.vhd
