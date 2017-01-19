
GoboNet
=======

An ultra-simple network management toolkit. Currently supports only Wifi + DHCP.

Requirements
------------

* wpa_supplicant
* dhcpcd
* rfkill
* net-tools (ifconfig)
* wireless-tools (iwlist)
* psmisc (killall)
* pinentry or zenity (for prompts)
* bash
* coreutils

Usage
-----

### GUI

There is a systray-style widget for Awesome WM: [gobo-awesome-gobonet](https://github.com/gobolinux/gobo-awesome-gobonet)

### Command-line

The interface is via a simple command-line client:

* `gobonet autoconnect` - Scan Wifi networks and connect to a known one
* `gobonet connect "<essid>"` - Connect to a given Wifi network
* `gobonet disconnect` - Disconnect Wifi
* `gobonet forget "<essid>"` - Disconnect Wifi and forget config for this network
* `gobonet list` - List available Wifi networks

