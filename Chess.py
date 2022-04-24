import tkinter as tk
import time
from PIL import Image, ImageTk

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
    "k" : Image.open("bking.png").resize((100,100), Image.ANTIALIAS),
    "q" : Image.open("bqueen.png").resize((100,100), Image.ANTIALIAS),
    "b" : Image.open("bbishop.png").resize((100,100), Image.ANTIALIAS),
    "n" : Image.open("bhorse.png").resize((100,100), Image.ANTIALIAS),
    "r" : Image.open("brook.png").resize((100,100), Image.ANTIALIAS),
    "p" : Image.open("bpawn.png").resize((100,100), Image.ANTIALIAS),
    "K" : Image.open("wking.png").resize((100,100), Image.ANTIALIAS),
    "Q" : Image.open("wqueen.png").resize((100,100), Image.ANTIALIAS),
    "B" : Image.open("wbishop.png").resize((100,100), Image.ANTIALIAS),
    "N" : Image.open("whorse.png").resize((100,100), Image.ANTIALIAS),
    "R" : Image.open("wrook.png").resize((100,100), Image.ANTIALIAS),
    "P" : Image.open("wpawn.png").resize((100,100), Image.ANTIALIAS),
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

class Piece:
    def __init__(self, pieceType: int, color: int):
        self.type = pieceToFen[(pieceType, color)]
        self.image = ImageTk.PhotoImage(fenToImage[self.type])
        self.color = color
        self.uniqueCode = ""

def createPiece(fenChr: str) -> Piece:
    return Piece(fenToPiece[fenChr][0], fenToPiece[fenChr][1])

def sleep(ms: float):
    timer = time.time()
    while timer + ms > time.time(): #wait is your sleep time
        pass
    

class GameBoard(tk.Frame):
    def __init__(self, parent, rows=8, columns=8, color1="#EEEED2", color2="#769656", start: str = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"):


        self.rows = rows
        self.columns = columns
        self.size = 93
        self.boardColor1 = color1
        self.boardColor2 = color2
        self.animationFps = 144
        self.currentPlayer = WHITE
        self.selectedPiece = ()
        self.whiteKingsideCastle = True
        self.whiteQueensideCastle = True
        self.blackKingsideCastle = True
        self.blackQueensideCastle = True
        self.moves = []

        self.board = {}

        canvas_width = columns * self.size
        canvas_height = rows * self.size

        tk.Frame.__init__(self, parent)
        tk.Frame.pack(self, side="top", fill="both", expand="true", padx=4, pady=4)
        self.canvas = tk.Canvas(self, borderwidth=0, highlightthickness=0,
                                width=canvas_width, height=canvas_height, background="bisque")
        self.canvas.pack(side="top", fill="both", expand=True, padx=2, pady=2)
        self.canvas.bind("<Button-1>", self.onClick)
        self.canvas.bind("<B1-Motion>", self.onDrag)
        self.canvas.bind("<ButtonRelease-1>", self.onRelease)
        self.drawBoard()
        self.fromFen(start)

    def refresh(self):
        #self.drawFrame()
        self.after(16, self.refresh)

    def drawBoard(self):
        color = self.boardColor2

        for row in range(self.rows):
            color = self.boardColor1 if color == self.boardColor2 else self.boardColor2
            for col in range(self.columns):
                x1 = (col * self.size)
                y1 = (row * self.size)
                x2 = x1 + self.size
                y2 = y1 + self.size
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="white", fill=color, tags=(f"{row}, {col}","square"))
                color = self.boardColor1 if color == self.boardColor2 else self.boardColor2
        

    # Load a board from a FEN notation
    def fromFen(self, fen: str):
        self.board = {}
        row = 0
        col = 0
        for char in fen:
            if char == "/":
                row += 1
                col = 0
            elif char.isdigit():
                col += int(char)
            if char in fenToPiece:
                self.addpiece(createPiece(char), row, col)
                col += 1
            self.currentPlayer = BLACK if char == "b" else WHITE
        for key in self.board:
            self.drawpiece(self.board[key], key[0], key[1])
        self.canvas.tag_raise("piece")
        self.canvas.tag_lower("square")

    # Current board to FEN notation
    def currentToFen(self):
        fenStr = ""
        spaces = 0
        for row in range(8):
            for col in range(8):
                if (row, col) in self.board:
                    fenStr += f"{spaces if spaces > 0 else ''}{self.board[(row, col)].type}"
                    spaces = 0
                else:
                    spaces += 1
            fenStr += f"{spaces if spaces > 0 else ''}/"
            spaces = 0
        fenStr += f" {self.currentPlayer}"
        return fenStr

    # Add a piece to self.board and assign each piece a unique code to be able to distinguish between same type&color pieces
    def addpiece(self, piece: Piece, row: int, column: int):
        numberOfIstances = 0
        for key in self.board:
            if self.board[key].type == piece.type and self.board[key].color == piece.color:
                numberOfIstances += 1

        piece.uniqueCode = f"{numberOfIstances}{piece.type}"
        self.board[(row, column)] = piece

    # During each frame, redraw each piece
    def drawpiece(self, piece: Piece, row: int, column: int):
        self.canvas.create_image(0,0, image=piece.image, tags=(piece.uniqueCode, "piece"), anchor="c")
        x0 = (column * self.size) + int(self.size/2)
        y0 = (row * self.size) + int(self.size/2)
        self.canvas.coords(piece.uniqueCode, x0, y0)

    # Given an old position and a new one, place the piece in oldPos in newPos
    def placepiece(self, oldRow: int, oldColumn: int, newRow: int, newColumn: int):
        if self.board[(oldRow, oldColumn)].type.lower() == "k":
            if self.board[(oldRow, oldColumn)].color == WHITE:
                self.whiteKingsideCastle = False
                self.whiteQueensideCastle = False
            else:
                self.blackKingsideCastle = False
                self.blackQueensideCastle = False
        if self.board[(oldRow, oldColumn)].uniqueCode == "0r":
            self.blackQueensideCastle = False
        elif self.board[(oldRow, oldColumn)].uniqueCode == "1r":
            self.blackKingsideCastle = False
        elif self.board[(oldRow, oldColumn)].uniqueCode == "0R":
            self.whiteQueensideCastle = False
        elif self.board[(oldRow, oldColumn)].uniqueCode == "1R":
            self.whiteKingsideCastle = False
        
        if (newRow, newColumn) in self.board:
            self.board.pop((newRow, newColumn))
        tempSquare = self.board[(oldRow, oldColumn)]
        self.board.pop((oldRow, oldColumn))
        self.board[(newRow, newColumn)] = tempSquare
        x0 = (oldColumn * self.size) + int(self.size/2)
        y0 = (oldRow * self.size) + int(self.size/2)
        x1 = (newColumn * self.size) + int(self.size/2)
        y1 = (newRow * self.size) + int(self.size/2)
        for i in range(self.animationFps+1):
            sleep(0.0000155/self.animationFps)
            self.canvas.coords(self.board[(newRow, newColumn)].uniqueCode, x0+((x1-x0)/self.animationFps)*i, y0+((y1-y0)/self.animationFps)*i)
            self.canvas.update()


    # Returns the clicked-on tile's coordinates 
    def getMouseClickPos(self, event) -> tuple:
        item_below = self.canvas.find_overlapping(event.x,event.y,event.x,event.y)[0]
        return ((item_below - 1) // 8, (item_below - 1 ) % 8)

    def onReleaseMove(self, startCoords: tuple, dropCoords: tuple):
        x1 = (dropCoords[1] * self.size) + int(self.size/2)
        y1 = (dropCoords[0] * self.size) + int(self.size/2)
        self.board[dropCoords] = self.board[startCoords]
        self.board.pop(startCoords)
        self.canvas.coords(self.board[(dropCoords[0], dropCoords[1])].uniqueCode, x1, y1)
        if startCoords[0] % 2 != 0:
            self.canvas.itemconfig(f"{startCoords[0]}, {startCoords[1]}", fill=self.boardColor1) if (8*(startCoords[0]) + startCoords[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{startCoords[0]}, {startCoords[1]}", fill=self.boardColor2)
        else:
            self.canvas.itemconfig(f"{startCoords[0]}, {startCoords[1]}", fill=self.boardColor2) if (8*(startCoords[0]) + startCoords[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{startCoords[0]}, {startCoords[1]}", fill=self.boardColor1)
        for move in self.moves:
            if move[0] % 2 != 0:
                self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor1) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor2)
            else:
                self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor2) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor1)

    def onRelease(self, event):
        dropCoords = self.getMouseClickPos(event)
        if dropCoords in self.moves and self.selectedPiece in self.board:
            if self.board[self.selectedPiece].uniqueCode == "0r":
                self.blackQueensideCastle = False
            elif self.board[self.selectedPiece].uniqueCode == "1r":
                self.blackKingsideCastle = False
            elif self.board[self.selectedPiece].uniqueCode == "0R":
                self.whiteQueensideCastle = False
            elif self.board[self.selectedPiece].uniqueCode == "1R":
                self.whiteKingsideCastle = False
            if self.board[self.selectedPiece].type == "K":
                self.whiteKingsideCastle = False
                self.whiteQueensideCastle = False
                if dropCoords == (7, 2):
                    self.placepiece(7, 0, 7, 3)
                elif dropCoords == (7, 6):
                    self.placepiece(7, 7, 7, 5)
            elif self.board[self.selectedPiece].type == "k":
                self.blackKingsideCastle = False
                self.blackQueensideCastle = False
                if dropCoords == (0, 2):
                    self.placepiece(0, 0, 0, 3)
                elif dropCoords == (0, 6):
                    self.placepiece(0, 7, 0, 5)

            self.onReleaseMove(self.selectedPiece, dropCoords)
            self.moves = []
            self.selectedPiece = ()
            self.currentPlayer = int(not self.currentPlayer)
        elif self.selectedPiece in self.board:
            x1 = (self.selectedPiece[1] * self.size) + int(self.size/2)
            y1 = (self.selectedPiece[0] * self.size) + int(self.size/2)
            self.canvas.coords(self.board[self.selectedPiece].uniqueCode, x1, y1)


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
                self.moves = self.generateLegalMoves(clickPos[0], clickPos[1])
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
            elif clickPos not in self.moves:
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
                    self.moves = self.generateLegalMoves(clickPos[0], clickPos[1])
                    self.canvas.itemconfig(f"{clickPos[0]}, {clickPos[1]}", fill="#f7f76a")
                    for move in self.moves:
                        self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill="#edda95")
                else:
                    self.moves = []
                    self.selectedPiece = ()

            # If the user clicks on a move
            elif clickPos in self.moves:
                if self.selectedPiece[0] % 2 != 0:
                    self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor1) if (8*(self.selectedPiece[0]) + self.selectedPiece[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor2)
                else:
                    self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor2) if (8*(self.selectedPiece[0]) + self.selectedPiece[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{self.selectedPiece[0]}, {self.selectedPiece[1]}", fill=self.boardColor1)
                self.placepiece(self.selectedPiece[0], self.selectedPiece[1], clickPos[0], clickPos[1])
                self.currentPlayer = int(not self.currentPlayer)
                for move in self.moves:
                    if move[0] % 2 != 0:
                        self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor1) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor2)
                    else:
                        self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor2) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.boardColor1)
                self.moves = []
                self.selectedPiece = ()

    def generateAllLegalMoves(self, row, col):
        allMoves = []
        kingColor = self.board[(row, col)].color
        for key in self.board:
            if self.board[key].color != kingColor and self.board[key].type.lower() != "k" and self.board[key].type.lower() != "p":
                allMoves += self.generateLegalMovesNoSafety(key[0], key[1])
            if self.board[key].color != kingColor and self.board[key].type.lower() == "k":
                allMoves += [(key[0]+1, key[1]), (key[0]-1, key[1]), (key[0]+1, key[1]+1), (key[0]-1, key[1]-1), (key[0]+1, key[1]-1), (key[0]-1, key[1]+1), (key[0], key[1]+1), (key[0], key[1]-1)]
            if self.board[key].type.lower() == "p" and self.board[key].color != kingColor:
                offset = -1 if self.board[key].color == WHITE else 1
                for i in [1, -1]:
                    allMoves.append((key[0]+offset, key[1]+i))

        return allMoves
     

    def checkForKingSafety(self):
        piecesMoves = {}
        kingPos = ()

        for key in self.board:
            if self.board[key].type.lower() == "k" and self.board[key].color == self.currentPlayer:
                kingPos = key

        for key in self.board:
            if (self.board[key].color != self.currentPlayer) and (self.board[key].type.lower() == "p" or self.board[key].type.lower() == "n" or self.board[key].type.lower() == "k"):
                dangerousMoves = self.generateLegalMovesNoSafety(key[0], key[1])
                if kingPos in dangerousMoves:
                    piecesMoves[kingPos] = self.kingMoves(kingPos[0], kingPos[1])
            elif (self.board[key].color != self.currentPlayer) and (self.board[key].type.lower() == "r" or self.board[key].type.lower() == "b" or self.board[key].type.lower() == "q"):
                kingBeam = self.findBeam(key[0], key[1], kingPos[0], kingPos[1], fenToPiece[self.board[key].type.lower()][0])
                if kingBeam:
                    piecesMoves[kingPos] = self.kingMoves(kingPos[0], kingPos[1])
                    for key2 in self.board:
                        if self.board[key2].color == self.currentPlayer:
                            savingMoves = []
                            for move in self.generateLegalMovesNoSafety(key2[0], key2[1]):
                                if move in kingBeam or move == key:
                                    savingMoves.append(move)
                            if savingMoves:
                                piecesMoves[key2] = savingMoves

        
        return piecesMoves
                
    def generateLegalMovesNoSafety(self, row: int, col: int) -> list:
        moves = []

        if not (row, col) in self.board:
            return moves
                 
        pieceType = self.board[(row, col)].type.lower()


        if pieceType == "p":
            return self.pawnMoves(row, col)

        if pieceType == "n":
            return self.knightMoves(row, col)

        if pieceType == "r":
            return self.rookMoves(row, col)

        if pieceType == "b":
            return self.bishopMoves(row, col)

        if pieceType == "q":
            return self.queenMoves(row, col)
        
        if pieceType == "k":
            return self.kingMoves(row, col)                 

    def generateLegalMoves(self, row: int, col: int) -> list:
        moves = []
        obligatoryMoves = self.checkForKingSafety()

        if not (row, col) in self.board:
            return moves
                 
        pieceType = self.board[(row, col)].type.lower()

        if obligatoryMoves:
            if (row, col) in obligatoryMoves:
                return obligatoryMoves[(row, col)]
            else:
                return moves


        if pieceType == "p":
            return self.pawnMoves(row, col)

        if pieceType == "n":
            return self.knightMoves(row, col)

        if pieceType == "r":
            return self.rookMoves(row, col)

        if pieceType == "b":
            return self.bishopMoves(row, col)

        if pieceType == "q":
            return self.queenMoves(row, col)
        
        if pieceType == "k":
            return self.kingMoves(row, col)
    
    # FINDBEAM 
    def findBeam(self, row: int, col: int, kingRow: int, kingCol: int, pieceType: int) -> list:
        """

        Find the beam that is pointing to the king, so that you can find a piece whose legal moves contain atleast one move in the beam

        """
        pieceColor = self.board[(row, col)].color
        beam = []

        if pieceType == ROOK:
            for i in range(1, 8-row):
                if (row+i, col) in self.board and self.board[(row+i, col)].color != pieceColor:
                    beam.append((row+i, col))
                    if (kingRow, kingCol) == (row+i, col):
                        return beam
                    break
                elif (row+i, col) in self.board:
                    break
                beam.append((row+i, col))
            beam.clear()
            for i in range(row-1, -1, -1):
                if (i, col) in self.board and self.board[(i, col)].color != pieceColor:
                    beam.append((i, col))
                    if (kingRow, kingCol) == (i, col):
                        return beam
                    break
                elif (i, col) in self.board:
                    break
                beam.append((i, col))
            beam.clear()
            for i in range(1, 8-col):
                if (row, col+i) in self.board and self.board[(row, col+i)].color != pieceColor:
                    beam.append((row, col+i))
                    if (kingRow, kingCol) == (row, col+i):
                        return beam
                    break
                elif (row, col+i) in self.board:
                    break
                beam.append((row, col+i))
            beam.clear()
            for i in range(col-1, -1, -1):
                if (row, i) in self.board and self.board[(row, i)].color != pieceColor:
                    beam.append((row, i))  
                    if (kingRow, kingCol) == (row, i):
                        return beam
                    break
                elif (row, i) in self.board:
                    break
                beam.append((row, i))
            beam.clear()
        elif pieceType == BISHOP:
            for i in range(1, min(8-row, 8-col)):
                if (row+i, col+i) in self.board and self.board[(row+i, col+i)].color != pieceColor:
                    beam.append((row+i, col+i))
                    if (kingRow, kingCol) == (row+i, col+i):
                        return beam
                    break
                elif (row+i, col+i) in self.board:
                    break
                beam.append((row+i, col+i))
            beam.clear()
            for i in range(1, min(row, col)+1):
                if (row-i, col-i) in self.board and self.board[(row-i, col-i)].color != pieceColor:
                    beam.append((row-i, col-i))
                    if (kingRow, kingCol) == (row-i, col-i):
                        return beam
                    break
                elif (row-i, col-i) in self.board:
                    break
                beam.append((row-i, col-i))
            beam.clear()
            for i in range(1, min(row+1, 8-col)):
                if (row-i, col+i) in self.board and self.board[(row-i, col+i)].color != pieceColor:
                    beam.append((row-i, col+i))
                    if (kingRow, kingCol) == (row-i, col+i):
                        return beam
                    break
                elif (row-i, col+i) in self.board:
                    break
                beam.append((row-i, col+i))
            beam.clear()
            for i in range(1, min(8-row, col+1)):
                if (row+i, col-i) in self.board and self.board[(row+i, col-i)].color != pieceColor:
                    beam.append((row+i, col-i))
                    if (kingRow, kingCol) == (row+i, col-i):
                        return beam
                    break
                elif (row+i, col-i) in self.board:
                    break
                beam.append((row+i, col-i))
            beam.clear()
        elif pieceType == QUEEN:
            for i in range(1, 8-row):
                if (row+i, col) in self.board and self.board[(row+i, col)].color != pieceColor:
                    beam.append((row+i, col))
                    if (kingRow, kingCol) == (row+i, col):
                        return beam
                    break
                elif (row+i, col) in self.board:
                    break
                beam.append((row+i, col))
            beam.clear()
            for i in range(row-1, -1, -1):
                if (i, col) in self.board and self.board[(i, col)].color != pieceColor:
                    beam.append((i, col))
                    if (kingRow, kingCol) == (i, col):
                        return beam
                    break
                elif (i, col) in self.board:
                    break
                beam.append((i, col))
            beam.clear()
            for i in range(1, 8-col):
                if (row, col+i) in self.board and self.board[(row, col+i)].color != pieceColor:
                    beam.append((row, col+i))
                    if (kingRow, kingCol) == (row, col+i):
                        return beam
                    break
                elif (row, col+i) in self.board:
                    break
                beam.append((row, col+i))
            beam.clear()
            for i in range(col-1, -1, -1):
                if (row, i) in self.board and self.board[(row, i)].color != pieceColor:
                    beam.append((row, i))  
                    if (kingRow, kingCol) == (row, i):
                        return beam
                    break
                elif (row, i) in self.board:
                    break
                beam.append((row, i))
            beam.clear()
            for i in range(1, min(8-row, 8-col)):
                if (row+i, col+i) in self.board and self.board[(row+i, col+i)].color != pieceColor:
                    beam.append((row+i, col+i))
                    if (kingRow, kingCol) == (row+i, col+i):
                        return beam
                    break
                elif (row+i, col+i) in self.board:
                    break
                beam.append((row+i, col+i))
            beam.clear()
            for i in range(1, min(row, col)+1):
                if (row-i, col-i) in self.board and self.board[(row-i, col-i)].color != pieceColor:
                    beam.append((row-i, col-i))
                    if (kingRow, kingCol) == (row-i, col-i):
                        return beam
                    break
                elif (row-i, col-i) in self.board:
                    break
                beam.append((row-i, col-i))
            beam.clear()
            for i in range(1, min(row+1, 8-col)):
                if (row-i, col+i) in self.board and self.board[(row-i, col+i)].color != pieceColor:
                    beam.append((row-i, col+i))
                    if (kingRow, kingCol) == (row-i, col+i):
                        return beam
                    break
                elif (row-i, col+i) in self.board:
                    break
                beam.append((row-i, col+i))
            beam.clear()
            for i in range(1, min(8-row, col+1)):
                if (row+i, col-i) in self.board and self.board[(row+i, col-i)].color != pieceColor:
                    beam.append((row+i, col-i))
                    if (kingRow, kingCol) == (row+i, col-i):
                        return beam
                    break
                elif (row+i, col-i) in self.board:
                    break
                beam.append((row+i, col-i))
            beam.clear()
        
    
    def pawnMoves(self, row: int, col: int) -> list:
        moves = []

        pieceColor = self.board[(row, col)].color


        offset = -1 if pieceColor == WHITE else 1
        if (row+offset, col) in self.board:
            return moves

        moves = [(row+offset, col)]
        if (row == 6 or row == 1) and not (row+2*offset, col) in self.board:
                moves.append((row+2*offset, col))
        for i in [1, -1]:
            if (row+offset, col+i) in self.board and self.board[(row+offset, col+i)].color != pieceColor:
                moves.append((row+offset, col+i))

        return moves

    def knightMoves(self, row: int, col: int) -> list:
        moves = []

        pieceColor = self.board[(row, col)].color

        for possibleMove in [(row+1, col+2),(row+1, col-2), (row+2, col+1), (row+2, col-1), (row-1, col+2),(row-1, col-2), (row-2, col+1), (row-2, col-1)]:
            if possibleMove in self.board and self.board[possibleMove].color == pieceColor:
                pass
            else:
                moves.append(possibleMove)

        return moves
    
    def rookMoves(self, row: int, col: int) -> list:
        moves = []

        pieceColor = self.board[(row, col)].color

        for i in range(1, 8-row):
            if (row+i, col) in self.board:
                if self.board[(row+i, col)].color != pieceColor:
                    moves.append((row+i, col))
                    break
                break
            moves.append((row+i, col))
        for i in range(row-1, -1, -1):
            if (i, col) in self.board:
                if self.board[(i, col)].color != pieceColor:
                    moves.append((i, col))
                    break
                break
            moves.append((i, col))
        for i in range(1, 8-col):
            if (row, col+i) in self.board:
                if self.board[(row, col+i)].color != pieceColor:
                    moves.append((row, col+i))
                    break
                break
            moves.append((row, col+i))
        for i in range(col-1, -1, -1):
            if (row, i) in self.board:
                if self.board[(row, i)].color != pieceColor:
                    moves.append((row, i))
                    break
                break
            moves.append((row, i))
        
        return moves

    def bishopMoves(self, row: int, col: int) -> list:
        moves = []

        pieceColor = self.board[(row, col)].color

        for i in range(1, min(8-row, 8-col)):
            if (row+i, col+i) in self.board:
                if self.board[(row+i, col+i)].color != pieceColor:
                    moves.append((row+i, col+i))
                    break
                break
            moves.append((row+i, col+i))
        for i in range(1, min(row, col)+1):
            if (row-i, col-i) in self.board:
                if self.board[(row-i, col-i)].color != pieceColor:
                    moves.append((row-i, col-i))
                    break
                break
            moves.append((row-i, col-i))
        for i in range(1, min(row+1, 8-col)):
            if (row-i, col+i) in self.board:
                if self.board[(row-i, col+i)].color != pieceColor:
                    moves.append((row-i, col+i))
                    break
                break
            moves.append((row-i, col+i))
        for i in range(1, min(8-row, col+1)):
            if (row+i, col-i) in self.board:
                if self.board[(row+i, col-i)].color != pieceColor:
                    moves.append((row+i, col-i))
                    break
                break
            moves.append((row+i, col-i))
        return moves

    def queenMoves(self, row: int, col: int) -> list:
        moves = []

        pieceColor = self.board[(row, col)].color

        for i in range(1, min(8-row, 8-col)):
            if (row+i, col+i) in self.board:
                if self.board[(row+i, col+i)].color != pieceColor:
                    moves.append((row+i, col+i))
                    break
                break
            moves.append((row+i, col+i))
        for i in range(1, min(row, col)+1):
            if (row-i, col-i) in self.board:
                if self.board[(row-i, col-i)].color != pieceColor:
                    moves.append((row-i, col-i))
                    break
                break
            moves.append((row-i, col-i))
        for i in range(1, min(row+1, 8-col)):
            if (row-i, col+i) in self.board:
                if self.board[(row-i, col+i)].color != pieceColor:
                    moves.append((row-i, col+i))
                    break
                break
            moves.append((row-i, col+i))
        for i in range(1, min(8-row, col+1)):
            if (row+i, col-i) in self.board:
                if self.board[(row+i, col-i)].color != pieceColor:
                    moves.append((row+i, col-i))
                    break
                break
            moves.append((row+i, col-i))
        for i in range(1, 8-row):
            if (row+i, col) in self.board:
                if self.board[(row+i, col)].color != pieceColor:
                    moves.append((row+i, col))
                    break
                break
            moves.append((row+i, col))
        for i in range(row-1, -1, -1):
            if (i, col) in self.board:
                if self.board[(i, col)].color != pieceColor:
                    moves.append((i, col))
                    break
                break
            moves.append((i, col))
        for i in range(1, 8-col):
            if (row, col+i) in self.board:
                if self.board[(row, col+i)].color != pieceColor:
                    moves.append((row, col+i))
                    break
                break
            moves.append((row, col+i))
        for i in range(col-1, -1, -1):
            if (row, i) in self.board:
                if self.board[(row, i)].color != pieceColor:
                    moves.append((row, i))
                    break
                break
            moves.append((row, i))
    
        return moves

    def kingMoves(self, row: int, col: int) -> list:
        moves = []

        pieceColor = self.board[(row, col)].color

        for key in self.board:
            if self.board[key].type == "r" and (row, col) == (0, 4) and self.board[key].color == self.currentPlayer and key in [(row,0), (row,7)]:
                blocked = False
                offset = -1 if key[1] < 4 else 1
                for element in self.board:
                    blocked = 0 < element[1] < 4 and element[0] == row if key[1] == 0 else 4 < element[1] < key[1] and element[0] == row
                    if blocked:
                        break
                if key == (row, 0) and not self.blackQueensideCastle:
                    blocked = True
                elif key == (row,7) and not self.blackKingsideCastle:
                    blocked = True
                if not blocked:
                    moves.append((row, 4 + 2*offset))
            if self.board[key].type == "R" and (row, col) == (7, 4) and self.board[key].color == self.currentPlayer and key in [(row,0), (row,7)]:
                blocked = False
                offset = -1 if key[1] < 4 else 1
                for element in self.board:
                    blocked = 0 < element[1] < 4 and element[0] == row if key[1] == 0 else 4 < element[1] < key[1] and element[0] == row
                    if blocked:
                        break
                if key == (row, 0) and not self.whiteQueensideCastle:
                    blocked = True
                elif key == (row,7) and not self.whiteKingsideCastle:
                    blocked = True
                if not blocked:
                    moves.append((row, 4 + 2*offset))

        for move in [(row+1, col), (row-1, col), (row+1, col+1), (row-1, col-1), (row+1, col-1), (row-1, col+1), (row, col+1), (row, col-1)]:
            if not move in self.generateAllLegalMoves(row, col):
                if move in self.board and self.board[move].color != pieceColor:
                    moves.append(move)
                elif move not in self.board:
                    moves.append(move)
        
        return moves
                


            


if __name__ == "__main__":
    root = tk.Tk()
    root.geometry(f"{95*8}x{95*8}")
    root.resizable(False, False)
    board = GameBoard(root, start="r3k2r/8/8/8/8/8/8/R3K2R w")
    print(board.currentToFen())
    board.after(16, board.refresh)
    root.mainloop()