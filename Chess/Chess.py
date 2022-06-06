import subprocess
import tkinter as tk
import time
import platform
import os
from PIL import Image, ImageTk

my_system = platform.uname()

CHESS_START = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
dir_path = os.path.dirname(os.path.realpath(__file__))

TIMEFL = 0.00000000150 if my_system.node.lower().find("desktop") == -1 else 0.000150

"""
    TODO list:
        - Add images of the eaten piece under or over the board - In progres...
        - Start working on the AI - DONE!      
"""

KING = 0
QUEEN = 1
BISHOP = 2
KNIGHT = 3
ROOK = 4
PAWN = 5

WHITE = 0
BLACK = 1

pieceToFen = {
    (KING, BLACK) : "k",
    (QUEEN, BLACK) : "q",
    (BISHOP, BLACK) : "b",
    (KNIGHT, BLACK) : "n",
    (ROOK, BLACK) : "r",
    (PAWN, BLACK) : "p",
    (KING, WHITE) : "K",
    (QUEEN, WHITE) : "Q",
    (BISHOP, WHITE) : "B",
    (KNIGHT, WHITE) : "N",
    (ROOK, WHITE) : "R",
    (PAWN, WHITE) : "P",
    }

fenToImage = {
        "k" : Image.open(f"{dir_path}\\assets\\bking.png"),
        "q" : Image.open(f"{dir_path}\\assets\\bqueen.png"),
        "b" : Image.open(f"{dir_path}\\assets\\bbishop.png"),
        "n" : Image.open(f"{dir_path}\\assets\\bhorse.png"),
        "r" : Image.open(f"{dir_path}\\assets\\brook.png"),
        "p" : Image.open(f"{dir_path}\\assets\\bpawn.png"),
        "K" : Image.open(f"{dir_path}\\assets\\wking.png"),
        "Q" : Image.open(f"{dir_path}\\assets\\wqueen.png"),
        "B" : Image.open(f"{dir_path}\\assets\\wbishop.png"),
        "N" : Image.open(f"{dir_path}\\assets\\whorse.png"),
        "R" : Image.open(f"{dir_path}\\assets\\wrook.png"),
        "P" : Image.open(f"{dir_path}\\assets\\wpawn.png"),
    }

fenToPiece = {
    "k" : (KING, BLACK),
    "q" : (QUEEN, BLACK),
    "b" : (BISHOP, BLACK),
    "n" : (KNIGHT, BLACK),
    "r" : (ROOK, BLACK),
    "p" : (PAWN, BLACK),
    "K" : (KING, WHITE),
    "Q" : (QUEEN, WHITE),
    "B" : (BISHOP, WHITE),
    "N" : (KNIGHT, WHITE),
    "R" : (ROOK, WHITE),
    "P" : (PAWN, WHITE),
    }

columnToLetter = {
    0 : "a",
    1 : "b",
    2 : "c",
    3 : "d",
    4 : "e",
    5 : "f",
    6 : "g",
    7 : "h"
}

letterToColumn = {
    "a" : 0,
    "b" : 1,
    "c" : 2,
    "d" : 3,
    "e" : 4,
    "f" : 5,
    "g" : 6,
    "h" : 7
}


class Piece:
    def __init__(self, pieceType: int, color: int, image: ImageTk.PhotoImage):
        self.type = pieceType
        self.color = color
        self.image = image
        self.fentype = pieceToFen[(pieceType, color)]
        self.uniqueCode = ""

def sleep(ms: float):
    timer = time.time()
    while timer + ms > time.time(): #wait is your sleep time
        pass

# str-keys only
def sort_keys(keys: list, crescente: bool = False) -> list:
    keys = list(keys)
    sorted_list = keys.copy() # 3 1 2
    temp_elem = ""
    if crescente:
        for g in range(len(keys)):
            for i in range(g+1, len(keys)):
                if int(keys[i][:keys[i].find(" ")]) < int(keys[g][:keys[g].find(" ")]):
                    temp_elem = sorted_list[i]
                    sorted_list[i] = sorted_list[g]
                    sorted_list[g] = temp_elem
    else:
        for g in range(len(keys)):
            for i in range(g+1, len(keys)):
                if int(keys[i][:keys[i].find(" ")]) > int(keys[g][:keys[g].find(" ")]):
                    temp_elem = sorted_list[i]
                    sorted_list[i] = sorted_list[g]
                    sorted_list[g] = temp_elem
    
    return sorted_list

class Board():
    def __init__(self, debug=False, tileSize=100, colorLight="#F0D9B5", colorDark="#B58863", start: str = CHESS_START, ai: bool = False, win_message: str = "won", win_img_path: str = f"{dir_path}\\assets\\chess_win.png", start_player: int = -1):
        self.debug = debug

        self.parent = tk.Tk()
        self.rows = 8
        self.columns = 8
        self.fullscreen = True
        self.size = tileSize
        self.boardColor1 = colorLight
        self.boardColor2 = colorDark
        self.animationFps = 144

        self.whiteKingsideCastle = False
        self.whiteQueensideCastle = False
        self.blackKingsideCastle = False
        self.blackQueensideCastle = False

        self.currentPlayer = start_player
        self.selectedPiece = ()
        self.enpassant = []
        self.moves = []
        self.popped_enpassant = []
        self.useAI = ai
        if self.useAI:
            self.ai = subprocess.check_output
        self.startPlayer = start_player if start_player != -1 else WHITE if start[start.find(" ")+1] == "w" else BLACK

        self.board = {}
        self.oldBoard = {}
        self.movelog = {}
        self.move_log_r = ""
        self.win_text = win_message if len(win_message) < 20 else "won"
        self.win_image = ImageTk.PhotoImage(Image.open(win_img_path).resize((500, 350), Image.ANTIALIAS))
        self.draw_image = ImageTk.PhotoImage(Image.open(f"{dir_path}\\assets\\chess_draw.png").resize((500, 350), Image.ANTIALIAS))

        self.auxiliary_pieces = [ImageTk.PhotoImage(fenToImage["q"].resize((int(100*(self.size/95)),int(100*(self.size/95))))), ImageTk.PhotoImage(fenToImage["b"].resize((int(100*(self.size/95)),int(100*(self.size/95))))), ImageTk.PhotoImage(fenToImage["r"].resize((int(100*(self.size/95)),int(100*(self.size/95))))), ImageTk.PhotoImage(fenToImage["n"].resize((int(100*(self.size/95)),int(100*(self.size/95))))), ImageTk.PhotoImage(fenToImage["Q"].resize((int(100*(self.size/95)),int(100*(self.size/95))))), ImageTk.PhotoImage(fenToImage["B"].resize((int(100*(self.size/95)),int(100*(self.size/95))))), ImageTk.PhotoImage(fenToImage["R"].resize((int(100*(self.size/95)),int(100*(self.size/95))))), ImageTk.PhotoImage(fenToImage["N"].resize((int(100*(self.size/95)),int(100*(self.size/95)))))]
        self.done = tk.BooleanVar(False)
        self.animations_done = True

        canvas_width = self.columns * self.size
        canvas_height = self.rows * self.size

        self.parent.geometry(f"{(self.size+30)*8}x{(self.size+30)*8}")
        self.parent.resizable(False, False)
        self.parent.attributes("-fullscreen", self.fullscreen)

        self.canvas = tk.Canvas(self.parent, borderwidth=0, highlightthickness=0,
                                width=canvas_width+1, height=canvas_height+1, background="#312E2B")
        self.canvas.pack(fill="both", expand=True)

        self.parent.update()
        self.promotion_squares_white = False
        self.promotion_squares_black = False
        self.promotion_ai = False
        self.windowWidth = self.parent.winfo_width()
        self.windowHeight = self.parent.winfo_height()
        self.offsetX = (self.windowWidth - (self.size*8)) // 2
        self.offsetY = (self.windowHeight - (self.size*8)) // 2
        self.drawBoard(self.windowWidth, self.windowHeight)

        self.parent.bind("<Configure>", self.on_resize)
        self.canvas.bind("<Button-1>", self.onClick)
        self.canvas.bind("<B1-Motion>", self.onDrag)
        self.canvas.bind("<ButtonRelease-1>", self.onRelease)
        if self.debug:
            self.parent.bind("<F8>", lambda e: print(self.toFen()))
        self.parent.bind("<F11>", self.toggle_fullscreen)
        self.parent.bind("<F5>", lambda e: self.loadBoard(start))
        self.parent.bind("<F6>", self.restore_move)
        if self.debug:
            self.parent.bind("<F7>", self.print_move_log)

        self.fromFen(start)
        self.parent.after(500, self.refresh)
    
    def mainloop(self):
        self.parent.mainloop()

    def print_move_log(self, event):
        os.system("cls")
        sorted_key_list = sort_keys(self.movelog.keys(), True)
        for element in sorted_key_list:
            print(element[:len(element)-3] + "player: " + ("BLACK" if int(element[len(element)-3]) else "WHITE") + " enpassant: " + ("TRUE" if int(element[len(element)-1]) else "FALSE"))

    def on_resize(self, event):
        self.parent.update()
        self.windowWidth = self.parent.winfo_width()
        self.windowHeight = self.parent.winfo_height()
        self.offsetX = (self.windowWidth - (self.size*8)) // 2
        self.offsetY = (self.windowHeight - (self.size*8)) // 2
        self.canvas.delete("all")
        self.drawBoard(self.parent.winfo_width(), self.parent.winfo_height())
        for key in self.board:
            self.drawpiece(self.board[key], key[0], key[1])
    
    def on_win(self):
        centreX = self.parent.winfo_width() // 2
        centreY = self.parent.winfo_height() // 2
        x0 = centreX - 300
        y0 = centreY - 260
        x1 = centreX + 300
        y1 = centreY + 260
        self.canvas.create_rectangle(x0, y0, x1, y1, fill=self.boardColor1, tags="win")
        self.canvas.create_text(centreX, centreY+130, text=f"{'White' if self.currentPlayer else 'Black'} {self.win_text}", fill="black", font=('Calibri 25 bold'), tags="win")
        self.canvas.create_image(centreX, centreY-70, image=self.win_image, tags="win")

    def on_draw(self):
        centreX = self.parent.winfo_width() // 2
        centreY = self.parent.winfo_height() // 2
        x0 = centreX - 300
        y0 = centreY - 260
        x1 = centreX + 300
        y1 = centreY + 260
        self.canvas.create_rectangle(x0, y0, x1, y1, fill=self.boardColor1, tags="draw")
        self.canvas.create_text(centreX, centreY+130, text=f"Draw!", fill="black", font=('Calibri 25 bold'), tags="draw")
        self.canvas.create_image(centreX, centreY-70, image=self.draw_image, tags="draw")

    def createPiece(self, fenChr: str) -> Piece:    
        return Piece(fenToPiece[fenChr][0], fenToPiece[fenChr][1], ImageTk.PhotoImage(fenToImage[fenChr].resize((int(100*(self.size/95)),int(100*(self.size/95))), Image.ANTIALIAS)))
    
    def toggle_fullscreen(self, event):
        self.fullscreen = not self.fullscreen
        self.parent.attributes("-fullscreen", self.fullscreen)

    def moveAI(self):
        self.canvas.unbind("<Button-1>")
        self.canvas.unbind("<B1-Motion>")
        try:
            move = self.ai([f"{dir_path}\\src\ChessAI", self.toFen()]).decode()
            if self.debug:
                print(move)
            if "O-O" in move and "h8" in move:
                move = "e8g8"
            if "O-O" in move and "h1" in move:
                move = "e1g1"
            if "O-O-O" in move and "c8" in move:
                move = "e8c8"
            if "O-O-O" in move and "c1" in move:
                move = "e1c1"
            
            oldPos = (8-int(move[1]), letterToColumn[move[0]])
            newPos = (8-int(move[3]), letterToColumn[move[2]])

            if "(promotion)" in move:
                self.promotion_ai = True
                self.placepiece(oldPos, newPos)
                self.promote_pawn_ai(newPos, move[5])
            self.selectedPiece = oldPos
            try:
                self.moves = self.generateLegalMoves(oldPos[0], oldPos[1], self.board)
            except:
                pass
            if not newPos in self.moves:
                self.moves.append(newPos)
            if not self.promotion_ai:
                self.placepiece(oldPos, newPos)
            self.promotion_ai = False
            self.moves = []
        except:
            pass
        self.currentPlayer = int(not self.currentPlayer)
        self.canvas.bind("<Button-1>", self.onClick)
        self.canvas.bind("<B1-Motion>", self.onDrag)

    def restore_move(self, event):
        if len(self.movelog.keys()) == 0:
            return

        if self.selectedPiece:
            if self.selectedPiece[0] % 2 != 0:
                    self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor1) if (8*(self.selectedPiece[0]) + self.selectedPiece[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor2)
            else:
                self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor2) if (8*(self.selectedPiece[0]) + self.selectedPiece[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor1)
            for move in self.moves:
                if move[0] % 2 != 0:
                    self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor1) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor2)
                else:
                    self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor2) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor1)

        last_move_key = sort_keys(self.movelog.keys())[0]
        last_move_key_space = last_move_key.find(" ")
        last_move_piece = self.movelog[last_move_key]

        newPos = (8-int(last_move_key[last_move_key_space+2]), letterToColumn[last_move_key[last_move_key_space+1]]) # "1 c1c3"
        oldPos = (8-int(last_move_key[last_move_key_space+4]), letterToColumn[last_move_key[last_move_key_space+3]])

        self.board[newPos] = self.board[oldPos]
        self.board.pop(oldPos)

        x0 = (oldPos[1] * self.size) + int(self.size/2) + self.offsetX
        y0 = (oldPos[0] * self.size) + int(self.size/2) + self.offsetY
        x1 = (newPos[1] * self.size) + int(self.size/2) + self.offsetX
        y1 = (newPos[0] * self.size) + int(self.size/2) + self.offsetY

        for i in range(self.animationFps+1):
            self.canvas.coords(self.board[newPos].uniqueCode, x0+((x1-x0)/self.animationFps)*i, y0+((y1-y0)/self.animationFps)*i)
            self.canvas.update()

        if len(self.popped_enpassant) > 0:
            self.enpassant.append(self.popped_enpassant.pop())
        if int(last_move_key[len(last_move_key)-1]):
            oldPos = (8-int(last_move_key[len(last_move_key)-5]), letterToColumn[last_move_key[len(last_move_key)-6]])
        if last_move_piece:
            self.board[oldPos] = last_move_piece
            self.drawpiece(self.board[oldPos], oldPos[0], oldPos[1])
        
        self.movelog.pop(last_move_key)
        if (newPos == (7, 4) and oldPos == (7, 2)):
            self.restore_move(event)
            self.whiteQueensideCastle = True
            self.whiteKingsideCastle = True
        if (newPos == (7, 4) and oldPos == (7, 6)):
            self.restore_move(event)
            self.whiteQueensideCastle = True
            self.whiteKingsideCastle = True
        if (newPos == (0, 4) and oldPos == (0, 2)):
            self.restore_move(event)
            self.blackQueensideCastle = True
            self.blackKingsideCastle = True
        if (newPos == (0, 4) and oldPos == (0, 6)):
            self.restore_move(event)
            self.blackQueensideCastle = True
            self.blackKingsideCastle = True
        self.currentPlayer = int(last_move_key[len(last_move_key)-3])

    def addmovetorestore(self, oldPos: tuple, newPos: tuple, eatenPiece: Piece, player: int, enpassant: bool, enpassant_capture_square: tuple = None):
        if enpassant_capture_square:
            new_move_key = f"{len(self.movelog.keys())+1} {columnToLetter[oldPos[1]]}{8-oldPos[0]}{columnToLetter[newPos[1]]}{8-newPos[0]} {columnToLetter[enpassant_capture_square[1]]}{8-enpassant_capture_square[0]} {player} {1 if enpassant else 0}"
        else:
            new_move_key = f"{len(self.movelog.keys())+1} {columnToLetter[oldPos[1]]}{8-oldPos[0]}{columnToLetter[newPos[1]]}{8-newPos[0]} {player} {1 if enpassant else 0}"

        self.movelog[new_move_key] = eatenPiece
        
    def refresh(self):
        if len(self.generateAllLegalMoves(self.board, self.currentPlayer)) == 0 and self.in_check(self.board, self.currentPlayer):
            self.on_win()
        elif len(self.generateAllLegalMoves(self.board, self.currentPlayer)) == 0:
            self.on_draw()

        if self.useAI and self.currentPlayer != self.startPlayer and self.animations_done:
            self.moveAI()

        self.parent.after(500, self.refresh)

    def findPiece(self, uniqueCode: str):
        for key in self.board:
            if self.board[key].uniqueCode == uniqueCode:
                return key

    def drawBoard(self, frameWidth, frameHeight):
        color = self.boardColor2

        for row in range(8):
            color = self.boardColor1 if color == self.boardColor2 else self.boardColor2
            for col in range(8):
                x1 = self.offsetX + (col * self.size) 
                y1 = self.offsetY + (row * self.size) 
                x2 = x1 + self.size
                y2 = y1 + self.size
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="white", fill=color, tags=(f"{row}, {col}","square"))
                color = self.boardColor1 if color == self.boardColor2 else self.boardColor2
        
        if self.promotion_squares_white:
            for i in range(4):
                x1 = self.offsetX + (i * self.size)
                x0 = self.offsetX + (i * self.size) + (self.size//2)
                y1 = self.offsetY - self.size - 5
                y0 = y1 + (self.size//2)
                x2 = x1 + self.size
                y2 = y1 + self.size
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="white", fill=self.boardColor1, tags=(f"{i}", "promotion", "square"))
                self.canvas.create_image(x0, y0, image=self.auxiliary_pieces[4+i], tags=("promotion", "pieces"), anchor="c")
                self.canvas.tag_raise("pieces")
                self.canvas.tag_lower("squares")
        if self.promotion_squares_black:
            for i in range(4):
                x1 = self.offsetX + (i * self.size)
                x0 = self.offsetX + (i * self.size) + (self.size//2)
                y1 = self.offsetY + 5 + (8 * self.size)
                y0 = y1 + (self.size//2)
                x2 = x1 + self.size
                y2 = y1 + self.size
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="white", fill=self.boardColor1, tags=(f"{i}", "promotion", "square"))
                self.canvas.create_image(x0, y0, image=self.auxiliary_pieces[i], tags=("promotion", "pieces"), anchor="c")
                self.canvas.tag_raise("pieces")
                self.canvas.tag_lower("squares")

        self.canvas.tag_lower("square")
        
    def loadBoard(self, fen: str = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq"):
        self.promotion_squares_white = False
        self.promotion_squares_black = False
        if self.selectedPiece:
            if self.selectedPiece[0] % 2 != 0:
                    self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor1) if (8*(self.selectedPiece[0]) + self.selectedPiece[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor2)
            else:
                self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor2) if (8*(self.selectedPiece[0]) + self.selectedPiece[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor1)
            for move in self.moves:
                if move[0] % 2 != 0:
                    self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor1) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor2)
                else:
                    self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor2) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor1)
        self.board.clear()
        self.oldBoard.clear()
        self.moves.clear()
        self.enpassant.clear()
        self.selectedPiece = ()
        self.movelog = {}
        self.move_log_r = ""
        self.currentPlayer = self.startPlayer

        self.canvas.delete("piece")
        self.canvas.delete("win")
        self.canvas.delete("draw")
        self.fromFen(fen)


    # Load a board from a FEN notation
    def fromFen(self, fen: str):
        self.board = {}
        row = 0
        col = 0
        spacePos = fen.find(" ")
        if fen[spacePos+1] == "b":
            if self.currentPlayer == -1:
                self.currentPlayer = BLACK
        else:
            if self.currentPlayer == -1:
                self.currentPlayer = WHITE
        for i in range(len(fen)):
            if fen[i] == "/":
                row += 1
                col = 0
            elif fen[i].isdigit():
                col += int(fen[i])
            if fen[i] in fenToPiece and i < spacePos:
                self.addpiece(self.createPiece(fen[i]), row, col)
                col += 1
            if fen[i] == "K" and i > spacePos:
                self.whiteKingsideCastle = True
            if fen[i] == "Q" and i > spacePos:
                self.whiteQueensideCastle = True
            if fen[i] == "k" and i > spacePos:
                self.blackKingsideCastle = True
            if fen[i] == "q" and i > spacePos:
                self.blackQueensideCastle = True
            if i > spacePos + 2 and fen[i] in letterToColumn:
                offset = 1 if int(fen[i+1]) == 6 else -1
                self.enpassant.append(self.board[(8-int(fen[i+1])+offset, letterToColumn[fen[i]])].uniqueCode)
        for key in self.board:
            self.drawpiece(self.board[key], key[0], key[1])
        self.oldBoard = self.board.copy()
        self.canvas.tag_raise("piece")
        self.canvas.tag_lower("square")

    # Current board to FEN notation
    def toFen(self):
        fenStr = ""
        spaces = 0
        for row in range(8):
            for col in range(8):
                if (row, col) in self.board:
                    fenStr += f"{spaces if spaces > 0 else ''}{self.board[(row, col)].fentype}"
                    spaces = 0
                else:
                    spaces += 1
            fenStr += f"{spaces if spaces > 0 else ''}/"
            spaces = 0
        fenStr += " b" if self.currentPlayer else " w"
        fenStr += " "
        if self.whiteKingsideCastle:
            fenStr += "K"
        if self.whiteQueensideCastle:
            fenStr += "Q"
        if self.blackKingsideCastle:
            fenStr += "k"
        if self.blackQueensideCastle:
            fenStr += "q"
        fenStr += " "

        try:
            for elem in self.enpassant:
                fenStr += f"{columnToLetter[self.findPiece(elem)[0]]}{self.findPiece(elem)[1]+1}"
                fenStr += " "
        except:
            pass
        return fenStr

    # Add a piece to self.board and assign each piece a unique code to be able to distinguish between same type&color pieces
    def addpiece(self, piece: Piece, row: int, column: int):
        numberOfIstances = 0
        for key in self.board:
            if self.board[key].fentype == piece.fentype and self.board[key].color == piece.color:
                numberOfIstances += 1

        piece.uniqueCode = f"{numberOfIstances}{piece.fentype}"
        self.board[(row, column)] = piece
        

    # During each frame, redraw each piece
    def drawpiece(self, piece: Piece, row: int, column: int):
        offsetX = (self.windowWidth - (self.size*8)) // 2
        offsetY = (self.windowHeight - (self.size*8)) // 2
        x0 = (column * self.size) + int(self.size/2) + offsetX
        y0 = (row * self.size) + int(self.size/2) + offsetY
        self.canvas.create_image(x0, y0, image=piece.image, tags=(piece.uniqueCode, "piece"), anchor="c")
        
    # Given an old position and a new one, place the piece in oldPos in newPos
    def placepiece(self, oldPos: tuple, newPos: tuple):

        # TODO 1: add images of the eaten piece under or over the board

        pawn_promotion = False
        pawn_enpassant = False
        if self.board[oldPos].fentype == "P" and newPos[0] == 0 or (self.board[oldPos].fentype == "p" and newPos[0] == 7):
            pawn_promotion = True 
        if self.board[oldPos].uniqueCode == "0r":
            self.blackQueensideCastle = False
        elif self.board[oldPos].uniqueCode == "1r":
            self.blackKingsideCastle = False
        elif self.board[oldPos].uniqueCode == "0R":
            self.whiteQueensideCastle = False
        elif self.board[oldPos].uniqueCode == "1R":
            self.whiteKingsideCastle = False
        if self.board[oldPos].fentype == "K":
            if newPos == (7, 2) and self.whiteQueensideCastle:
                self.placepiece((7, 0), (7, 3))
            elif newPos == (7, 6) and self.whiteKingsideCastle:
                self.placepiece((7, 7), (7, 5))
            self.whiteKingsideCastle = False
            self.whiteQueensideCastle = False
        elif self.board[oldPos].fentype == "k":
            if newPos == (0, 2) and self.blackQueensideCastle:
                self.placepiece((0, 0), (0, 3))
            elif newPos == (0, 6) and self.blackKingsideCastle:
                self.placepiece((0, 7), (0, 5))
            self.blackKingsideCastle = False
            self.blackQueensideCastle = False
        offset = 1 if oldPos in self.board and self.board[oldPos].color else -1
        try:
            for i in [1, -1]:
                if (oldPos[0], oldPos[1]+i) in self.board and self.board[oldPos].type == PAWN and newPos == (oldPos[0]+offset, oldPos[1]+i) and (oldPos[0], oldPos[1]+i) in self.board and self.board[(oldPos[0], oldPos[1]+i)].uniqueCode in self.enpassant and self.board[oldPos].color != self.board[(oldPos[0], oldPos[1]+i)].color:
                    self.canvas.delete(self.board[(oldPos[0], oldPos[1]+i)].uniqueCode)
                    self.addmovetorestore(oldPos, newPos, self.board[(oldPos[0], oldPos[1]+i)], self.currentPlayer, True, (oldPos[0], oldPos[1]+i))
                    pawn_enpassant = True
                    self.board.pop((oldPos[0], oldPos[1]+i))
        except:
            pass
        
        if len(self.enpassant) > 0:
            self.popped_enpassant.append(self.enpassant.pop())
        try:
            if self.board[oldPos].type == PAWN and newPos[0] == oldPos[0]+2*offset:
                self.enpassant.append(self.board[oldPos].uniqueCode)
        except:
            pass
        if newPos in self.board:
            self.canvas.delete(self.board[newPos].uniqueCode)
            captured_piece = self.board[newPos]
        else:
            captured_piece = None
        
        tempSquare = self.board[oldPos]
        self.board.pop(oldPos)
        self.board[newPos] = tempSquare

        if not pawn_enpassant:
            self.addmovetorestore(oldPos, newPos, captured_piece, self.currentPlayer, False)
                
        x0 = (oldPos[1] * self.size) + int(self.size/2) + self.offsetX
        y0 = (oldPos[0] * self.size) + int(self.size/2) + self.offsetY
        x1 = (newPos[1] * self.size) + int(self.size/2) + self.offsetX
        y1 = (newPos[0] * self.size) + int(self.size/2) + self.offsetY
        
        for i in range(self.animationFps+1):
            self.canvas.coords(self.board[newPos].uniqueCode, x0+((x1-x0)/self.animationFps)*i, y0+((y1-y0)/self.animationFps)*i)
            self.canvas.update()
        if pawn_promotion and not self.promotion_ai:
            self.promote_pawn(newPos, self.currentPlayer) 


    # Returns the clicked-on tile's coordinates 
    def getMouseClickPos(self, event) -> tuple:
        try:
            item_below = self.canvas.find_overlapping(event.x,event.y,event.x,event.y)[0]
            
            return (int(self.canvas.itemcget(item_below, "tags")[1]), int(self.canvas.itemcget(item_below, "tags")[4]))
        except:
            return(8,8)

    def onReleaseMove(self, startCoords: tuple, dropCoords: tuple):

        pawn_promotion = False
        pawn_enpassant = False
        if (self.board[startCoords].fentype == "P" and dropCoords[0] == 0) or (self.board[startCoords].fentype == "p" and dropCoords[0] == 7):
            pawn_promotion = True
        if self.board[startCoords].uniqueCode == "0r":
            self.blackQueensideCastle = False
        elif self.board[startCoords].uniqueCode == "1r":
            self.blackKingsideCastle = False
        elif self.board[startCoords].uniqueCode == "0R":
            self.whiteQueensideCastle = False
        elif self.board[startCoords].uniqueCode == "1R":
            self.whiteKingsideCastle = False
        if self.board[startCoords].fentype == "K":
            if dropCoords == (7, 2) and self.whiteQueensideCastle:
                self.placepiece((7, 0), (7, 3))
            elif dropCoords == (7, 6) and self.whiteKingsideCastle:
                self.placepiece((7, 7), (7, 5))
            self.whiteKingsideCastle = False
            self.whiteQueensideCastle = False
        elif self.board[startCoords].fentype == "k":
            if dropCoords == (0, 2) and self.blackQueensideCastle:
                self.placepiece((0, 0), (0, 3))
            elif dropCoords == (0, 6) and self.blackKingsideCastle:
                self.placepiece((0, 7), (0, 5))
            self.blackKingsideCastle = False
            self.blackQueensideCastle = False
        offset = 1 if self.board[startCoords].color else -1
        try:
            for i in [1, -1]:
                if (startCoords[0], startCoords[1]+i) in self.board and self.board[startCoords].type == PAWN and dropCoords == (startCoords[0]+offset, startCoords[1]+i) and (startCoords[0], startCoords[1]+i) in self.board and self.board[(startCoords[0], startCoords[1]+i)].uniqueCode in self.enpassant and self.board[startCoords].color != self.board[(startCoords[0], startCoords[1]+i)].color:
                    self.canvas.delete(self.board[(startCoords[0], startCoords[1]+i)].uniqueCode)
                    self.addmovetorestore(startCoords, dropCoords, self.board[(startCoords[0], startCoords[1]+i)], self.currentPlayer, True, (startCoords[0], startCoords[1]+i))
                    pawn_enpassant = True
                    self.board.pop((startCoords[0], startCoords[1]+i))
        except:
            pass

        if len(self.enpassant) > 0:
            self.popped_enpassant.append(self.enpassant.pop())
        try:
            if self.board[startCoords].type == PAWN and dropCoords[0] == startCoords[0]+2*offset:
                self.enpassant.append(self.board[startCoords].uniqueCode)
        except:
            pass
        if dropCoords in self.board:
            self.canvas.delete(self.board[dropCoords].uniqueCode)
            captured_piece = self.board[dropCoords]
        else:
            captured_piece = None

        if not pawn_enpassant:
            self.addmovetorestore(startCoords, dropCoords, captured_piece, self.currentPlayer, False)

        x1 = (dropCoords[1] * self.size) + int(self.size/2) + self.offsetX
        y1 = (dropCoords[0] * self.size) + int(self.size/2) + self.offsetY
        self.canvas.delete(self.board[dropCoords].uniqueCode) if dropCoords in self.board else None
        self.board[dropCoords] = self.board[startCoords]
        self.board.pop(startCoords)
        self.canvas.coords(self.board[dropCoords].uniqueCode, x1, y1)
        if startCoords[0] % 2 != 0:
            self.canvas.itemconfig(f"{startCoords[0]}, {startCoords[1]}", fill=self.boardColor1) if (8*(startCoords[0]) + startCoords[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{startCoords[0]}, {startCoords[1]}", fill=self.boardColor2)
        else:
            self.canvas.itemconfig(f"{startCoords[0]}, {startCoords[1]}", fill=self.boardColor2) if (8*(startCoords[0]) + startCoords[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{startCoords[0]}, {startCoords[1]}", fill=self.boardColor1)
        for move in self.moves:
            if move[0] % 2 != 0:
                self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor1) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor2)
            else:
                self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor2) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor1)
        if pawn_promotion:
            self.promote_pawn(dropCoords, self.currentPlayer)
        

    def onRelease(self, event):
        self.animations_done = False
        if self.selectedPiece:
            try:
                dropCoords = self.getMouseClickPos(event)
                if dropCoords in self.moves and self.selectedPiece in self.board:
                    self.onReleaseMove(self.selectedPiece, dropCoords)
                    self.moves = []
                    self.selectedPiece = ()
                    self.currentPlayer = int(not self.currentPlayer)
                elif self.selectedPiece in self.board:
                    x1 = (self.selectedPiece[1] * self.size) + int(self.size/2) + self.offsetX
                    y1 = (self.selectedPiece[0] * self.size) + int(self.size/2) + self.offsetY
                    self.canvas.coords(self.board[self.selectedPiece].uniqueCode, x1, y1)
                self.animations_done = True 
            except Exception as e:
                x1 = (self.selectedPiece[1] * self.size) + int(self.size/2) + self.offsetX
                y1 = (self.selectedPiece[0] * self.size) + int(self.size/2) + self.offsetY
                self.canvas.coords(self.board[self.selectedPiece].uniqueCode, x1, y1)
                self.animations_done = True
        


    def onDrag(self, event):
        try:
            if self.selectedPiece:
                self.canvas.coords(self.board[(self.selectedPiece[0], self.selectedPiece[1])].uniqueCode, event.x, event.y)
        except:
            pass

    # What to do on clicks        
    def onClick(self, event):
        if not self.selectedPiece:
            clickPos = self.getMouseClickPos(event)

            if clickPos in self.board and self.board[clickPos].color == self.currentPlayer:
                self.canvas.itemconfig(f"{clickPos[0]}, {clickPos[1]}", fill="#f7f76a")
                self.moves = self.generateLegalMoves(clickPos[0], clickPos[1], self.board)
                for move in self.moves:
                    self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill="#edda95")
                self.selectedPiece = clickPos
        else:
            clickPos = self.getMouseClickPos(event)

            # If the user clicks on an empty square
            if clickPos not in self.moves and clickPos not in self.board:
                if self.selectedPiece[0] % 2 != 0:
                    self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor1) if (8*(self.selectedPiece[0]) + self.selectedPiece[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor2)
                else:
                    self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor2) if (8*(self.selectedPiece[0]) + self.selectedPiece[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor1)
                for move in self.moves:
                    if move[0] % 2 != 0:
                        self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor1) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor2)
                    else:
                        self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor2) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor1)
                self.selectedPiece = ()

            # If the user clicks on another piece
            elif clickPos not in self.moves and clickPos in self.board:
                if self.selectedPiece[0] % 2 != 0:
                    self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor1) if (8*(self.selectedPiece[0]) + self.selectedPiece[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor2)
                else:
                    self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor2) if (8*(self.selectedPiece[0]) + self.selectedPiece[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor1)
                self.selectedPiece = clickPos
                for move in self.moves:
                    if move[0] % 2 != 0:
                        self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor1) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor2)
                    else:
                        self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor2) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor1)
                if self.board[clickPos].color == self.currentPlayer:
                    self.moves = self.generateLegalMoves(clickPos[0], clickPos[1], self.board)
                    self.canvas.itemconfig(f"{clickPos[0]}, {clickPos[1]}", fill="#f7f76a")
                    for move in self.moves:
                        self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill="#edda95")
                else:
                    self.moves = []
                    self.selectedPiece = ()

            # If the user clicks on a move
            elif clickPos in self.moves:
                try:
                    self.canvas.unbind("<ButtonRelease-1>")
                    if self.selectedPiece[0] % 2 != 0:
                        self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor1) if (8*(self.selectedPiece[0]) + self.selectedPiece[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor2)
                    else:
                        self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor2) if (8*(self.selectedPiece[0]) + self.selectedPiece[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor1)
            
                    for move in self.moves:
                        if move[0] % 2 != 0:
                            self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor1) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor2)
                        else:
                            self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor2) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor1)
                    self.placepiece(self.selectedPiece, clickPos)
                    self.currentPlayer = int(not self.currentPlayer)
                    self.moves = []
                    self.selectedPiece = ()
                    self.canvas.bind("<ButtonRelease-1>", self.onRelease)
                except:
                    self.canvas.bind("<ButtonRelease-1>", self.onRelease)
                    return

    def generateAllLegalMoves(self, board: dict, side_color: int):
        allMoves = []
        for key in board:
            moves = []
            if board[key].type == PAWN and board[key].color == side_color:
                moves = self.pawnMoves(key[0], key[1], board)
            if board[key].type == KNIGHT and board[key].color == side_color:
                moves = self.knightMoves(key[0], key[1], board)
            if board[key].type == BISHOP and board[key].color == side_color:
                moves = self.bishopMoves(key[0], key[1], board)
            if board[key].type == ROOK and board[key].color == side_color:
                moves = self.rookMoves(key[0], key[1], board)
            if board[key].type == QUEEN and board[key].color == side_color:
                moves = self.queenMoves(key[0], key[1], board)
            if board[key].type == KING and board[key].color == side_color:
                moves = self.kingMoves(key[0], key[1], board)
            
            for move in moves:
                copy_board = board.copy()
                copy_board[move] = copy_board[key]
                copy_board.pop(key)
                if not self.in_check(copy_board, side_color) and 0 < move[0] < 8 and 0 < move[1] < 8:
                    allMoves.append(move)

        return allMoves
    
    def generateAllPseudoMoves(self, board: dict, side_color: int):
        allMoves = []

        for key in board:
            moves = []
            if board[key].type == PAWN and board[key].color == side_color:
                moves = self.pawnMoves(key[0], key[1], board)
            if board[key].type == KNIGHT and board[key].color == side_color:
                moves = self.knightMoves(key[0], key[1], board)
            if board[key].type == BISHOP and board[key].color == side_color:
                moves = self.bishopMoves(key[0], key[1], board)
            if board[key].type == ROOK and board[key].color == side_color:
                moves = self.rookMoves(key[0], key[1], board)
            if board[key].type == QUEEN and board[key].color == side_color:
                moves = self.queenMoves(key[0], key[1], board)
                
            if board[key].type == KING and board[key].color == side_color:
                moves = self.kingMoves(key[0], key[1], board)
            
            allMoves += moves

        return allMoves


    def in_check(self, board: dict, color: int) -> bool:
        kingPos = ()

        for key in board:
            if board[key].type == KING and board[key].color == color:
                kingPos = key

        all_opponent_moves = self.generateAllPseudoMoves(board, int(not color))
    
        if kingPos in all_opponent_moves:
            return True
        
        return False                 

    def generateLegalMoves(self, row: int, col: int, board: dict) -> list:
        moves = []

        if not (row, col) in self.board:
            return moves
                 
        pieceType = board[(row, col)].type
        pieceColor = board[(row, col)].color

        if pieceType == PAWN:
            moves = self.pawnMoves(row, col, board)

        if pieceType == KNIGHT:
            moves = self.knightMoves(row, col, board)

        if pieceType == ROOK:
            moves = self.rookMoves(row, col, board)

        if pieceType == BISHOP:
            moves = self.bishopMoves(row, col, board)

        if pieceType == QUEEN:
            moves = self.queenMoves(row, col, board)
        
        if pieceType == KING:
            moves = self.kingMoves(row, col, board)

        
        finalMoves = []
        for move in moves:
            tempBoard = self.board.copy()
            tempPiece = tempBoard[(row, col)]
            tempBoard[move] = tempPiece
            tempBoard.pop((row, col))
            if not self.in_check(tempBoard, pieceColor):
                finalMoves.append(move)
        
        return finalMoves
    
    def on_promotion_click(self, event, pawnPos: tuple, color: int):
        try:
            clicked_square = int(self.canvas.itemcget(self.canvas.find_overlapping(event.x,event.y,event.x,event.y)[0], "tags")[0])
        
            if clicked_square == 0:
                self.canvas.delete(self.board[pawnPos].uniqueCode)
                self.board.pop(pawnPos)
                self.addpiece(self.createPiece("q") if color else self.createPiece("Q"), pawnPos[0], pawnPos[1])
                self.drawpiece(self.board[pawnPos], pawnPos[0], pawnPos[1])
                self.canvas.delete("promotion")
                self.done.set(True)
                self.promotion_squares_white=False
                self.promotion_squares_black=False
            if clicked_square == 1:
                self.canvas.delete(self.board[pawnPos].uniqueCode)
                self.board.pop(pawnPos)
                self.addpiece(self.createPiece("b") if color else self.createPiece("B"), pawnPos[0], pawnPos[1])
                self.drawpiece(self.board[pawnPos], pawnPos[0], pawnPos[1])
                self.canvas.delete("promotion")
                self.done.set(True)
                self.promotion_squares_white=False
                self.promotion_squares_black=False
            if clicked_square == 2:
                self.canvas.delete(self.board[pawnPos].uniqueCode)
                self.board.pop(pawnPos)
                self.addpiece(self.createPiece("r") if color else self.createPiece("R"), pawnPos[0], pawnPos[1])
                self.drawpiece(self.board[pawnPos], pawnPos[0], pawnPos[1])
                self.canvas.delete("promotion")
                self.done.set(True)
                self.promotion_squares_white=False
                self.promotion_squares_black=False
            if clicked_square == 3:
                self.canvas.delete(self.board[pawnPos].uniqueCode)
                self.board.pop(pawnPos)
                self.addpiece(self.createPiece("n") if color else self.createPiece("N"), pawnPos[0], pawnPos[1])
                self.drawpiece(self.board[pawnPos], pawnPos[0], pawnPos[1])
                self.canvas.delete("promotion")
                self.done.set(True)
                self.promotion_squares_white=False
                self.promotion_squares_black=False
        except:
            pass

    def promote_pawn(self, pawnPos: tuple, color: int):
        
        self.canvas.unbind("<Button-1>")
        self.canvas.bind("<Button-1>", lambda event: self.on_promotion_click(event, pawnPos, color))
        
        if color == WHITE:
            self.promotion_squares_white = True
            for i in range(4):
                x1 = self.offsetX + (i * self.size)
                x0 = self.offsetX + (i * self.size) + (self.size//2)
                y1 = self.offsetY - self.size - 5
                y0 = y1 + (self.size//2)
                x2 = x1 + self.size
                y2 = y1 + self.size
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="white", fill=self.boardColor1, tags=(f"{i}", "promotion", "square"))
                self.canvas.create_image(x0, y0, image=self.auxiliary_pieces[4+i], tags=("promotion", "pieces"), anchor="c")
                self.canvas.tag_raise("pieces")
                self.canvas.tag_lower("squares")
        if color == BLACK:
            self.promotion_squares_black = True
            for i in range(4):
                x1 = self.offsetX + (i * self.size)
                x0 = self.offsetX + (i * self.size) + (self.size//2)
                y1 = self.offsetY + 5 + (8 * self.size)
                y0 = y1 + (self.size//2)
                x2 = x1 + self.size
                y2 = y1 + self.size
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="white", fill=self.boardColor1, tags=(f"{i}", "promotion", "square"))
                self.canvas.create_image(x0, y0, image=self.auxiliary_pieces[i], tags=("promotion", "pieces"), anchor="c")
                self.canvas.tag_raise("pieces")
                self.canvas.tag_lower("squares")

        self.canvas.wait_variable(self.done)
        self.canvas.bind("<Button-1>", self.onClick)

    def promote_pawn_ai(self, pawnPos: tuple, promotion: str):
        self.canvas.delete(self.board[pawnPos].uniqueCode)
        self.addpiece(self.createPiece(promotion if not self.currentPlayer else promotion.lower()), pawnPos[0], pawnPos[1])
        self.drawpiece(self.board[pawnPos], pawnPos[0], pawnPos[1])

    def pawnMoves(self, row: int, col: int, board: dict) -> list:
        moves = []

        pieceColor = board[(row, col)].color

        offset = -1 if pieceColor == WHITE else 1

        moves = [(row+offset, col)]
        if (row == 6 or row == 1) and not (row+2*offset, col) in board and not (row+offset, col) in board:
            moves.append((row+2*offset, col))
        for i in [1, -1]:
            if (row, col+i) in board and board[(row, col+i)].type == PAWN and board[(row, col+i)].color != pieceColor and board[(row, col+i)].uniqueCode in self.enpassant:
                moves.append((row+offset, col+i))
            if (row+offset, col+i) in board and board[(row+offset, col+i)].color != pieceColor:
                moves.append((row+offset, col+i))
        if (row+offset, col) in board:
            moves.remove((row+offset, col))
            
        return moves

    def knightMoves(self, row: int, col: int, board: dict) -> list:
        moves = []

        pieceColor = board[(row, col)].color

        for possibleMove in [(row+1, col+2), (row+1, col-2), (row+2, col+1), (row+2, col-1), (row-1, col+2), (row-1, col-2), (row-2, col+1), (row-2, col-1)]:
            if not (possibleMove in board and board[possibleMove].color == pieceColor) and 0 <= possibleMove[0] <= 8 and 0 <= possibleMove[1] <= 8:
                moves.append(possibleMove)
                
        return moves
    
    def rookMoves(self, row: int, col: int, board: dict) -> list:
        moves = []

        pieceColor = board[(row, col)].color

        for i in range(1, 8-row):
            if (row+i, col) in board:
                if board[(row+i, col)].color != pieceColor:
                    moves.append((row+i, col))
                    break
                break
            moves.append((row+i, col))
        for i in range(row-1, -1, -1):
            if (i, col) in board:
                if board[(i, col)].color != pieceColor:
                    moves.append((i, col))
                    break
                break
            moves.append((i, col))
        for i in range(1, 8-col):
            if (row, col+i) in board:
                if board[(row, col+i)].color != pieceColor:
                    moves.append((row, col+i))
                    break
                break
            moves.append((row, col+i))
        for i in range(col-1, -1, -1):
            if (row, i) in board:
                if board[(row, i)].color != pieceColor:
                    moves.append((row, i))
                    break
                break
            moves.append((row, i))
        
        return moves

    def bishopMoves(self, row: int, col: int, board: dict) -> list:
        moves = []

        pieceColor = board[(row, col)].color

        for i in range(1, min(8-row, 8-col)):
            if (row+i, col+i) in board:
                if board[(row+i, col+i)].color != pieceColor:
                    moves.append((row+i, col+i))
                    break
                break
            moves.append((row+i, col+i))
        for i in range(1, min(row, col)+1):
            if (row-i, col-i) in board:
                if board[(row-i, col-i)].color != pieceColor:
                    moves.append((row-i, col-i))
                    break
                break
            moves.append((row-i, col-i))
        for i in range(1, min(row+1, 8-col)):
            if (row-i, col+i) in board:
                if board[(row-i, col+i)].color != pieceColor:
                    moves.append((row-i, col+i))
                    break
                break
            moves.append((row-i, col+i))
        for i in range(1, min(8-row, col+1)):
            if (row+i, col-i) in board:
                if board[(row+i, col-i)].color != pieceColor:
                    moves.append((row+i, col-i))
                    break
                break
            moves.append((row+i, col-i))
        return moves

    def queenMoves(self, row: int, col: int, board: dict) -> list:
        moves = []

        pieceColor = board[(row, col)].color

        for i in range(1, min(8-row, 8-col)):
            if (row+i, col+i) in board:
                if board[(row+i, col+i)].color != pieceColor:
                    moves.append((row+i, col+i))
                    break
                break
            moves.append((row+i, col+i))
        for i in range(1, min(row, col)+1):
            if (row-i, col-i) in board:
                if board[(row-i, col-i)].color != pieceColor:
                    moves.append((row-i, col-i))
                    break
                break
            moves.append((row-i, col-i))
        for i in range(1, min(row+1, 8-col)):
            if (row-i, col+i) in board:
                if board[(row-i, col+i)].color != pieceColor:
                    moves.append((row-i, col+i))
                    break
                break
            moves.append((row-i, col+i))
        for i in range(1, min(8-row, col+1)):
            if (row+i, col-i) in board:
                if board[(row+i, col-i)].color != pieceColor:
                    moves.append((row+i, col-i))
                    break
                break
            moves.append((row+i, col-i))
        for i in range(1, 8-row):
            if (row+i, col) in board:
                if board[(row+i, col)].color != pieceColor:
                    moves.append((row+i, col))
                    break
                break
            moves.append((row+i, col))
        for i in range(row-1, -1, -1):
            if (i, col) in board:
                if board[(i, col)].color != pieceColor:
                    moves.append((i, col))
                    break
                break
            moves.append((i, col))
        for i in range(1, 8-col):
            if (row, col+i) in board:
                if board[(row, col+i)].color != pieceColor:
                    moves.append((row, col+i))
                    break
                break
            moves.append((row, col+i))
        for i in range(col-1, -1, -1):
            if (row, i) in board:
                if board[(row, i)].color != pieceColor:
                    moves.append((row, i))
                    break
                break
            moves.append((row, i))
    
        return moves

    def kingMoves(self, row: int, col: int, board: dict) -> list:
        moves = []

        for move in [(row+1, col), (row-1, col), (row+1, col+1), (row-1, col-1), (row+1, col-1), (row-1, col+1), (row, col+1), (row, col-1)]:
            if move in board and board[move].color != board[(row, col)].color:
                moves.append(move)
            if move not in board:
                moves.append(move)
            
        
        for key in board:
            if board[key].fentype == "r" and (row, col) == (0, 4) and board[key].color == self.currentPlayer and key in [(row,0), (row,7)]:
                blocked = False
                offset = -1 if key[1] < 4 else 1
                for element in board:
                    blocked = 0 < element[1] < 4 and element[0] == row if key[1] == 0 else 4 < element[1] < key[1] and element[0] == row
                    if blocked:
                        break
                if key == (row, 0) and not self.blackQueensideCastle:
                    blocked = True
                elif key == (row,7) and not self.blackKingsideCastle:
                    blocked = True
                if not blocked and (row, 4 + offset) in moves:
                    moves.append((row, 4 + 2*offset))
            if board[key].fentype == "R" and (row, col) == (7, 4) and board[key].color == self.currentPlayer and key in [(row,0), (row,7)]:
                blocked = False
                offset = -1 if key[1] < 4 else 1
                for element in board:
                    blocked = 0 < element[1] < 4 and element[0] == row if key[1] == 0 else 4 < element[1] < key[1] and element[0] == row
                    if blocked:
                        break
                if key == (row, 0) and not self.whiteQueensideCastle:
                    blocked = True
                elif key == (row,7) and not self.whiteKingsideCastle:
                    blocked = True
                if not blocked and (row, 4 + offset) in moves:
                    moves.append((row, 4 + 2*offset))
        
        return moves
                


if __name__ == "__main__":
    board = Board(debug = True)
    board.mainloop()
