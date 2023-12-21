import win32file, pywintypes

import logging
import subprocess
import random
import string

from ctypes import windll
from typing import List
from Move import * 

BUF_LEN = 1024
OK = 0x100

BLACK = 1
WHITE = 0

ACK = 0x06

# List of available command
NONE = 0x00
SET = 0x01          # Payload: fen string (converted in bytes)
MOVE = 0x02         # Payload: moving player (1 BYTE) + Move (2 BYTES)
UNDO = 0x03         # Payload: nothing (undoes the last player move)
MOVEREQ = 0x04      # Payload: AI Level (1 BYTE) + AI Color (1 BYTE)
GENMOVES = 0x05     # Payload: Square (1 BYTE) [100 + color to generate all the legal move for a specific player] of the piece to generate moves of
INCHECK = 0x06      # Payload: Color (1 BYTE)
FEN = 0x07          # Payload: None
EXIT = 0x08         # Payload: None

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
    def __init__(self, exe_path: str):
        self.engine_path = exe_path
        self.engine_pipe_id: str = self.generate_unique_id()
        self.engine_handle, self.engine_process = self.initialize_engine(exe_path)

    def initialize_engine(self, exe_path: str):
        try:
            engine_process = subprocess.Popen([exe_path, self.engine_pipe_id])

            engine_handle = self.get_handle_from_pipe(self.engine_pipe_id)

            return engine_handle, engine_process
        except:
            pass
        
        return None, None

    def generate_unique_id(self) -> str:
        return ''.join(random.choices(string.ascii_uppercase + string.digits, k = 8))

    def attach(self, pipe_id: str):
        self.engine_handle = self.get_handle_from_pipe(pipe_id)

    def get_handle_from_pipe(pipe_id: str):
        try:
            # Create the pipe for communication
            handle = win32file.CreateFile(pipe_id, 
                                          win32file.GENERIC_READ | win32file.GENERIC_WRITE,
                                          0,
                                          None,
                                          win32file.OPEN_EXISTING,
                                          0,
                                          None)
            
            return handle
        except pywintypes.error as e:
            print(e)
            if e.args[0] == 2:
                print("[ERROR] No pipe available")
            elif e.args[0] == 109:
                print("[ERROR] Could not open pipe, exiting")
                exit(0) 
    
    def set(self, fen_str: str):
        status = self.send_command(SET, fen_str.encode())

        if status != OK:
            logging.warn("[W] Could not set the engine's postion")

    def move(self, playing_color: int, move: Move):
        status = self.send_command(SET, bytearray([playing_color.to_bytes(1) + move.getInternal()]))

        if status != OK:
            logging.warn("[W] Could not transmit move to engine")

    def undo(self):
        status = self.send_command(UNDO)

        if status != OK:
            logging.warn("[W] Could not undo last move")

    def ai_move(self, ai_level: int, ai_color: int) -> Move:
        move_internal = self.send_command(MOVEREQ, bytearray([ai_level.to_bytes(1), ai_color.to_bytes(1)]))

        return Move(move_internal)
    
    def generate_moves(self, square: int) -> List[Move]:
        move_array = self.send_command(GENMOVES, square.to_bytes(1))

        return arrayU16toMoves(move_array)
    
    def in_check(self, color: int) -> bool:
        result = self.send_command(INCHECK, color.to_bytes(1))

        return bool(result)
    
    def fen(self) -> str:
        fen_str: str = self.send_command(FEN)

        return fen_str.decode()
            
    def send_command(self, command_id: int, payload: bytearray, is_async: bool = False) -> bytearray:
        if command_id > EXIT or command_id < NONE: 
            return None

        try:
            _, _ = win32file.WriteFile(self.engine_handle, command_id.to_bytes(1) + payload)
        
        except pywintypes.error as e:
            logging.error(f"[E] Could not send command! ERROR CODE: {e.args[0]}")
        
        if not is_async:
            response = win32file.ReadFile(self.engine_handle, BUF_LEN)

            return response
    
        return None


    def recv_response(self) -> bytearray: 
        try:
            _, _ = win32file.ReadFile(self.engine_handle, BUF_LEN)

        except pywintypes.error as e:
            logging.error(f"[E] Could not receive response! ERROR CODE: {e.args[0]}")
        