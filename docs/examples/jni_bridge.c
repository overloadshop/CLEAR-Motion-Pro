// CLEARMotion Pro — JNI C 桥接层示例
//
// 这是 Java 调用 CLEARMotionPro 所需的 JNI 桥接代码。
// 编译为 .so 后与 CLEARMotionPro 一起加载。
//
// 编译（Android NDK）：
//   ndk-build APP_ABI=arm64-v8a

#include <jni.h>
#include "CLEARMotionPro.h"

// ====================================================================
// PhysicsEngine JNI 桥接
// ====================================================================

JNIEXPORT jlong JNICALL
Java_com_example_CLEARMotion_native_1engine_1create(JNIEnv *env, jobject thiz) {
    return (jlong)cm_engine_create();
}

JNIEXPORT void JNICALL
Java_com_example_CLEARMotion_native_1engine_1destroy(JNIEnv *env, jobject thiz, jlong engine) {
    cm_engine_destroy((CMPhysicsEngine)engine);
}

JNIEXPORT jlong JNICALL
Java_com_example_CLEARMotion_native_1engine_1create_1entity(
    JNIEnv *env, jobject thiz,
    jlong engine, jstring id,
    jdouble posX, jdouble posY, jdouble velX, jdouble velY,
    jdouble mass, jdouble restitution, jdouble drag)
{
    const char *id_str = (*env)->GetStringUTFChars(env, id, NULL);

    CMEntityConfig config;
    config.position.x = posX;
    config.position.y = posY;
    config.velocity.x = velX;
    config.velocity.y = velY;
    config.physics.mass = mass;
    config.physics.restitution = restitution;
    config.physics.drag = drag;
    config.transform.scale = 1.0;
    config.transform.rotation = 0.0;

    CMEntity entity = cm_engine_create_entity((CMPhysicsEngine)engine, id_str, config);
    (*env)->ReleaseStringUTFChars(env, id, id_str);
    return (jlong)entity;
}

JNIEXPORT void JNICALL
Java_com_example_CLEARMotion_native_1engine_1remove_1entity(
    JNIEnv *env, jobject thiz, jlong engine, jstring id)
{
    const char *id_str = (*env)->GetStringUTFChars(env, id, NULL);
    cm_engine_remove_entity((CMPhysicsEngine)engine, id_str);
    (*env)->ReleaseStringUTFChars(env, id, id_str);
}

JNIEXPORT void JNICALL
Java_com_example_CLEARMotion_native_1engine_1create_1spring(
    JNIEnv *env, jobject thiz,
    jlong engine, jstring id, jstring pointA, jstring pointB,
    jdouble restLength, jdouble stiffness, jdouble damping)
{
    const char *id_str = (*env)->GetStringUTFChars(env, id, NULL);
    const char *a_str = (*env)->GetStringUTFChars(env, pointA, NULL);
    const char *b_str = (*env)->GetStringUTFChars(env, pointB, NULL);

    CMSpringConfig config = { restLength, stiffness, damping };
    cm_engine_create_spring((CMPhysicsEngine)engine, id_str, a_str, b_str, config);

    (*env)->ReleaseStringUTFChars(env, id, id_str);
    (*env)->ReleaseStringUTFChars(env, pointA, a_str);
    (*env)->ReleaseStringUTFChars(env, pointB, b_str);
}

JNIEXPORT void JNICALL
Java_com_example_CLEARMotion_native_1engine_1update(
    JNIEnv *env, jobject thiz, jlong engine, jdouble dt)
{
    cm_engine_update((CMPhysicsEngine)engine, dt);
}

JNIEXPORT void JNICALL
Java_com_example_CLEARMotion_native_1engine_1start(JNIEnv *env, jobject thiz, jlong engine) {
    cm_engine_start((CMPhysicsEngine)engine);
}

JNIEXPORT void JNICALL
Java_com_example_CLEARMotion_native_1engine_1stop(JNIEnv *env, jobject thiz, jlong engine) {
    cm_engine_stop((CMPhysicsEngine)engine);
}

JNIEXPORT jdouble JNICALL
Java_com_example_CLEARMotion_native_1entity_1get_1x(
    JNIEnv *env, jobject thiz, jlong engine, jstring id)
{
    const char *id_str = (*env)->GetStringUTFChars(env, id, NULL);
    CMEntity entity = cm_engine_get_entity((CMPhysicsEngine)engine, id_str);
    (*env)->ReleaseStringUTFChars(env, id, id_str);
    if (!entity) return 0.0;
    return cm_entity_get_position(entity).x;
}

JNIEXPORT jdouble JNICALL
Java_com_example_CLEARMotion_native_1entity_1get_1y(
    JNIEnv *env, jobject thiz, jlong engine, jstring id)
{
    const char *id_str = (*env)->GetStringUTFChars(env, id, NULL);
    CMEntity entity = cm_engine_get_entity((CMPhysicsEngine)engine, id_str);
    (*env)->ReleaseStringUTFChars(env, id, id_str);
    if (!entity) return 0.0;
    return cm_entity_get_position(entity).y;
}

// ====================================================================
// SpringAnimator JNI 桥接
// ====================================================================

JNIEXPORT jlong JNICALL
Java_com_example_CLEARMotion_native_1spring_1create(
    JNIEnv *env, jobject thiz,
    jdouble initial, jdouble stiffness, jdouble damping,
    jdouble mass, jdouble gravity, jdouble roughness, jdouble airResistance)
{
    return (jlong)cm_spring_animator_create(initial, stiffness, damping,
                                            mass, gravity, roughness, airResistance);
}

JNIEXPORT void JNICALL
Java_com_example_CLEARMotion_native_1spring_1destroy(JNIEnv *env, jobject thiz, jlong anim) {
    cm_spring_animator_destroy((CMSpringAnimator)anim);
}

JNIEXPORT void JNICALL
Java_com_example_CLEARMotion_native_1spring_1set_1target(
    JNIEnv *env, jobject thiz, jlong anim, jdouble target)
{
    cm_spring_animator_set_target((CMSpringAnimator)anim, target);
}

JNIEXPORT void JNICALL
Java_com_example_CLEARMotion_native_1spring_1update(
    JNIEnv *env, jobject thiz, jlong anim, jdouble dt)
{
    cm_spring_animator_update((CMSpringAnimator)anim, dt);
}

JNIEXPORT jdouble JNICALL
Java_com_example_CLEARMotion_native_1spring_1get_1value(JNIEnv *env, jobject thiz, jlong anim) {
    return cm_spring_animator_get_value((CMSpringAnimator)anim);
}

JNIEXPORT jint JNICALL
Java_com_example_CLEARMotion_native_1spring_1is_1running(JNIEnv *env, jobject thiz, jlong anim) {
    return cm_spring_animator_is_running((CMSpringAnimator)anim);
}

// ====================================================================
// ScrollSimulator JNI 桥接
// ====================================================================

JNIEXPORT jlong JNICALL
Java_com_example_CLEARMotion_native_1scroll_1create(JNIEnv *env, jobject thiz) {
    return (jlong)cm_scroll_create();
}

JNIEXPORT void JNICALL
Java_com_example_CLEARMotion_native_1scroll_1destroy(JNIEnv *env, jobject thiz, jlong scroll) {
    cm_scroll_destroy((CMScrollSimulator)scroll);
}

JNIEXPORT void JNICALL
Java_com_example_CLEARMotion_native_1scroll_1set_1bounds(
    JNIEnv *env, jobject thiz, jlong scroll, jdouble min, jdouble max)
{
    cm_scroll_set_bounds((CMScrollSimulator)scroll, min, max);
}

JNIEXPORT void JNICALL
Java_com_example_CLEARMotion_native_1scroll_1start(
    JNIEnv *env, jobject thiz, jlong scroll, jdouble position)
{
    cm_scroll_start((CMScrollSimulator)scroll, position);
}

JNIEXPORT void JNICALL
Java_com_example_CLEARMotion_native_1scroll_1move(
    JNIEnv *env, jobject thiz, jlong scroll, jdouble delta)
{
    cm_scroll_move((CMScrollSimulator)scroll, delta);
}

JNIEXPORT void JNICALL
Java_com_example_CLEARMotion_native_1scroll_1end(
    JNIEnv *env, jobject thiz, jlong scroll, jdouble velocity)
{
    cm_scroll_end((CMScrollSimulator)scroll, velocity);
}

JNIEXPORT void JNICALL
Java_com_example_CLEARMotion_native_1scroll_1update(
    JNIEnv *env, jobject thiz, jlong scroll, jdouble dt)
{
    cm_scroll_update((CMScrollSimulator)scroll, dt);
}

JNIEXPORT jdouble JNICALL
Java_com_example_CLEARMotion_native_1scroll_1get_1position(
    JNIEnv *env, jobject thiz, jlong scroll)
{
    return cm_scroll_get_position((CMScrollSimulator)scroll);
}
