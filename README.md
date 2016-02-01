Keywatch
========
**Automatic key  change detection for PGP**

Public keyservers are inherently untrustworthy: anyone can submit a key,
and even servers such as the PGP Global Directory, which provides email
verification, can only be as secure as the email verification process.

Ideally we would all use the web of trust, however this does not occur
very often in practice.  If PGP is used at all, in many cases it is used
with a key downloaded directly from a website, or&mdash;more
precariously&mdash;from a keyserver.

Keywatch audits these sources in order to prevent malicious users or
servers from providing incorrect keys.  Should a malicious user
submit a new key to the keyserver, the true user will be made
aware.  A unique feature of Keywatch is the ability to act against
malicious _keyservers_ as well.  By connecting to the keyserver
using Tor, it is possible to provide a conditional guarantee that the
keyserver is not selectively providing different keys to different
users.

Synopsis
--------

Connect to a keyserver via specified proxy and watch for changes in the
fingerprint of the first response.
```
  keywatchd [-s keyserver] [-p proxy] <email> ...
```
By default, keywatchd will use Tor to connect to
http://jirk5u4osbsr34t5.onion:11371,
a hidden service that provides access to the SKS Keyserver network.  We use
This is defensive design decision to ensure that a bug cannot cause the
site to be resolved directly, without the use of Tor.

You can test the program by putting a sample HKP response onto a local
webserver at `/pks/lookup`:
```
info:1:2
pub:F3E3889185605B82933D6180D28891D2136B33B0:1:4096:1445547156::
uid:Lachlan Gunn <lachlan@twopif.net>:1449261644::
uid:Lachlan Gunn <lachlan.gunn@adelaide.edu.au>:1449262330::
pub:AE75A45641CD9EC9FF468912B679F3623E7B3C06:1:4096:1433251694::
uid:Lachlan Gunn <lachlan@twopif.net>:1433254056::
uid:Lachlan Gunn <lachlan.gunn@adelaide.edu.au>:1433254020::
```
Then, run `keywatchd` as follows:
```
keywatchd -s http://localhost -p "" lachlan@twopif.net
```
Sometime during the first twenty seconds, it will load the file from
the web server.  Now modify the fingerprint in the first key somehow
(we have added an initial 'X').  The next time the key is downloaded,
the changed fingerprint will be detected and the new key will be printed:
```
> keywatchd -s http://localhost -p "" lachlan@twopif.net
Key: XF3E3889185605B82933D6180D28891D2136B33B0
    UID: Lachlan Gunn <lachlan@twopif.net>
    UID: Lachlan Gunn <lachlan.gunn@adelaide.edu.au>
```
This will continue until the file is returned to its original state.

Building keywatchd
------------------

Keywatch is written in C++, and requires the Boost and libcurl libraries.
The build infrastructure uses CMake, and has been tested on Linux with both
GCC and clang.  It can be built as follows:
```
keywatch > mkdir build && cd build && cmake .. && make
-- The C compiler identification is GNU 4.8.4
-- The CXX compiler identification is GNU 4.8.4
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Boost version: 1.55.0
-- Found the following Boost libraries:
--   program_options
-- Found CURL: /usr/lib/x86_64-linux-gnu/libcurl.so (found version "7.35.0") 
-- Configuring done
-- Generating done
-- Build files have been written to: /home/lachlan/source/keywatchd/build
Scanning dependencies of target keywatchd
[ 16%] Building CXX object src/CMakeFiles/keywatchd.dir/main.cc.o
[ 33%] Building CXX object src/CMakeFiles/keywatchd.dir/keys/keys.cc.o
[ 50%] Building CXX object src/CMakeFiles/keywatchd.dir/hkp/hkp.cc.o
[ 66%] Building CXX object src/CMakeFiles/keywatchd.dir/hkp/parser.cc.o
[ 83%] Building CXX object src/CMakeFiles/keywatchd.dir/daemon/config.cc.o
[100%] Building CXX object src/CMakeFiles/keywatchd.dir/daemon/worker.cc.o
Linking CXX executable keywatchd
[100%] Built target keywatchd
```
The resulting executable is `build/src/keywatchd`.