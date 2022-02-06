#pragma once

#include "assignments.h"

namespace bulk {

class Handler;

class StateBase
{
public:
    StateBase(Handler *handler);
    virtual ~StateBase() = default;
    virtual void cmdOpenedBracket() = 0;
    virtual void cmdClosedBracket() = 0;
    virtual void cmdEof() = 0;
    virtual void cmdOther(const Cmd &cmd) = 0;

protected:
    Handler *m_handler;
};

class StateEmpty : public StateBase
{
public:
    StateEmpty(Handler *handler);
    void cmdOpenedBracket() override;
    void cmdClosedBracket() override;
    void cmdEof() override;
    void cmdOther(const Cmd &cmd) override;

private:
    void pushCmd(const Cmd &cmd);
};

class StateStatic : public StateBase
{
public:
    StateStatic(Handler *handler);
    void cmdOpenedBracket() override;
    void cmdClosedBracket() override;
    void cmdEof() override;
    void cmdOther(const Cmd &cmd) override;

private:
    void pushCmd(const Cmd &cmd);
};

class StateDynamic : public StateBase
{
public:
    StateDynamic(Handler *handler);
    void cmdOpenedBracket() override;
    void cmdClosedBracket() override;
    void cmdEof() override;
    void cmdOther(const Cmd &cmd) override;
};

using StateBasePtr = std::unique_ptr<StateBase>;

}

