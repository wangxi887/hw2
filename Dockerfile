# 使用 Ubuntu 22.04 作为基础镜像
FROM ubuntu:22.04

# 设置非交互模式，避免 apt-get 安装时卡住
ENV DEBIAN_FRONTEND=noninteractive

# 更新包列表并安装 g++ 和 make
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    && rm -rf /var/lib/apt/lists/*

# 设置工作目录
WORKDIR /app

# 将当前项目文件拷贝到容器中
COPY . .

# 编译所有 cpp 文件生成可执行文件 my_program
RUN g++ -std=c++17 *.cpp -o my_program

# 设置容器启动时执行程序
CMD ["./my_program"]
