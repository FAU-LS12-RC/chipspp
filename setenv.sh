#if [ $# -lt 1 ]; then
#  echo "Usage: ./$0 <llvm-installation-folder>" >&2
#else
  llvm="clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz"
  #llvm='clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz'
  if command -v lsb_release; then
      if [[ "$(lsb_release -a)" = *"14.04"* ]]; then
          llvm="clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04.tar.xz"
      fi
  fi
  cmake="cmake-3.11.4-Linux-x86_64.tar.gz"

  dir=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
  oldd="$(pwd)"

  #llvm_dir="${llvm%.*.*}"
  llvm_dir="$dir"/llvm-install
  cmake_dir="${cmake%.*.*}"
  icarus_dir=iverilog

  echo "llvm_dir = $llvm_dir"
  echo "cmake_dir = $cmake_dir"
  echo "icarus_dir = $icarus_dir"

  cd $dir

  if [ ! -d "$llvm_dir" ]; then
      echo "Fetching and building $llvm"
      bash install-llvm.sh "$llvm_dir"
#      wget http://releases.llvm.org/6.0.0/$llvm && tar -xf "$llvm" && rm $llvm
#      wget http://releases.llvm.org/8.0.0/$llvm && tar -xf "$llvm" && rm $llvm
  else
      echo "llvm found"
  fi

  if [ ! -d "$cmake_dir" ]; then 
      echo "Fetching $cmake"
      wget https://cmake.org/files/v3.11/$cmake && tar -xzf $cmake && rm $cmake
  else
      echo "cmake found"
  fi

  if [ ! -d "$icarus_dir" ]; then 
      echo "Fetching icarus"
      gperf=gperf-3.1
      git clone --depth 1 https://github.com/steveicarus/iverilog iverilog-src
      wget http://ftp.gnu.org/pub/gnu/gperf/$gperf.tar.gz
      tar xf $gperf.tar.gz
      (
        cd $gperf
        ./configure --prefix=/
        make -j8 DESTDIR="$(pwd)"/gperf-install install
      )
      (
        cd iverilog-src
        aclocal
        autoconf
        ./configure --prefix=/
        PATH="$(pwd)"/../$gperf/gperf-install/bin:"$PATH" make \
          -j8 prefix="$(pwd)"/../iverilog install
      )
      rm -rf iverilog-src
      rm -rf $gperf
      rm $gperf.tar.gz
  else
      echo "icarus found"
  fi

  cd $oldd

  if [[ $PATH = *"$cmake_dir"* ]]; then
      echo "cmake already loaded"
  else
      export PATH=$dir/$cmake_dir/bin:$PATH
      echo "cmake loaded"
  fi

  if [[ $PATH = *"$llvm_dir"* ]]; then
      echo "llvm already loaded"
  else
      export PATH=$dir/$llvm_dir/bin:$PATH
      export CMAKE_PREFIX_PATH=$llvm_dir
      echo "llvm loaded"
  fi

  if [[ $PATH = *"$icarus_dir"* ]]; then
      echo "icarus already loaded"
  else
      export PATH=$dir/$icarus_dir/bin:$PATH
      echo "icarus loaded"
  fi
  export ICARUSLIB=$dir/$icarus_dir/lib

  export PATH=$dir/benchmarks/chipsc/bin:"$PATH"
#  export LLVM_DIR="$1"
#fi
