#include "node.hpp"

Node::Node() {
    n_roles = 3u;
    acceptor = new Acceptor(); roles[RoleType::ACCEPTOR] = acceptor;
    learner = new Learner(); roles[RoleType::LEARNER] = learner;
    proposer = new Proposer(); roles[RoleType::PROPOSER] = proposer;
}

Node::~Node() {
    shutdown();
    delete acceptor;
    delete learner;
    delete proposer;
}

void Node::run() {
    is_running = true;
    
    t_listener = std::thread(&Node::listen, this);
    if (acceptor) t_acceptor = std::thread(&Acceptor::run, &acceptor);
    if (learner) t_learner = std::thread(&Learner::run, &learner);
    if (proposer) t_proposer = std::thread(&Proposer::run, &proposer);
}

void Node::shutdown() {
    auto& mtx = mailbox.get_mutex();
    auto& cond = mailbox.get_cond();

    // implementar


    acceptor->shutdown();
    learner->shutdown();
    proposer->shutdown();    
    t_listener.join();
    if (acceptor) t_acceptor.join();
    if (learner) t_learner.join();
    if (proposer) t_proposer.join();
}

void Node::send_message(Message& msg) {
    network->send(msg);
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
        bool last_dispatch = !is_running;
        lock.unlock();
        cond.notify_all();

        RoleType receiver_type = RoleType::ACCEPTOR;

        if (msg.type == MessageType::PREPARE_RESP)
            receiver_type = RoleType::PROPOSER;
        else if (msg.type == MessageType::DECISION)
            receiver_type = RoleType::LEARNER;
        
        dispatch(receiver_type, msg);

        if (last_dispatch)
            break;
    }
}

void Node::dispatch(RoleType type, Message& msg) {
    auto& queue = roles[type]->get_dispatch_queue();
    auto& mtx = roles[type]->get_mutex();
    auto& cond = roles[type]->get_cond();
    std::unique_lock<std::mutex> lock(mtx);
    queue.push_back(msg);
    lock.unlock();
    cond.notify_all();
}