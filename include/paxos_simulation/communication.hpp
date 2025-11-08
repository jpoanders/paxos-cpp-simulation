#pragma once
#include "roles.hpp"
#include "node.hpp"
#include <mutex>
#include <condition_variable>
#include <deque>
#include <unordered_map>

enum class MessageType {PREPARE_REQ, ACCEPT_REQ, PREPARE_RESP, DECISION};

struct Message {
    MessageType type;
    Role* sender;
    Role* receiver;
    void* data;    
};

class Mailbox {
public:
    void push(Message& msg);
    Message pop();

    std::mutex& get_mutex() {return mtx;}

    std::condition_variable& get_cond() {return cond;}

    std::deque<Message>& get_msg_queue() {return q;}

private:
    std::mutex mtx;
    std::condition_variable cond;
    std::deque<Message> q;
};

class Network {
public:
    void send(Message& msg);

private:
    double generator();

    std::unordered_map<Node&, Mailbox> mailboxes_map;
    double drop_rate;
    bool delay_enabled;
    int min_delay_ms;
    int max_delay_ms;
};