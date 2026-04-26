
#ifndef KRPC_LOG_H
#define KRPC_LOG_H

#include <glog/logging.h>
#include <string>

#define KRPC_LOG_DEBUG(msg) LOG(INFO) << msg
#define KRPC_LOG_INFO(msg) LOG(INFO) << msg
#define KRPC_LOG_WARNING(msg) LOG(WARNING) << msg
#define KRPC_LOG_ERROR(msg) LOG(ERROR) << msg
#define KRPC_LOG_FATAL(msg) LOG(FATAL) << msg

class KrpcLogger {
public:
    explicit KrpcLogger(const char* argv0, const std::string& log_dir = "") {
        
        FLAGS_colorlogtostderr = true;
        
        if (!log_dir.empty()) {
            FLAGS_log_dir = log_dir;
            FLAGS_logtostderr = false;
        } else {
            FLAGS_logtostderr = true;
        }
        
        FLAGS_minloglevel = 0;
        FLAGS_max_log_size = 100;

        google::InitGoogleLogging(argv0);
    }
    
    ~KrpcLogger() {
        google::ShutdownGoogleLogging();
    }
    
    static void Info(const std::string& message) { LOG(INFO) << message; }
    static void Warning(const std::string& message) { LOG(WARNING) << message; }
    static void ERROR(const std::string& message) { LOG(ERROR) << message; }
    static void Fatal(const std::string& message) { LOG(FATAL) << message; }

private:
    KrpcLogger(const KrpcLogger&) = delete;
    KrpcLogger& operator=(const KrpcLogger&) = delete;
};

#endif