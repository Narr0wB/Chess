
#include <engine/Log.hpp>

std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
// std::ostringstream oss_;

void Log::Init(){
    spdlog::set_pattern("%^[%T] [%L] %n: %v%$");
    s_CoreLogger = spdlog::stdout_color_mt("DEBUG");
    s_CoreLogger->set_level(spdlog::level::trace);
}


// void Log::Init(){
//     spdlog::set_pattern("%^[%T] %n: %v%$"); 
//     s_CoreLogger = spdlog::stdout_color_mt("CORE");
//     s_CoreLogger->set_level(spdlog::level::trace);
// }