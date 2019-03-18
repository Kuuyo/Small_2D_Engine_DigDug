#include "MiniginPCH.h"
#include "Log.h"
#include <iostream>

HANDLE dae::Log::m_ConsoleHandle = nullptr;

dae::Log::Log()
	: m_ConvertBuffer(nullptr)
	, m_ConvertBufferSize(1024)
{
#ifdef _DEBUG
	if (AllocConsole())
	{
		FILE* pCout;
		freopen_s(&pCout, "CONIN$", "r", stdin);
		freopen_s(&pCout, "CONOUT$", "w", stdout);
		freopen_s(&pCout, "CONOUT$", "w", stderr);

		std::wcout.clear();
		std::cout.clear();
		std::wcerr.clear();
		std::cerr.clear();
		std::wcin.clear();
		std::cin.clear();
		std::cin.clear();

		m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTitle("DebugConsole");
	}
#endif
	
	m_ConvertBuffer = new char[m_ConvertBufferSize];
}

dae::Log::~Log()
{
#ifdef _DEBUG
	FreeConsole();
#endif

	delete[] m_ConvertBuffer;
}

void dae::Log::LogFormat(LogLevel level, const std::string& caller, const char* msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	_vsnprintf_s(&m_ConvertBuffer[0], m_ConvertBufferSize, m_ConvertBufferSize, msg, ap);
	va_end(ap);

	InternalLog(level, std::string(&m_ConvertBuffer[0]), caller);
}

void dae::Log::LogInfo(const std::string& msg, const std::string& caller)
{
#ifdef _DEBUG
	InternalLog(LogLevel::Info, msg, caller);
#endif
}

void dae::Log::LogDebug(const std::string& msg, const std::string& caller)
{
#ifdef _DEBUG
	InternalLog(LogLevel::Debug, msg, caller);
#endif
}

void dae::Log::LogWarning(const std::string& msg, const std::string& caller)
{
	InternalLog(LogLevel::Warning, msg, caller);
}

void dae::Log::LogError(const std::string& msg, const std::string& caller)
{
	InternalLog(LogLevel::Error, msg, caller);
}

void dae::Log::InternalLog(LogLevel level, const std::string& msg, const std::string& caller)
{
	std::stringstream ss;

	switch (level)
	{
	case dae::LogLevel::Info:
		SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		ss << "[INFO]:: << ";
		break;
	case dae::LogLevel::Debug:
		SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		ss << "[DEBUG]:: << ";
		break;
	case dae::LogLevel::Warning:
		SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		ss << "[WARNING]:: << ";
		break;
	case dae::LogLevel::Error:
		SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_INTENSITY | FOREGROUND_RED);
		ss << "[ERROR]:: << ";
		break;
	default:
		break;
	}

	ss << caller << " >> ";
	ss << msg;
	ss << "\n";

	std::cout << ss.str();
}