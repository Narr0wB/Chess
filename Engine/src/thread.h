
#ifndef THREAD_H
#define THREAD_H

#include <thread>

#include "position.h"
#include "search.h"

enum class ThreadState {
    IDLE = 0, SEARCHING
};

class SearchWorker {
    private:
        std::unique_ptr<std::thread> m_Thread;
        SearchContext* m_SearchContext;
        ThreadState m_State;

    public:
        SearchWorker(std::string& log) : m_State(ThreadState::IDLE) {

        }

        ThreadState GetState();        
        void Stop();
        
        template <Color C>
        void Run(SearchContext* ctx) {
            if (ctx == nullptr) {
                LOG_ERROR("Invalid search context, cannot start search worker!");
            }

            Stop();
            m_SearchContext = ctx;
            ctx->stop_flag = false;
            
            m_State = ThreadState::SEARCHING;
            m_Thread = std::unique_ptr<std::thread>(new std::thread(&Search::Search<C>, ctx));
        }
};

#endif // THREAD_H

