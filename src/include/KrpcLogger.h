#ifndef KRPC_LOG_H
#define KRPC_LOG_H
#include<glog/logging.h>
#include<string>
#include<sstream>

// 日志宏定义
#define KRPC_LOG_DEBUG(message) do { \
    std::ostringstream oss; \
    oss << message; \
    LOG(INFO) << oss.str(); \
} while(0)

#define KRPC_LOG_INFO(message) do { \
    std::ostringstream oss; \
    oss << message; \
    LOG(INFO) << oss.str(); \
} while(0)

#define KRPC_LOG_WARNING(message) do { \
    std::ostringstream oss; \
    oss << message; \
    LOG(WARNING) << oss.str(); \
} while(0)

#define KRPC_LOG_ERROR(message) do { \
    std::ostringstream oss; \
    oss << message; \
    LOG(ERROR) << oss.str(); \
} while(0)

#define KRPC_LOG_FATAL(message) do { \
    std::ostringstream oss; \
    oss << message; \
    LOG(FATAL) << oss.str(); \
} while(0)

//采用RAII的思想
class KrpcLogger
{
public:
      //构造函数，自动初始化glog
      explicit KrpcLogger(const char *argv0, const std::string& log_dir = "")
      {
        google::InitGoogleLogging(argv0);
        FLAGS_colorlogtostderr=true;//启用彩色日志
        
        if (!log_dir.empty()) {
            // 设置日志目录
            FLAGS_log_dir = log_dir;
            FLAGS_logtostderr = false; // 输出到文件
        } else {
            FLAGS_logtostderr = true; // 默认输出到标准错误
        }
        
        FLAGS_minloglevel = 0; // 最低日志级别
        FLAGS_max_log_size = 100; // 日志文件大小限制（MB）
      }
      ~KrpcLogger(){
        google::ShutdownGoogleLogging();
      }
      //提供静态日志方法
      static void Info(const std::string &message)
      {
        LOG(INFO)<<message;
      }
      static void Warning(const std::string &message){
        LOG(WARNING)<<message;
      }
      static void ERROR(const std::string &message){
        LOG(ERROR)<<message;
      }
      static void Fatal(const std::string& message) {
        LOG(FATAL) << message;
      }
//禁用拷贝构造函数和重载赋值函数
private:
    KrpcLogger(const KrpcLogger&)=delete;
    KrpcLogger& operator=(const KrpcLogger&)=delete;
};



#endif