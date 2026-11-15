// CLEARMotionPro.h - 物理动效引擎 C FFI 接口
// CLEARoffice Physics Engine (C++17)
// 供 Rust 通过 FFI 调用

#pragma once

#include <cstdint>

// ===== 版本 =====
#define CLEARMOTIONPRO_VERSION_MAJOR 1
#define CLEARMOTIONPRO_VERSION_MINOR 0
#define CLEARMOTIONPRO_VERSION_PATCH 0
#define CLEARMOTIONPRO_VERSION_STRING "1.0.0"

// ===== 跨平台 DLL 导出宏 =====
#if defined(_WIN32) && defined(CLEARMOTIONPRO_STATIC)
  #define CM_API
#elif defined(_WIN32) && defined(CLEARMOTIONPRO_EXPORTS)
  #define CM_API __declspec(dllexport)
#elif defined(_WIN32)
  #define CM_API __declspec(dllimport)
#elif defined(__GNUC__) && __GNUC__ >= 4
  #define CM_API __attribute__((visibility("default")))
#else
  #define CM_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ===== 向量 =====
typedef struct {
    double x;
    double y;
} CMVec2;

// ===== 物理配置 =====
typedef struct {
    double mass;
    double restitution;
    double drag;
} CMPhysicsConfig;

// ===== 变换配置 =====
typedef struct {
    double scale;
    double rotation;
} CMTransformConfig;

// ===== 实体创建配置 =====
typedef struct {
    CMVec2 position;
    CMVec2 velocity;
    CMPhysicsConfig physics;
    CMTransformConfig transform;
} CMEntityConfig;

// ===== 弹簧配置 =====
typedef struct {
    double rest_length;
    double stiffness;
    double damping;
} CMSpringConfig;

// ===== 引擎不透明指针 =====
typedef void* CMPhysicsEngine;

// ===== 实体不透明指针 =====
typedef void* CMEntity;

// ===== 弹簧动画器不透明指针 =====
typedef void* CMSpringAnimator;

// ===== 滚动模拟器不透明指针 =====
typedef void* CMScrollSimulator;

// --- PhysicsEngine API ---
CM_API CMPhysicsEngine cm_engine_create(void);
CM_API void cm_engine_destroy(CMPhysicsEngine engine);
CM_API CMEntity cm_engine_create_entity(CMPhysicsEngine engine, const char* id, CMEntityConfig config);
CM_API void cm_engine_remove_entity(CMPhysicsEngine engine, const char* id);
CM_API void cm_engine_create_spring(CMPhysicsEngine engine, const char* id,
                              const char* point_a, const char* point_b,
                              CMSpringConfig config);
CM_API void cm_engine_remove_spring(CMPhysicsEngine engine, const char* id);
CM_API void cm_engine_update(CMPhysicsEngine engine, double dt);
CM_API void cm_engine_start(CMPhysicsEngine engine);
CM_API void cm_engine_stop(CMPhysicsEngine engine);
CM_API int32_t cm_engine_is_running(CMPhysicsEngine engine);
CM_API CMVec2 cm_entity_get_position(CMEntity entity);
CM_API CMVec2 cm_entity_get_velocity(CMEntity entity);
CM_API void cm_entity_set_velocity(CMEntity entity, CMVec2 velocity);
CM_API void cm_entity_apply_force(CMEntity entity, CMVec2 force);
CM_API CMEntity cm_engine_get_entity(CMPhysicsEngine engine, const char* id);

// --- SpringAnimator API ---
CM_API CMSpringAnimator cm_spring_animator_create(double initial_value,
                                            double stiffness, double damping,
                                            double mass, double gravity,
                                            double roughness, double air_resistance);
CM_API void cm_spring_animator_destroy(CMSpringAnimator anim);
CM_API void cm_spring_animator_set_target(CMSpringAnimator anim, double target);
CM_API void cm_spring_animator_update(CMSpringAnimator anim, double dt);
CM_API double cm_spring_animator_get_value(CMSpringAnimator anim);
CM_API double cm_spring_animator_get_velocity(CMSpringAnimator anim);
CM_API int32_t cm_spring_animator_is_running(CMSpringAnimator anim);

// --- ScrollSimulator API ---
CM_API CMScrollSimulator cm_scroll_create(void);
CM_API void cm_scroll_destroy(CMScrollSimulator scroll);
CM_API void cm_scroll_set_bounds(CMScrollSimulator scroll, double min_pos, double max_pos);
CM_API void cm_scroll_start(CMScrollSimulator scroll, double position);
CM_API void cm_scroll_move(CMScrollSimulator scroll, double delta);
CM_API void cm_scroll_end(CMScrollSimulator scroll, double velocity);
CM_API void cm_scroll_update(CMScrollSimulator scroll, double dt);
CM_API double cm_scroll_get_position(CMScrollSimulator scroll);
CM_API double cm_scroll_get_velocity(CMScrollSimulator scroll);
CM_API int32_t cm_scroll_is_dragging(CMScrollSimulator scroll);

#ifdef __cplusplus
}
#endif
