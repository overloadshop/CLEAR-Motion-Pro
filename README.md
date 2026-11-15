# CLEAR Motion Pro

> **双源同步** · [GitHub](https://github.com/overloadshop/CLEAR-Motion-Pro) / [GitCode](https://gitcode.com/bananabanxian/CLEAR-Motion-Pro)

C++17 原生物理动效引擎。**本仓库只分发预编译二进制与公开头文件，不包含实现源码。**
通过稳定的 C FFI 接口供 C / C++ / Rust / Java(JNI) / Kotlin / C# / Go / Dart 等语言调用。

物理模型与 [CLEAR Motion](https://github.com/overloadshop/CLEAR-Motion) 标准版一致（胡克定律、
欧拉积分、半隐式欧拉），区别在于原生实现：没有脚本解释器开销，更新节奏完全由调用方控制。

- **下载**：[GitHub Releases](https://github.com/overloadshop/CLEAR-Motion-Pro/releases/latest) /
  [GitCode Releases](https://gitcode.com/bananabanxian/CLEAR-Motion-Pro/releases)，zip 内含头文件、各平台二进制与文档。

## 仓库内容

| 路径 | 内容 |
| --- | --- |
| `include/CLEARMotionPro.h` | 唯一的公开头文件（C FFI 接口） |
| `prebuilt/android/<abi>/` | Android 预编译库，四个 ABI |
| `prebuilt/android-16kb/arm64-v8a/` | Android 15+ 16KB 页对齐版本 |
| `prebuilt/windows/<arch>/` | Windows DLL 与导入库 |
| `docs/` | 白皮书、操作手册与各语言调用示例 |
| `LICENSE` / `NOTICE` | 二进制库许可协议与版权声明 |

不含：引擎源码、构建脚本、单元测试。需要自己编译或做深度定制，请走商务授权。

## 三个模块

| 模块 | 解决什么 | 典型场景 |
| --- | --- | --- |
| `PhysicsEngine` | 二维实体与弹簧网络 | 粒子、布料、弹性布局 |
| `ScrollSimulator` | 一维滚动：拖拽跟随、松手惯性、越界回弹 | 自定义滚动视图 |
| `SpringAnimator` | 任意数值沿弹簧曲线过渡 | 位移、透明度、弹性跟随 |

三者互相独立，可同时运行多个实例。

## 支持平台

| 平台 | 产物 | 位置 |
| --- | --- | --- |
| Android | `libCLEARMotionPro.so`（arm64-v8a / armeabi-v7a / x86 / x86_64） | `prebuilt/android/<abi>/` |
| Android 15+ 16KB 页 | `libCLEARMotionPro.so`（arm64-v8a） | `prebuilt/android-16kb/arm64-v8a/` |
| Windows x64 / x86 | `CLEARMotionPro_x64.dll` / `_x86.dll` + 导入库 `.lib` | `prebuilt/windows/<arch>/` |

> Android 侧文件名统一为 `libCLEARMotionPro.so`，直接放进 `jniLibs/<abi>/` 即可被
> `System.loadLibrary("CLEARMotionPro")` 找到，不需要改名。

## 快速开始

### C / C++

```cpp
#include "CLEARMotionPro.h"

CMSpringAnimator anim = cm_spring_animator_create(
    0.0,    // 初始值
    380.0,  // 刚度
    14.0,   // 阻尼
    1.0,    // 质量
    0.0,    // 附加重力
    0.05,   // 粗糙度
    0.01);  // 空气阻力

cm_spring_animator_set_target(anim, 1.0);
while (running) {
    cm_spring_animator_update(anim, 1.0 / 60.0);
    const double value = cm_spring_animator_get_value(anim);
}
cm_spring_animator_destroy(anim);
```

Windows 链接 `prebuilt/windows/x64/CLEARMotionPro_x64.lib`，运行时把同名 DLL 放到可执行文件旁；
Android 把 `.so` 放进 `jniLibs/<abi>/`。

### Android / JNI

1. 复制 `prebuilt/android/<abi>/libCLEARMotionPro.so` 到 `app/src/main/jniLibs/<abi>/`；
2. `static { System.loadLibrary("CLEARMotionPro"); }`；
3. JNI 声明与调用见 [`docs/examples/java_jni_example.java`](docs/examples/java_jni_example.java)。

Android 15+ 且要求 16KB 页对齐的设备，改用 `prebuilt/android-16kb/arm64-v8a/` 下的库。

### 其他语言

[`docs/examples/`](docs/examples/) 提供 C++、C#（P/Invoke）、Java JNI、Rust 与纯 C 的 JNI 桥接示例。

## 文档与白皮书

- [`docs/CLEAR-Motion-Pro.html`](docs/CLEAR-Motion-Pro.html) — 设计白皮书与完整操作手册
  （物理模型、三个模块、参数语义、十种语言的调用示例）
- [`docs/CLEAR-Motion.html`](docs/CLEAR-Motion.html) — 标准版（JavaScript）的设计文档，
  两版共用同一套物理模型
- 直接双击 HTML 打开即可，无需服务器

## 接口速查

所有对象都是不透明指针，调用方只持有句柄：

- 引擎：`cm_engine_create` / `cm_engine_create_entity` / `cm_engine_create_spring` /
  `cm_engine_update` / `cm_engine_destroy`
- 滚动：`cm_scroll_create` / `cm_scroll_set_bounds` / `cm_scroll_start` / `cm_scroll_move` /
  `cm_scroll_end` / `cm_scroll_update` / `cm_scroll_destroy`
- 弹簧：`cm_spring_animator_create` / `cm_spring_animator_set_target` /
  `cm_spring_animator_update` / `cm_spring_animator_destroy`

时间步长由调用方传入（通常 `1.0 / 60.0`），引擎不做内部计时。

## 许可

本仓库以预编译二进制形式分发，适用 [CLEAR Motion Pro 二进制库许可协议](LICENSE)。
**不是开源项目，不提供源代码**：反编译、修改二进制、单独转售或作为独立 SDK 再分发需要书面授权。

允许的使用方式：

- 从官方仓库（含 Releases）、官网或授权渠道获取；
- 嵌入应用或随项目仓库、构建产物一并分发，保留 `LICENSE` 与 `NOTICE`；
- 对外发布成品时标注 `Powered by CLEAR Motion Pro` 与版权信息。

标准版 CLEAR Motion 为 MIT，与本仓库相互独立。

---

© 2026 芜湖灵动亿创网络科技有限公司 (Wuhu Lindicra Network Technology Co., Ltd)

本项目的法律主体以上述中文名称为准。
