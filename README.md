# Snake made in C using ncurses library
Implementation of old Snake game using ncurses library written in C

You'll obviously need ncurses library installed on your system
* Ubuntu/Debian:
```
apt-get install libncurses5-dev
```
* Arch Linux:
```
pacman -S ncurses
```
* Gentoo:
```
emerge --ask sys-libs/ncurses
```

Compile it like this
```
make
```

Run it like this
```
./snake.out
```

It uses Unix signals so probably wont run on Windows.
Feel free to contribute and shout at me if I made something wrong.