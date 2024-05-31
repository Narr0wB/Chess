
#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>

#include "tables.h"
#include "types.h"
#include "position.h"
#include "search.h"
#include "Log.h"
#include "misc.h"
#include "thread.h"


#define START_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/ w KQkq"
#define DEBUG_POSITION "rnb2b2/2p1k1p1/1p1q4/3p4/P7/8/P4PPP/RNB1KBNR w KQ - 0 9"

#define MAX_DEPTH 20

using namespace std::chrono_literals;

namespace Engine {
	// typedef uint8_t u8;
	// typedef uint16_t u16;
	// #define ENGINE_API __declspec(dllexport)

	// static const char* COMMAND_STR_B[] = {"NONE", "SET", "MOVE", "UNDO", "MOVEREQ", "GENMOVES", "CHECK", "CHECKMATE", "FEN", "COLOR"};

	// enum CommandType : uint16_t {
	// 	NONE = 0, SET, MOVE, UNDO, MOVEREQ, GENMOVES, CHECK, CHECKMATE, FEN, COLOR
	// };

	// Structure of the command

	//       HEADER            ------- Payload ([PAYLEN] BYTES)
	//  ----------------      |
	// |                |     v
	// 0x00 | 0x00 0x00 | 0x00 ...
	//   ^       ^
	//   |       | 
	//   |        --------------- Payload length (2 BYTES)
	//    -------- Command identifier (1 BYTE)

	// struct CommandHeader {
	// 	CommandType type;
	// 	u16 payload_size;
	// };

	// struct Command {
	// 	CommandHeader header;
	// 	u8 payload[256];
	// };

	// struct Response {
	// 	u16 payload_size;
	// 	u8 payload[512];
	// };

	// #define OK_RESPONSE 	Engine::Response{2, {0xFF, 0xFF}}
	// #define BAD_RESPONSE 	Engine::Response{2, {0xDE, 0xAD}}
	// #define WRITE_MOVELIST(move_list, resp) \
	// {\
	// 	resp.payload_size =  move_list.size() * sizeof(Move);\
	// 	std::memcpy(resp.payload, (void*)move_list.begin(), resp.payload_size);\
	// };

	struct UCIOptions {
		size_t hash_table_size_mb;
		uint8_t threads;
	};

	void Optimize(SearchInfo& info, int time, int inc);

	class Application {
		private:
			Position m_Position;
			std::vector<Move> m_MoveList;
			
			bool m_Debug;
			bool m_ShouldClose;
			std::string m_DebugLog;

			SearchWorker m_SearchWorker;
			SearchContext* m_SearchContext;
			UCIOptions m_Options;

			#define TT_MAX_ENTRIES 0x4000000

			int tt_hits = 0;
			TranspositionTable m_Table;

		private:
			void NewGame();
			void UCIParseCommand(std::string cmd);

		public:
			Application(bool debug) : 
			m_Debug(debug),  
			m_ShouldClose(false), 
			m_SearchWorker(m_DebugLog),
			m_Table(0x1000000)
			{ 
				initialise_all_databases();
    			zobrist::initialise_zobrist_keys();
				Log::Init();

				// Initialize search context
				m_SearchContext = new SearchContext();
				m_SearchContext->table = &m_Table;
			};
			~Application() { m_SearchWorker.Stop(); }
			

			// UCI communication
			// Takes control of the application (when used must be compiled to a binary executable)
			void UCICommandLoop();
	};

} // namespace Engine

// extern "C" {
// 	ENGINE_API bool CreateInstance(Engine::u16 instance_id, bool debug_console);
// 	ENGINE_API bool DestroyInstance(Engine::u16 instance_id);
// 	ENGINE_API bool ExistsInstance(Engine::u16 instance_id);
// 	ENGINE_API Engine::Response RequestCommand(Engine::u16 iid, Engine::Command cmd);
// 	ENGINE_API char* DebugData(Engine::u16 iid);
// }

#endif // ENGINE_H
