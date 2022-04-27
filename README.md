# Narrow's chess engine
This is a chess engine and soon to be a chess AI program written in python.

## Installation
In order to be able to use this engine, you will need the tkinter module that can be easily obtained through the python package manager.

```bash
$ pip install tkinter
```

Once installed the tkinter module, clone the repository in your source folder with the following commands (if you have GIT installed)

```bash
$ git init

$ git clone https://github.com/Narr0wB/Chess
```

If you dont have git version control then you can download the repository as zip, and unzip it in your source folder

![Audio](https://raw.githubusercontent.com/Mottie/GitHub-userscripts/master/images/github-download-zip.gif)

## Usage

```python
import tkinter as tk
from Chess import GameBoard

root = tk.Tk()
board = GameBoard(root) # or GameBoard(root, start="fen string") to initialize a specific board
board.mainloop()
```

## Licence
[MIT](https://en.wikipedia.org/wiki/MIT_License)
