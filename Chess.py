import subprocess
import threading
import time
import random
import tkinter as tk
import time
import platform
import os
from math import *
import math
import atexit

from Event import *

from tkinter import font

import win32pipe, win32file, pywintypes
from PIL import Image, ImageTk
from PIL.Image import Resampling

my_system = platform.uname()

from ctypes import windll
windll.shcore.SetProcessDpiAwareness(1)

CHESS_START = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
dir_path = os.path.dirname(os.path.realpath(__file__))

TIMEFL = 0.00000000150 if my_system.node.lower().find("desktop") == -1 else 0.000150

"""
    TODO list:
        - Add images of the eaten piece under or over the board - In progres...
        - Start working on the AI - DONE!      
"""
# -------------------------------------------------------------------------------
KING = 0
QUEEN = 1
BISHOP = 2
KNIGHT = 3
ROOK = 4
PAWN = 5

WHITE = 0
BLACK = 1

# -------------------------------------------------------------------------------
NONE = 0x00
SET = 0x01
MOVE = 0x02
UNDO = 0x03
MOVEREQ = 0x04
GENMOVES = 0x05
INCHECK = 0x06
EXIT = 0x07

# -------------------------------------------------------------------------------
SQSTR = {
	"a1": 0, "b1": 1, "c1": 2, "d1": 3, "e1": 4, "f1": 5, "g1": 6, "h1": 7,
	"a2": 8, "b2": 9, "c2": 10, "d2": 11, "e2": 12, "f2": 13, "g2": 14, "h2": 15,
	"a3": 16, "b3": 17, "c3": 18, "d3": 19, "e3": 20, "f3": 21, "g3": 22, "h3": 23,
	"a4": 24, "b4": 25, "c4": 26, "d4": 27, "e4": 28, "f4": 29, "g4": 30, "h4": 31,
	"a5": 32, "b5": 33, "c5": 34, "d5": 35, "e5": 36, "f5": 37, "g5": 38, "h5": 39,
	"a6": 40, "b6": 41, "c6": 42, "d6": 43, "e6": 44, "f6": 45, "g6": 46, "h6": 47,
	"a7": 48, "b7": 49, "c7": 50, "d7": 51, "e7": 52, "f7": 53, "g7": 54, "h7": 55,
	"a8": 56, "b8": 57, "c8": 58, "d8": 59, "e8": 60, "f8": 61, "g8": 62, "h8": 63,
	"None": 64
}

MOVE_TYPESTR_B = [
	"", "", " O-O", " O-O-O", " N (promotion)", " B (promotion)", " R (promotion)", " Q (promotion)", " (capture)", "", " e.p.", "",
	" N (promotion)", " B (promotion)", " R (promotion)", " Q (promotion)"
];

# -------------------------------------------------------------------------------
QUIET = 0b0000
DOUBLE_PUSH = 0b0001
OO = 0b0010
OOO = 0b0011
CAPTURE = 0b1000
CAPTURES = 0b1111
EN_PASSANT = 0b1010
PROMOTIONS = 0b0111
PROMOTION_CAPTURES = 0b1100
PR_KNIGHT = 0b0100
PR_BISHOP = 0b0101
PR_ROOK = 0b0110
PR_QUEEN = 0b0111
PC_KNIGHT = 0b1100
PC_BISHOP = 0b1101
PC_ROOK = 0b1110
PC_QUEEN = 0b1111

# -------------------------------------------------------------------------------
HUMAN = 0x01
AI = 0x02
NOMOVE = 0xFF

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

def flagToType(flag: int) -> int:
    if (flag < PR_KNIGHT or flag > PC_QUEEN):
        return -1
    
    if flag == PR_KNIGHT or flag == PC_KNIGHT:
        return KNIGHT
    if flag == PR_BISHOP or flag == PC_BISHOP:
        return BISHOP
    if flag == PR_ROOK or flag == PC_ROOK:
        return ROOK
    if flag == PR_QUEEN or flag == PC_QUEEN:
        return QUEEN

class Piece:
    def __init__(self, pieceType: int, color: int, image: ImageTk.PhotoImage):
        self.type = pieceType
        self.color = color
        self.image = image
        self.fentype = pieceToFen[(pieceType, color)]
        self.uniqueCode = 0

class Move:
    def __init__(self, move_internal: int = 0, from_sq: int = 0, to_sq: int = 0, flags: int = 0):
        if (move_internal):
            self.move = move_internal
        else:
            self.move = int((flags << 12) | (from_sq << 6) | to_sq);
    
    def getFrom(self):
        return (self.move >> 6) & 0x3f

    def getTo(self):
        return self.move & 0x3f
    
    def getFlags(self):
        return self.move >> 12
    
    def toBytes(self):
        return self.move.to_bytes(2, "little")
    
    def toString(self):
        return SQSTR[self.getFrom()] + SQSTR[self.getTo()] + MOVE_TYPESTR_B[self.getFlags()]
    
    def getInternal(self):
        return self.move
    
    def __str__(self):
        return_string = ""

        for square_string, square in SQSTR.items():
            if square == self.getFrom():
                return_string += square_string

        for square_string, square in SQSTR.items():    
            if square == self.getTo():
                return_string += square_string 

        return return_string + MOVE_TYPESTR_B[self.getFlags()]

def arrayU16toMoves(buffer: bytearray) -> list:
    moves = []

    if int.from_bytes(buffer[:2], "little") == 0:
            return moves

    for i in range(0, len(buffer), 2):
        moves.append(Move(int.from_bytes(buffer[i:i + 2], "little")))

    return moves


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

def dist(p1: tuple, p2: tuple) -> float:
    return math.sqrt(math.pow(p1[0] - p2[0], 2)+ math.pow(p1[1] - p2[1], 2))

def alpha_blend(color_dict: dict) -> str:
    colors = sorted(color_dict.items())
    w_sum = sum(color_dict.values())

    # Weighted Average between the different colors' rgb values

    red = int(sum(int(x.replace("#", "")[:2], 16)*w for x, w in colors)/w_sum)
    green = int(sum(int(x.replace("#", "")[2:4], 16)*w for x, w in colors)/w_sum)
    blue = int(sum(int(x.replace("#", "")[4:6], 16)*w for x, w in colors)/w_sum)

    # Output the final color

    return "#" + f"{red:x}" + f"{green:x}" + f"{blue:x}"

class RoundRectangle():
    def __init__(self, canvas: tk.Canvas, x1, y1, x2, y2, radius = 20, **kwargs):
        self.rect_x1 = x1
        self.rect_y1 = y1
        self.rect_x2 = x2
        self.rect_y2 = y2

        self.circle_radius = radius
        self.canvas = canvas

        offset = floor(radius/2)

        rect_points = [
            x1, y1 + offset,
            x1 + offset, y1,
            x2 - offset, y1,
            x2, y1 + offset,
            x2, y2 - offset,
            x2 - offset, y2,
            x1 + offset, y2,
            x1, y2 - offset
        ]

        self.rectangle = canvas.create_polygon(rect_points, **kwargs)
        self.circles = [
            canvas.create_oval(x1, y1, x1 + self.circle_radius, y1 + self.circle_radius, outline="", **kwargs),
            canvas.create_oval(x2 - 1, y1, x2 - self.circle_radius, y1 + self.circle_radius, outline="", **kwargs),
            canvas.create_oval(x2 - 1, y2 - 1, x2 - self.circle_radius, y2 - self.circle_radius, outline="", **kwargs),
            canvas.create_oval(x1, y2 - 1, x1 + self.circle_radius, y2 - self.circle_radius, outline="", **kwargs),
        ]



class Board():
    def __init__(self, debug=False, tileSize=110, colorLight="#F1D9C2", colorDark="#AB7966", start: str = CHESS_START, ai: bool = False, win_message: str = "won", win_img_path: str = f"{dir_path}\\assets\\chess_win.png", start_player: int = -1, ):
        self.debug = debug

        self.parent = tk.Tk()
        self.should_close = False
        self.rows = 8
        self.columns = 8
        self.fullscreen = True
        self.size = tileSize
        self.piece_scale = 0.82
        self.board_color_light = colorLight
        self.board_color_dark = colorDark
        self.selected_piece_color = "#DED080"
        self.move_color = "#D0AC6A"
        self.application_fps = 240

        self.fenToImage = {
        "k" : ImageTk.PhotoImage(Image.open(f"{dir_path}\\assets\\bking.png").resize((int(self.piece_scale * self.size),int(self.piece_scale * self.size)), Resampling.LANCZOS)),
        "q" : ImageTk.PhotoImage(Image.open(f"{dir_path}\\assets\\bqueen.png").resize((int(self.piece_scale * self.size),int(self.piece_scale * self.size)), Resampling.LANCZOS)),
        "n" : ImageTk.PhotoImage(Image.open(f"{dir_path}\\assets\\bhorse.png").resize((int(self.piece_scale * self.size),int(self.piece_scale * self.size)), Resampling.LANCZOS)),
        "r" : ImageTk.PhotoImage(Image.open(f"{dir_path}\\assets\\brook.png").resize((int(self.piece_scale * self.size),int(self.piece_scale * self.size)), Resampling.LANCZOS)),
        "b" : ImageTk.PhotoImage(Image.open(f"{dir_path}\\assets\\bbishop.png").resize((int(self.piece_scale * self.size),int(self.piece_scale * self.size)), Resampling.LANCZOS)),
        "p" : ImageTk.PhotoImage(Image.open(f"{dir_path}\\assets\\bpawn.png").resize((int(self.piece_scale * self.size),int(self.piece_scale * self.size)), Resampling.LANCZOS)),
        "K" : ImageTk.PhotoImage(Image.open(f"{dir_path}\\assets\\wking.png").resize((int(self.piece_scale * self.size),int(self.piece_scale * self.size)), Resampling.LANCZOS)),
        "Q" : ImageTk.PhotoImage(Image.open(f"{dir_path}\\assets\\wqueen.png").resize((int(self.piece_scale * self.size),int(self.piece_scale * self.size)), Resampling.LANCZOS)),
        "N" : ImageTk.PhotoImage(Image.open(f"{dir_path}\\assets\\whorse.png").resize((int(self.piece_scale * self.size),int(self.piece_scale * self.size)), Resampling.LANCZOS)),
        "R" : ImageTk.PhotoImage(Image.open(f"{dir_path}\\assets\\wrook.png").resize((int(self.piece_scale * self.size),int(self.piece_scale * self.size)), Resampling.LANCZOS)),
        "B" : ImageTk.PhotoImage(Image.open(f"{dir_path}\\assets\\wbishop.png").resize((int(self.piece_scale * self.size),int(self.piece_scale * self.size)), Resampling.LANCZOS)),
        "P" : ImageTk.PhotoImage(Image.open(f"{dir_path}\\assets\\wpawn.png").resize((int(self.piece_scale * self.size),int(self.piece_scale * self.size)), Resampling.LANCZOS)),
        }

        self.white_kingside_castle = False
        self.white_queenside_castle = False
        self.black_kingside_castle = False
        self.black_queenside_castle = False

        self.current_player = start_player
        self.selected_piece = ()
        self.enpassant = []
        self.moves = []
        self.poppedEnpassant = []
        self.use_ai = ai
        self.ai_level = 4
        self.engine_handle = 0
        self.game_ply = 0
        self.start_player = start_player if start_player != -1 else WHITE if start[start.find(" ")+1] == "w" else BLACK

        self.board = {}
        self.old_board = {}
        self.move_log = []
        self.move_log_label = tk.Label()
        self.win_text = win_message if len(win_message) < 20 else "won"
        self.win_image = ImageTk.PhotoImage(Image.open(win_img_path).resize((500, 350), Resampling.LANCZOS))
        self.draw_image = ImageTk.PhotoImage(Image.open(f"{dir_path}\\assets\\chess_draw.png").resize((500, 350), Resampling.LANCZOS))

        self.auziliary_pieces = [self.fenToImage["q"], self.fenToImage["b"], self.fenToImage["r"], self.fenToImage["n"], self.fenToImage["Q"], self.fenToImage["B"], self.fenToImage["R"], self.fenToImage["N"]]
        self.promotion_ui = tk.BooleanVar(self.parent)
        self.animations_done = True

        canvas_width = self.columns * self.size
        canvas_height = self.rows * self.size

        self.parent.geometry(f"{(self.size+30)*8}x{(self.size+30)*8}")
        self.parent.resizable(False, False)
        self.parent.attributes("-fullscreen", self.fullscreen)

        self.canvas = tk.Canvas(self.parent, borderwidth=0, highlightthickness=0,
                                width=canvas_width+1, height=canvas_height+1, background="#302E2B")
        self.canvas.pack(fill="both", expand=True)

        self.parent.update()
        self.promotion_type = None
        self.window_width = self.parent.winfo_width()
        self.window_height = self.parent.winfo_height()
        self.offset_x = ((self.window_width - (self.size*8)) // 3) * 2
        self.offset_y = ((self.window_height - (self.size*8)) // 2) * 1

        self.canvas.bind("<Button-1>", self.onClick)
        self.canvas.bind("<B1-Motion>", self.onDrag)
        self.canvas.bind("<ButtonRelease-1>", self.onRelease)
        self.parent.bind("<F11>", self.toggle_fullscreen)
        self.parent.bind("<F5>", lambda e: self.loadBoard(start))
        self.parent.bind("<F6>", self.restoreMove)

        self.parent.protocol("WM_DELETE_WINDOW", self.onClose)
        if self.debug:
            self.parent.bind("<F7>", self.print_move_log)
            self.parent.bind("<F8>", lambda e: print(self.toFen()))

        atexit.register(self.engineSendCommand, EXIT)

        self.handler = Handler()
        self.handler.addHandle(ENGINE_MOVE_EVENT, self.moveAI)

        self.gui_font = "Noto Sans Medium"
        self.gui = {"offx": 0, "offy": 0, "width": 0, "height": 0}
        
        
        self.loading_animation = []
        loading_gif = Image.open(f"{dir_path}\\assets\\anim.gif")

        for i in range(4):
            loading_gif.seek(i)
            self.loading_animation.append(ImageTk.PhotoImage(loading_gif.resize((20, 20), resample=Resampling.NEAREST)))

        self.initializeEngine()
        self.loadBoard(start)
        
    def onClose(self):
        self.engineSendCommand(EXIT)
        win32file.CloseHandle(self.engine_handle)

        self.should_close = True

    def initializeEngine(self):
        #subprocess.Popen([f"{dir_path}\\src\ChessEngine"])
        try:
            # Create the pipe for communication
            self.engine_handle = win32file.CreateFile(r"\\.\\pipe\\chs", 
                                                    win32file.GENERIC_READ | win32file.GENERIC_WRITE,
                                                    0,
                                                    None,
                                                    win32file.OPEN_EXISTING,
                                                    0,
                                                    None)
            
        except pywintypes.error as e:
            print(e)
            if e.args[0] == 2:
                print("[ERROR] No pipe available")
            elif e.args[0] == 109:
                print("[ERROR] Could not open pipe, exiting")
                exit(0) 

    def engineSendCommand(self, command_type: int, command_payload: bytearray = None, wait: bool = True) -> bytearray:
        if command_payload == None:
            command_payload = bytearray()

        _, _ = win32file.WriteFile(self.engine_handle, command_type.to_bytes(1) + command_payload) 

        if self.debug:
            print("Sent the engine", command_type, command_payload)

        if wait:
            res = win32file.ReadFile(self.engine_handle, 1024)
            if self.debug:
                print("Received from the engine", res[1])

            return res[1]

        return bytearray()

    def mainloop(self):
        while not self.should_close:
            #time.sleep(1/self.application_fps)

            # if len(self.generateAllLegalMoves(self.board, self.current_player)) == 0 and self.inCheck(self.board, self.current_player):
            #     self.on_win()
            # elif len(self.generateAllLegalMoves(self.board, self.current_player)) == 0:
            #     self.on_draw()
            self.handler.pollEvents()            

            self.parent.update()
            self.parent.update_idletasks()

            #self.parent.title(f"time since last update: {(end-start)/10e6}ms, FPS: {float((end-start))}")

    def createPiece(self, square: int, piece_type: int, piece_color: int): 
        piece = Piece(piece_type, piece_color, self.fenToImage[pieceToFen[piece_type, piece_color]])

        # numberOfIstances = 0
        # for key in self.board:
        #     if self.board[key].type == piece.type and self.board[key].color == piece.color:
        #         numberOfIstances += 1

        uC = random.randint(0x00FF, 0xFFFF)
        
        same = True
        while (same):
            same = False
            for item in self.board.values():
                if (item.uniqueCode == f"pc{uC}"):
                    uC = random.randint(0x00FF, 0xFFFF)
                    same = True

        piece.uniqueCode = f"pc{uC}"
        self.board[square] = piece

    def print_move_log(self, event):
        os.system("cls")
        sorted_key_list = sort_keys(self.move_log.keys(), True)
        for element in sorted_key_list:
            print(element[:len(element)-3] + "player: " + ("BLACK" if int(element[len(element)-3]) else "WHITE") + " enpassant: " + ("TRUE" if int(element[len(element)-1]) else "FALSE"))

    def reload(self, event):
        self.parent.update()
        self.window_width = self.parent.winfo_width()
        self.window_height = self.parent.winfo_height()
        self.offset_x = (self.window_width - (self.size*8)) // 2
        self.offset_y= (self.window_height - (self.size*8)) // 2
        self.canvas.delete("all")
        self.drawBoard()
        for key in self.board:
            self.drawPiece(self.board[key], key[0], key[1])
    
    def on_win(self):
        centreX = self.parent.winfo_width() // 2
        centreY = self.parent.winfo_height() // 2
        x0 = centreX - 300
        y0 = centreY - 260
        x1 = centreX + 300
        y1 = centreY + 260
        self.canvas.create_rectangle(x0, y0, x1, y1, fill=self.board_color_light, tags="win")
        self.canvas.create_text(centreX, centreY+130, text=f"{'White' if self.current_player else 'Black'} {self.win_text}", fill="black", font=('Calibri 25 bold'), tags="win")
        self.canvas.create_image(centreX, centreY-70, image=self.win_image, tags="win")

    def on_draw(self):
        centreX = self.parent.winfo_width() // 2
        centreY = self.parent.winfo_height() // 2
        x0 = centreX - 300
        y0 = centreY - 260
        x1 = centreX + 300
        y1 = centreY + 260
        self.canvas.create_rectangle(x0, y0, x1, y1, fill=self.board_color_light, tags="draw")
        self.canvas.create_text(centreX, centreY+130, text=f"Draw!", fill="black", font=('Calibri 25 bold'), tags="draw")
        self.canvas.create_image(centreX, centreY-70, image=self.draw_image, tags="draw")
    
    def toggle_fullscreen(self, event):
        self.fullscreen = not self.fullscreen
        self.parent.attributes("-fullscreen", self.fullscreen)
        #self.reload(None)

    def requestAIMove(self):
        self.canvas.unbind("<Button-1>")
        self.canvas.unbind("<B1-Motion>")

        self.engineSendCommand(MOVEREQ, bytearray([self.ai_level, self.current_player]), wait = False)

        engine_move_thread = threading.Thread(target=self.moveAIThread, args=(self.handler, self.engine_handle,))
        engine_move_thread.start()

        self.canvas.itemconfig("engine_text", text="Calculating... ")
        self.playCalculatingAnimation(0)

    def playCalculatingAnimation(self, idx):
        self.canvas.delete("anim")

        self.canvas.create_image(self.gui["offx"] + int(self.gui["width"] * 0.27), self.gui["offy"] + int(self.gui["height"] * 0.056), image=self.loading_animation[idx], tags=("anim",), anchor=tk.W)
        idx += 1

        if idx == 4:
            idx = 0
        
        if self.current_player == self.start_player:
            return
        
        self.canvas.after(130, self.playCalculatingAnimation, idx)




    def moveAIThread(self, handler, engine_pipe_handle):
        res = win32file.ReadFile(engine_pipe_handle, 1024)

        print("Received: ", res[1])

        handler.queuePush(EngineMoveEvent(res[1]))


    def moveAI(self, event: EngineMoveEvent):
        try:
            move_code = event.move_code
            move = Move(int.from_bytes(move_code, "little"))

            if self.debug:
                print("The engine played", move)

            self.playMove(move, AI)
            # if "O-O" in move and "h8" in move:
            #     move = "e8g8"
            # if "O-O" in move and "h1" in move:
            #     move = "e1g1"
            # if "O-O-O" in move and "c8" in move:
            #     move = "e8c8"
            # if "O-O-O" in move and "c1" in move:
            #     move = "e1c1"
            
            # oldPos = (8-int(move[1]), letterToColumn[move[0]])
            # newPos = (8-int(move[3]), letterToColumn[move[2]])

            # if "(promotion)" in move:
            #     self.promotion_ai = True
            #     self.playMove(oldPos, newPos)
            #     self.promotePawn_ai(newPos, move[5])
            # self.selected_piece = oldPos
            # try:
            #     self.moves = self.generateLegalMoves(oldPos[0], oldPos[1], self.board)
            # except:
            #     pass
            # if not newPos in self.moves:
            #     self.moves.append(newPos)
            # if not self.promotion_ai:
            #     self.playMove(oldPos, newPos)
            # self.promotion_ai = False
            # self.moves = []
        except:
            pass

        self.canvas.bind("<Button-1>", self.onClick)
        self.canvas.bind("<B1-Motion>", self.onDrag)

        

    def restoreMove(self, event):
        if len(self.move_log) == 0 or self.promotion_ui.get():
            return

        self.engineSendCommand(UNDO)
        self.current_player ^= 1

        # if self.selected_piece:
        #     if self.selected_piece[0] % 2 != 0:
        #             self.canvas.itemconfig(f"{self.selected_piece[0]}, {self.selected_piece[1]}", fill=self.board_color_light) if (8*(self.selected_piece[0]) + self.selected_piece[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{self.selected_piece[0]}, {self.selected_piece[1]}", fill=self.board_color_dark)
        #     else:
        #         self.canvas.itemconfig(f"{self.selected_piece[0]}, {self.selected_piece[1]}", fill=self.board_color_dark) if (8*(self.selected_piece[0]) + self.selected_piece[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{self.selected_piece[0]}, {self.selected_piece[1]}", fill=self.board_color_light)
        #     for move in self.moves:
        #         if move[0] % 2 != 0:
        #             self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.board_color_light) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.board_color_dark)
        #         else:
        #             self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.board_color_dark) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.board_color_light)


        last_move, last_move_capture = self.move_log.pop(self.game_ply)

        from_sq = last_move.getFrom()
        to_sq = last_move.getTo()
        flags = last_move.getFlags()

        self.movePiece(to_sq, from_sq)

        if flags == EN_PASSANT:
            if self.current_player == BLACK:
                self.board[to_sq + 8] = last_move_capture
                self.drawPiece(last_move_capture, to_sq + 8)
            else:
                self.board[to_sq - 8] = last_move_capture
                self.drawPiece(last_move_capture, to_sq - 8)
        
        if flags == CAPTURE or flags >= PC_KNIGHT:
            self.board[to_sq] = last_move_capture
            self.drawPiece(last_move_capture, to_sq)
        
        if flags >= PR_KNIGHT:
            self.canvas.delete(self.board[from_sq].uniqueCode)
            self.createPiece(from_sq, PAWN, self.current_player)
            self.drawPiece(self.board[from_sq], from_sq)

        if flags == OO:
            if self.current_player == BLACK:
                self.movePiece(SQSTR["f8"], SQSTR["h8"])
            else:
                self.movePiece(SQSTR["f1"], SQSTR["h1"])

        if flags == OOO:
            if self.current_player == BLACK:
                self.movePiece(SQSTR["d8"], SQSTR["a8"])
            else:
                self.movePiece(SQSTR["d1"], SQSTR["a1"])

        # last_move_key = sort_keys(self.move_log.keys())[0]
        # last_move_key_space = last_move_key.find(" ")
        # last_move_piece = self.move_log[last_move_key]

        # newPos = (8-int(last_move_key[last_move_key_space+2]), letterToColumn[last_move_key[last_move_key_space+1]]) # "1 c1c3"
        # oldPos = (8-int(last_move_key[last_move_key_space+4]), letterToColumn[last_move_key[last_move_key_space+3]])

        # self.board[newPos] = self.board[oldPos]
        # self.board.pop(oldPos)

        

        # x0 = ((last_move.getTo() % 8) * self.size) + int(self.size/2) + self.offset_x
        # y0 = ((last_move.getTo() // 8) * self.size) + int(self.size/2) + self.offset_y
        # x1 = ((last_move.getFrom() % 8) * self.size) + int(self.size/2) + self.offset_x
        # y1 = ((last_move.getFrom() // 8) * self.size) + int(self.size/2) + self.offset_y

        # for i in range(self.animationFps+1):
        #     self.canvas.coords(self.board[newPos].uniqueCode, x0+((x1-x0)/self.animationFps)*i, y0+((y1-y0)/self.animationFps)*i)
        #     self.canvas.update()

        # if len(self.poppedEnpassant) > 0:
        #     self.enpassant.append(self.poppedEnpassant.pop())
        # if int(last_move_key[len(last_move_key)-1]):
        #     oldPos = (8-int(last_move_key[len(last_move_key)-5]), letterToColumn[last_move_key[len(last_move_key)-6]])
        # if last_move_piece:
        #     self.board[oldPos] = last_move_piece
        #     self.drawPiece(self.board[oldPos], oldPos[0], oldPos[1])
        
        # self.move_log.pop(last_move_key)
        # if (newPos == (7, 4) and oldPos == (7, 2)):
        #     self.restoreMove(event)
        #     self.white_queenside_castle = True
        #     self.white_kingside_castle = True
        # if (newPos == (7, 4) and oldPos == (7, 6)):
        #     self.restoreMove(event)
        #     self.white_queenside_castle = True
        #     self.white_kingside_castle = True
        # if (newPos == (0, 4) and oldPos == (0, 2)):
        #     self.restoreMove(event)
        #     self.black_queenside_castle = True
        #     self.black_kingside_castle = True
        # if (newPos == (0, 4) and oldPos == (0, 6)):
        #     self.restoreMove(event)
        #     self.black_queenside_castle = True
        #     self.black_kingside_castle = True
        # self.current_player = int(last_move_key[len(last_move_key)-3])

    # def addmovetorestore(self, oldPos: tuple, newPos: tuple, eatenPiece: Piece, player: int, enpassant: bool, enpassant_capture_square: tuple = None):
    #     if enpassant_capture_square:
    #         new_move_key = f"{len(self.move_log.keys())+1} {columnToLetter[oldPos[1]]}{8-oldPos[0]}{columnToLetter[newPos[1]]}{8-newPos[0]} {columnToLetter[enpassant_capture_square[1]]}{8-enpassant_capture_square[0]} {player} {1 if enpassant else 0}"
    #     else:
    #         new_move_key = f"{len(self.move_log.keys())+1} {columnToLetter[oldPos[1]]}{8-oldPos[0]}{columnToLetter[newPos[1]]}{8-newPos[0]} {player} {1 if enpassant else 0}"

    #     self.move_log[new_move_key] = eatenPiece
        

    def findPiece(self, uniqueCode: str):
        for key in self.board:
            if self.board[key].uniqueCode == uniqueCode:
                return key

    def drawGUI(self):
        offset_y = int(self.window_height * 0.10)
        offset_x = int(self.offset_x * 0.10)

        rect_width = (self.offset_x - 2*offset_x)
        rect_height = (self.window_height - 2*offset_y)

        self.gui["offx"] = offset_x
        self.gui["offy"] = offset_y
        self.gui["width"] = rect_width
        self.gui["height"] = rect_height

        gui_rect = RoundRectangle(self.canvas, offset_x, offset_y, offset_x + rect_width, offset_y + rect_height, fill="#262522", tags=("sideboard",))
        dividing_line = self.canvas.create_line(offset_x + int(rect_width * 0.05), offset_y + int(rect_height * 0.10), offset_x + int(rect_width * (0.05 + 0.9)), offset_y + int(rect_height * 0.10), fill="#3C3B39")

        engine_text = self.canvas.create_text(offset_x + int(rect_width * 0.26), offset_y + int(rect_height * 0.055), anchor=tk.E, font=(self.gui_font, 14), text="", fill="#999999", tags=("engine_text"))
    
        #move_log = self.canvas.create_text()

    def drawBoard(self):
        self.canvas.delete("all")

        # for sq in range(64):
        #     if (sq // 8) % 2 == 0:
        #         color = self.board
        
        color = self.board_color_dark
        for row in range(8):
            color = self.board_color_light if color == self.board_color_dark else self.board_color_dark
            for col in range(8):
                x1 = self.offset_x + (col * self.size) 
                y1 = self.offset_y + (row * self.size) 
                x2 = x1 + self.size
                y2 = y1 + self.size
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="white", fill=color, tags=("sq%02dn" % ((7 - row)*8 + col), "square"))
                color = self.board_color_light if color == self.board_color_dark else self.board_color_dark
        
        for sq, piece in self.board.items():
            self.drawPiece(piece, sq)
                # if (row == 0 or row == 7):
                #     x1 = self.offset_x + ((col+1) * self.size) 
                #     y1 = self.offset_y + ((row + (2 if row == 7 else 0)) * self.size) 
                #     x2 = x1 + (-1 * (self.size + 20))
                #     y2 = y1 + (-1 * (self.size + 20))
                #     label = tk.Label(self.canvas, text=f"{columnToLetter[col]}", background=self.canvas["background"], fg="#FFFFFF", font=20).place(x=(x1 + x2)/2, y=(y1 + y2)/2)
                # if (col == 0 or col == 7):
                #     x1 = self.offset_x + ((col + (2 if col == 7 else 0)) * self.size) 
                #     y1 = self.offset_y + ((row+1) * self.size) 
                #     x2 = x1 + (-1 * (self.size + 20))
                #     y2 = y1 + (-1 * (self.size + 20))
                #     label = tk.Label(self.canvas, text=f"{8-row}", background=self.canvas["background"], fg="#FFFFFF", font=20).place(x=(x1 + x2)/2, y=(y1 + y2)/2)
        

        if self.promotion_ui.get():
            self.showPromotionUI()
        # if self.promotion_ui and self.current_player == WHITE:
        #     for i in range(4):
        #         x1 = self.offset_x + (i * self.size)
        #         x0 = self.offset_x + (i * self.size) + (self.size//2)
        #         y1 = self.offset_y- self.size - 5
        #         y0 = y1 + (self.size//2)
        #         x2 = x1 + self.size
        #         y2 = y1 + self.size
        #         self.canvas.create_rectangle(x1, y1, x2, y2, outline="white", fill=self.board_color_light, tags=(f"{i}", "promotion", "square"))
        #         self.canvas.create_image(x0, y0, image=self.auziliary_pieces[4+i], tags=("promotion", "pieces"), anchor="c")
        #         self.canvas.tag_raise("pieces")
        #         self.canvas.tag_lower("squares")
        # if self.promotion_ui and self.current_player == BLACK:
        #     for i in range(4):
        #         x1 = self.offset_x + (i * self.size)
        #         x0 = self.offset_x + (i * self.size) + (self.size//2)
        #         y1 = self.offset_y+ 5 + (8 * self.size)
        #         y0 = y1 + (self.size//2)
        #         x2 = x1 + self.size
        #         y2 = y1 + self.size
        #         self.canvas.create_rectangle(x1, y1, x2, y2, outline="white", fill=self.board_color_light, tags=(f"{i}", "promotion", "square"))
        #         self.canvas.create_image(x0, y0, image=self.auziliary_pieces[i], tags=("promotion", "pieces"), anchor="c")
        #         self.canvas.tag_raise("pieces")
        #         self.canvas.tag_lower("squares")

        self.canvas.tag_lower("square")
        self.canvas.tag_raise("piece")
        
    def loadBoard(self, fen: str = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq"):
        self.board.clear()
        self.old_board.clear()
        self.moves.clear()
        self.enpassant.clear()
        self.selected_piece = None
        self.move_log.clear()

        self.engineSendCommand(SET, fen.encode())
        self.initFromFen(fen)
        
        self.drawBoard()
        self.drawGUI()

        # self.promotion_squares_white = False
        # self.promotion_squares_black = False
        if self.selected_piece:
            self.resetSelectedColor()
            self.resetSelectedMovesColor()


    # Load a board from a FEN notation
    def initFromFen(self, fen: str):
        self.board.clear()

        spacePos = fen.find(" ")
        if fen[spacePos+1] == "b":
            self.current_player = BLACK
        else:
            self.current_player = WHITE

        square = 56
        for i in range(len(fen)):
            if fen[i] == "/":
                square -= 16
            elif fen[i].isdigit():
                square += int(fen[i])
            elif fen[i] in fenToPiece and i < spacePos:
                self.createPiece(square, fenToPiece[fen[i]][0], fenToPiece[fen[i]][1])
                square += 1
            # if fen[i] == "K" and i > spacePos:
            #     self.white_kingside_castle = True
            # if fen[i] == "Q" and i > spacePos:
            #     self.white_queenside_castle = True
            # if fen[i] == "k" and i > spacePos:
            #     self.black_kingside_castle = True
            # if fen[i] == "q" and i > spacePos:
            #     self.black_queenside_castle = True
            # if i > spacePos + 2 and fen[i] in letterToColumn:
            #     offset = 1 if int(fen[i+1]) == 6 else -1
            #     self.enpassant.append(self.board[(8-int(fen[i+1])+offset, letterToColumn[fen[i]])].uniqueCode)
        # for key in self.board:
        #     self.drawPiece(self.board[key], key[0], key[1])
        # self.old_board = self.board.copy()
        # self.canvas.tag_raise("piece")
        # self.canvas.tag_lower("square")

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
        fenStr += " b" if self.current_player else " w"
        fenStr += " "
        if self.white_kingside_castle:
            fenStr += "K"
        if self.white_queenside_castle:
            fenStr += "Q"
        if self.black_kingside_castle:
            fenStr += "k"
        if self.black_queenside_castle:
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
    def drawPiece(self, piece: Piece, square: int):
        x0 = (((square % 8)) * self.size) + int(self.size/2) + self.offset_x
        y0 = self.parent.winfo_height() - (((square // 8)) * self.size) - int(self.size/2) - self.offset_y

        self.canvas.create_image(x0, y0, image=piece.image, tags=(piece.uniqueCode, "piece"), anchor=tk.CENTER)
        
    def movePiece(self, from_sq: int, to_sq: int):
        self.board[to_sq] = self.board.pop(from_sq)

        x0 = ((from_sq % 8) * self.size) + int(self.size/2) + self.offset_x
        y0 = ((7 - (from_sq // 8)) * self.size) + int(self.size/2) + self.offset_y
        x1 = ((to_sq % 8) * self.size) + int(self.size/2) + self.offset_x
        y1 = ((7 - (to_sq // 8)) * self.size) + int(self.size/2) + self.offset_y

        distance = dist((x0, y0), (x1, y1))
        frames = 240
        
        for i in range(frames):
            self.canvas.coords(self.board[to_sq].uniqueCode, x0+((x1-x0)/float(frames))*(i), y0+((y1-y0)/float(frames))*(i))
            self.canvas.update()
        #print(frames, x0, y0, x1, y1, self.board[to_sq].uniqueCode, self.canvas.find_withtag(self.board[to_sq].uniqueCode))
        self.canvas.coords(self.board[to_sq].uniqueCode, x1, y1)
    

    # Given an old position and a new one, place the piece in oldPos in newPos
    def playMove(self, move: Move, player: int, move_piece: int = MOVE):

        # TODO 1: add images of the eaten piece under or over the 
        
        self.engineSendCommand(MOVE, self.current_player.to_bytes(1) + move.toBytes())

        from_sq = move.getFrom()
        to_sq = move.getTo()
        flags = move.getFlags()

        self.game_ply += 1

        # if self.board[from_sq].fentype == "P" and newPos[0] == 0 or (self.board[oldPos].fentype == "p" and newPos[0] == 7):
        #     pawn_promotion = True 
        # if self.board[from_sq].uniqueCode == "0r":
        #     self.black_queenside_castle = False
        # elif self.board[from_sq].uniqueCode == "1r":
        #     self.black_kingside_castle = False
        # elif self.board[oldPos].uniqueCode == "0R":
        #     self.white_queenside_castle = False
        # elif self.board[oldPos].uniqueCode == "1R":
        #     self.white_kingside_castle = False
                         
        # if self.board[oldPos].fentype == "K":
        #     if newPos == (7, 2) and self.white_queenside_castle:
        #         self.playMove((7, 0), (7, 3))
        #     elif newPos == (7, 6) and self.white_kingside_castle:
        #         self.playMove((7, 7), (7, 5))
        #     self.white_kingside_castle = False
        #     self.white_queenside_castle = False
        # elif self.board[oldPos].fentype == "k":
        #     if newPos == (0, 2) and self.black_queenside_castle:
        #         self.playMove((0, 0), (0, 3))
        #     elif newPos == (0, 6) and self.black_kingside_castle:
        #         self.playMove((0, 7), (0, 5))
        #     self.black_kingside_castle = False
        #     self.black_queenside_castle = False

        if flags == OO:
            if self.current_player == BLACK:
                self.movePiece(SQSTR["h8"], SQSTR["f8"])
            else:
                self.movePiece(SQSTR["h1"], SQSTR["f1"])

        if flags == OOO:
            if self.current_player == BLACK:
                self.movePiece(SQSTR["a8"], SQSTR["d8"])
            else:
                self.movePiece(SQSTR["a1"], SQSTR["d1"])

        if flags == EN_PASSANT:
            if self.current_player == BLACK:
                captured_pawn = self.board.pop(to_sq + 8)
            else:
                captured_pawn = self.board.pop(to_sq - 8)

            self.move_log.append((move, captured_pawn))
            self.canvas.delete(captured_pawn.uniqueCode)  

        if flags == CAPTURE or flags >= PC_KNIGHT:
            captured_piece = self.board.pop(to_sq)
            self.move_log.append((move, captured_piece))
            self.canvas.delete(captured_piece.uniqueCode)
            print(captured_piece.fentype, flags)

        if move_piece == MOVE:
            self.movePiece(from_sq, to_sq)
        else:
            self.board[to_sq] = self.board.pop(from_sq)

        if flags >= PR_KNIGHT and flags != CAPTURE:
            if player == AI:
                self.promotePawn(to_sq, self.current_player, flagToType(flags))
            if player == HUMAN:
                self.showPromotionUI()

                self.canvas.wait_variable(self.promotion_ui)
                self.canvas.unbind("<Button-1>")
                self.canvas.bind("<Button-1>", self.onClick)

                self.promotePawn(to_sq, self.current_player, self.promotion_type)
            
        self.current_player ^= 1

        # offset = 1 if oldPos in self.board and self.board[oldPos].color else -1
        # try:
        #     for i in [1, -1]:
        #         if (oldPos[0], oldPos[1]+i) in self.board and self.board[oldPos].type == PAWN and newPos == (oldPos[0]+offset, oldPos[1]+i) and (oldPos[0], oldPos[1]+i) in self.board and self.board[(oldPos[0], oldPos[1]+i)].uniqueCode in self.enpassant and self.board[oldPos].color != self.board[(oldPos[0], oldPos[1]+i)].color:
        #             self.canvas.delete(self.board[(oldPos[0], oldPos[1]+i)].uniqueCode)
        #             self.addmovetorestore(oldPos, newPos, self.board[(oldPos[0], oldPos[1]+i)], self.current_player, True, (oldPos[0], oldPos[1]+i))
        #             pawn_enpassant = True
        #             self.board.pop((oldPos[0], oldPos[1]+i))
        # except:
        #     pass
        
        # if len(self.enpassant) > 0:
        #     self.poppedEnpassant.append(self.enpassant.pop())
        # try:
        #     if self.board[oldPos].type == PAWN and newPos[0] == oldPos[0]+2*offset:
        #         self.enpassant.append(self.board[oldPos].uniqueCode)
        # except:
        #     pass
        # if newPos in self.board:
        #     self.canvas.delete(self.board[newPos].uniqueCode)
        #     captured_piece = self.board[newPos]
        # else:
        #     captured_piece = None
        
        # tempSquare = self.board[oldPos]
        # self.board.pop(oldPos)
        # self.board[newPos] = tempSquare

        # if not pawn_enpassant:
        #     self.addmovetorestore(oldPos, newPos, captured_piece, self.current_player, False)
                
        # x0 = (oldPos[1] * self.size) + int(self.size/2) + self.offset_x
        # y0 = (oldPos[0] * self.size) + int(self.size/2) + self.offset_y
        # x1 = (newPos[1] * self.size) + int(self.size/2) + self.offset_x
        # y1 = (newPos[0] * self.size) + int(self.size/2) + self.offset_y
        
        # for i in range(self.animationFps-80):
        #     self.canvas.coords(self.board[newPos].uniqueCode, x0+((x1-x0)/(self.animationFps-80))*(i), y0+((y1-y0)/(self.animationFps-80))*(i))
        #     self.canvas.update()
        # self.canvas.coords(self.board[newPos].uniqueCode, x1, y1)

        # if pawn_promotion and not self.promotion_ai:
        #     self.promotePawn(newPos, self.current_player) 


    # Returns the clicked-on tile's coordinates 
    def getMouseClickPos(self, event) -> tuple:
        try:
            item_below = self.canvas.find_overlapping(event.x,event.y,event.x,event.y)[0]

            return (int(self.canvas.itemcget(item_below, "tags")[2:4]))
        except:
            return SQSTR["None"]

    # def onReleaseMove(self, startCoords: tuple, dropCoords: tuple):
    #     pawn_promotion = False
    #     pawn_enpassant = False
    #     if (self.board[startCoords].fentype == "P" and dropCoords[0] == 0) or (self.board[startCoords].fentype == "p" and dropCoords[0] == 7):
    #         pawn_promotion = True
    #     if self.board[startCoords].uniqueCode == "0r":
    #         self.black_queenside_castle = False
    #     elif self.board[startCoords].uniqueCode == "1r":
    #         self.black_kingside_castle = False
    #     elif self.board[startCoords].uniqueCode == "0R":
    #         self.white_queenside_castle = False
    #     elif self.board[startCoords].uniqueCode == "1R":
    #         self.white_kingside_castle = False
    #     if self.board[startCoords].fentype == "K":
    #         if dropCoords == (7, 2) and self.white_queenside_castle:
    #             self.playMove((7, 0), (7, 3))
    #         elif dropCoords == (7, 6) and self.white_kingside_castle:
    #             self.playMove((7, 7), (7, 5))
    #         self.white_kingside_castle = False
    #         self.white_queenside_castle = False
    #     elif self.board[startCoords].fentype == "k":
    #         if dropCoords == (0, 2) and self.black_queenside_castle:
    #             self.playMove((0, 0), (0, 3))
    #         elif dropCoords == (0, 6) and self.black_kingside_castle:
    #             self.playMove((0, 7), (0, 5))
    #         self.black_kingside_castle = False
    #         self.black_queenside_castle = False
    #     offset = 1 if self.board[startCoords].color else -1
    #     try:
    #         for i in [1, -1]:
    #             if (startCoords[0], startCoords[1]+i) in self.board and self.board[startCoords].type == PAWN and dropCoords == (startCoords[0]+offset, startCoords[1]+i) and (startCoords[0], startCoords[1]+i) in self.board and self.board[(startCoords[0], startCoords[1]+i)].uniqueCode in self.enpassant and self.board[startCoords].color != self.board[(startCoords[0], startCoords[1]+i)].color:
    #                 self.canvas.delete(self.board[(startCoords[0], startCoords[1]+i)].uniqueCode)
    #                 self.addmovetorestore(startCoords, dropCoords, self.board[(startCoords[0], startCoords[1]+i)], self.current_player, True, (startCoords[0], startCoords[1]+i))
    #                 pawn_enpassant = True
    #                 self.board.pop((startCoords[0], startCoords[1]+i))
    #     except:
    #         pass

    #     if len(self.enpassant) > 0:
    #         self.poppedEnpassant.append(self.enpassant.pop())
    #     try:
    #         if self.board[startCoords].type == PAWN and dropCoords[0] == startCoords[0]+2*offset:
    #             self.enpassant.append(self.board[startCoords].uniqueCode)
    #     except:
    #         pass
    #     if dropCoords in self.board:
    #         self.canvas.delete(self.board[dropCoords].uniqueCode)
    #         captured_piece = self.board[dropCoords]
    #     else:
    #         captured_piece = None

    #     if not pawn_enpassant:
    #         self.addmovetorestore(startCoords, dropCoords, captured_piece, self.current_player, False)

    #     x1 = (dropCoords[1] * self.size) + int(self.size/2) + self.offset_x
    #     y1 = (dropCoords[0] * self.size) + int(self.size/2) + self.offset_y
    #     self.canvas.delete(self.board[dropCoords].uniqueCode) if dropCoords in self.board else None
    #     self.board[dropCoords] = self.board[startCoords]
    #     self.board.pop(startCoords)
    #     self.canvas.coords(self.board[dropCoords].uniqueCode, x1, y1)
    #     if startCoords[0] % 2 != 0:
    #         self.canvas.itemconfig(f"{startCoords[0]}, {startCoords[1]}", fill=self.board_color_light) if (8*(startCoords[0]) + startCoords[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{startCoords[0]}, {startCoords[1]}", fill=self.board_color_dark)
    #     else:
    #         self.canvas.itemconfig(f"{startCoords[0]}, {startCoords[1]}", fill=self.board_color_dark) if (8*(startCoords[0]) + startCoords[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{startCoords[0]}, {startCoords[1]}", fill=self.board_color_light)
    #     for move in self.moves:
    #         if move[0] % 2 != 0:
    #             self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.board_color_light) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.board_color_dark)
    #         else:
    #             self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.board_color_dark) if (8*(move[0]) + move[1] + 1) % 2 == 0 else self.canvas.itemconfig(f"{move[0]}, {move[1]}", fill=self.board_color_light)
    #     if pawn_promotion:
    #         self.promotePawn(dropCoords, self.current_player)
    
    def setTileColor(self, tile: int, color: str):
        self.canvas.itemconfig(self.canvas.find_withtag("sq%02dn" % (tile))[0], fill=color)

    def resetSelectedColor(self):
        if (self.selected_piece // 8) % 2 == 0:
            self.setTileColor(self.selected_piece, self.board_color_dark) if (self.selected_piece % 8) % 2 == 0 else self.setTileColor(self.selected_piece, self.board_color_light)
        else:
            self.setTileColor(self.selected_piece, self.board_color_light) if (self.selected_piece % 8) % 2 == 0 else self.setTileColor(self.selected_piece, self.board_color_dark)

    def resetSelectedMovesColor(self, sq_array: list):
        for dest in sq_array:
            if (dest // 8) % 2 == 0:
                self.setTileColor(dest, self.board_color_dark) if (dest % 8) % 2 == 0 else self.setTileColor(dest, self.board_color_light)
            else:
                self.setTileColor(dest, self.board_color_light) if (dest % 8) % 2 == 0 else self.setTileColor(dest, self.board_color_dark)

    def setSelectedMovesColor(self, sq_array: list):
        for dest in sq_array:
            if (dest // 8) % 2 == 0:
                self.setTileColor(dest, alpha_blend({self.board_color_dark: 0.8, "#edda95": 0.8})) if (dest % 8) % 2 == 0 else self.setTileColor(dest, alpha_blend({self.board_color_light: 0.8, "#edda95": 0.8}))
            else:
                self.setTileColor(dest, alpha_blend({self.board_color_light: 0.8, "#edda95": 0.8})) if (dest % 8) % 2 == 0 else self.setTileColor(dest, alpha_blend({self.board_color_dark: 0.8, "#edda95": 0.8}))    

    def findMove(self, moves: list, from_sq: int = 0, to_sq: int = 0, flags: int = 0) -> Move:
        mask = 0

        if to_sq:
            mask |= (0b111111 << 0)
        if from_sq:
            mask |= (0b111111 << 6) 
        if flags:
            mask |= (0b1111   << 12)

        for move in moves:
            if (move.getInternal() & mask) == ((flags << 12) | (from_sq << 6) | to_sq):
                return move
            
        return Move(0)

    def onRelease(self, event):
        if self.selected_piece != None:
            destinations = [move.getTo() for move in self.moves]
            drop_sq = self.getMouseClickPos(event)
            if drop_sq in destinations:
                self.resetSelectedColor()
                self.resetSelectedMovesColor(destinations)

                move = self.findMove(self.moves, self.selected_piece, drop_sq)
                self.playMove(move, HUMAN, NOMOVE)

                x1 = ((drop_sq % 8) * self.size) + int(self.size/2) + self.offset_x
                y1 = ((7 - (drop_sq // 8)) * self.size) + int(self.size/2) + self.offset_y
                self.canvas.coords(self.board[drop_sq].uniqueCode, x1, y1)

                if self.use_ai:
                    self.requestAIMove()

                self.moves.clear()
                self.selected_piece = None
                # self.onReleaseMove(self.selected_piece, dropCoords)
                # self.moves = []
                # self.selected_piece = ()
                # self.current_player = int(not self.current_player)
            # elif self.selected_piece in self.board:
            #     x1 = (self.selected_piece[1] * self.size) + int(self.size/2) + self.offset_x
            #     y1 = (self.selected_piece[0] * self.size) + int(self.size/2) + self.offset_y
            #     self.canvas.coords(self.board[self.selected_piece].uniqueCode, x1, y1)
            # self.animations_done = True 
            else:
                x1 = ((self.selected_piece % 8) * self.size) + int(self.size/2) + self.offset_x
                y1 = ((7 - (self.selected_piece // 8)) * self.size) + int(self.size/2) + self.offset_y
                self.canvas.coords(self.board[self.selected_piece].uniqueCode, x1, y1)
            
            self.canvas.unbind("<ButtonRelease-1>")
        
    def onDrag(self, event):
        self.canvas.bind("<ButtonRelease-1>", self.onRelease)
        try:
            if self.selected_piece != None:
                self.canvas.tag_raise(self.board[self.selected_piece].uniqueCode)
                self.canvas.coords(self.board[self.selected_piece].uniqueCode, event.x, event.y)
                pass
        except:
            pass

    # What to do on clicks        
    def onClick(self, event):
        if self.selected_piece == None:
            click_sq = self.getMouseClickPos(event)

            if click_sq in self.board and self.board[click_sq].color == self.current_player:
                self.selected_piece = click_sq
                
                
                self.moves = self.generateLegalMoves(click_sq)
                destinations = [move.getTo() for move in self.moves]
                self.setTileColor(self.selected_piece, self.selected_piece_color)

                self.setSelectedMovesColor(destinations)
        else:
            click_sq = self.getMouseClickPos(event)
            destinations = [move.getTo() for move in self.moves]

            # If the user clicks on another piece
            if click_sq not in destinations and click_sq in self.board and self.board[click_sq].color == self.current_player:
                self.resetSelectedColor()

                self.resetSelectedMovesColor(destinations)

                self.selected_piece = click_sq
                self.setTileColor(click_sq, self.selected_piece_color)
                    
                
                self.moves = self.generateLegalMoves(self.selected_piece)
                destinations = [move.getTo() for move in self.moves]
            
                
                self.setSelectedMovesColor(destinations)

            # If the user clicks on a move
            elif click_sq in destinations:
                self.canvas.unbind("<ButtonRelease-1>")
                self.resetSelectedColor()
        
                self.resetSelectedMovesColor(destinations)

                move = self.findMove(self.moves, self.selected_piece, click_sq)
                self.playMove(move, HUMAN)
                self.moves.clear()
                self.selected_piece = None

                if self.use_ai:
                    self.requestAIMove()

                self.canvas.bind("<ButtonRelease-1>", self.onRelease)
                
            # If the user clicks on an empty square
            else:
                self.resetSelectedColor()
                self.resetSelectedMovesColor(destinations)
                self.selected_piece = None

    def generateAllLegalMoves(self, side_color: int):
        move_codes = self.engineSendCommand(GENMOVES, (100 + side_color).to_bytes(1))

        moves = arrayU16toMoves(move_codes)

        # allMoves = []
        # for key in board:
        #     moves = []
        #     if board[key].type == PAWN and board[key].color == side_color:
        #         moves = self.pawnMoves(key[0], key[1], board)
        #     if board[key].type == KNIGHT and board[key].color == side_color:
        #         moves = self.knightMoves(key[0], key[1], board)
        #     if board[key].type == BISHOP and board[key].color == side_color:
        #         moves = self.bishopMoves(key[0], key[1], board)
        #     if board[key].type == ROOK and board[key].color == side_color:
        #         moves = self.rookMoves(key[0], key[1], board)
        #     if board[key].type == QUEEN and board[key].color == side_color:
        #         moves = self.queenMoves(key[0], key[1], board)
        #     if board[key].type == KING and board[key].color == side_color:
        #         moves = self.kingMoves(key[0], key[1], board)
            
        #     for move in moves:
        #         copy_board = board.copy()
        #         copy_board[move] = copy_board[key]
        #         copy_board.pop(key)
        #         if not self.inCheck(copy_board, side_color) and 0 < move[0] < 8 and 0 < move[1] < 8:
        #             allMoves.append(move)

        return moves
    
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


    def inCheck(self, color: int) -> bool:
        return int(self.engineSendCommand(INCHECK, color))              

    def generateLegalMoves(self, square: int) -> list:
        moves = []

        if square > 63 or square < 0:
            return moves
                 
        # pieceType = self.board[square].type
        # pieceColor = self.board[square].color

        # if pieceType == PAWN:
        #     moves = self.pawnMoves(row, col, board)

        # if pieceType == KNIGHT:
        #     moves = self.knightMoves(row, col, board)

        # if pieceType == ROOK:
        #     moves = self.rookMoves(row, col, board)

        # if pieceType == BISHOP:
        #     moves = self.bishopMoves(row, col, board)

        # if pieceType == QUEEN:
        #     moves = self.queenMoves(row, col, board)
        
        # if pieceType == KING:
        #     moves = self.kingMoves(row, col, board)

        
        # finalMoves = []
        # for move in moves:
        #     tempBoard = self.board.copy()
        #     tempPiece = tempBoard[(row, col)]
        #     tempBoard[move] = tempPiece
        #     tempBoard.pop((row, col))
        #     if not self.inCheck(tempBoard, pieceColor):
        #         finalMoves.append(move)

        move_codes = self.engineSendCommand(GENMOVES, square.to_bytes(1))
        moves = arrayU16toMoves(move_codes)
        
        return moves
    
    def onPromotionClick(self, event):
        try:
            clicked_square = int(self.canvas.itemcget(self.canvas.find_overlapping(event.x,event.y,event.x,event.y)[0], "tags")[0])
        
            if clicked_square == 0:
                # self.canvas.delete(self.board[pawnPos].uniqueCode)
                # self.board.pop(pawnPos)
                # self.addpiece(self.createPiece("q") if color else self.createPiece("Q"), pawnPos[0], pawnPos[1])
                # self.drawPiece(self.board[pawnPos], pawnPos[0], pawnPos[1])
                # self.canvas.delete("promotion")
                # self.promotion_ui.set(True)
                # self.promotion_squares_white=False
                # self.promotion_squares_black=False

                self.promotion_type = QUEEN
            elif clicked_square == 1:
                # self.canvas.delete(self.board[pawnPos].uniqueCode)
                # self.board.pop(pawnPos)
                # self.addpiece(self.createPiece("b") if color else self.createPiece("B"), pawnPos[0], pawnPos[1])
                # self.drawPiece(self.board[pawnPos], pawnPos[0], pawnPos[1])
                # self.canvas.delete("promotion")
                # self.promotion_ui.set(True)
                # self.promotion_squares_white=False
                # self.promotion_squares_black=False

                self.promotion_type = BISHOP
            elif clicked_square == 2:
                # self.canvas.delete(self.board[pawnPos].uniqueCode)
                # self.board.pop(pawnPos)
                # self.addpiece(self.createPiece("r") if color else self.createPiece("R"), pawnPos[0], pawnPos[1])
                # self.drawPiece(self.board[pawnPos], pawnPos[0], pawnPos[1])
                # self.canvas.delete("promotion")
                # self.promotion_ui.set(True)
                # self.promotion_squares_white=False
                # self.promotion_squares_black=False

                self.promotion_type = ROOK
            elif clicked_square == 3:
                # self.canvas.delete(self.board[pawnPos].uniqueCode)
                # self.board.pop(pawnPos)
                # self.addpiece(self.createPiece("n") if color else self.createPiece("N"), pawnPos[0], pawnPos[1])
                # self.drawPiece(self.board[pawnPos], pawnPos[0], pawnPos[1])
                # self.canvas.delete("promotion")
                # self.promotion_ui.set(True)
                # self.promotion_squares_white=False
                # self.promotion_squares_black=False

                self.promotion_type = KNIGHT
            else:
                return
            
            self.canvas.delete("promotion")
            self.promotion_ui.set(False) 
        except:
            pass

    def showPromotionUI(self):
        self.canvas.unbind("<Button-1>")
        self.canvas.bind("<Button-1>", lambda event: self.onPromotionClick(event))
        self.promotion_ui.set(True)
        
        if self.current_player == WHITE:
            for i in range(4):
                x1 = self.offset_x + (i * self.size)
                y1 = self.offset_y - self.size - 5

                x0 = self.offset_x + (i * self.size) + (self.size//2)
                y0 = y1 + (self.size//2)
                
                x2 = x1 + self.size
                y2 = y1 + self.size
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="white", fill=self.board_color_light, tags=(f"{i}", "promotion", "square"))
                self.canvas.create_image(x0, y0, image=self.fenToImage[self.fenToImage.keys[7+i]], tags=("promotion", "piece"), anchor="c")
                self.canvas.tag_raise("piece")
                self.canvas.tag_lower("square")
        if self.current_player == BLACK:
            for i in range(4):
                x1 = self.offset_x + (i * self.size)
                y1 = self.offset_y + 5 + (8 * self.size)

                x0 = self.offset_x + (i * self.size) + (self.size//2)
                y0 = y1 + (self.size//2)

                x2 = x1 + self.size
                y2 = y1 + self.size
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="white", fill=self.board_color_light, tags=(f"{i}", "promotion", "square"))
                self.canvas.create_image(x0, y0, image=self.fenToImage[self.fenToImage.keys[1+i]], tags=("promotion", "piece"), anchor="c")
                self.canvas.tag_raise("piece")
                self.canvas.tag_lower("square")

    def promotePawn(self, pawn_sq: int, player_color: int, promotion_type: int):

        self.canvas.delete(self.board[pawn_sq].uniqueCode)
        self.createPiece(pawn_sq, promotion_type, player_color)
        self.drawPiece(self.board[pawn_sq], pawn_sq)
        
        # self.canvas.unbind("<Button-1>")
        # self.canvas.bind("<Button-1>", lambda event: self.onPromotionClick(event, pawnPos, color))
        
        # if color == WHITE:
        #     self.promotion_squares_white = True
        #     for i in range(4):
        #         x1 = self.offset_x + (i * self.size)
        #         x0 = self.offset_x + (i * self.size) + (self.size//2)
        #         y1 = self.offset_y- self.size - 5
        #         y0 = y1 + (self.size//2)
        #         x2 = x1 + self.size
        #         y2 = y1 + self.size
        #         self.canvas.create_rectangle(x1, y1, x2, y2, outline="white", fill=self.board_color_light, tags=(f"{i}", "promotion", "square"))
        #         self.canvas.create_image(x0, y0, image=self.auziliary_pieces[4+i], tags=("promotion", "pieces"), anchor="c")
        #         self.canvas.tag_raise("pieces")
        #         self.canvas.tag_lower("squares")
        # if color == BLACK:
        #     self.promotion_squares_black = True
        #     for i in range(4):
        #         x1 = self.offset_x + (i * self.size)
        #         x0 = self.offset_x + (i * self.size) + (self.size//2)
        #         y1 = self.offset_y+ 5 + (8 * self.size)
        #         y0 = y1 + (self.size//2)
        #         x2 = x1 + self.size
        #         y2 = y1 + self.size
        #         self.canvas.create_rectangle(x1, y1, x2, y2, outline="white", fill=self.board_color_light, tags=(f"{i}", "promotion", "square"))
        #         self.canvas.create_image(x0, y0, image=self.auziliary_pieces[i], tags=("promotion", "pieces"), anchor="c")
        #         self.canvas.tag_raise("pieces")
        #         self.canvas.tag_lower("squares")

        # self.canvas.wait_variable(self.promotion_ui)
        # self.canvas.bind("<Button-1>", self.onClick)

    # def promotePawn_ai(self, pawnPos: tuple, promotion: str):
    #     self.canvas.delete(self.board[pawnPos].uniqueCode)
    #     self.addpiece(self.createPiece(promotion if not self.current_player else promotion.lower()), pawnPos[0], pawnPos[1])
    #     self.drawPiece(self.board[pawnPos], pawnPos[0], pawnPos[1])

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
            if board[key].fentype == "r" and (row, col) == (0, 4) and board[key].color == self.current_player and key in [(row,0), (row,7)]:
                blocked = False
                offset = -1 if key[1] < 4 else 1
                for element in board:
                    blocked = 0 < element[1] < 4 and element[0] == row if key[1] == 0 else 4 < element[1] < key[1] and element[0] == row
                    if blocked:
                        break
                if key == (row, 0) and not self.black_queenside_castle:
                    blocked = True
                elif key == (row,7) and not self.black_kingside_castle:
                    blocked = True
                if not blocked and (row, 4 + offset) in moves:
                    moves.append((row, 4 + 2*offset))
            if board[key].fentype == "R" and (row, col) == (7, 4) and board[key].color == self.current_player and key in [(row,0), (row,7)]:
                blocked = False
                offset = -1 if key[1] < 4 else 1
                for element in board:
                    blocked = 0 < element[1] < 4 and element[0] == row if key[1] == 0 else 4 < element[1] < key[1] and element[0] == row
                    if blocked:
                        break
                if key == (row, 0) and not self.white_queenside_castle:
                    blocked = True
                elif key == (row,7) and not self.white_kingside_castle:
                    blocked = True
                if not blocked and (row, 4 + offset) in moves:
                    moves.append((row, 4 + 2*offset))
        
        return moves
                


if __name__ == "__main__":
    board = Board(debug = True, start_player=WHITE, ai = True);
    board.mainloop()
