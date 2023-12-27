
#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <chrono>
#include <Windows.h>
#include <thread>

#include "tables.h"
#include "types.h"
#include "position.h"
#include "Evaluate.h"
#include "Log.h"

#define ACK 0xFF

#define WRITE_MOVELIST(move_list) \
{\
	uint16_t buf[256] = { 0 }; \
	int counter = 0; \
	for (Move move : move_list) {\
	buf[counter++] = move.to_from_flags(); \
	}\
	if (move_list.size() == 0) {\
	Write(buf, 2);}\
	else\
	Write(buf, counter * sizeof(uint16_t));\
};

using namespace std::chrono_literals;

namespace Engine {
	typedef uint8_t u8;

	enum CommandType : uint16_t {
		NONE = 0, SET, MOVE, UNDO, MOVEREQ, GENMOVES, CHECK, CHECKMATE, FEN, COLOR, EXIT
	};

	static const char* COMMAND_STR_B[] = {"NONE", "SET", "MOVE", "UNDO", "MOVEREQ", "GENMOVES", "CHECK", "CHECKMATE", "FEN", "COLOR", "EXIT"};

	struct CommandHeader {
		CommandType type;
		WORD payload_size;
	};

	struct Command {
		CommandHeader header;

		u8 payload[256];
	};

	CommandType Parse(u8 ct);

	class Application {
		private:
			bool m_ShouldClose;

			Position m_Position;
			std::vector<Move> m_MoveList;

			HANDLE m_hPipe;
			bool m_Debug;
			std::string m_sPipe;

		private:
			void CreatePipe();
			void ClosePipe();
			Command Read();
			void Write(void* data, size_t data_len, int attempt = 0);

		public:
			Application(char* pipe_key) : m_Debug(true), m_ShouldClose(false), m_sPipe(pipe_key) { CreatePipe(); }
			void Run();	
	};

	inline Application* CreateApplication(char* pipe_key) { return new Application(pipe_key); };
}
#endif // ENGINE_H
