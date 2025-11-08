#pragma once
#include "roles.hpp"
#include "communication.hpp"
#include <thread>
#include <vector>
#include <deque>
#include <semaphore>

#define DEFAULT_MSG_BUFFER_SIZE 50

class Node {
public:
    Node (bool acceptor = false, bool learner = true, bool proposer = false);

    Node();

    ~Node();

    void run();

    void join();

private:
    void listen();

    void dispatch (std::size_t role, Message& msg);
    
    void worker();
    
    bool is_running = false;
    std::vector<std::deque<Message>> dispatch_queues;
    std::mutex* disp_q_mtxs;
    std::condition_variable* disp_q_cond;
    std::size_t msg_buffer_size = DEFAULT_MSG_BUFFER_SIZE;
    std::size_t n_roles;

    Mailbox mailbox;
    Acceptor* acceptor;
    Learner* learner;
    Proposer* proposer;

    std::thread t_listener;
    std::thread t_acceptor;
    std::thread t_learner;
    std::thread t_proposer;
};