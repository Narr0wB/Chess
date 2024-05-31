
#include "Engine.h"

// static std::map<Engine::u16, std::unique_ptr<Engine::Application>> active_instances;
// #define MAX_INSTANCES 10

namespace Engine {

	void Optimize(SearchInfo& info, int time, int inc) {
		if (time < 0) time = 5000;

		// Consider the overhead lost during uci communication and subtract it to the avilable time
		const int overhead = std::min(230, time / 2);
		time -= overhead;

		// If are given how long should a move take then we use that for our search
		if (info.movetimeset) {
			info.search_time_max = info.search_start_time + time;
		}

		// If we are given how much time a player has left and how many more moves should the player make
		else if (info.timeset && info.movestogo != 0) {
			const float time_per_move = time / info.movestogo;
			// Never use more than 80% of the available time for a move
			const float max_time_bound = 0.8f * time;
			const float max_time = std::min(0.8f * time_per_move, max_time_bound);

			info.search_time_max = info.search_start_time + (uint64_t) max_time;
		}
		
		// If we are given only how much time is left then we define an arbitrary move time
		else if (info.timeset) {
			const float time_per_move = 0.05 * time + inc * 0.85;
			// Never use more than 80% of the available time for a move
			const float max_time_bound = 0.8f * time;
			const float max_time = std::min(3.0f * time_per_move, max_time_bound);

			info.search_time_max = info.search_start_time + (uint64_t) max_time;
		}
	}

	void Application::NewGame() {
		m_Position.reset();
		Position::set(START_POSITION, m_Position);
		m_Table.clear();
		m_MoveList.clear();
	}

	void Application::UCICommandLoop() {
		std::string command;
		
		while (not m_ShouldClose) {
			// Prompt a command from the user
			//std::cout << "$ ";
			if (!std::getline(std::cin, command)) {
				break;
			}

			if (!command.length()) {
				continue;
			}

			// Parse the command
			UCIParseCommand(command);
		}
	}

	void Application::UCIParseCommand(std::string command) {

		std::vector<std::string> tokens = tokenize(command, ' ');

		if (tokens[0] == "position") {
			if (tokens[1] == "startpos") {
				m_Position.reset();
				Position::set(START_POSITION, m_Position);
			} 
			else if (tokens[1] == "fen") {
				m_Position.reset();
				Position::set(command.substr(command.find("fen") + 4, std::string::npos), m_Position);
				// position fen rnbqkbnr/ppp2ppp/4p3/8/3PP3/8/PPP3PP/RNBQKBNR b KQkq
				// position startpos moves g1f3 d7d5 d2d4 c8f5
				// position startpos moves d2d4 c7c6 e2e4 d8a5 b1d2 g7g6 g1f3 f7f6 f1d3 b7b6 e1g1
				// position startpos moves e2e4 f7f5 e4f5 g7g6 f5g6 h7h6 g6g7 h6h5 g7h8q
				// go wtime 4124 btime 4946 movestogo 35
			}

			if (command.find("moves") != std::string::npos) {
				int string_start = command.find("moves") + 6;

				if (not (string_start > command.length())) {
					std::string moves_sub = command.substr(string_start, std::string::npos);

					std::vector<std::string> moves = tokenize(moves_sub, ' ');
					
					for (size_t i = 0; i < moves.size(); ++i) {
						uint8_t promotion = 0;

						if (moves[i].length() == 5) {
							auto p = moves[i][4];
							switch (p) {
								case 'n': promotion = MoveFlags::PR_KNIGHT; break;
								case 'b': promotion = MoveFlags::PR_BISHOP; break;
								case 'r': promotion = MoveFlags::PR_ROOK; break;
								case 'q': promotion = MoveFlags::PR_QUEEN; break;
							}
						}

						if (m_Position.getCurrentColor() == WHITE) {
							MoveList<WHITE> move_list(m_Position);
							Move move = move_list.find(Move(moves[i]).to_from(), promotion);


							// std::cout << " " << move << " " << moves[i] << std::endl;

							if (move != NO_MOVE) {
								m_Position.play<WHITE>(move);
							}
						}
						else {
							MoveList<BLACK> move_list(m_Position);
							Move move = move_list.find(Move(moves[i]).to_from(), promotion);

							//std::cout << " " << move << " " << moves[i] << std::endl;
							
							if (move != NO_MOVE) {
								m_Position.play<BLACK>(move);
							}
						}
					}
				}
			}
			
		}

		else if (tokens[0] == "printpos") {
			std::cout << m_Position << std::endl;
		}

		else if (tokens[0] == "uci") {
			std::cout << "id name " << "Narrow v1.0" << std::endl;
			std::cout << "id author Narrow" << std::endl;
			std::cout << "option name Hash type spin default 16 min 1 max 262144 " << std::endl;
			std::cout << "uciok" << std::endl;
		}

		else if (tokens[0] == "debug") {
			// Toggle debug
			m_Debug != m_Debug; 
			if (m_Debug) LOG_INFO("Debug enabled");
			else LOG_INFO("Debug disabled");
		}

		else if (tokens[0] == "isready") {
			std::cout << "readyok" << std::endl;
		}	

		else if (tokens[0] == "setoption") {
			if (tokens.size() < 5) {
				std::cout << "Invalid setoption format" << std::endl;
				return;
			}

			if (tokens.at(2) == "Hash") {
				size_t table_size = std::stoi(tokens.at(4));
				std::cout << "Setting hash table size to" << table_size << "MB" << std::endl;

				m_Options.hash_table_size_mb = table_size;
				m_Table.realloc((table_size * 1024 * 1024) / sizeof(Transposition));
			}
			
			else if (tokens.at(2) == "Threads") {
				m_Options.threads = std::stoi(tokens.at(4));

				// TODO
			}
			else {
				std::cout << "Unknown command: " << command << std::endl;
			}
		}

		else if (tokens[0] == "ucinewgame") {
			NewGame();
		}

		else if (tokens[0] == "stop") {
			if (m_SearchWorker.GetState() == ThreadState::SEARCHING) {
				m_SearchWorker.Stop();
			}
		}

		else if (tokens[0] == "quit" or tokens[0] == "exit") {
			if (m_SearchWorker.GetState() == ThreadState::SEARCHING) {
				m_SearchWorker.Stop();
			}

			m_ShouldClose = true;
		}

		else if (tokens[0] == "go") {
			// Interrupt any previous search
			m_SearchWorker.Stop();

			// Check if current position is set
			if (!m_Position) {
				Position::set(START_POSITION, m_Position);
			}

			int depth = -1, movetime = -1;
			int movestogo = 0, nodeslimit = 0, time = 0, inc = 0;

			for (size_t i = 0; i < tokens.size(); ++i) {
				if (tokens.at(i) == "binc" and m_Position.getCurrentColor() == BLACK) {
					inc = std::stoi(tokens.at(i + 1));
				}

				else if (tokens.at(i) == "winc" and m_Position.getCurrentColor() == WHITE) {
					inc = std::stoi(tokens.at(i + 1));
				}

				else if (tokens.at(i) == "btime" and m_Position.getCurrentColor() == BLACK) {
					time = std::stoi(tokens.at(i + 1));
					m_SearchContext->info.timeset = true;
				}

				else if (tokens.at(i) == "wtime" and m_Position.getCurrentColor() == WHITE) {
					time = std::stoi(tokens.at(i + 1));
					m_SearchContext->info.timeset = true;
				}

				else if (tokens.at(i) == "movestogo") {
					movestogo = std::stoi(tokens.at(i + 1));
					m_SearchContext->info.movestogo = movestogo;
				}

				else if (tokens.at(i) == "movetime") {
					time = std::stoi(tokens.at(i + 1));
					m_SearchContext->info.timeset = true;
				}

				else if (tokens.at(i) == "depth") {
					depth = std::stoi(tokens.at(i + 1));
				}

				else if (tokens.at(i) == "nodes") {
					nodeslimit = std::stoi(tokens.at(i + 1));
					m_SearchContext->info.nodeset = true;
					m_SearchContext->info.nodeslimit = nodeslimit;
				}
			}

			if (depth == -1) {
				depth = MAX_DEPTH;
			}

			m_SearchContext->board = m_Position;

			m_SearchContext->info.search_start_time = GetTimeMS();
			m_SearchContext->info.depth = depth;
			m_SearchContext->info.quiescence_depth = 3;

			// Optimize time available for this search
			Optimize(m_SearchContext->info, time, inc);

			std::cout << "info" << std::endl;
			std::cout << "time: " << time << std::endl;
			std::cout << "start: " << m_SearchContext->info.search_start_time << std::endl;
			std::cout << "stop: " << m_SearchContext->info.search_time_max << std::endl;
			std::cout << "depth: " << m_SearchContext->info.depth << std::endl;
			std::cout << "timeset: " << m_SearchContext->info.timeset << std::endl;
			std::cout << "nodeset: " << m_SearchContext->info.nodeset << std::endl;

			// MoveList<WHITE> mL(m_Position);

			// // position fen 5r2/k6P/7p/2K2n2/PP6/N7/1P2PP2/8 w - - 0 1

			// for (Move m : mL) {
			// 	std::cout << m << " " << Search::score_move(m, m_SearchContext, 1) << std::endl;
			// }

			// Clear data before starting a new search
			m_SearchContext->data = { 0 };
				
			if (m_Position.getCurrentColor() == WHITE) {
				m_SearchWorker.Run<WHITE>(m_SearchContext);
			}
			else {
				m_SearchWorker.Run<BLACK>(m_SearchContext);
			}
		}
	}


	// Response Application::Parse(Command command) {

	// 	switch (command.header.type) {
	// 		case CommandType::SET: {
	// 			std::string fen((char*)&command.payload[0], command.header.payload_size);

	// 			if (fen == "start")
	// 				fen = START_POSITION;

	// 			m_Position.reset();
	// 			m_MoveList.clear();
	// 			Position::set(fen, m_Position);

	// 			if (m_Debug) { LOG_INFO("[{}] Position set", m_InstanceID); }

	// 			return OK_RESPONSE;
	// 		}

	// 		case CommandType::MOVE: {
	// 			Color C = (Color)command.payload[0];
	// 			u16 move_internal = *(u16*)&command.payload[1];

	// 			Move move(move_internal);
	// 			m_MoveList.push_back(move);

	// 			switch (C) {
	// 				case BLACK: {
	// 					m_Position.play<BLACK>(move);
	// 					break;
	// 				}
	// 				case WHITE: {
	// 					m_Position.play<WHITE>(move);
	// 					break;
	// 				}
	// 			}

	// 			return OK_RESPONSE;
	// 		}

	// 		case CommandType::UNDO: {
	// 			if (m_MoveList.empty())
	// 				return OK_RESPONSE;

	// 			switch (m_Position.getCurrentColor()) {
	// 				case BLACK: {
	// 					m_Position.undo<WHITE>(m_MoveList.back());
	// 					break;
	// 				}
	// 				case WHITE: {
	// 					m_Position.undo<BLACK>(m_MoveList.back());
	// 					break;
	// 				}
	// 			}

	// 			m_MoveList.pop_back();

	// 			return OK_RESPONSE;
	// 		}

	// 		case CommandType::MOVEREQ: {
	// 			u8 depth = command.payload[0];
	// 			Color C = (Color)command.payload[1];

	// 			if (depth > 20)
	// 				depth = 20;

	// 			SearchInfo s_info;

	// 			switch (C) {
	// 				case BLACK: {
	// 					s_info = Search::Search<BLACK>(m_Position, depth, true);
	// 					break;
	// 				}
	// 				case WHITE: {
	// 					s_info = Search::Search<WHITE>(m_Position, depth, true);
	// 					break;
	// 				}
	// 			}

	// 			if (m_Debug) { LOG_INFO("[{}] Searched {:L} total nodes in {:.1f}ms", m_InstanceID, s_info.total_nodes, s_info.search_time_ms); }

	// 			uint16_t move_internal = s_info.best.to_from_flags();

	// 			Response resp;
	// 			resp.payload_size = sizeof(move_internal);
	// 			resp.payload[0] = (u8)(move_internal >> 0);
	// 			resp.payload[1] = (u8)(move_internal >> 8);

	// 			return resp;
	// 		}

	// 		case CommandType::GENMOVES: {
	// 			Response resp;

	// 			/* 
	// 			                                                                 0                  1 
	// 			   If the first byte of the payload is equal to either (100 + [WHITE]) or (100 + [BLACK]) then generate all legal moves for the specified side
	// 			*/
	// 			if (command.payload[0] == (100 + BLACK)) {
	// 				MoveList<BLACK> mL(m_Position);

	// 				WRITE_MOVELIST(mL, resp);
	// 				break;
	// 			}
	// 			else if (command.payload[0] == (100 + WHITE)) {
	// 				MoveList<WHITE> mL(m_Position);

	// 				WRITE_MOVELIST(mL, resp);
	// 				break;
	// 			}

	// 			Square sq = (Square)command.payload[0];
	// 			Piece pc = m_Position.at(sq);

	// 			if (pc > 7 && pc != NO_PIECE) {
	// 				MoveList<BLACK> mL(m_Position, sq);

	// 				WRITE_MOVELIST(mL, resp);
	// 			}
	// 			else {
	// 				MoveList<WHITE> mL(m_Position, sq);

	// 				WRITE_MOVELIST(mL, resp);
	// 			}

	// 			return resp;
	// 		}

	// 		case CommandType::CHECK: {
	// 			Response resp;

	// 			bool in_check = 0;
	// 			Color C = (Color)command.payload[0];

	// 			switch (C) {
	// 			case BLACK: {
	// 				in_check = m_Position.in_check<BLACK>();
	// 				break;
	// 			}
	// 			case WHITE: {
	// 				in_check = m_Position.in_check<WHITE>();
	// 				break;
	// 			}
	// 			}

	// 			resp.payload_size = 1;
	// 			resp.payload[0] = (u8)in_check;

	// 			return resp;
	// 		}

	// 		case CommandType::CHECKMATE: {
	// 			Response resp;

	// 			bool checkmate = 0;
	// 			Color C = (Color)command.payload[0];

	// 			switch (C) {
	// 			case BLACK: {
	// 				checkmate = m_Position.checkmate<BLACK>();
	// 				break;
	// 			}
	// 			case WHITE: {
	// 				checkmate = m_Position.checkmate<WHITE>();
	// 				break;
	// 			}
	// 			}

	// 			resp.payload_size = 1;
	// 			resp.payload[0] = (u8)checkmate;
				
	// 			return resp;
	// 		}

	// 		case CommandType::FEN: {
	// 			Response resp;

	// 			std::string fen = m_Position.fen();
				
	// 			resp.payload_size = fen.size();
	// 			std::memcpy(resp.payload, (void*)fen.c_str(), resp.payload_size);

	// 			return resp;
	// 		}
			
	// 		case CommandType::COLOR: {
	// 			Response resp;

	// 			u8 color_data = m_Position.getCurrentColor();

	// 			resp.payload_size = 1;
	// 			resp.payload[0] = color_data;

	// 			return resp;
	// 		}

	// 		// case CommandType::EXIT: {
	// 		// 	if (m_Debug) LOG_INFO("[{}] Closing...", m_InstanceID);

	// 		// 	return OK_RESPONSE;
	// 		// }
	// 	}

	// 	return OK_RESPONSE;
	// }

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

// bool CreateInstance(Engine::u16 instance_id, bool debug_console) {
// 	if (active_instances.size() > MAX_INSTANCES) {
// 		return false;
// 	}

// 	active_instances[instance_id] = std::unique_ptr<Engine::Application>(new Engine::Application(instance_id, debug_console));
// 	return true;
// }

// bool DestroyInstance(Engine::u16 iid) {
	
// 	// Calling std::map::erase makes sure to call the deleter of the element stored at [key] thus leaving clean up operations to the deleter
// 	active_instances.erase(iid);

// 	return true;
// }

// bool ExistsInstance(Engine::u16 iid) {
// 	if (active_instances.find(iid) == active_instances.end()) {
// 		return false;
// 	}
	
// 	return true;
// }

// Engine::Response RequestCommand(Engine::u16 iid, Engine::Command cmd) {
// 	auto it = active_instances.find(iid);

// 	if (it != active_instances.end()) {
// 		return it->second->Parse(cmd);
// 	}

// 	return BAD_RESPONSE;
// }