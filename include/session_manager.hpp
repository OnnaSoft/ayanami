#ifndef SESSION_MANAGER_HPP
#define SESSION_MANAGER_HPP

#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <string>

class SessionManager {
public:
    void register_session(const std::string& id, std::function<void(std::string)> callback);
    void remove_session(const std::string& id);
    void dispatch_response(const std::string& id, const std::string& response);
    void wait_for_response(const std::string& id);

private:
    std::unordered_map<std::string, std::function<void(std::string)>> sessions_;
    std::unordered_map<std::string, std::string> responses_;
    std::unordered_map<std::string, std::condition_variable> condition_variables_;
    std::mutex mutex_;
};

#endif
