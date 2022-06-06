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
 - F8 - If debug is enabled, print to stdout the fen representation of the current board
 - F7 - If debug is enabled. print to stdout the move log ({move index} {move} {if its an enpassant, position of the capured pawn} {player who made the move} {if enpassant} ---> "5 c5d6 d5 player: WHITE enpassant: TRUE" or "4 d7d5 player: BLACK enpassant: FALSE")
 - F6 - Undo the last move
 - F5 - Restart the board to the `start` position
 
 ## Initialization parameters
 Moreove, there are several Board() initialization parameters that allow for further personalization of the game application:
 
 - `debug` allows for debug prints (included the hotkey F8) and move prints when the engine is enabled:
```python
from Chess import Board

board = Board(debug=True)
board.mainloop()
```
 - `start="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq"` to initialize the board to a certain fen position:
 ```python
from Chess import Board

board = Board(start="rn1qkbnr/pp1p4/8/2p5/4P3/5N2/2PP1PPP/2BQKB1R b KQkq")
board.mainloop()
```
- `start_player` to change the start player without having to change the fen position, `start_player` can only be Chess.WHITE or Chess.BLACK (respectively 0 or 1):
 ```python
from Chess import Board
from Chess import WHITE, BLACK

board = Board(start_player=BLACK) # start_player = 1
board.mainloop()
```
- `ai` makes the engine play against the player:
```python
from Chess import Board

board = Board(ai=True)
board.mainloop()
```
- `colorLight="#F0D9B5"` and `colorDark="#B58863"` allow to change the color of the board tiles, the color must be in hex:
```python
from Chess import Board

board = Board(colorLight="#ffffff", colorDark="000000")
board.mainloop()
```
- `win_message` to show a certain message once one side has won (NOTE: the game will display whichever side has won before displaying the win_message):
```python
from Chess import Board

board = Board(win_message="has won! yay!")
board.mainloop()
```
- `win_img_path` to show a certain image once someone has won:
```python
from Chess import Board

board = Board(win_img_path="C:\\Images\\Cat.png")
board.mainloop()
```
## License
[MIT](https://en.wikipedia.org/wiki/MIT_License)

