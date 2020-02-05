#ifndef I_IPOOL_HPP
#define I_IPOOL_HPP

class IPool
{
public:
    virtual void notify_command(int command) = 0;
    virtual int get_actual_workers_number() = 0;
};

#endif