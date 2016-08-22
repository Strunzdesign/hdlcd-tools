# The Toolbox for the HDLC Daemon (hdlcd-tools)
Additional tools to be used together with the "HDLC Daemon" (HDLCd).

This package offers additional tools to be used together with the "HDLC Daemon" (HDLCd) that implements the
"High-level Data Link Control" protocol (HDLC). The HDLC-Daemon is found here:
- https://github.com/Strunzdesign/hdlcd
- Install the HDLCd and its bundeled header files before compiling the software from this repository!

This software is intended to be portable and makes use of the boost libraries. It was tested on GNU/Linux (GCC toolchain)
and Microsoft Windows (nuwen MinGW).

Releases of the HDLCd-Tools:
- none yet

Current state:
- Works well with s-net(r) BASE release 3.6

Required libraries and tools:
- GCC, the only tested compiler collection thus far (tested: GCC 4.9.3, GCC 6.1)
- Boost, a platform-independent toolkit for development of C++ applications
- CMake, the build system
- Doxygen, for development
- nuwen MinGW, to compile the software on Microsoft Windows (tested: 13.4, 14.0)

See online doxygen documentation at http://strunzdesign.github.io/hdlcd-tools/
