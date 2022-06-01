# Narrow's chess engine
This is a chess application written in python and a chess engine written in C++.

## Installation
In order to use the application, you will need the tkinter and pillow modules that can be easily installed through the python package manager.

```bash
$ pip install tkinter
...
$ pip install pillow
```

Once installed the necessary modules, clone the repository in your source folder with the following commands (if you have GIT installed)

```bash
$ git init

$ git clone https://github.com/Narr0wB/Chess
```

If you dont have git version control then you can download the repository as zip, and unzip it in your source folder

![Audio](https://raw.githubusercontent.com/Mottie/GitHub-userscripts/master/images/github-download-zip.gif)

## Usage

```python
from Chess import Board

board = Board()
board.mainloop()
```

## Hotkeys

 - F11 - Toggle fullscreen
 - F8 - if debug is true, print to stdout the fen representation of the current board
 - F5 - restart the board to the START_POSITION
 
 ## Initialization parameters
 Moreove, there are several Board() initialization parameters that allow for further personalization of the game application:
 
 - `debug=False` allows for debug prints (included the hotkey F8) and move prints when the engine is enabled
```python
from Chess import Board

board = Board(debug=True)
board.mainloop()
```
 - `start="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq"` to initialize the board to a certain position
 ```python
from Chess import Board

board = Board(start="rn1qkbnr/pp1p4/8/2p5/4P3/5N2/2PP1PPP/2BQKB1R b KQkq")
board.mainloop()
```
- `ai=False` to make the ai play against the player (NOTE: make sure the file Chessai.exe is compiled and in ~/src)
```python
from Chess import Board

board = Board(ai=True)
board.mainloop()
```
-`colorLight="#F0D9B5"` and `colorDark="#B58863"` allow to change the color of the board tiles, the color must be in hex
```python
from Chess import Board

board = Board(colorLight="#ffffff", colorDark="000000")
board.mainloop()
```

## Licence
[MIT](https://en.wikipedia.org/wiki/MIT_License)

