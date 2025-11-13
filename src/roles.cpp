#include "roles.hpp"

void Acceptor::run() {
    /*
    (b) If an acceptor receives a prepare request with number n greater
    than that of any prepare request to which it has already responded,
    then it responds to the request with a promise not to accept any more
    proposals numbered less than n and with the highest-numbered proposal (if any) that it has accepted.
    */
    is_running = true;
    auto& mtx = get_mutex();
    auto& q = get_dispatch_queue();
    auto& c = get_cond();
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        if (is_running && q.empty()) c.wait(lock, [&]{return !q.empty() || !is_running;});
        if (!is_running && q.empty()) break;
        auto msg = q.front();
        q.pop_front();
        if (!msg.proposal) {
            lock.unlock();
            continue;
        }
        
        if (msg.type == MessageType::PREPARE_REQ) {
            
            if (promise) {
                if (msg.proposal->n > promise->proposal.n) {
                    promise->promisee = msg.sender;
                    promise->proposal = *msg.proposal;
                    Message prep_response = Message(MessageType::PREPARE_RESP, msg.receiver, msg.sender, accepted);
                    msg.receiver->send_message(prep_response);
                }
            } else {
                promise = std::make_shared<Promise>();
                promise->promisee = msg.sender;
                promise->proposal = *msg.proposal;
                Message prep_response = Message(MessageType::PREPARE_RESP, msg.receiver, msg.sender, accepted);
                msg.receiver->send_message(prep_response);
            }
        } else if (msg.type == MessageType::ACCEPT_REQ) {
            // acceptor must have responded to at least one prepare request
            if (promise) {
                if (!(promise->proposal.n > msg.proposal->n)) {
                    accepted = msg.proposal;
                    Message decision = Message(MessageType::DECISION, msg.receiver, msg.sender, msg.proposal);
                    msg.receiver->broadcast(decision);
                }
            }
        }
    }   
}

void Proposer::run() {
    /*
    (a) A proposer selects a proposal number n and sends a prepare
    request with number n to a majority of acceptors.
    */
    is_running = true;
    auto& mtx = get_mutex();
    auto& q = get_dispatch_queue();
    auto& c = get_cond();
    std::shared_ptr<Proposal> proposal = generate_proposal();
    while (true) {
        
        std::unique_lock<std::mutex> lock(mtx);


        lock.unlock();

    }







}


