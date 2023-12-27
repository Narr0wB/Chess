import win32file, pywintypes, win32console

import logger
import subprocess
import random
import string
import time
import atexit
import struct

from ctypes import windll
from typing import List
from Move import * 

from colorama import *

START = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

COMMAND_STR_B = ["NONE", "SET", "MOVE", "UNDO", "MOVEREQ", "GENMOVES", "CHECK", "CHECKMATE", "FEN", "COLOR", "EXIT"]
COLOR_STR_B = ["WHITE", "BLACK"]

BUF_LEN = 1024
ACK = 0xFF

BLACK = 1
WHITE = 0

# List of available commands
NONE      = 0x00    # Payload: None
SET       = 0x01    # Payload: Fen string ([FEN_STR_LEN] BYTES)
MOVE      = 0x02    # Payload: Moving player (1 BYTE) + Move (2 BYTES)
UNDO      = 0x03    # Payload: None
MOVEREQ   = 0x04    # Payload: AI Level (1 BYTE) + AI Color (1 BYTE)
GENMOVES  = 0x05    # Payload: Square (1 BYTE) of the piece to generate moves of (100 + [COLOR] to generate all the legal move for a specific color)
CHECK     = 0x06    # Payload: Color (1 BYTE)
CHECKMATE = 0x07    # Payload: Color (1 BYTE)
FEN       = 0x08    # Payload: None
COLOR     = 0x09    # Payload: None
EXIT      = 0x0A    # Payload: None

# Structure of the command

#       HEADER            ------- Payload ([PAYLEN] BYTES)
#  ----------------      |
# |                |     v
# 0x00 | 0x00 0x00 | 0x00 ...
#   ^       ^
#   |       | 
#   |        --------------- Payload length (2 BYTES)
#    -------- Command identifier (1 BYTE)



class Engine():
    def __init__(self, exe_path: str = None, pipe_id: str = None):
        self.engine_path = exe_path
        self.engine_handle = None
        self.engine_process = None
        self.engine_pipe_id = pipe_id

        if (self.engine_pipe_id == None):
            self.engine_pipe_id: str = self.generate_unique_id()

        if (self.engine_path == None):
            self.attach(pipe_id)
        else:
            self.initialize_engine(exe_path)
        
        atexit.register(self.cleanup)

    def cleanup(self):
        if (self.engine_handle != None):
            self.exit()

    def initialize_engine(self, exe_path: str):      
        try:
            self.engine_process = subprocess.Popen([exe_path, self.engine_pipe_id], creationflags=subprocess.CREATE_NEW_CONSOLE)

        except Exception as e:
            logger.error(f"Could not start engine {e}")

        time.sleep(1.5)

        self.attach(self.engine_pipe_id)
        
    def generate_unique_id(self) -> str:
        return ''.join(random.choices(string.ascii_uppercase + string.digits, k = 8))

    def attach(self, pipe_id: str):
        self.engine_handle = self.get_handle_from_pipe(pipe_id)

    def get_handle_from_pipe(self, pipe_id: str):
        try:
            # Create the pipe for communication

            handle = win32file.CreateFile(f"\\\\.\\pipe\\{pipe_id}", 
                                          win32file.GENERIC_READ | win32file.GENERIC_WRITE,
                                          0,
                                          None,
                                          win32file.OPEN_EXISTING,
                                          0,
                                          None)
            
            return handle
        except pywintypes.error as e:
            if e.args[0] == 2:
                logger.error("No pipe available")
            elif e.args[0] == 109:
                logger.error("Could not open pipe, exiting")
                exit(0) 

        return None
    
    def set(self, fen_str: str):
        self.send_command(SET, fen_str.encode())

    def move(self, playing_color: int, move: Move):
        self.send_command(MOVE, bytearray(playing_color.to_bytes(1) + move.getInternal().to_bytes(2, "little")))

    def undo(self):
        self.send_command(UNDO)

    def ai_move(self, ai_level: int, ai_color: int) -> Move:
        self.send_command(MOVEREQ, bytearray([ai_level, ai_color]))
        move_internal = self.recv_response()

        return Move(int.from_bytes(move_internal, "little"))
    
    def generate_moves(self, square: int) -> List[Move]:
        self.send_command(GENMOVES, square.to_bytes(1))
        move_array = self.recv_response()

        return arrayU16toMoves(move_array)
    
    def in_check(self, color: int) -> bool:
        self.send_command(CHECK, color.to_bytes(1))
        result = self.recv_response()

        return bool(int.from_bytes(result, "little"))
    
    def fen(self) -> str:
        self.send_command(FEN)
        fen_str = self.recv_response()

        return fen_str.decode()
    
    def checkmate(self, color: int):
        self.send_command(CHECKMATE, color.to_bytes(1))
        result = self.recv_response()

        return bool(int.from_bytes(result, "little"))
    
    def color(self):
        self.send_command(COLOR)
        result = self.recv_response()

        return int.from_bytes(result, "little")
    
    def exit(self):
        self.send_command(EXIT)

        self.engine_handle = None
            
    def send_command(self, command_id: int, payload: bytearray = bytearray(), attempt: int = 0) -> int:
        if command_id > EXIT or command_id < NONE: 
            return -2
        
        if attempt == 3:
            return -3

        try:
            # According to the payload header specification
            command_header = struct.pack("<HH", command_id, len(payload))

            # for b in command_header: print(b)

            _, _ = win32file.WriteFile(self.engine_handle, command_header + payload)
            _, rsp = win32file.ReadFile(self.engine_handle, 1)

            if int.from_bytes(rsp, "little") != ACK:
                self.send_command(command_id, payload, attempt + 1) 
        except pywintypes.error as e:
            logger.error(f"Could not send {COMMAND_STR_B[command_id]} command! ERROR CODE: {e.args[0]}")
            return -1
    
        return 0


    def recv_response(self) -> bytearray: 
        try:
            _, rsp = win32file.ReadFile(self.engine_handle, BUF_LEN)

            _, _ = win32file.WriteFile(self.engine_handle, ACK.to_bytes(1))

            return rsp
        except pywintypes.error as e:
            logger.error(f"Could not receive response! ERROR CODE: {e.args[0]}")
        