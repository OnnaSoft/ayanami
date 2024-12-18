#include "session_manager.hpp"
#include "strings.hpp"
#include <iostream>

void SessionManager::register_session(const std::string& id, std::function<void(std::string)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_[id] = callback;
}

void SessionManager::remove_session(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.erase(id);
}

void SessionManager::dispatch_response(const std::string& id, const std::string& response) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto id_clean = trim(clean_null_terminated(id));

    if (sessions_.count(id_clean)) {
        responses_[id_clean] = response;
        sessions_[id_clean](response);
        cv_.notify_all();
        sessions_.erase(id_clean);
    }
}

void SessionManager::wait_for_response(const std::string& id) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [&]() { 
        return responses_.count(id) > 0;
    });
    responses_.erase(id);
}
