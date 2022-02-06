#include "logger.h"
#include <chrono>
#include <sstream>
#include <boost/filesystem.hpp>

using namespace logger;
namespace fs = boost::filesystem;

Logger::Logger() : m_stopped(false)
{
}

void Logger::process(const bulk_t &)
{
}

void Logger::worker()
{
}

void Logger::stop()
{
}

void Logger::processOne(std::ostream &os, bulk_t &bulk)
{
    os << bulk.front();
    bulk.pop();
    if (!bulk.empty()) {
        os << ", ";
    }
}

Console::Console(std::ostream &os)
    : Logger()
    , m_os(os)
    , m_threadLog(std::thread(&Console::worker, this))
{
}

Console::~Console()
{
    Console::stop();
}

void Console::process(const bulk_t &bulk)
{
    std::lock_guard<std::mutex> lk(m_mutex);
    m_bulks.push(bulk);
    m_cv.notify_one();
}

void Console::stop()
{
    if (!m_stopped) {
        m_stopped = true;
        m_cv.notify_all();
        m_threadLog.join();
    }
}

void Console::worker()
{
    do {

        std::unique_lock<std::mutex> lk(m_mutex);
        m_cv.wait(lk, [this]() { return m_stopped || !m_bulks.empty(); });

        while (!m_bulks.empty()) {
            processBulk(m_os, m_bulks.front());
            m_bulks.pop();
        }

        if (m_stopped) {
            break;
        }
    } while (!m_stopped);
    std::cout << "Console thread finished" << std::endl;
}

void Console::processBulk(std::ostream &os, bulk_t &bulk)
{
    os << "bulk: ";
    while (!bulk.empty()) {
        os << bulk.front();
        bulk.pop();
        if (!bulk.empty()) {
            os << ", ";
        }
    }
    os << std::endl;
}

LogFile::LogFile()
    : Logger()
    , m_threadFile1(std::thread(&LogFile::worker, this))
    , m_threadFile2(std::thread(&LogFile::worker, this))
{
    fs::create_directory("./log");
}

LogFile::~LogFile()
{
    LogFile::stop();
}

void LogFile::process(const bulk_t &bulk)
{
    std::lock_guard<std::mutex> lk(m_mutex);
    m_bulks.push(bulk);
    m_cv.notify_all();
}

void LogFile::stop()
{
    if (!m_stopped) {
        m_stopped = true;
        m_cv.notify_all();
        m_threadFile1.join();
        m_threadFile2.join();
    }
}

void LogFile::worker()
{
    do {

        std::unique_lock<std::mutex> lk(m_mutex);
        m_cv.wait(lk, [this]() { return m_stopped || !m_bulks.empty(); });

        if (!m_bulks.empty()) {

            auto &bulk = m_bulks.front();

            if (!m_logFile.is_open()) {
                openNewLogFile();
            }

            processOne(m_logFile, bulk);

            if (bulk.empty()) {
                m_bulks.pop();
                m_logFile.close();
            }
        }

    } while (!m_stopped || !m_bulks.empty());
}

void LogFile::openNewLogFile()
{
    static auto fileNum = 0;
    auto result = std::chrono::system_clock::now().time_since_epoch().count();
    std::ostringstream ossFilename;
    std::ostream &osFilename = ossFilename;
    osFilename << "log/bulk" << result
               << "_" << std::this_thread::get_id() << fileNum++
               << ".log";
    m_logFileName = ossFilename.str();
    m_logFile.open(m_logFileName);

    // write some service info to file
    if (m_logFile.is_open()) {
        m_logFile << "bulk: ";
    } else {
        throw "Error! File " + m_logFileName + "is not opened";
    }
}
