Please read [DIC Projects](../7370projects.html) first. This `0README`
is a supplement to the lectures. It describes a bit of the philosophy
behind the course and some details of the source code given to you as a
start.

Prerequisites
=============

This course expects that you are already comfortable in the use of
networked machines based on Linux/Unix and X11. Learn to login to remote
machines via ssh without having to interactively type your password each
time; read [2350/ NetworkLab
\#ssh](http://cecs.wright.edu/~pmateti/Courses/2350/Labs/Network/NetworkLab.html#ssh)

Students whose Linux OS programming skills are rusty should study
examples for {`alarm, fork, pipes, signals, bash` scripting} much
simpler than what you see in our project files. At WSU, we expect CEG
4350/6350 OS Internals and Design to have taught these. Recommended
book: W. Richard Stevens and Stephen A. Rago, Advanced Programming in
the Unix Environment, Addison-Wesley, ISBN: 0-201-56317-7.
<http://www.apuebook.com/>

Some of you worry this is C and not C++. C is a subset of C++. Also, the
code does use lower-than-what-is-typical-in-C++ libraries. One of the
goals of this course: Expose you to the basics of systems programming
relevant to distributed computing. If you know C++ well enough, you
should be able to fully understand C. Some quick equiv C v C++:
`malloc/free` v `new/delete` mem allocation; `static` v `private` scope;
`static` v `static` life time; `e1? e2 : e3` v `e1?
e2 : e3` conditional expression;

RPC background is not expected. Several of the RPC lib procedures will
be discussed in class. On Linux, these have become part of C std lib; on
other OS you may have to explicitly give `-lrpcsvc` to the linker (`ld`
of `gcc` or `g++`).

How Does It Work?
=================

We will go over this in the lectures.

Compilation + Make
==================

Expand the tar ball of source code `7370-WB-C-RPC-2014.tbz` using
`tar xvvfj 7370-WB-C-RPC-2014.tbz`

It is expected that you will study the source code closely. It is only
800+ lines. You should become so familiar with it **as if** you wrote
it. The only exception is xwindow.c; just read the comments.

The source code as given is ready to be built using `make`. There should
be no compilation/link warnings except possibly in `rpcgen`-erated code.
It should also run without any glitches. If these are not working out
(i) in the OSIS Lab, report problems to me, (ii) on Linux systems
elsewhere, I may be able to help, but there is too much ground to cover.
Below `%` is my bash prompt as a non-root user.

Original v Generated Files
--------------------------

The following are the \"original\" (i.e., created by a human programmer)
files.

``` {.bash}
Makefile    the makefile for the project
server.x    server rpc interface
server.c    server specific code
client.x    client rpc interface
client.c    client specific code
ed-script   to generate client_s.c from client_svc.c
xwindow.c   a collection of simple X11 procedures
transient.c gets a vacant program number
```

Details
-------

1.  `rpcgen server.x` generates
    `server.h server_xdr.c server_clnt.c server_svc.c`

2.  `rpcgen client.x` generates
    `client.h client_xdr.c client_clnt.c client_svc.c`

3.  `client_s.c` is a minor variation of `client_svc.c` mechanically
    produced by running an editor script on `client_svc.c`. We insert
    `startclient(...)` just above the `svcrun()` line.

4.  Just the `xwindow.c` can be compiled as a stand-alone program
    `xwindow7370` to check for X11 problems.

5.  Note the use of `static int i` within procedures; such an `i` is not
    allocated on the stack. This makes `i` \"stay/live\" even after that
    procedure returns.

6.  Keep the compiler flags `-Wall -ansi -pedantic -std=c99` on; read-up
    on what these do. There should be zero errors/ warnings. The
    `rpcgen` generated code unfortunately is not as kosher. Note also
    that non-Linux OS may have ancient rpcgens that generate even worse
    code so far as these flags are concerned.

7.  Our code uses `alarm`, `fork`, `pipe` s, and `signal` s. Our (simple
    minded) use of signals requires `-D_BSD_SIGNALS.`

8.  The `% make` will compile the source code file `deregall.c` into a
    program named `deregall7370`. This can de-register transient RPC
    processes.

9.  To clean up the project dir, invoke `% make clean`.
10. To archive a version, do `% make tar`

How to Run It
=============

Some of you may wish to use your own Linux boxes for 7370 work. But
Linux installations out-of-the-box are typically not \"open\" enough for
us. Check your Linux Setup first.

1.  Host names below can be IP addresses (better/ simpler actually). In
    the OSIS lab, we use private IP addresses `192.168.17.*`

2.  In the source code as given: The server is one process running on
    host H1. The client consists of two processes both running on host
    H2. It is ok, but not desirable that H1 == H2. H2 is where the X11
    server is also running, using the bash-equaivalent of `export
      DISPLAY=H2:0.0`

3.  Run the \"server\" as a background process on a machine of your
    choice.\
    `% server7370 &`

4.  Run the \"client\" from one or more machines of your choice. The
    client needs four arguments:\
    `% client7370 <srv-mchn-nm> <wb-nm> <wb-disp> <color>=\\
     =<srv-mchn-nm>` is the name of the machine running your server;
    `<wb-nm>` is the name of the whiteboard you wish to create/join;
    `<wb-disp>` is the name of the machine you are logged in for the X11
    display; `<color>` is a 6-hex-digit X11 RGB color number for lines
    of this client;

Here are a few bash procedures: A simple \"smoke test\" with all the
processes running on `localhost` and kill all and deregister 7370
related processes. Read `man killall` and `man pkill`.

``` {.bash}
smokeTest7370() {
  ./server7370 &
  echo -n Server started, press Return to continue; read
  ./client7370 localhost b0 localhost ff0000 & # RGB=ff0000 = red
  echo -n client started, press Return to continue; read
  ./client7370 localhost b0 localhost ff00 & # RGB=00ff00 = green
  echo -n client started; press Return to continue; read
  ./client7370 localhost b0 localhost ff & # RGB=0000ff = blue
  echo -n client started, press Return to continue; read
  rpcinfo -p
}

killall7370() {
  killall -q server7370 client7370
  ./deregall7370 536871065  # change this to your server num
  ./deregall7370
  rpcinfo -p
}

freshStart7370() {
  killall7370
  smokeTest7370
  ps
  rpcinfo -p
}
```

Locate where `rpcinfo` is. Perhaps it is in `/usr/sbin` instead of
`/usr/bin`. Read its man page. Here is an example invocation on Linux
machines:

``` {.bash}
% rpcinfo -p
   program vers proto   port
    100000    2   tcp    111  portmapper
    100000    2   udp    111  portmapper
    ... more lines
```

Do the above after a fresh reboot. This shows the presence of other
standard RPC processes. After you \"close\" your WB project demo, the
list then should be the same.

Miscellaneous
=============

1.  Learn to use `backtrace()`; it will offset the time you may
    otherwise waste in debugging.
    <http://www.gnu.org/software/libc/manual/html_node/Backtraces.html>

2.  See what libraries are actually in use:
    `% ldd client7370 server7370`
