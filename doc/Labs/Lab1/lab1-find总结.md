# xv6 Lab1 find 题：作业总结与踩坑记录

> 题目：遍历目录树，打印所有**文件名等于指定字符串**的文件的完整路径（如 `find . b` → `./b`、`./a/b`、`./a/aa/b`）

## 0. 这题真正引入的新东西

find 是 lab1 第一道**文件系统题**——一口气引入了 5 个新概念，这也是它感觉难的原因（单看每个都不难，叠加起来认知负荷爆表）：

1. 目录 = 文件，内容是一行行 dirent
2. struct dirent / DIRSIZ
3. stat/fstat 查询元数据（文件带哪些标准信息）
4. "路径拼接"模式（\0 处覆盖写）
5. C 字符串操作的本质理解（指针遍历）

## 1. OS 层面新知识

### 目录 = 内容是一行行 dirent 的"文件"

磁盘只有 inode 编号没有名字；目录负责"名字 → 编号"翻译。读目录 = open + 按 16 字节循环 read：

```c
struct dirent {      // kernel/fs.h，共 16 字节
    ushort inum;     // inode 编号（2 字节）
    char name[DIRSIZ]; // 文件名（14 字节，DIRSIZ=14，不足补 \0）
};

while(read(fd, &de, sizeof(de)) == sizeof(de)){  // 每次读一条
    if(de.inum == 0) continue;            // 空条目（删除留下的洞）
    if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;                         // 不跳 → 无限递归（. 指自己 / .. 指父目录，来回振荡）
    ...
}
```

`.` 和 `..` 是 mkdir 时**真实写入目录内容**的条目，read 一定会读到——find 不跳过就是死循环。

### stat/fstat：文件的标准信息（5 个字段）

```c
struct stat {        // kernel/stat.h
  int dev; uint ino; short type; short nlink; uint64 size;
};
// type: T_DIR(1) 目录 / T_FILE(2) 文件 / T_DEVICE(3) 设备
```

- `fstat(fd, &st)`：系统调用，对已打开 fd
- `stat(path, &st)`：库函数 = open + fstat + close 的包装
- **返回值**：0 = 成功（st 才有效）；-1 = 失败（st 无效，绝不要读！）
- **检查与读取必须成对**：`if(stat(buf, &st) < 0) continue;` 然后才能用 st.type

## 2. C 语言层面新知识（本次最大收获）

### strcmp/strcpy 的本质 = 指针遍历

strcmp(p, q) 从 p、q 指向的位置同步逐字符比较到第一个不同或 \0。**指针指向字符串中间 = 从那里开始的子串**——所以"提取文件名"就是移动指针到最后一个 / 之后：

```c
char *q = path + strlen(path);        // q 指向 \0
while(q > path && q[-1] != '/')       // 往前退到最后一个 '/' 之后
    q--;
strcmp(q, target);                    // 比的已经是文件名，不是路径
```

### memmove = 覆盖式定长拷贝（不是追加！）

- memmove(dst, src, n)：把 src 的 n 字节**覆盖**到 dst 位置，不懂字符串、不补 \0
- "追加"效果 = 调用者先定位到 \0（strlen）再 memmove 覆盖写
- 和 strcpy 区别：strcpy 拷到 \0 停并自动补 \0（字符串语义）；memmove 拷满 n 字节（字节语义）
- 搬定长数组（dirent.name）必须用 memmove：strcpy 会在内部 \0 处只拷一半

### \0 纪律（三句话）

1. read 和 memmove 都**不补 \0**——需要字符串结尾就手动补（`rec[n] = 0` / `p[DIRSIZ] = 0`）
2. 追加 = 在旧 \0 处覆盖写，新写入的内容负责提供新 \0（跳过 \0 写 = 字符串断链）
3. `0` 和 `'\0'` 完全等价（都是字节 0x00）；`'0'` 是 48，是另一个东西！

### 指针 vs 整数

- 指针是"地址"，整数是"距离"：`buf + strlen(buf)` = 起点 + 走完全长 = 字符串末尾（\0 处）
- `strlen(buf)` 是"内容长度"不是"数组长度"（buf[512] 里可能只用了 2 字节）

## 3. 踩坑清单

### 致命坑：用外层 st 判断子项类型（本作业核心 bug）

```c
if(fstat(fd, &st) < 0) ...      // st 被填成【path】的类型

while(read(...)) {
    ...
    if (st.type == T_DIR)       // ✗ st 还是 path 的类型（恒 T_DIR）！
        find(buf, target);      //   每个条目（含文件）都被递归
    else if(...)                // ✗ else if 永远不执行 → 永远不打印！
}
```

**原因**：`struct stat st` 是普通变量，只在 fstat/stat 调用时被填。循环里 read 不碰 st——想知道**当前条目 buf** 的类型，必须显式调用 `stat(buf, &st)` 覆盖 st。

**修复**：
```c
if(stat(buf, &st) < 0) continue;   // 查 buf 的类型（覆盖 st）
if(st.type == T_DIR) find(buf, target);
else if(strcmp(de.name, target) == 0) printf("%s\n", buf);
```

### 小坑清单

| 坑 | 修正 |
|----|------|
| `fprintf(2, "...%s\n")` 缺 path 实参 | 补参数（%s 无实参打印垃圾/崩溃）|
| 错误信息前缀抄成 "ls: cannot stat" | 改成 find: |
| open 失败后没 return，fd=-1 继续 fstat | 加 return |
| 拼写 "opne" | open |
| switch 没有 case T_FILE | 起点是文件时（find b b）无输出，加分支 |
| usage 错误 exit(0) | 惯例 exit(1) |

## 4. 正确代码（完整参考）

```c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void
find(char *path, char *target)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  fd = open(path, O_RDONLY);
  if(fd < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    {
      char *q = path + strlen(path);
      while(q > path && q[-1] != '/')
        q--;
      if(strcmp(q, target) == 0)
        printf("%s\n", path);
    }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0)
        continue;
      if(st.type == T_DIR)
        find(buf, target);
      else if(st.type == T_FILE && strcmp(de.name, target) == 0)
        printf("%s\n", buf);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc != 3){
    fprintf(2, "Usage: find path filename\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
```

## 5. 测试用例（xv6 内）

```
echo > b
mkdir a
echo > a/b
mkdir a/aa
echo > a/aa/b
mkdir a/aa/empty
mkdir deep
echo > deep/x
mkdir deep/deep2
echo > deep/deep2/x
```

| 命令 | 预期 | 验证 |
|------|------|------|
| `find . b` | ./b ./a/b ./a/aa/b | 同名多文件 |
| `find . x` | ./deep/x ./deep/deep2/x | 深层递归 |
| `find . zzz` | 无输出 | 无匹配安静返回 |
| `find a b` | a/b a/aa/b | 非根路径起点 |
| `find b b` | b | 起点是文件（case T_FILE）|

外部评分：`./grade-lab-util find`（仓库根目录跑）

## 6. 心智模型

**find = 拿着"名片簿"（目录）逐张翻：跳过 . 和 ..，拼出新路径，问 stat 这张名片指向的是目录还是文件——目录就走进下一本名片簿（递归），文件就把名字和目标比对，匹配就报出完整地址。**
