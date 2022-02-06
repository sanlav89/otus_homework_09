#include "handler.h"

#include <sstream>

using namespace bulk;

Handler::Handler(const size_t &bulkSize)
    : m_bulkSize(bulkSize)
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
    return m_cmds.size();
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

void Handler::pushCmd(const Cmd &cmd)
{
    m_cmds.push(cmd);
}

void Handler::processBulk()
{
    if (!m_cmds.empty()) {
        for (const auto &logger : m_loggers) {
            logger->process(m_cmds);
        }
        while (!m_cmds.empty()) {
            m_cmds.pop();
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
