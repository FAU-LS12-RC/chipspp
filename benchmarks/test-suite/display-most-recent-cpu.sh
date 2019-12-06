declare -a cmd=(vim "$(ls -dt "$1"* | head -1)"/main_0.v)
if [ -n "$2" ]; then
  cmd+=(+"$2")
fi

"${cmd[@]}"
