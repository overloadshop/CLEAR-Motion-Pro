// CLEARMotion Pro — Java JNI 调用示例
//
// 前提：
// 1. 将对应架构的 .so 文件放入 jniLibs 目录：
//    app/src/main/jniLibs/arm64-v8a/libCLEARMotionPro.so
//    app/src/main/jniLibs/armeabi-v7a/libCLEARMotionPro.so
// 2. 在 Java 类中声明 native 方法并加载库

public class CLEARMotionExample {

    // ====================================================================
    // JNI 声明
    // ====================================================================
    static {
        System.loadLibrary("CLEARMotionPro");
    }

    // 向量
    static class CMVec2 {
        double x, y;
        CMVec2(double x, double y) { this.x = x; this.y = y; }
    }

    // 实体配置
    static class CMEntityConfig {
        double posX, posY, velX, velY;
        double mass, restitution, drag;
        double scale, rotation;

        CMEntityConfig(double posX, double posY) {
            this.posX = posX;
            this.posY = posY;
            this.mass = 1.0;
            this.restitution = 0.6;
            this.drag = 0.1;
            this.scale = 1.0;
        }
    }

    // 弹簧配置
    static class CMSpringConfig {
        double restLength, stiffness, damping;
        CMSpringConfig(double restLength, double stiffness, double damping) {
            this.restLength = restLength;
            this.stiffness = stiffness;
            this.damping = damping;
        }
    }

    // PhysicsEngine native 方法
    private static native long   native_engine_create();
    private static native void   native_engine_destroy(long engine);
    private static native long   native_engine_create_entity(long engine, String id,
            double posX, double posY, double velX, double velY,
            double mass, double restitution, double drag);
    private static native void   native_engine_remove_entity(long engine, String id);
    private static native void   native_engine_create_spring(long engine, String id,
            String pointA, String pointB,
            double restLength, double stiffness, double damping);
    private static native void   native_engine_update(long engine, double dt);
    private static native void   native_engine_start(long engine);
    private static native void   native_engine_stop(long engine);
    private static native double native_entity_get_x(long engine, String id);
    private static native double native_entity_get_y(long engine, String id);

    // SpringAnimator native 方法
    private static native long   native_spring_create(double initial, double stiffness,
            double damping, double mass, double gravity,
            double roughness, double airResistance);
    private static native void   native_spring_destroy(long anim);
    private static native void   native_spring_set_target(long anim, double target);
    private static native void   native_spring_update(long anim, double dt);
    private static native double native_spring_get_value(long anim);
    private static native int    native_spring_is_running(long anim);

    // ScrollSimulator native 方法
    private static native long   native_scroll_create();
    private static native void   native_scroll_destroy(long scroll);
    private static native void   native_scroll_set_bounds(long scroll, double min, double max);
    private static native void   native_scroll_start(long scroll, double position);
    private static native void   native_scroll_move(long scroll, double delta);
    private static native void   native_scroll_end(long scroll, double velocity);
    private static native void   native_scroll_update(long scroll, double dt);
    private static native double native_scroll_get_position(long scroll);

    // ====================================================================
    // 示例 1：PhysicsEngine — 粒子弹簧网络
    // ====================================================================
    static void examplePhysicsEngine() {
        System.out.println("=== 示例 1: PhysicsEngine 粒子弹簧网络 ===\n");

        long engine = native_engine_create();

        // 创建实体
        native_engine_create_entity(engine, "a", 0, 0, 0, 0, 1.0, 0.6, 0.1);
        native_engine_create_entity(engine, "b", 100, 0, 0, 0, 1.0, 0.6, 0.1);
        native_engine_create_entity(engine, "c", 50, 80, 0, 0, 2.0, 0.6, 0.1);

        // 创建弹簧
        native_engine_create_spring(engine, "spring_ab", "a", "b", 100, 0.5, 0.3);
        native_engine_create_spring(engine, "spring_bc", "b", "c", 100, 0.5, 0.3);

        native_engine_start(engine);

        double dt = 1.0 / 60.0;
        for (int i = 0; i < 120; i++) {
            native_engine_update(engine, dt);
            if (i % 30 == 0) {
                double ax = native_entity_get_x(engine, "a");
                double ay = native_entity_get_y(engine, "a");
                double bx = native_entity_get_x(engine, "b");
                double by = native_entity_get_y(engine, "b");
                System.out.printf("  帧 %3d: A(%.1f, %.1f)  B(%.1f, %.1f)%n", i, ax, ay, bx, by);
            }
        }

        native_engine_stop(engine);
        native_engine_destroy(engine);
        System.out.println();
    }

    // ====================================================================
    // 示例 2：SpringAnimator — 弹性动画
    // ====================================================================
    static void exampleSpringAnimator() {
        System.out.println("=== 示例 2: SpringAnimator 弹性动画 ===\n");

        long anim = native_spring_create(0.0, 170.0, 26.0, 1.0, 0.0, 0.05, 0.01);
        native_spring_set_target(anim, 300.0);

        double dt = 1.0 / 60.0;
        int frames = 0;
        while (native_spring_is_running(anim) != 0) {
            native_spring_update(anim, dt);
            double value = native_spring_get_value(anim);
            if (frames % 15 == 0) {
                System.out.printf("  帧 %3d: value = %.2f%n", frames, value);
            }
            frames++;
        }

        System.out.printf("  动画结束，最终 value = %.4f（共 %d 帧）%n%n",
                native_spring_get_value(anim), frames);
        native_spring_destroy(anim);
    }

    // ====================================================================
    // 示例 3：ScrollSimulator — 滚动模拟
    // ====================================================================
    static void exampleScrollSimulator() {
        System.out.println("=== 示例 3: ScrollSimulator 滚动模拟 ===\n");

        long scroll = native_scroll_create();
        native_scroll_set_bounds(scroll, 0.0, 500.0);

        native_scroll_start(scroll, 100.0);
        native_scroll_move(scroll, -50.0);
        native_scroll_move(scroll, -30.0);
        native_scroll_end(scroll, -800.0);

        double dt = 1.0 / 60.0;
        int frames = 0;
        while (frames < 300) {
            native_scroll_update(scroll, dt);
            double pos = native_scroll_get_position(scroll);
            if (frames % 20 == 0) {
                System.out.printf("  帧 %3d: position = %.2f%n", frames, pos);
            }
            frames++;
            if (Math.abs(pos) < 0.1 && frames > 10) break;
        }

        System.out.printf("  滚动结束，最终 position = %.2f%n%n",
                native_scroll_get_position(scroll));
        native_scroll_destroy(scroll);
    }

    // ====================================================================
    public static void main(String[] args) {
        System.out.println("CLEAR Motion Pro — Java JNI 调用示例");
        System.out.println("====================================\n");

        examplePhysicsEngine();
        exampleSpringAnimator();
        exampleScrollSimulator();

        System.out.println("所有示例执行完毕。");
    }
}
