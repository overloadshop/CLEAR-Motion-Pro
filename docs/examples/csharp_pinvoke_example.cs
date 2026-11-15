// CLEARMotion Pro — C# P/Invoke 调用示例
//
// 前提：
// 1. 将 CLEARMotionPro_x64.dll 放到程序运行目录
// 2. 项目目标平台设为 x64
//
// 编译：dotnet build -c Release

using System;
using System.Runtime.InteropServices;

public static class CLEARMotionNative
{
    private const string Dll = "CLEARMotionPro_x64";

    // ===== 向量 =====
    [StructLayout(LayoutKind.Sequential)]
    public struct CMVec2
    {
        public double x, y;
        public CMVec2(double x, double y) { this.x = x; this.y = y; }
    }

    // ===== 物理配置 =====
    [StructLayout(LayoutKind.Sequential)]
    public struct CMPhysicsConfig
    {
        public double mass, restitution, drag;
        public CMPhysicsConfig(double mass, double restitution, double drag)
        {
            this.mass = mass; this.restitution = restitution; this.drag = drag;
        }
    }

    // ===== 变换配置 =====
    [StructLayout(LayoutKind.Sequential)]
    public struct CMTransformConfig
    {
        public double scale, rotation;
    }

    // ===== 实体配置 =====
    [StructLayout(LayoutKind.Sequential)]
    public struct CMEntityConfig
    {
        public CMVec2 position;
        public CMVec2 velocity;
        public CMPhysicsConfig physics;
        public CMTransformConfig transform;
    }

    // ===== 弹簧配置 =====
    [StructLayout(LayoutKind.Sequential)]
    public struct CMSpringConfig
    {
        public double rest_length, stiffness, damping;
    }

    // ===== PhysicsEngine =====
    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr cm_engine_create();

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern void cm_engine_destroy(IntPtr engine);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr cm_engine_create_entity(
        IntPtr engine, string id, CMEntityConfig config);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern void cm_engine_remove_entity(IntPtr engine, string id);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern void cm_engine_create_spring(
        IntPtr engine, string id, string pointA, string pointB, CMSpringConfig config);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern void cm_engine_update(IntPtr engine, double dt);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern void cm_engine_start(IntPtr engine);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern void cm_engine_stop(IntPtr engine);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr cm_engine_get_entity(IntPtr engine, string id);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern CMVec2 cm_entity_get_position(IntPtr entity);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern CMVec2 cm_entity_get_velocity(IntPtr entity);

    // ===== SpringAnimator =====
    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr cm_spring_animator_create(
        double initialValue, double stiffness, double damping,
        double mass, double gravity, double roughness, double airResistance);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern void cm_spring_animator_destroy(IntPtr anim);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern void cm_spring_animator_set_target(IntPtr anim, double target);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern void cm_spring_animator_update(IntPtr anim, double dt);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern double cm_spring_animator_get_value(IntPtr anim);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern double cm_spring_animator_get_velocity(IntPtr anim);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern int cm_spring_animator_is_running(IntPtr anim);

    // ===== ScrollSimulator =====
    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr cm_scroll_create();

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern void cm_scroll_destroy(IntPtr scroll);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern void cm_scroll_set_bounds(IntPtr scroll, double min, double max);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern void cm_scroll_start(IntPtr scroll, double position);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern void cm_scroll_move(IntPtr scroll, double delta);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern void cm_scroll_end(IntPtr scroll, double velocity);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern void cm_scroll_update(IntPtr scroll, double dt);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    public static extern double cm_scroll_get_position(IntPtr scroll);
}

// ====================================================================
// 示例程序
// ====================================================================
public class CLEARMotionExample
{
    // 示例 1：PhysicsEngine
    static void ExamplePhysicsEngine()
    {
        Console.WriteLine("=== 示例 1: PhysicsEngine 粒子弹簧网络 ===\n");

        var engine = CLEARMotionNative.cm_engine_create();

        // 创建实体
        var cfg = new CLEARMotionNative.CMEntityConfig
        {
            position = new CLEARMotionNative.CMVec2(0, 0),
            velocity = new CLEARMotionNative.CMVec2(0, 0),
            physics = new CLEARMotionNative.CMPhysicsConfig(1.0, 0.6, 0.1),
            transform = new CLEARMotionNative.CMTransformConfig { scale = 1.0 }
        };
        CLEARMotionNative.cm_engine_create_entity(engine, "a", cfg);

        cfg.position = new CLEARMotionNative.CMVec2(100, 0);
        CLEARMotionNative.cm_engine_create_entity(engine, "b", cfg);

        // 创建弹簧
        var springCfg = new CLEARMotionNative.CMSpringConfig
        {
            rest_length = 100.0, stiffness = 0.5, damping = 0.3
        };
        CLEARMotionNative.cm_engine_create_spring(engine, "spring_ab", "a", "b", springCfg);

        CLEARMotionNative.cm_engine_start(engine);

        double dt = 1.0 / 60.0;
        for (int i = 0; i < 120; i++)
        {
            CLEARMotionNative.cm_engine_update(engine, dt);
            if (i % 30 == 0)
            {
                var ea = CLEARMotionNative.cm_entity_get_position(
                    CLEARMotionNative.cm_engine_get_entity(engine, "a"));
                var eb = CLEARMotionNative.cm_entity_get_position(
                    CLEARMotionNative.cm_engine_get_entity(engine, "b"));
                Console.WriteLine($"  帧 {i,3}: A({ea.x:F1}, {ea.y:F1})  B({eb.x:F1}, {eb.y:F1})");
            }
        }

        CLEARMotionNative.cm_engine_stop(engine);
        CLEARMotionNative.cm_engine_destroy(engine);
        Console.WriteLine();
    }

    // 示例 2：SpringAnimator
    static void ExampleSpringAnimator()
    {
        Console.WriteLine("=== 示例 2: SpringAnimator 弹性动画 ===\n");

        var anim = CLEARMotionNative.cm_spring_animator_create(
            0.0, 170.0, 26.0, 1.0, 0.0, 0.05, 0.01);
        CLEARMotionNative.cm_spring_animator_set_target(anim, 300.0);

        double dt = 1.0 / 60.0;
        int frames = 0;
        while (CLEARMotionNative.cm_spring_animator_is_running(anim) != 0)
        {
            CLEARMotionNative.cm_spring_animator_update(anim, dt);
            double value = CLEARMotionNative.cm_spring_animator_get_value(anim);
            if (frames % 15 == 0)
                Console.WriteLine($"  帧 {frames,3}: value = {value:F2}");
            frames++;
        }

        Console.WriteLine($"  动画结束，最终 value = {CLEARMotionNative.cm_spring_animator_get_value(anim):F4}（共 {frames} 帧）\n");
        CLEARMotionNative.cm_spring_animator_destroy(anim);
    }

    // 示例 3：ScrollSimulator
    static void ExampleScrollSimulator()
    {
        Console.WriteLine("=== 示例 3: ScrollSimulator 滚动模拟 ===\n");

        var scroll = CLEARMotionNative.cm_scroll_create();
        CLEARMotionNative.cm_scroll_set_bounds(scroll, 0.0, 500.0);

        CLEARMotionNative.cm_scroll_start(scroll, 100.0);
        CLEARMotionNative.cm_scroll_move(scroll, -50.0);
        CLEARMotionNative.cm_scroll_move(scroll, -30.0);
        CLEARMotionNative.cm_scroll_end(scroll, -800.0);

        double dt = 1.0 / 60.0;
        int frames = 0;
        while (frames < 300)
        {
            CLEARMotionNative.cm_scroll_update(scroll, dt);
            double pos = CLEARMotionNative.cm_scroll_get_position(scroll);
            if (frames % 20 == 0)
                Console.WriteLine($"  帧 {frames,3}: position = {pos:F2}");
            frames++;
            if (Math.Abs(pos) < 0.1 && frames > 10) break;
        }

        Console.WriteLine($"  滚动结束，最终 position = {CLEARMotionNative.cm_scroll_get_position(scroll):F2}\n");
        CLEARMotionNative.cm_scroll_destroy(scroll);
    }

    // ====================================================================
    static void Main()
    {
        Console.WriteLine("CLEAR Motion Pro — C# P/Invoke 调用示例");
        Console.WriteLine("========================================\n");

        ExamplePhysicsEngine();
        ExampleSpringAnimator();
        ExampleScrollSimulator();

        Console.WriteLine("所有示例执行完毕。");
    }
}
