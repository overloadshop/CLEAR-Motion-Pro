// CLEARMotion Pro — Rust FFI 调用示例
//
// 方式 A：通过 C FFI 直接调用 C++ 动态库（推荐）
// 方式 B：通过 Rust 包装器（rust/src/ffi/mod.rs）调用
//
// 编译（方式 A）：
//   cargo build --release
//   需要 CLEARMotionPro.dll 在运行时路径中

use std::ffi::CString;

// ====================================================================
// 方式 A：直接 C FFI 绑定
// ====================================================================

#[repr(C)]
#[derive(Debug, Clone, Copy, Default)]
struct CMVec2 {
    x: f64,
    y: f64,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
struct CMPhysicsConfig {
    mass: f64,
    restitution: f64,
    drag: f64,
}

#[repr(C)]
#[derive(Debug, Clone, Copy, Default)]
struct CMTransformConfig {
    scale: f64,
    rotation: f64,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
struct CMEntityConfig {
    position: CMVec2,
    velocity: CMVec2,
    physics: CMPhysicsConfig,
    transform: CMTransformConfig,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
struct CMSpringConfig {
    rest_length: f64,
    stiffness: f64,
    damping: f64,
}

extern "C" {
    fn cm_engine_create() -> *mut std::ffi::c_void;
    fn cm_engine_destroy(engine: *mut std::ffi::c_void);
    fn cm_engine_create_entity(
        engine: *mut std::ffi::c_void,
        id: *const std::os::raw::c_char,
        config: CMEntityConfig,
    ) -> *mut std::ffi::c_void;
    fn cm_engine_create_spring(
        engine: *mut std::ffi::c_void,
        id: *const std::os::raw::c_char,
        point_a: *const std::os::raw::c_char,
        point_b: *const std::os::raw::c_char,
        config: CMSpringConfig,
    );
    fn cm_engine_update(engine: *mut std::ffi::c_void, dt: f64);
    fn cm_engine_start(engine: *mut std::ffi::c_void);
    fn cm_engine_stop(engine: *mut std::ffi::c_void);
    fn cm_engine_get_entity(
        engine: *mut std::ffi::c_void,
        id: *const std::os::raw::c_char,
    ) -> *mut std::ffi::c_void;
    fn cm_entity_get_position(entity: *mut std::ffi::c_void) -> CMVec2;

    fn cm_spring_animator_create(
        initial_value: f64,
        stiffness: f64,
        damping: f64,
        mass: f64,
        gravity: f64,
        roughness: f64,
        air_resistance: f64,
    ) -> *mut std::ffi::c_void;
    fn cm_spring_animator_destroy(anim: *mut std::ffi::c_void);
    fn cm_spring_animator_set_target(anim: *mut std::ffi::c_void, target: f64);
    fn cm_spring_animator_update(anim: *mut std::ffi::c_void, dt: f64);
    fn cm_spring_animator_get_value(anim: *mut std::ffi::c_void) -> f64;
    fn cm_spring_animator_is_running(anim: *mut std::ffi::c_void) -> i32;

    fn cm_scroll_create() -> *mut std::ffi::c_void;
    fn cm_scroll_destroy(scroll: *mut std::ffi::c_void);
    fn cm_scroll_set_bounds(scroll: *mut std::ffi::c_void, min_pos: f64, max_pos: f64);
    fn cm_scroll_start(scroll: *mut std::ffi::c_void, position: f64);
    fn cm_scroll_move(scroll: *mut std::ffi::c_void, delta: f64);
    fn cm_scroll_end(scroll: *mut std::ffi::c_void, velocity: f64);
    fn cm_scroll_update(scroll: *mut std::ffi::c_void, dt: f64);
    fn cm_scroll_get_position(scroll: *mut std::ffi::c_void) -> f64;
}

// ====================================================================
// 示例 1：PhysicsEngine — 粒子弹簧网络
// ====================================================================
fn example_physics_engine() {
    println!("=== 示例 1: PhysicsEngine 粒子弹簧网络 ===\n");

    unsafe {
        let engine = cm_engine_create();

        // 创建实体
        let cfg = CMEntityConfig {
            position: CMVec2 { x: 0.0, y: 0.0 },
            velocity: CMVec2 { x: 0.0, y: 0.0 },
            physics: CMPhysicsConfig { mass: 1.0, restitution: 0.6, drag: 0.1 },
            transform: CMTransformConfig::default(),
        };
        cm_engine_create_entity(engine, c"a".as_ptr(), cfg);

        let cfg2 = CMEntityConfig {
            position: CMVec2 { x: 100.0, y: 0.0 },
            ..cfg
        };
        cm_engine_create_entity(engine, c"b".as_ptr(), cfg2);

        // 创建弹簧
        let spring = CMSpringConfig {
            rest_length: 100.0,
            stiffness: 0.5,
            damping: 0.3,
        };
        cm_engine_create_spring(engine, c"spring_ab".as_ptr(), c"a".as_ptr(), c"b".as_ptr(), spring);

        cm_engine_start(engine);

        let dt = 1.0 / 60.0;
        for i in 0..120 {
            cm_engine_update(engine, dt);
            if i % 30 == 0 {
                let ea = cm_entity_get_position(cm_engine_get_entity(engine, c"a".as_ptr()));
                let eb = cm_entity_get_position(cm_engine_get_entity(engine, c"b".as_ptr()));
                println!("  帧 {:3}: A({:.1}, {:.1})  B({:.1}, {:.1})", i, ea.x, ea.y, eb.x, eb.y);
            }
        }

        cm_engine_stop(engine);
        cm_engine_destroy(engine);
    }
    println!();
}

// ====================================================================
// 示例 2：SpringAnimator — 弹性动画
// ====================================================================
fn example_spring_animator() {
    println!("=== 示例 2: SpringAnimator 弹性动画 ===\n");

    unsafe {
        let anim = cm_spring_animator_create(0.0, 170.0, 26.0, 1.0, 0.0, 0.05, 0.01);
        cm_spring_animator_set_target(anim, 300.0);

        let dt = 1.0 / 60.0;
        let mut frames = 0;
        while cm_spring_animator_is_running(anim) != 0 {
            cm_spring_animator_update(anim, dt);
            let value = cm_spring_animator_get_value(anim);
            if frames % 15 == 0 {
                println!("  帧 {:3}: value = {:.2}", frames, value);
            }
            frames += 1;
        }

        println!("  动画结束，最终 value = {:.4}（共 {} 帧）\n",
                 cm_spring_animator_get_value(anim), frames);
        cm_spring_animator_destroy(anim);
    }
}

// ====================================================================
// 示例 3：ScrollSimulator — 滚动模拟
// ====================================================================
fn example_scroll_simulator() {
    println!("=== 示例 3: ScrollSimulator 滚动模拟 ===\n");

    unsafe {
        let scroll = cm_scroll_create();
        cm_scroll_set_bounds(scroll, 0.0, 500.0);

        cm_scroll_start(scroll, 100.0);
        cm_scroll_move(scroll, -50.0);
        cm_scroll_move(scroll, -30.0);
        cm_scroll_end(scroll, -800.0);

        let dt = 1.0 / 60.0;
        let mut frames = 0;
        loop {
            cm_scroll_update(scroll, dt);
            let pos = cm_scroll_get_position(scroll);
            if frames % 20 == 0 {
                println!("  帧 {:3}: position = {:.2}", frames, pos);
            }
            frames += 1;
            if frames > 300 || (pos - 0.0).abs() < 0.1 && frames > 10 {
                break;
            }
        }

        println!("  滚动结束，最终 position = {:.2}\n",
                 cm_scroll_get_position(scroll));
        cm_scroll_destroy(scroll);
    }
}

// ====================================================================
fn main() {
    println!("CLEAR Motion Pro — Rust FFI 调用示例");
    println!("====================================\n");

    example_physics_engine();
    example_spring_animator();
    example_scroll_simulator();

    println!("所有示例执行完毕。");
}
