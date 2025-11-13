#pragma once
#include "communication.hpp"
#include <deque>
#include <mutex>
#include <condition_variable>
#include <memory>

enum class RoleType {ACCEPTOR, LEARNER, PROPOSER};

class Role {
public:
    Role();
    virtual ~Role();
    virtual void run();
    void shutdown();
    std::mutex& get_mutex();
    std::condition_variable& get_cond();
    std::deque<Message>& get_dispatch_queue();
protected:
    bool is_running = false;
    std::deque<Message> dispatch_queue;
    std::mutex dq_mtx;
    std::condition_variable dq_cond;
};

class Acceptor : public Role {
public:
    Acceptor() = default;
    void run() override;
private:
    
    struct Promise {
        Node* promisee;
        Proposal proposal;
    };

    std::shared_ptr<Promise> promise;
    std::shared_ptr<Proposal> accepted;
};

class Learner : public Role {
public:
    Learner() = default;
    void run() override;
};


class Proposer : public Role {
public:
    Proposer() = default;
    void run() override;
private:
    Proposal* generate_proposal();

    std::shared_ptr<Proposal> last_proposal;
};