# bexpect

**Description**\
bexpect is a command that interacts with two-way data packets over a network socket. Its core functionality is to send data and then wait for a response. It operates on a packet-by-packet basis.

**Setup and run tests**\
./setup.sh\
export HOME_DIR=$PWD\
cmake ./\
make\
./test/server 127.0.0.1:8090 &\
./bexpect 127.0.0.1:8090 blang/cmdfile.txt\
cd /usr/src/gtest\
cmake ./\
make\
cp lib/*.a /usr/lib\
cd $HOME_DIR/gtest\
cmake ./\
make\
./runTests
