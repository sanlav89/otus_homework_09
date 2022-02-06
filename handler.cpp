#include "handler.h"

#include <sstream>

using namespace bulk;

Handler::Handler(const size_t &bulkSize, std::queue<Cmd> &cmdsStatic)
    : m_bulkSize(bulkSize)
    , m_cmdsStatic(cmdsStatic)
    , m_state(StateBasePtr{new StateEmpty(this)})
{
}

void Handler::receive(const char *data, size_t size)
{
    for (auto i = 0u; i < size; i++) {
        if (data[i] == '\n') {
            reveiveCmd(m_buffer);
            m_buffer.clear();
        } else {
            m_buffer.append({data[i]});
        }
    }
}

void Handler::reveiveCmd(const Cmd &cmd)
{
    if (isOpenedBracket(cmd)) {
        m_state->cmdOpenedBracket();
    } else if (isClosedBracket(cmd)) {
        m_state->cmdClosedBracket();
    } else {
        m_state->cmdOther(cmd);
    }
}

void Handler::receiveEof()
{
    m_state->cmdEof();
}

void Handler::registerLogger(logger::LogPtr logger)
{
    m_loggers.emplace_back(std::move(logger));
}

void Handler::setState(StateBasePtr state)
{
    m_state = std::move(state);
}

size_t Handler::bulkSize() const
{
    return m_bulkSize;
}

size_t Handler::cmdsSize() const
{
    return m_cmdsStatic.size();
}

size_t Handler::bracketsSize() const
{
    return m_brackets.size();
}

void Handler::pushOpenedBracket()
{
    m_brackets.push('{');
}

void Handler::popOpenedBracket()
{
    m_brackets.pop();
}

void Handler::pushCmdStatic(const Cmd &cmd)
{
    m_cmdsStatic.push(cmd);
}

void Handler::pushCmdDynamic(const Cmd &cmd)
{
    m_cmdsDynamic.push(cmd);
}

void Handler::processBulkStatic()
{
    processBulk(m_cmdsStatic);
}

void Handler::processBulkDynamic()
{
    processBulk(m_cmdsDynamic);
}

void Handler::processBulk(std::queue<Cmd> &bulk)
{
    if (!bulk.empty()) {
        for (const auto &logger : m_loggers) {
            logger->process(bulk);
        }
        while (!bulk.empty()) {
            bulk.pop();
        }
    }
}

bool Handler::isOpenedBracket(const Cmd &cmd)
{
    return isAnyBracket(cmd, '{');
}

bool Handler::isClosedBracket(const Cmd &cmd)
{
    return isAnyBracket(cmd, '}');
}

bool Handler::isAnyBracket(const Cmd &cmd, Bracket anyBracket)
{
    if (cmd.size() == 1) {
        return cmd.at(0) == anyBracket;
    } else {
        return false;
    }
}
