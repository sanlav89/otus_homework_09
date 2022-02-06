#pragma once

#include <iostream>
#include <stack>
#include <queue>
#include <list>
#include "logger.h"
#include "assignments.h"
#include "state.h"

namespace bulk {

class Handler
{
public:

    // High level logic
    Handler(const size_t &bulkSize, std::queue<Cmd> &cmdsStatic);
    void registerLogger(logger::LogPtr logger);
    void receive(const char *data, size_t size);
    void receiveEof();

    // State Pattern side
    void setState(StateBasePtr state);

    // Properties
    size_t bulkSize() const;
    size_t cmdsSize() const;
    size_t bracketsSize() const;

    // Handler functionality
    void pushOpenedBracket();
    void popOpenedBracket();
    void pushCmdStatic(const Cmd &cmd);
    void pushCmdDynamic(const Cmd &cmd);
    void processBulkStatic();
    void processBulkDynamic();

private:
    size_t m_bulkSize;
    std::string m_buffer;
    std::stack<Bracket> m_brackets;
    std::queue<Cmd> m_cmdsDynamic;
    std::queue<Cmd> &m_cmdsStatic;
    std::list<logger::LogPtr> m_loggers;
    StateBasePtr m_state;

    void reveiveCmd(const Cmd &cmd);
    void processBulk(std::queue<Cmd> &bulk);

    static bool isOpenedBracket(const Cmd &cmd);
    static bool isClosedBracket(const Cmd &cmd);
    static bool isAnyBracket(const Cmd &cmd, Bracket anyBracket);

};

using HandlerPtr = std::unique_ptr<Handler>;

}

