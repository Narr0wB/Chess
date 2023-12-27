
#include "Engine.h"

#define START_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/ w KQkq"
#define DEBUG_POSITION "rnb2b2/2p1k1p1/1p1q4/3p4/P7/8/P4PPP/RNB1KBNR w KQ - 0 9"

#define PIPE_NAME TEXT("\\\\.\\pipe\\chess")

namespace Engine {
	void Application::Run() {

		while (!m_ShouldClose) {
			Command command = Read();

			switch (command.header.type) {
			case CommandType::SET: {
				std::string fen((char*)&command.payload[0], command.header.payload_size);

				if (fen == "default")
					fen = START_POSITION;

				m_Position.reset();
				Position::set(fen, m_Position);

				if (m_Debug)
					LOG_INFO("[{}] Position set", m_sPipe);
				break;
			}

			case CommandType::MOVE: {
				Color C = (Color)command.payload[0];
				uint16_t move_internal = *(uint16_t*)&command.payload[1];

				Move move(move_internal);
				m_MoveList.push_back(move);

				switch (C) {
					case BLACK: {
						m_Position.play<BLACK>(move);
						break;
					}
					case WHITE: {
						m_Position.play<WHITE>(move);
						break;
					}
				}

				break;
			}

			case CommandType::UNDO: {
				if (m_MoveList.empty())
					return;

				switch (m_Position.getCurrentColor()) {
				case BLACK: {
					m_Position.undo<WHITE>(m_MoveList.back());
					break;
				}
				case WHITE: {
					m_Position.undo<BLACK>(m_MoveList.back());
					break;
				}
				}

				m_MoveList.pop_back();

				break;
			}

			case CommandType::MOVEREQ: {
				u8 level = command.payload[0];

				if (level > 6)
					level = 6;

				Color C = (Color)command.payload[1];
				Move move;

				switch (C) {
					case BLACK: {
						move = findBestMove<BLACK>(m_Position, level);
						break;
					}
					case WHITE: {
						move = findBestMove<WHITE>(m_Position, level);
						break;
					}
				}

				if (m_Debug) { LOG_INFO("[{}] Searched {} positions", m_sPipe, nodes_searched); }
				nodes_searched = 0;

				uint16_t move_internal = move.to_from_flags();

				Write(&move_internal, 2);
				break;
			}

			case CommandType::GENMOVES: {

				// If the byte after the command selection byte (payload[0]) is equal to either (100 + [WHITE]) or (100 + [BLACK]) then generate all legal moves for the specified side
				if (command.payload[0] == (100 + BLACK)) {
					// Generate the moves
					MoveList<BLACK> mL(m_Position);

					WRITE_MOVELIST(mL);
					break;
				}
				else if (command.payload[0] == (100 + WHITE)) {
					// Generate the moves
					MoveList<WHITE> mL(m_Position);

					WRITE_MOVELIST(mL);
					break;
				}

				Square sq = (Square)command.payload[0];
				Piece pc = m_Position.at(sq);

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

			case CommandType::CHECK: {
				bool in_check = 0;
				Color C = (Color)command.payload[0];

				switch (C) {
				case BLACK: {
					in_check = m_Position.in_check<BLACK>();
					break;
				}
				case WHITE: {
					in_check = m_Position.in_check<WHITE>();
					break;
				}
				}

				Write(&in_check, 1);
				break;
			}

			case CommandType::CHECKMATE: {
				bool checkmate = 0;
				Color C = (Color)command.payload[0];

				switch (C) {
				case BLACK: {
					checkmate = m_Position.checkmate<BLACK>();
					break;
				}
				case WHITE: {
					checkmate = m_Position.checkmate<WHITE>();
					break;
				}
				}

				Write(&checkmate, 1);
				break;
			}

			case CommandType::FEN: {
				std::string fen = m_Position.fen();

				Write((void*)fen.c_str(), fen.size());

				break;
			}
			
			case CommandType::COLOR: {
				u8 color_data = m_Position.getCurrentColor();

				Write(&color_data, 1);
				break;
			}

			case CommandType::EXIT: {
				if (m_Debug) LOG_INFO("[{}] Closing...", m_sPipe);

				ClosePipe();
				exit(0);

				/*if (ConnectNamedPipe(m_hPipe, NULL) == FALSE) LOG_ERROR("[{}] Cannot reconnect to the client {}", m_sPipe, GetLastError());*/
				break;
			}
			}
		}
	}

	CommandType Parse(u8 ct) {
		if (ct > (int)CommandType::EXIT)
			return CommandType::NONE;

		return (CommandType)ct;
	}

	void Application::CreatePipe() {
		std::wstring pipe_name = std::wstring(L"\\\\.\\pipe\\") + std::wstring(m_sPipe.begin(), m_sPipe.end());

		m_hPipe = CreateNamedPipe(
			pipe_name.c_str(),
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
			1,
			16 * 1024,
			16 * 1024,
			NMPWAIT_WAIT_FOREVER,
			NULL
		);

		if (m_hPipe == INVALID_HANDLE_VALUE) {
			if (m_Debug) LOG_ERROR("[{}] Could not initialize the pipe {}", m_sPipe, GetLastError());
			return;
		}

		if (ConnectNamedPipe(m_hPipe, NULL) == FALSE) {
			if (m_Debug) LOG_ERROR("[{}] Could not connect the pipe", m_sPipe);
			return;
		}

		if (m_Debug) LOG_INFO("[{}] Pipe connected to the client", m_sPipe);
	}

	// Read a command from the python API
	Command Application::Read() {
		Command cmd = { { CommandType::NONE, 0 }, 0, {0} };

		DWORD dwBytesRead = 0;
		if (ReadFile(m_hPipe, (LPVOID)&cmd, 256, &dwBytesRead, NULL) == FALSE) {
			return Command{};
		}

		if (cmd.header.payload_size != dwBytesRead - sizeof(CommandHeader)) {
			/*LOG_INFO("{:x} {:x} {:x}, {:x} {:x}", ((u8*)&cmd)[0], ((u8*)&cmd)[1], ((u8*)&cmd)[2], ((u8*)&cmd.header)[1], ((u8*)&cmd.header)[2]);
			printf(" %p %p", &cmd.header.payload_size, ((u8*)&cmd + 1));*/
			LOG_ERROR("[{}] Invalid payload size! dwRead: {} paysize: {}", m_sPipe, dwBytesRead, cmd.header.payload_size);
		}

		if (m_Debug) LOG_INFO("[{}] Received command {}", m_sPipe, COMMAND_STR_B[cmd.header.type]);

		// Send ACK to confirm receipt
		BYTE ack_byte = ACK;
		WriteFile(m_hPipe, (LPCVOID)&ack_byte, 1, NULL, NULL);

		return cmd;
	}

	void Application::Write(void* data, size_t data_len, int attempt) {
		if (attempt == 3) {
			return;
		}

		DWORD dwWritten = 0;

		if (WriteFile(
			m_hPipe,
			data,
			data_len,
			&dwWritten,
			NULL
		) == FALSE)
		{
			std::cout << Move(*(uint16_t*)data) << std::endl;
			if (m_Debug) LOG_ERROR("[{}] Could not Write data through the pipe. ERROR CODE {}", m_sPipe, GetLastError());
		}

		if (m_Debug) LOG_INFO("[{}] Sent response", m_sPipe);

		BYTE ack_response = 0x00;
		if (ReadFile(m_hPipe, (LPVOID)&ack_response, 1, NULL, NULL) == FALSE) {
			if (m_Debug) LOG_ERROR("[{}] Lost communication with the GUI process. Exiting...", m_sPipe);

			ClosePipe();
			exit(EXIT_FAILURE);
		}

		// If there is no ack response, then resend the data once more for a max of 3 times
		if (ack_response != ACK) {
			std::this_thread::sleep_for(100ms);

			Write(data, data_len, attempt + 1);
		}
	}

	void Application::ClosePipe() {
		if (DisconnectNamedPipe(m_hPipe) == FALSE) {
			if (m_Debug) LOG_ERROR("[{}] Could not disconnect the pipe", m_sPipe);
		};
	}

}