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

| Lab | 分支 | 状态 | 完成日期 | 内容 | make grade |
|---|---|---|---|---|---|
| Lab 1: Xv6 and Unix utilities | `util` | ✅ 完成 | 2026-09-06 | sleep / pingpong / primes / find / xargs | — |
| Lab 2: System calls | `syscall` | ✅ 完成 | 2026-11-05 | trace / sysinfo | 40/40 |
| Lab 3: Page tables | `pgtbl` | ✅ 完成 | 2026-11-14 | ugetpid / vmprint / pgaccess | 46/46 |
| Lab 4: Traps | `traps` | 🚧 进行中 | — | RISC-V assembly / backtrace / alarm | — |

每个 lab 在独立分支上实现；`util` 是默认分支，只维护这份 README 和实验进度。

## 环境

- Windows 11 + WSL2 (Ubuntu 24.04)
- 交叉编译：gcc-riscv64-linux-gnu / binutils-riscv64-linux-gnu
- 模拟器：qemu-system-misc
- 本机 binutils 2.42 会把用户程序链接成单个 RWX 段，导致代码页可写；用户程序链接时加 `-z separate-code`，让代码段为 R-X、数据段为 RW。

## 目录结构

```text
user/    # 用户程序（实验作业代码）
kernel/  # xv6 内核源码
Makefile # 构建规则（UPROGS 登记新程序）
```

## 个人实验代码

- Lab 1：`user/sleep.c`、`user/pingpong.c`、`user/primes.c`、`user/find.c`、`user/xargs.c`
- Lab 2 / Lab 3：内核改动位于各自的 `syscall`、`pgtbl` 分支

## 使用方法

```bash
make TOOLPREFIX=riscv64-linux-gnu- qemu    # 编译并启动 xv6
make TOOLPREFIX=riscv64-linux-gnu- grade   # 运行当前 lab 的评分测试
```
