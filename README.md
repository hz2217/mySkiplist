# KV存储引擎

基于跳表实现的轻量级键值型存储引擎，C++实现插入数据、删除数据、查询数据、数据展示、数据落盘、文件加载数据，以及数据库大小显示等功能。



## 项目文件

```c++
.
├── README.md
├── bin					// 生成可执行文件目录
│   ├── main
│   └── stress
├── main.cc				// 包含skiplist.h使用跳表进行数据操作
├── makefile			// 编译脚本
├── skiplist			// 仿照redis实现的跳表数据结构
│   ├── a.out
│   ├── main.cc
│   └── skiplist.h
├── skiplist.h			// kv存储引擎核心实现
├── store
│   └── dumpFile		// 存储文件
└── test
    ├── stress_test.cc	// 压力测试
    └── stress_test.sh	// 压力测试脚本
```

API

```c++
bool search(K);		// 查询数据
int insert(K, V);	// 插入数据
void remove(K);		// 删除数据
void display();		// 展示存储
int size();			// 存储大小
void dump_file();	// 存储到文件
void load_file();	// 从文件读取
```



## 运行

```c++
make
./bin/main
```

使用存储引擎只需要在项目中添加 `#include "skiplist.h"`

### 压力测试

```c++
sh ./test/stress_test.sh
```

树高32

| 插入数据规模（万条） | 耗时（秒） |
| -------------------- | ---------- |
| 10                   | 0.105501   |
| 100                  | 2.19232    |

| 查询数据规模（万条） | 耗时（秒） |
| -------------------- | ---------- |
| 10                   | 0.12241    |
| 100                  | 2.57326    |



## 待优化

- 压力测试并不是全自动的

- 如果再加上一致性协议，例如raft就构成了分布式存储，再启动一个http server就可以对外提供分布式存储服务了