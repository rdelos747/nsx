# nsx
`nsx` is a lightweight and minimal text editor written in c++. It uses the `ncurses` library for rendering items in the terminal. 

This is currently a simple side project for fun. I often find myself wanting something more featured than nano but not as cumbersome as an IDE (eg kate or vscode). Running from the terminal is also a plus. So, instead of learning vim like I'm supposed to, I will attempt to write my own editor :)

`nsx` stands for `n`ew `s`oftware e`x`perimental and is no way a reference to anything else.

## building nsx
Note, you must have `ncurses` on your system. Eg:
```
pacman -S ncurses
```

1. `cd <my favorite path>/nsx`
2. `make`


## running nsx
1. `cd <path to my favorite directory for working on projects>`
2. `nsx` opens a blank instance of `nsx` in the current directory.
3. `nsx <myFavoriteFileName.myFavoriteFileExtension>` opens a file with `nsx`. If the file doesn't exist, `nsx` will create it on save.
