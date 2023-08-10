
#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <chrono>
#include <Windows.h>
#include "tables.h"
#include "types.h"
#include "position.h"
#include "Evaluate.h"

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


namespace Engine {
	typedef uint8_t u8;

	enum class CommandType {
		NONE = 0, SET, MOVE, UNDO, MOVEREQ, GENMOVES, INCHECK, EXIT
	};

	struct Command {
		CommandType type;
		u8 args[256];
		DWORD arg_len;
	};

	CommandType Parse(u8 ct);

	class Application {
		private:
			bool m_ShouldClose;

			Position m_Position;
			std::vector<Move> m_MoveList;

			HANDLE m_hPipe;
			bool m_Debug;

		private:
			void CreatePipe();
			void ClosePipe();
			Command Read();
			void Write(void* data, size_t data_len);

		public:
			Application() : m_Debug(true), m_ShouldClose(false) { CreatePipe(); }
			void Run();	
	};

	inline Application* CreateApplication() { return new Application(); };
}
#endif // ENGINE_H
