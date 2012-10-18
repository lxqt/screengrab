ScreenGrab
==========

version 0.9.96 (1.0-rc)

ScreenGrab - program for fast creating screenshots, and  simple publishing their on the internet  image hostings. It working on Linux and Windows operating systems. The program uses Qt and is independent of any desktop environment.

Build requirements
------------------

 * Qt >= 4.6
 * CMake >= 2.6 (only for build)
 * GCC > 4.4 (MSVS now is not supported )
 * [optional] Qxt Library > 0.6 (if you want build with using your system Qxt version - see "Build options" section in this file)

Build
-----

Build ScreenGrab, using these commands:

	mkdir build
	cd build
	cmake [BUILD OPTIONS] ../
	make
	make install

Build options
-------------

You can use some or all of these params for customize your build.

 * **-DCMAKE_INSTALL_PREFIX** - Install prefix for Linux distro. By default is "/usr".
 * **-DSG_XDG_CONFIG_SUPPORT** - Place config files into XDGC_ONFIG_HOME directory (usualy - ~/.config/${app_name) ). By default - in ON. In previous versions config files was storaged in ~/.screengrab (this behavior is when its option is OFF)
 * **-DSG_EXT_UPLOADS** - Enableupload screnshots to image hostings. By default is ON.
 * **-DSG_GLOBALSHORTCUTS** - Enable global shortcuts for main cations to create screenshots. By default is ON.
 * **-DSG_USE_SYSTEM_QXT** - Use system version Qxt Library for global shortcuts. By default if OFF.
 * **-DSG_DOCDIR** - Name for directory of user's documentation. By default is  "screengrab".
 
**Build notes:**

 * For Debian based Linux distro (Debian Squeezy, Ubuntu 12.04, etc) - if you want to build Screengrab wuth using system version of Qxt Library,  please use this command to run CMake:
 
	cmake -DSG_USE_SYSTEM_QXT=ON -DQXT_QXTCORE_INCLUDE_DIR=/usr/include/qxt/QxtCore -DQXT_QXTGUI_INCLUDE_DIR=/usr/include/qxt -DCMAKE_INSTALL_PREFIX=/usr ../


LICENSE
-------

Screengrab is licensed under the GPL v2. See file LICENSE in docs directory for more information

Contacts
--------

E-mail: doomer3d@gmail.com
Jabber: doomer@jabber.linux.it
Web homepage: http://screengrab.doomer.org/


Copyright (c) 2009-2012, Artem 'DOOMer' Galichkin

