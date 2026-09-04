# xv6-labs-2023 · MIT 6.1810 个人学习记录

> MIT 6.1810 Operating System Engineering（xv6 RISC-V）课程实验的个人学习记录。

## 声明

本仓库是**个人学习记录**，包含 xv6 实验的个人实现代码与学习笔记。

- 代码仅供学习参考，**禁止用于任何课程的作业提交或抄袭**
- xv6 本身由 MIT 以 MIT License 开源，本仓库代码基于 MIT 官方 lab 框架（来源镜像：[eecsmap/xv6-labs-2023](https://github.com/eecsmap/xv6-labs-2023)）修改而来

## 课程信息

- 课程官网：<https://pdos.csail.mit.edu/6.1810/2023/>
- xv6 book：<https://pdos.csail.mit.edu/6.1810/2023/xv6/book-riscv-rev3.pdf>

## 实验进度

| Lab | 状态 | 说明 |
|-----|------|------|
| Lab 1: Xv6 and Unix utilities | 🚧 进行中 | sleep ✅ / pingpong ✅ / primes / find / xargs 待做 |

## 环境

- Windows 11 + WSL2 (Ubuntu 24.04)
- 交叉编译：gcc-riscv64-linux-gnu / binutils-riscv64-linux-gnu
- 模拟器：qemu-system-misc

## 目录结构

```
user/   # 用户程序（实验作业都写在这里）
kernel/ # xv6 内核源码
Makefile# 构建规则（UPROGS 登记新程序）
```

个人实验代码：`user/sleep.c`（sleep 系统调用练习）、`user/pingpong.c`（管道练习）

## 使用方法

```bash
make qemu    # 编译并启动 xv6
./grade-lab-util xxx   # 运行单个 lab 的评分测试
```
