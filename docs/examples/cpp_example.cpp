// CLEARMotion Pro — C/C++ 调用示例
// 编译: g++ -std=c++17 -O2 -o example cpp_example.cpp -lCLEARMotionPro
// 或 MSVC: cl /std:c++17 /O2 cpp_example.cpp CLEARMotionPro_x64.lib

#include "CLEARMotionPro.h"
#include <cstdio>
#include <chrono>
#include <thread>

// ====================================================================
// 示例 1：PhysicsEngine — 粒子弹簧网络
// ====================================================================
void example_physics_engine() {
    printf("=== 示例 1: PhysicsEngine 粒子弹簧网络 ===\n\n");

    // 1. 创建引擎
    CMPhysicsEngine engine = cm_engine_create();

    // 2. 创建三个实体
    CMEntityConfig cfg_a = {
        .position = {0.0, 0.0},
        .velocity = {0.0, 0.0},
        .physics = {1.0, 0.6, 0.1},
        .transform = {1.0, 0.0}
    };
    CMEntityConfig cfg_b = {
        .position = {100.0, 0.0},
        .velocity = {0.0, 0.0},
        .physics = {1.0, 0.6, 0.1},
        .transform = {1.0, 0.0}
    };
    CMEntityConfig cfg_c = {
        .position = {50.0, 80.0},
        .velocity = {0.0, 0.0},
        .physics = {2.0, 0.6, 0.1},  // 质量更大
        .transform = {1.0, 0.0}
    };

    cm_engine_create_entity(engine, "a", cfg_a);
    cm_engine_create_entity(engine, "b", cfg_b);
    cm_engine_create_entity(engine, "c", cfg_c);

    // 3. 创建弹簧连接
    CMSpringConfig spring_cfg = {
        .rest_length = 100.0,
        .stiffness = 0.5,
        .damping = 0.3
    };
    cm_engine_create_spring(engine, "spring_ab", "a", "b", spring_cfg);
    cm_engine_create_spring(engine, "spring_bc", "b", "c", spring_cfg);
    cm_engine_create_spring(engine, "spring_ca", "c", "a", spring_cfg);

    // 4. 启动引擎
    cm_engine_start(engine);

    // 5. 模拟 120 帧（约 2 秒 @ 60fps）
    const double dt = 1.0 / 60.0;
    for (int i = 0; i < 120; i++) {
        cm_engine_update(engine, dt);

        if (i % 30 == 0) {  // 每 30 帧打印一次状态
            CMEntity ea = *static_cast<Entity*>(cm_engine_get_entity(engine, "a"));
            CMEntity eb = *static_cast<Entity*>(cm_engine_get_entity(engine, "b"));
            CMEntity ec = *static_cast<Entity*>(cm_engine_get_entity(engine, "c"));
            printf("  帧 %3d: A(%.1f, %.1f)  B(%.1f, %.1f)  C(%.1f, %.1f)\n",
                   i, ea.position.x, ea.position.y,
                   eb.position.x, eb.position.y,
                   ec.position.x, ec.position.y);
        }
    }

    // 6. 停止并销毁
    cm_engine_stop(engine);
    cm_engine_destroy(engine);
    printf("\n");
}

// ====================================================================
// 示例 2：SpringAnimator — 弹性动画
// ====================================================================
void example_spring_animator() {
    printf("=== 示例 2: SpringAnimator 弹性动画 ===\n\n");

    // 创建弹簧动画器：从 0 弹到 300
    CMSpringAnimator anim = cm_spring_animator_create(
        0.0,    // initial_value
        170.0,  // stiffness（默认）
        26.0,   // damping（默认）
        1.0,    // mass（默认）
        0.0,    // gravity
        0.05,   // roughness
        0.01    // air_resistance
    );

    // 设置目标值
    cm_spring_animator_set_target(anim, 300.0);

    // 驱动动画
    const double dt = 1.0 / 60.0;
    int frames = 0;
    while (cm_spring_animator_is_running(anim)) {
        cm_spring_animator_update(anim, dt);
        double value = cm_spring_animator_get_value(anim);
        double vel = cm_spring_animator_get_velocity(anim);

        if (frames % 15 == 0) {
            printf("  帧 %3d: value = %.2f, velocity = %.2f\n", frames, value, vel);
        }
        frames++;
    }

    printf("  动画结束，最终 value = %.4f（共 %d 帧）\n\n",
           cm_spring_animator_get_value(anim), frames);

    cm_spring_animator_destroy(anim);
}

// ====================================================================
// 示例 3：ScrollSimulator — 滚动模拟
// ====================================================================
void example_scroll_simulator() {
    printf("=== 示例 3: ScrollSimulator 滚动模拟 ===\n\n");

    CMScrollSimulator scroll = cm_scroll_create();

    // 设置滚动范围：0 ~ 500
    cm_scroll_set_bounds(scroll, 0.0, 500.0);

    // 模拟触摸拖拽
    cm_scroll_start(scroll, 100.0);       // 从 position=100 开始
    cm_scroll_move(scroll, -50.0);        // 向上拖 50px
    cm_scroll_move(scroll, -30.0);        // 继续向上拖 30px
    cm_scroll_end(scroll, -800.0);        // 松手，速度 -800

    // 驱动惯性动画
    const double dt = 1.0 / 60.0;
    int frames = 0;
    while (cm_scroll_get_velocity(scroll) != 0.0 ||
           cm_scroll_is_dragging(scroll)) {
        cm_scroll_update(scroll, dt);
        double pos = cm_scroll_get_position(scroll);
        double vel = cm_scroll_get_velocity(scroll);

        if (frames % 20 == 0) {
            printf("  帧 %3d: position = %.2f, velocity = %.2f\n", frames, pos, vel);
        }
        frames++;

        if (frames > 300) break;  // 安全退出
    }

    printf("  滚动结束，最终 position = %.2f\n\n",
           cm_scroll_get_position(scroll));

    cm_scroll_destroy(scroll);
}

// ====================================================================
// 示例 4：组合使用 — 物理按钮弹性跟随
// ====================================================================
void example_combined() {
    printf("=== 示例 4: 组合使用 — 按钮弹性跟随 ===\n\n");

    // 弹簧动画器模拟按钮按下后弹性恢复
    CMSpringAnimator btn_x = cm_spring_animator_create(0.0, 200.0, 20.0, 1.0, 0.0, 0.03, 0.005);
    CMSpringAnimator btn_y = cm_spring_animator_create(0.0, 200.0, 20.0, 1.0, 0.0, 0.03, 0.005);
    CMSpringAnimator btn_scale = cm_spring_animator_create(1.0, 300.0, 30.0, 1.0, 0.0, 0.02, 0.01);

    // 模拟按钮被按下
    printf("  按钮被按下 → scale 缩小到 0.95, 偏移 (5, 5)\n");
    cm_spring_animator_set_target(btn_x, 5.0);
    cm_spring_animator_set_target(btn_y, 5.0);
    cm_spring_animator_set_target(btn_scale, 0.95);

    const double dt = 1.0 / 60.0;
    for (int i = 0; i < 60; i++) {
        cm_spring_animator_update(btn_x, dt);
        cm_spring_animator_update(btn_y, dt);
        cm_spring_animator_update(btn_scale, dt);
    }

    // 模拟按钮松手
    printf("  按钮松手 → 弹性恢复到原位\n");
    cm_spring_animator_set_target(btn_x, 0.0);
    cm_spring_animator_set_target(btn_y, 0.0);
    cm_spring_animator_set_target(btn_scale, 1.0);

    int frames = 0;
    while (cm_spring_animator_is_running(btn_x) ||
           cm_spring_animator_is_running(btn_scale)) {
        cm_spring_animator_update(btn_x, dt);
        cm_spring_animator_update(btn_y, dt);
        cm_spring_animator_update(btn_scale, dt);

        if (frames % 15 == 0) {
            printf("  帧 %3d: pos(%.2f, %.2f) scale=%.4f\n",
                   frames,
                   cm_spring_animator_get_value(btn_x),
                   cm_spring_animator_get_value(btn_y),
                   cm_spring_animator_get_value(btn_scale));
        }
        frames++;
    }

    printf("\n");
    cm_spring_animator_destroy(btn_x);
    cm_spring_animator_destroy(btn_y);
    cm_spring_animator_destroy(btn_scale);
}

// ====================================================================
int main() {
    printf("CLEAR Motion Pro — C/C++ 调用示例\n");
    printf("==================================\n\n");

    example_physics_engine();
    example_spring_animator();
    example_scroll_simulator();
    example_combined();

    printf("所有示例执行完毕。\n");
    return 0;
}
