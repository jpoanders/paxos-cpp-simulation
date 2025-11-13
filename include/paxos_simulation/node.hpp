#pragma once
#include "roles.hpp"
#include "communication.hpp"
#include <thread>
#include <unordered_map>
#include <deque>

class Node {
public:
    Node (bool acceptor = false, bool learner = true, bool proposer = false);

    Node();

    ~Node();

    void run();

    void shutdown();

    void send_message(Message& msg);

    void broadcast(Message& msg);

private:
    void listen();

    void dispatch (RoleType type, Message& msg);
    
    bool is_running = false;
    std::size_t n_roles;

    Network* network;
    Mailbox mailbox;
    std::unordered_map<RoleType, Role*> roles;
    Role* acceptor;
    Role* learner;
    Role* proposer;
    std::thread t_listener;
    std::thread t_acceptor;
    std::thread t_learner;
    std::thread t_proposer;
};