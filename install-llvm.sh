set -e
installDir="$1"

if [ -e "$installDir"/bin/llvm-config ]; then
  exit
fi

git clone https://github.com/llvm/llvm-project.git llvm-src
cd llvm-project
git checkout llvmorg-6.0.0
cd llvm
mkdir build
cd build
cmake ..
cmake --build .
cmake -DCMAKE_INSTALL_PREFIX="$installDir" -P cmake_install.cmake
#rm -r llvm-src
