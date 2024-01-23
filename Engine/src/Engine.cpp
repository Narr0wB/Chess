
#include "Engine.h"

static std::map<Engine::u16, std::unique_ptr<Engine::Application>> active_instances;
#define MAX_INSTANCES 10

namespace Engine {

	void Application::CreateConsole() {
		
	}

	void Application::DestroyConsole() {
		
	}

	Response Application::Parse(Command command) {

		switch (command.header.type) {
			case CommandType::SET: {
				std::string fen((char*)&command.payload[0], command.header.payload_size);

				if (fen == "start")
					fen = START_POSITION;

				m_Position.reset();
				m_MoveList.clear();
				Position::set(fen, m_Position);

				if (m_Debug) { LOG_INFO("[{}] Position set", m_InstanceID); }

				return OK_RESPONSE;
			}

			case CommandType::MOVE: {
				Color C = (Color)command.payload[0];
				u16 move_internal = *(u16*)&command.payload[1];

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

				return OK_RESPONSE;
			}

			case CommandType::UNDO: {
				if (m_MoveList.empty())
					return OK_RESPONSE;

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

				return OK_RESPONSE;
			}

			case CommandType::MOVEREQ: {
				u8 level = command.payload[0];
				Color C = (Color)command.payload[1];

				if (level > 20)
					level = 20;

				SearchInfo s_info;

				switch (C) {
					case BLACK: {
						s_info = search_best_move<BLACK>(m_Position, level);
						break;
					}
					case WHITE: {
						s_info = search_best_move<WHITE>(m_Position, level);
						break;
					}
				}

				if (m_Debug) { LOG_INFO("[{}] Searched {:L} total nodes in {:.1f}ms", m_InstanceID, s_info.total_nodes, s_info.search_duration_ms); }

				uint16_t move_internal = s_info.best.to_from_flags();

				Response resp;
				resp.payload_size = sizeof(move_internal);
				resp.payload[0] = (u8)(move_internal >> 0);
				resp.payload[1] = (u8)(move_internal >> 8);

				return resp;
			}

			case CommandType::GENMOVES: {
				Response resp;

				/* 
				                                                                 0                  1 
				   If the first byte of the payload is equal to either (100 + [WHITE]) or (100 + [BLACK]) then generate all legal moves for the specified side
				*/
				if (command.payload[0] == (100 + BLACK)) {
					MoveList<BLACK> mL(m_Position);

					WRITE_MOVELIST(mL, resp);
					break;
				}
				else if (command.payload[0] == (100 + WHITE)) {
					MoveList<WHITE> mL(m_Position);

					WRITE_MOVELIST(mL, resp);
					break;
				}

				Square sq = (Square)command.payload[0];
				Piece pc = m_Position.at(sq);

				if (pc > 7 && pc != NO_PIECE) {
					MoveList<BLACK> mL(m_Position, sq);

					WRITE_MOVELIST(mL, resp);
				}
				else {
					MoveList<WHITE> mL(m_Position, sq);

					WRITE_MOVELIST(mL, resp);
				}

				return resp;
			}

			case CommandType::CHECK: {
				Response resp;

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

				resp.payload_size = 1;
				resp.payload[0] = (u8)in_check;

				return resp;
			}

			case CommandType::CHECKMATE: {
				Response resp;

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

				resp.payload_size = 1;
				resp.payload[0] = (u8)checkmate;
				
				return resp;
			}

			case CommandType::FEN: {
				Response resp;

				std::string fen = m_Position.fen();
				
				resp.payload_size = fen.size();
				std::memcpy(resp.payload, (void*)fen.c_str(), resp.payload_size);

				return resp;
			}
			
			case CommandType::COLOR: {
				Response resp;

				u8 color_data = m_Position.getCurrentColor();

				resp.payload_size = 1;
				resp.payload[0] = color_data;

				return resp;
			}

			// case CommandType::EXIT: {
			// 	if (m_Debug) LOG_INFO("[{}] Closing...", m_InstanceID);

			// 	return OK_RESPONSE;
			// }
		}

		return OK_RESPONSE;
	}

	// CommandType Parse(u8 ct) {
	// 	if (ct > (int)CommandType::EXIT)
	// 		return CommandType::NONE;

	// 	return (CommandType)ct;
	// }

	// void Application::CreatePipe() {
	// 	std::wstring pipe_name = std::wstring(L"\\\\.\\pipe\\") + std::wstring(m_sPipe.begin(), m_sPipe.end());

	// 	m_hPipe = CreateNamedPipe(
	// 		pipe_name.c_str(),
	// 		PIPE_ACCESS_DUPLEX,
	// 		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
	// 		1,
	// 		16 * 1024,
	// 		16 * 1024,
	// 		NMPWAIT_WAIT_FOREVER,
	// 		NULL
	// 	);

	// 	if (m_hPipe == INVALID_HANDLE_VALUE) {
	// 		if (m_Debug) LOG_ERROR("[{}] Could not initialize the pipe {}", m_sPipe, GetLastError());
	// 		return;
	// 	}

	// 	if (ConnectNamedPipe(m_hPipe, NULL) == FALSE) {
	// 		if (m_Debug) LOG_ERROR("[{}] Could not connect the pipe", m_sPipe);
	// 		return;
	// 	}

	// 	if (m_Debug) LOG_INFO("[{}] Pipe connected to the client", m_sPipe);
	// }

	// void Application::ClosePipe() {
	// 	if (DisconnectNamedPipe(m_hPipe) == FALSE) {
	// 		if (m_Debug) LOG_ERROR("[{}] Could not disconnect the pipe", m_sPipe);
	// 	};
	// }

	// Command Application::Read() {
	// 	Command cmd = { { CommandType::NONE, 0 }, 0, {0} };

	// 	DWORD dwBytesRead = 0;
	// 	if (ReadFile(m_hPipe, (LPVOID)&cmd, 256, &dwBytesRead, NULL) == FALSE) {
	// 		return Command{};
	// 	}

	// 	if (cmd.header.payload_size != dwBytesRead - sizeof(CommandHeader)) {
	// 		/*LOG_INFO("{:x} {:x} {:x}, {:x} {:x}", ((u8*)&cmd)[0], ((u8*)&cmd)[1], ((u8*)&cmd)[2], ((u8*)&cmd.header)[1], ((u8*)&cmd.header)[2]);
	// 		printf(" %p %p", &cmd.header.payload_size, ((u8*)&cmd + 1));*/
	// 		LOG_ERROR("[{}] Invalid payload size! dwRead: {} paysize: {}", m_sPipe, dwBytesRead, cmd.header.payload_size);
	// 	}

	// 	if (m_Debug) LOG_INFO("[{}] Received command {}", m_sPipe, COMMAND_STR_B[cmd.header.type]);

	// 	// Send ACK to confirm receipt
	// 	BYTE ack_byte = ACK;
	// 	WriteFile(m_hPipe, (LPCVOID)&ack_byte, 1, NULL, NULL);

	// 	return cmd;
	// }

	// void Application::Write(void* data, size_t data_len, int attempt) {
	// 	if (attempt == 3) {
	// 		return;
	// 	}

	// 	DWORD dwWritten = 0;

	// 	if (WriteFile(
	// 		m_hPipe,
	// 		data,
	// 		data_len,
	// 		&dwWritten,
	// 		NULL
	// 	) == FALSE)
	// 	{
	// 		std::cout << Move(*(uint16_t*)data) << std::endl;
	// 		if (m_Debug) LOG_ERROR("[{}] Could not Write data through the pipe. ERROR CODE {}", m_sPipe, GetLastError());
	// 	}

	// 	if (m_Debug) LOG_INFO("[{}] Sent response", m_sPipe);

	// 	BYTE ack_response = 0x00;
	// 	if (ReadFile(m_hPipe, (LPVOID)&ack_response, 1, NULL, NULL) == FALSE) {
	// 		if (m_Debug) LOG_ERROR("[{}] Lost communication with the GUI process. Exiting...", m_sPipe);

	// 		ClosePipe();
	// 		exit(EXIT_FAILURE);
	// 	}

	// 	// If there is no ack response, then resend the data once more for a max of 3 times
	// 	if (ack_response != ACK) {
	// 		std::this_thread::sleep_for(100ms);

	// 		Write(data, data_len, attempt + 1);
	// 	}
	// }
} // namespace Engine

bool CreateInstance(Engine::u16 instance_id, bool debug_console) {
	if (active_instances.size() > MAX_INSTANCES) {
		return false;
	}

	active_instances[instance_id] = std::unique_ptr<Engine::Application>(new Engine::Application(instance_id, debug_console));
	return true;
}

bool DestroyInstance(Engine::u16 iid) {
	
	// Calling std::map::erase makes sure to call the deleter of the element stored at [key] thus leaving clean up operations to the deleter
	active_instances.erase(iid);

	return true;
}

bool ExistsInstance(Engine::u16 iid) {
	if (active_instances.find(iid) == active_instances.end()) {
		return false;
	}
	
	return true;
}

Engine::Response RequestCommand(Engine::u16 iid, Engine::Command cmd) {
	auto it = active_instances.find(iid);

	if (it != active_instances.end()) {
		return it->second->Parse(cmd);
	}

	return BAD_RESPONSE;
}