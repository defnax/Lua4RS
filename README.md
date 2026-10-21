Lua4RS
======

A RetroShare plugin to automate and script things on RetroShare using the Lua language.

What is RetroShare?
-------------------
RetroShare is a secure decentralised communication platform.
[http://retroshare.sourceforge.net/](https://github.com/RetroShare/RetroShare)

What is Lua4RS?
---------------
This plugin let you interact with RetroShare. It exposes most RetroShare functions to Lua and provides a trigger system. You can write scripts that react to events (from RetroShare) like _a friends comes online_ or _received a new mail_. With access to most RetroShare functions, you can do the same in the Lua language as you can do in C++.

Requirements
------------
* RetroShare v0.6
* Lua 5.5

MSYS2:
pacman -S mingw-w64-x86_64-lua

Ubuntu:
sudo apt install lua5.5

Arch Linux:
sudo pacman -S lua

# Build & install
------------

put/clone `Lua4RS` to `RetroShare/plugins/` recommend

	cd ${YOUR_DIR}/RetroShare/plugins/Lua4RS/
	qmake
	make 

Then place libLua4RS.so/Lua4RS.dll in your extension folder.
