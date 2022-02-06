#pragma once

#include <fstream>
#include <memory>
#include <iostream>
#include <queue>
#include <condition_variable>
#include <thread>
#include <atomic>
#include "assignments.h"

namespace logger {

using bulk_t = std::queue<bulk::Cmd>;

class Logger;
using LogPtr = std::shared_ptr<Logger>;

class ILogger
{
public:
    virtual void process(const bulk_t &bulk) = 0;
    virtual void stop() = 0;
    virtual void worker() = 0;
};

class Logger : public ILogger
{
public:
    Logger();
    virtual ~Logger() = default;
    virtual void process(const bulk_t &) override;
    virtual void stop() override;

protected:

    virtual void worker() override;

    std::queue<bulk_t> m_bulks;
    std::condition_variable m_cv;
    std::mutex m_mutex;
    std::atomic<bool> m_stopped;

    void processOne(std::ostream &os, bulk_t &bulk);

};


class Console : public Logger
{
public:
    Console(std::ostream &os = std::cout);
    ~Console();

    void process(const bulk_t &bulk) override;
    void stop() override;

private:

    std::ostream &m_os;
    std::thread m_threadLog;

    void worker() override;

    void processBulk(std::ostream &os, bulk_t &bulk);

};

class LogFile : public Logger
{
public:
    LogFile();
    ~LogFile();

    void process(const bulk_t &bulk) override;
    void stop() override;

private:

    std::ofstream m_logFile;
    std::string m_logFileName;
    std::thread m_threadFile1;
    std::thread m_threadFile2;

    void worker() override;

    void openNewLogFile();

};

}

