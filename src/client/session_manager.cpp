#include "session_manager.hpp"
#include "strings.hpp"
#include <iostream>

void SessionManager::register_session(const std::string& id, std::function<void(std::string)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_[id] = callback;
    condition_variables_[id];
}

void SessionManager::remove_session(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.erase(id);
    condition_variables_.erase(id);
}

void SessionManager::dispatch_response(const std::string& id, const std::string& response) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (sessions_.count(id)) {
        responses_[id] = response;
        sessions_[id](response);
        condition_variables_[id].notify_one();
        sessions_.erase(id);
    }
}

void SessionManager::wait_for_response(const std::string& id) {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_variables_[id].wait(lock, [&]() { 
        return responses_.count(id) > 0; 
    });
    responses_.erase(id);
    condition_variables_.erase(id);
}
