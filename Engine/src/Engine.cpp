
#include "Engine.h"

#define START_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/ w KQkq"
#define DEBUG_POSITION "rnb2b2/2p1k1p1/1p1q4/3p4/P7/8/P4PPP/RNB1KBNR w KQ - 0 9"

#define PIPE_NAME TEXT("\\\\.\\pipe\\chess")

namespace Engine {
	void Application::Run() {

		while (!m_ShouldClose) {
			Command command = Read();

			switch (command.type) {
			case CommandType::SET: {
				std::string fen((char*) &command.args[1], command.arg_len);

				if (fen == "default")
					fen = START_POSITION;

				m_Position.reset();
				Position::set(fen, m_Position);
				Write((void*) "OK", 2);

				if (m_Debug)
					std::cout << "[DEBUG] Position set" << std::endl;
				break;
			}

			case CommandType::MOVE: {
				Color color = (Color)command.args[1];
				uint16_t move_internal = *(uint16_t*)&command.args[2];

				Move move(move_internal);
				m_MoveList.push_back(move);

				if (color == BLACK)
					m_Position.play<BLACK>(move);
				else
					m_Position.play<WHITE>(move);

				Write((void*) "OK", 2);
				break;
			}

			case CommandType::UNDO: {
				if (m_MoveList.empty())
					return;

				if (m_Position.getCurrentColor() == BLACK)
					m_Position.undo<WHITE>(m_MoveList.back());
				else
					m_Position.undo<BLACK>(m_MoveList.back());

				m_MoveList.pop_back();

				Write((void*) "OK", 2);
				break;
			}

			case CommandType::MOVEREQ: {
				u8 level = command.args[1];

				if (level > 6)
					level = 6;

				Color aiColor = static_cast<Color>(command.args[2]);
				Move move;

				if (aiColor == BLACK) {
					move = findBestMove<BLACK>(m_Position, level);
				}
				else {
					move = findBestMove<WHITE>(m_Position, level);
				}

				uint16_t move_internal = move.to_from_flags();

				Write(&move_internal, 2);
				break;
			}

			case CommandType::GENMOVES: {

				// If the byte after the command selection byte (args[0]) is equal to either WHITE or BLACK then generate all legal moves for the specified side
				if (command.args[1] == 101) {
					// Generate the moves
					MoveList<BLACK> mL(m_Position);

					WRITE_MOVELIST(mL);
					continue;
				}
				else if (command.args[1] == 100) {
					// Generate the moves
					MoveList<WHITE> mL(m_Position);

					WRITE_MOVELIST(mL);
					continue;
				}
						
				Square sq = (Square)command.args[1];
				Piece pc = m_Position.at(sq);

				std::cout << (int)pc << std::endl;

				if (pc > 7 && pc != NO_PIECE) {
					MoveList<BLACK> mL(m_Position, sq);
					
					WRITE_MOVELIST(mL);
				}
				else {
					MoveList<WHITE> mL(m_Position, sq);

					WRITE_MOVELIST(mL);
				}

				break;
			}
			
			case CommandType::INCHECK: {
				u8 in_check = 0;

				if (command.args[1] == BLACK) {
					in_check = m_Position.in_check<BLACK>();
				}
				else {
					in_check = m_Position.in_check<WHITE>();
				}

				Write(&in_check, sizeof(u8));
				break;
			}

			case CommandType::FEN: {
				std::string fen = m_Position.fen();

				Write((void*)fen.c_str(), fen.size());

				break;
			}

			case CommandType::EXIT: {
				if (m_Debug) std::cout << "[DEBUG] Closing..." << std::endl;

				Write((void*)"OK", 2);

				ClosePipe();
				//CreatePipe();
				//m_ShouldClose = true;
				//exit(0);

				if (ConnectNamedPipe(m_hPipe, NULL) == FALSE) std::cout << "[ERROR] Cannot reconnect to the client " << GetLastError() << std::endl;
				break;
			}
			}
		}
	}

	CommandType Parse(u8 ct) {
		if (ct > 0x08)
			return CommandType::NONE;

		return (CommandType)ct;
	}

	void Application::CreatePipe() {
		m_hPipe = CreateNamedPipe(
			PIPE_NAME,
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
			1,
			16 * 1024,
			16 * 1024,
			NMPWAIT_WAIT_FOREVER,
			NULL
		);

		if (m_hPipe == INVALID_HANDLE_VALUE) {
			if (m_Debug) std::cout << "[ERROR] Could not initialize the pipe " << GetLastError() << std::endl;
			return;
		}

		if (ConnectNamedPipe(m_hPipe, NULL) == FALSE) {
			if (m_Debug) std::cout << "[ERROR] Could not connect the pipe" << std::endl;
			return;
		}

		if (m_Debug) std::cout << "[DEBUG] Pipe connected to the client" << std::endl;
	}

	Command Application::Read() {
		Command cmd{CommandType::NONE, NULL, 0};

		if (ReadFile(m_hPipe, (LPVOID)cmd.args, 256, &cmd.arg_len, NULL) == FALSE) {
			return Command{ CommandType::NONE, NULL, 0 };
		}

		cmd.type = Parse(cmd.args[0]);
		cmd.arg_len--;

		if (m_Debug) std::cout << "[DEBUG] Received command no. " << (int)cmd.type << std::endl;
		return cmd;
	}

	void Application::Write(void* data, size_t data_len) {
		DWORD dwWritten = 0;

		if (WriteFile(
			m_hPipe,
			data,
			data_len,
			&dwWritten,
			NULL
		) == FALSE)
		{
			if (m_Debug) std::cout << "[ERROR] Could not Write data through the pipe" << std::endl;
		}

		if (m_Debug) std::cout << "[DEBUG] Sent response" << std::endl;
	}

	void Application::ClosePipe() {
		if (DisconnectNamedPipe(m_hPipe) == FALSE) {
			if (m_Debug) std::cout << "[ERROR] Could not disconnect the pipe" << std::endl;
		};
	}

}