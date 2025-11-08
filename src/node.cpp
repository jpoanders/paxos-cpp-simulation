#include "node.hpp"

Node::Node() {
    n_roles = 3u;
    acceptor = new Acceptor();
    learner = new Learner();
    proposer = new Proposer();
    disp_q_mtxs = new std::mutex[n_roles];
    disp_q_cond = new std::condition_variable[n_roles];
}

Node::~Node() {
    join();
    delete acceptor;
    delete learner;
    delete proposer;
    delete[] disp_q_mtxs;
    delete[] disp_q_cond;
}

void Node::run() {
    is_running = true;
    
    t_listener = std::thread(&Node::listen, this);
    if (acceptor) t_acceptor = std::thread(&Acceptor::run, &acceptor);
    if (learner) t_learner = std::thread(&Learner::run, &learner);
    if (proposer) t_proposer = std::thread(&Proposer::run, &proposer);
}

void Node::join() {
    t_listener.join();
    if (acceptor) t_acceptor.join();
    if (learner) t_learner.join();
    if (proposer) t_proposer.join();
    is_running = false;
}

void Node::listen() {
    auto& mtx = mailbox.get_mutex();
    auto& cond = mailbox.get_cond();
    auto& q = mailbox.get_msg_queue();
    while(true) {
        std::unique_lock<std::mutex> lock(mtx);
        if (q.empty()) cond.wait(lock, [&]{ return !q.empty();});
        auto msg = q.front();
        q.pop_front();
        lock.unlock();
        
        switch (msg.type) {
            case MessageType::PREPARE_REQ:
                dispatch(0u, msg);
            case MessageType::ACCEPT_REQ:
                dispatch(0u, msg);
            case MessageType::PREPARE_RESP:
                dispatch(2u, msg);
            case MessageType::DECISION:
                dispatch(1u, msg);
        }

        if (!is_running)
            break;
    }
}