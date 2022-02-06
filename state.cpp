#include "state.h"
#include "handler.h"

using namespace bulk;

StateBase::StateBase(Handler *handler) : m_handler(handler)
{
}

StateEmpty::StateEmpty(Handler *handler) : StateBase(handler)
{
}

void StateEmpty::cmdOpenedBracket()
{
    m_handler->pushOpenedBracket();
    m_handler->setState(StateBasePtr{new StateDynamic(m_handler)});
}

void StateEmpty::cmdClosedBracket()
{
    pushCmd("}");
}

void StateEmpty::cmdEof()
{
    // nothing to do
}

void StateEmpty::cmdOther(const Cmd &cmd)
{
    pushCmd(cmd);
}

void StateEmpty::pushCmd(const Cmd &cmd)
{
    m_handler->pushCmdStatic(cmd);
    if (m_handler->cmdsSize() == m_handler->bulkSize()) {
        m_handler->processBulkStatic();
    } else {
        m_handler->setState(StateBasePtr{new StateStatic(m_handler)});
    }
}

StateStatic::StateStatic(Handler *handler) : StateBase(handler)
{
}

void StateStatic::cmdOpenedBracket()
{
    m_handler->pushOpenedBracket();
    m_handler->setState(StateBasePtr{new StateDynamic(m_handler)});
}

void StateStatic::cmdClosedBracket()
{
    pushCmd("}");
}

void StateStatic::cmdEof()
{
    m_handler->processBulkStatic();
}

void StateStatic::cmdOther(const Cmd &cmd)
{
    pushCmd(cmd);
}

void StateStatic::pushCmd(const Cmd &cmd)
{
    m_handler->pushCmdStatic(cmd);
    if (m_handler->cmdsSize() == m_handler->bulkSize()) {
        m_handler->processBulkStatic();
    }
}

StateDynamic::StateDynamic(Handler *handler) : StateBase(handler)
{
}

void StateDynamic::cmdOpenedBracket()
{
    m_handler->pushOpenedBracket();
}

void StateDynamic::cmdClosedBracket()
{
    m_handler->popOpenedBracket();
    if (m_handler->bracketsSize() == 0) {
        m_handler->processBulkDynamic();
        m_handler->setState(StateBasePtr{new StateEmpty(m_handler)});
    }
}

void StateDynamic::cmdEof()
{
    // nothing to do
}

void StateDynamic::cmdOther(const Cmd &cmd)
{
    m_handler->pushCmdDynamic(cmd);
}
