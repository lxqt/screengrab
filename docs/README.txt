ScreenGrab  readme file
version 0.9

ScreenGrab - program geting screenshots working in Linux and Windows. The program uses Qt and is independent of any desktop environment.

INSTALL
-------
Windows:
1. Run screengrab-0.9-win32.exe, and follow setup wizzard.
2. Run SWcreenGrab into Start Menu
4. Enhoy =)

Linux
0. Install qt package from our Linux disto repos.
1. Unpack screengrab-0.9.tar.gz)
 into same directory.
2. Run cmake from directory with CMakeLists.txt and make.
    After this run make install (as root) to installing.
3. Run screengrab.
4. Enhoy =)

USAGE
-----

  It is a very simple =)
On first run application get current state of your desktop. Preview screen displays into main window.

Buttons on main window:
    "New Screen" - take new screenshot	.
    "Save Screen" - write getting screenshtit on hard disk (in PNG or JPEG format).
    "Copy Screen" - copy screen into clipboard.
    "Options" - show program options window.
	  "Help" -- display help information 
    "Information" - display information about ScreenGrab , developer and license.
	  "Quit" - Exit program.

Other main window options:
    "Type" - type of grabbing [full sacreen, active window or selection area].
    "Delay" - delay in seconds before grab screen.

This buttons is a duplicate into systemtray menu.

Settings:
    "Default directory" - default directory for save screenshots.
     - in Linux is a user home dir,
     - in Windows is a userprofile settings dir.
    "Default filename" - name of saved file by default.
    "Format" - default saving image format.
    "Format" - grab active window without decoraytions (Linux only)
    "Insert DateTime in filename" - insetring date and time in saving filename.
    "Autosave screen" -- automaticaly saving screenshot in grab process.    
    "Tray messages" - mode of displays tray messages 9on grab|save events.
	"Time of display tray messages" -- time within which notices are displayed.
    "Save window size on exit" - saving current size of main window at the exit.
        Restoring saved window size if you rerun the application
    "Close in tray" - minimize main window in tray by click on close window button.
    "Allow multiple copies" - allow run multiple copies of ScreenGrab.

Application cam be hiden into system tray, and managed through the contxt menu from system tray.

Command Line parameters:
    --fullscreen - get desktop screenshot [default]
    --active - get active window screenshot
    --region - get screenshots of desktop selection area

LICENSE
-------

Screengrab is licensed under the GPL v2. See file LICENSE for more information

--------
    Copyright (c) 2009, Artem 'DOOMer' Galichkin

