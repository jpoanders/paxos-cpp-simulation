#pragma once

class Role {
public:
    virtual ~Role();
    virtual void run();
    
};

class Acceptor : public Role {
public:
    void run() override;
};

class Learner : public Role {
public:
    void run() override;
};


class Proposer : public Role {
public:
    void run() override;
};