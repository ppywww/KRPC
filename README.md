# Krpc - 高性能分布式RPC框架

## 项目简介

Krpc 是一个基于 C++11 实现的高性能分布式 RPC（远程过程调用）框架，参考 Raft 共识算法的 RPC 设计理念，自主设计并实现了完整的服务注册、发现与远程调用系统。

## 技术栈

| 技术 | 说明 |
|------|------|
| C++11 | 主要开发语言 |
| Muduo | 高性能网络库，基于 Reactor 模式 |
| Protobuf | 数据序列化和 RPC 接口定义 |
| ZooKeeper | 服务注册与发现中心 |
| glog | 高性能日志库 |

## 核心特性

- **高性能网络模型**：基于 Muduo Reactor 模式，支持 5000+ 并发连接
- **自定义通信协议**：解决 TCP 粘包/拆包问题，支持 QPS > 10000，平均延迟 < 5ms
- **服务注册与发现**：基于 ZooKeeper 实现服务的自动注册和发现，保证高可用性
- **多日志级别**：支持 DEBUG、INFO、WARNING、ERROR、FATAL 等级别
- **配置管理**：支持从配置文件读取日志路径、服务地址等参数

## 目录结构

```
Krpc/
├── CMakeLists.txt          # CMake 构建配置
├── README.md               # 项目文档
├── bin/
│   └── test.conf           # 测试配置文件
├── build/                  # 构建目录
├── docs/                   # 项目文档
│   ├── 敏捷开发文档.md
│   └── 开发过程详细记录.md
├── example/                # 示例代码
│   ├── caller/             # RPC 客户端示例
│   │   └── Kclient.cc
│   ├── callee/             # RPC 服务端示例
│   │   └── Kserver.cc
│   └── user.proto          # 用户服务接口定义
└── src/                    # 核心框架源码
    ├── KrpcApplication.cc  # 框架初始化
    ├── Krpcconfig.cc       # 配置加载
    ├── Krpcprovider.cc    # 服务端提供者
    ├── Krpcchannel.cc      # 客户端 Channel
    ├── Krpccontroller.cc  # RPC 控制器
    ├── zookeeperutil.cc    # ZooKeeper 工具
    ├── KrpcLogger.cc      # 日志模块
    └── include/           # 头文件目录
        ├── Krpcapplication.h
        ├── Krpcconfig.h
        ├── Krpcprovider.h
        ├── Krpcchannel.h
        ├── Krpccontroller.h
        ├── zookeeperutil.h
        └── KrpcLogger.h
```

## 自定义 RPC 协议

为解决 TCP 粘包/拆包问题，设计了基于长度前缀的自定义协议：

```
┌────────────┬──────────────┬────────────┬────────────┐
│ 4B Total   │ 4B HeaderLen │  Header    │   Body     │
│   Length   │             │ (Protobuf) │ (Protobuf) │
└────────────┴──────────────┴────────────┴────────────┘
```

- **Total Length**：消息总长度（4字节，uint32）
- **Header Length**：头部长度（4字节，uint32）
- **Header**：服务名、方法名、请求ID等元信息
- **Body**：序列化的请求/响应数据

## 快速开始

### 环境要求

- GCC 4.8+
- CMake 3.0+
- ZooKeeper
- Protobuf
- glog
- Muduo

### 编译项目

```bash
# 创建构建目录
mkdir build && cd build

# 配置 CMake
cmake ..

# 编译
make -j4
```

### 运行示例

```bash
# 启动 ZooKeeper 服务
zkServer.sh start

# 启动 RPC 服务端
./bin/server -i ../bin/test.conf

# 启动 RPC 客户端
./bin/client -i ../bin/test.conf
```

## 项目架构

### 整体架构图

```
┌─────────────────────────────────────────────────────────────┐
│                          客户端                             │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────────┐ │
│  │   Stub      │───▶│ KrpcChannel │───▶│  ZooKeeper 查询  │ │
│  │ (RPC Proxy) │    │             │    │   获取服务地址   │ │
│  └─────────────┘    └─────────────┘    └─────────────────┘ │
└─────────────────────────────────────────────────────────────┘
                           │ TCP
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                          服务端                             │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────────┐ │
│  │  网络接收   │───▶│ KrpcProvider│───▶│   业务处理       │ │
│  │  (Muduo)   │    │  (分发 RPC) │    │  (UserService)  │ │
│  └─────────────┘    └─────────────┘    └─────────────────┘ │
│                           │                                 │
│                           ▼                                 │
│                   ┌─────────────┐                           │
│                   │   ZooKeeper │                           │
│                   │  (服务注册)  │                           │
│                   └─────────────┘                           │
└─────────────────────────────────────────────────────────────┘
```

### 核心模块

| 模块 | 文件 | 职责 |
|------|------|------|
| 服务发布 | Krpcprovider.cc | 使用 Muduo 实现网络接收，发布服务到 ZooKeeper |
| 服务调用 | Krpcchannel.cc | 客户端 RPC 调用，查询 ZooKeeper 获取服务地址 |
| 配置管理 | Krpcconfig.cc | 解析配置文件 (key=value 格式) |
| 服务注册 | zookeeperutil.cc | 将服务节点注册到 ZooKeeper，实现服务发现 |
| 日志系统 | KrpcLogger.cc | 基于 glog 的高性能日志，支持多级别输出 |

## 性能指标

| 指标 | 数值 |
|------|------|
| 并发连接数 | 5000+ |
| QPS | > 10000 |
| 平均延迟 | < 5ms |

## 个人收获

- 深入理解分布式 RPC 架构的核心原理与实现细节
- 熟练使用 ZooKeeper 进行服务注册与发现，掌握其节点监听机制
- 基于 Muduo 库实现高并发网络通信，提升网络编程能力
- 培养从需求分析到架构设计、编码实现的全流程工程能力

## License

MIT License
