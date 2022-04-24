# Narrow's chess engine
This is a chess engine and soon to be a chess AI program written in python.

## Installation
In order to be able to use this engine, you will need the tkinter module that can be easily obtained through the python package manager.

```bash
pip install tkinter
```

Once installed the tkinter module, clone the repository in your src folder.

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
