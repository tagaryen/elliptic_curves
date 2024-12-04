#include <jni.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>

#ifndef _Included_com_archer_math_MathLib
#define _Included_com_archer_math_MathLib

#ifdef __cplusplus
extern "C" {
#endif


/*  
* # Linux
* gcc -fPIC -shared math.c -static-libgcc -static-libstdc++ --std=c99 -o libmath.so
* 
* # Windows
* gcc -fPIC -c math.c -static-libgcc -static-libstdc++
* ar -x libgmp.a
* gcc -fPIC -shared *.o -static-libgcc -static-libstdc++ -o libmath.dll
*/


/*
 * Class:     com_archer_math_MathLib
 * Method:    sub
 * Signature: ([B)Ljava/lang/String;
 */
JNIEXPORT jbyteArray JNICALL Java_com_archer_math_MathLib_subm
  (JNIEnv *env, jclass clazz, jbyteArray ja, jbyteArray jb, jbyteArray jp) {
    if(NULL == ja || NULL == jb || NULL == jp) {
        return NULL;
    }

    uint32_t a_len = (*env)->GetArrayLength(env, ja);
    uint32_t b_len = (*env)->GetArrayLength(env, jb);
    uint32_t p_len = (*env)->GetArrayLength(env, jp);

    uint8_t ac[a_len], bc[b_len], pc[p_len];

    (*env)->GetByteArrayRegion(env, ja, 0, a_len, (jbyte *)ac);
    (*env)->GetByteArrayRegion(env, jb, 0, b_len, (jbyte *)bc);
    (*env)->GetByteArrayRegion(env, jp, 0, p_len, (jbyte *)pc);

    mpz_t r, a, b, p;
    // mpz_inits(r, p, e, m);
    mpz_init(r);
    mpz_init(a);
    mpz_init(b);
    mpz_init(p);
    mpz_import(a, a_len, 1, 1, 0, 0, ac);
    mpz_import(b, b_len, 1, 1, 0, 0, bc);
    mpz_import(p, p_len, 1, 1, 0, 0, pc);
    mpz_sub(r, a, b);
    mpz_mod(r, r, p);

    size_t l = p_len;
    uint8_t rc[l];
    mpz_export(rc, &l, 1, 1, 0, 0, r);
    jbyteArray ret = (*env)->NewByteArray(env, l);
    if(NULL != ret) {
        (*env)->SetByteArrayRegion(env, ret, 0, l, (jbyte *)rc);
    }
    //some bugs with gmp library, mpz_clears do not work
    mpz_clear(r);
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(p);
    return ret;
}

/*
 * Class:     com_archer_math_MathLib
 * Method:    mul
 * Signature: ([B)Ljava/lang/String;
 */
JNIEXPORT jbyteArray JNICALL Java_com_archer_math_MathLib_mul
  (JNIEnv *env, jclass clazz, jbyteArray ja, jbyteArray jb) {
    if(NULL == ja || NULL == jb) {
        return NULL;
    }
    
    uint32_t a_len = (*env)->GetArrayLength(env, ja);
    uint32_t b_len = (*env)->GetArrayLength(env, jb);

    uint8_t ac[a_len], bc[b_len];

    (*env)->GetByteArrayRegion(env, ja, 0, a_len, (jbyte *)ac);
    (*env)->GetByteArrayRegion(env, jb, 0, b_len, (jbyte *)bc);

    mpz_t r, a, b;
    // mpz_inits(r, p, e, m);
    mpz_init(r);
    mpz_init(a);
    mpz_init(b);
    mpz_import(a, a_len, 1, 1, 0, 0, ac);
    mpz_import(b, b_len, 1, 1, 0, 0, bc);
    mpz_mul(r, a, b);

    size_t l = a_len + b_len + 1;
    uint8_t rc[l];
    mpz_export(rc, &l, 1, 1, 0, 0, r);
    jbyteArray ret = (*env)->NewByteArray(env, l);
    if(NULL != ret) {
        (*env)->SetByteArrayRegion(env, ret, 0, l, (jbyte *)rc);
    }
    //some bugs with gmp library, mpz_clears do not work
    mpz_clear(r);
    mpz_clear(a);
    mpz_clear(b);
    return ret;
}

/*
 * Class:     com_archer_math_MathLib
 * Method:    pow
 * Signature: ([B)Ljava/lang/String;
 */
JNIEXPORT jbyteArray JNICALL Java_com_archer_math_MathLib_divui
  (JNIEnv *env, jclass clazz, jbyteArray ja, jint jb) {
    if(NULL == ja || 0 >= jb) {
        return NULL;
    }
    
    uint32_t a_len = (*env)->GetArrayLength(env, ja);
    uint8_t ac[a_len];
    (*env)->GetByteArrayRegion(env, ja, 0, a_len, (jbyte *)ac);
    
    uint32_t b = jb;

    mpz_t r, a;
    // mpz_inits(r, p, e, m);
    mpz_init(r);
    mpz_init(a);
    mpz_import(a, a_len, 1, 1, 0, 0, ac);
    mpz_div_ui(r, a, b);

    size_t l = a_len;
    uint8_t rc[l];
    mpz_export(rc, &l, 1, 1, 0, 0, r);
    jbyteArray ret = (*env)->NewByteArray(env, l);
    if(NULL != ret) {
        (*env)->SetByteArrayRegion(env, ret, 0, l, (jbyte *)rc);
    }
    //some bugs with gmp library, mpz_clears do not work
    mpz_clear(r);
    mpz_clear(a);
    return ret;
}


/*
 * Class:     com_archer_math_MathLib
 * Method:    div
 * Signature: ([B)Ljava/lang/String;
 */
JNIEXPORT jbyteArray JNICALL Java_com_archer_math_MathLib_div
  (JNIEnv *env, jclass clazz, jbyteArray ja, jbyteArray jb) {
    if(NULL == ja || NULL == jb) {
        return NULL;
    }
    
    uint32_t a_len = (*env)->GetArrayLength(env, ja);
    uint32_t b_len = (*env)->GetArrayLength(env, jb);

    uint8_t ac[a_len], bc[b_len];

    (*env)->GetByteArrayRegion(env, ja, 0, a_len, (jbyte *)ac);
    (*env)->GetByteArrayRegion(env, jb, 0, b_len, (jbyte *)bc);

    mpz_t r, a, b;
    // mpz_inits(r, p, e, m);
    mpz_init(r);
    mpz_init(a);
    mpz_init(b);
    mpz_import(a, a_len, 1, 1, 0, 0, ac);
    mpz_import(b, b_len, 1, 1, 0, 0, bc);
    mpz_div(r, a, b);

    size_t l = a_len > b_len ? a_len : b_len;
    uint8_t rc[l];
    mpz_export(rc, &l, 1, 1, 0, 0, r);
    jbyteArray ret = (*env)->NewByteArray(env, l);
    if(NULL != ret) {
        (*env)->SetByteArrayRegion(env, ret, 0, l, (jbyte *)rc);
    }
    //some bugs with gmp library, mpz_clears do not work
    mpz_clear(r);
    mpz_clear(a);
    mpz_clear(b);
    return ret;
}


/*
 * Class:     com_archer_math_MathLib
 * Method:    div
 * Signature: ([B)Ljava/lang/String;
 */
JNIEXPORT jbyteArray JNICALL Java_com_archer_math_MathLib_mod
  (JNIEnv *env, jclass clazz, jbyteArray ja, jbyteArray jb) {
    if(NULL == ja || NULL == jb) {
        return NULL;
    }
    
    uint32_t a_len = (*env)->GetArrayLength(env, ja);
    uint32_t b_len = (*env)->GetArrayLength(env, jb);

    uint8_t ac[a_len], bc[b_len];

    (*env)->GetByteArrayRegion(env, ja, 0, a_len, (jbyte *)ac);
    (*env)->GetByteArrayRegion(env, jb, 0, b_len, (jbyte *)bc);

    mpz_t r, a, b;
    // mpz_inits(r, p, e, m);
    mpz_init(r);
    mpz_init(a);
    mpz_init(b);
    mpz_import(a, a_len, 1, 1, 0, 0, ac);
    mpz_import(b, b_len, 1, 1, 0, 0, bc);
    mpz_mod(r, a, b);

    size_t l = b_len;
    uint8_t rc[l];
    mpz_export(rc, (size_t *)(&l), 1, 1, 0, 0, r);
    jbyteArray ret = (*env)->NewByteArray(env, l);
    if(NULL != ret) {
        (*env)->SetByteArrayRegion(env, ret, 0, l, (jbyte *)rc);
    }
    //some bugs with gmp library, mpz_clears do not work
    mpz_clear(r);
    mpz_clear(a);
    mpz_clear(b);
    return ret;
}

/*
 * Class:     com_archer_math_MathLib
 * Method:    pow
 * Signature: ([B)Ljava/lang/String;
 */
JNIEXPORT jbyteArray JNICALL Java_com_archer_math_MathLib_pow
  (JNIEnv *env, jclass clazz, jbyteArray ja, jint jb) {
    if(NULL == ja || 0 >= jb) {
        return NULL;
    }
    
    uint32_t a_len = (*env)->GetArrayLength(env, ja);
    uint8_t ac[a_len];
    (*env)->GetByteArrayRegion(env, ja, 0, a_len, (jbyte *)ac);

    uint32_t b = jb;

    mpz_t r, a;
    // mpz_inits(r, p, e, m);
    mpz_init(r);
    mpz_init(a);
    mpz_import(a, a_len, 1, 1, 0, 0, ac);
    mpz_pow_ui(r, a, b);

    size_t l = a_len * b + 1;
    uint8_t rc[l];
    mpz_export(rc, &l, 1, 1, 0, 0, r);
    jbyteArray ret = (*env)->NewByteArray(env, l);
    if(NULL != ret) {
        (*env)->SetByteArrayRegion(env, ret, 0, l, (jbyte *)rc);
    }
    //some bugs with gmp library, mpz_clears do not work
    mpz_clear(r);
    mpz_clear(a);
    return ret;
}


/*
 * Class:     com_archer_math_MathLib
 * Method:    pow
 * Signature: ([B)Ljava/lang/String;
 */
JNIEXPORT jbyteArray JNICALL Java_com_archer_math_MathLib_sqrt
  (JNIEnv *env, jclass clazz, jbyteArray ja) {
    if(NULL == ja) {
        return NULL;
    }
    
    uint32_t a_len = (*env)->GetArrayLength(env, ja);
    uint8_t ac[a_len];
    (*env)->GetByteArrayRegion(env, ja, 0, a_len, (jbyte *)ac);

    mpz_t r, a;
    // mpz_inits(r, p, e, m);
    mpz_init(r);
    mpz_init(a);
    mpz_import(a, a_len, 1, 1, 0, 0, ac);
    mpz_sqrt(r, a);

    size_t l = a_len;
    uint8_t rc[l];
    mpz_export(rc, &l, 1, 1, 0, 0, r);
    jbyteArray ret = (*env)->NewByteArray(env, l);
    if(NULL != ret) {
        (*env)->SetByteArrayRegion(env, ret, 0, l, (jbyte *)rc);
    }
    //some bugs with gmp library, mpz_clears do not work
    mpz_clear(r);
    mpz_clear(a);
    return ret;
}


/*
 * Class:     com_archer_math_MathLib
 * Method:    pow
 * Signature: ([B)Ljava/lang/String;
 */
JNIEXPORT jbyteArray JNICALL Java_com_archer_math_MathLib_sqrtif
  (JNIEnv *env, jclass clazz, jbyteArray ja) {
    if(NULL == ja) {
        return NULL;
    }
    
    uint32_t a_len = (*env)->GetArrayLength(env, ja);
    uint8_t ac[a_len];
    (*env)->GetByteArrayRegion(env, ja, 0, a_len, (jbyte *)ac);

    mpz_t r, r2, a;
    // mpz_inits(r, p, e, m);
    mpz_init(r);
    mpz_init(r2);
    mpz_init(a);
    mpz_import(a, a_len, 1, 1, 0, 0, ac);
    mpz_sqrt(r, a);
    mpz_mul(r2, r, r);
    if(mpz_cmp(r2, a) != 0 ) {
        mpz_clear(r);
        mpz_clear(r2);
        mpz_clear(a);
        return NULL;
    }

    size_t l = a_len;
    uint8_t rc[l];
    mpz_export(rc, &l, 1, 1, 0, 0, r);
    jbyteArray ret = (*env)->NewByteArray(env, l);
    if(NULL != ret) {
        (*env)->SetByteArrayRegion(env, ret, 0, l, (jbyte *)rc);
    }
    mpz_clear(r);
    mpz_clear(r2);
    mpz_clear(a);
    return ret;
}

/*
 * Class:     com_archer_math_MathLib
 * Method:    powmui
 * Signature: ([B)Ljava/lang/String;
 */
JNIEXPORT jbyteArray JNICALL Java_com_archer_math_MathLib_powmui
  (JNIEnv *env, jclass clazz, jbyteArray ja, jint jb, jbyteArray jp) {
    if(NULL == ja || 0 >= jb) {
        return NULL;
    }
    
    uint32_t a_len = (*env)->GetArrayLength(env, ja);
    uint32_t p_len = (*env)->GetArrayLength(env, jp);

    uint8_t ac[a_len], pc[p_len];

    (*env)->GetByteArrayRegion(env, ja, 0, a_len, (jbyte *)ac);
    (*env)->GetByteArrayRegion(env, jp, 0, p_len, (jbyte *)pc);

    uint32_t b = jb;


    mpz_t r, a, p;
    // mpz_inits(r, p, e, m);
    mpz_init(r);
    mpz_init(a);
    mpz_init(p);
    mpz_import(a, a_len, 1, 1, 0, 0, ac);
    mpz_import(p, p_len, 1, 1, 0, 0, pc);
    mpz_powm_ui(r, a, b, p);

    size_t l = p_len;
    uint8_t rc[l];
    mpz_export(rc, &l, 1, 1, 0, 0, r);
    jbyteArray ret = (*env)->NewByteArray(env, l);
    if(NULL != ret) {
        (*env)->SetByteArrayRegion(env, ret, 0, l, (jbyte *)rc);
    }
    //some bugs with gmp library, mpz_clears do not work
    mpz_clear(r);
    mpz_clear(a);
    mpz_clear(p);
    return ret;
}


/*
 * Class:     com_archer_math_MathLib
 * Method:    powm
 * Signature: ([B)Ljava/lang/String;
 */
JNIEXPORT jbyteArray JNICALL Java_com_archer_math_MathLib_powm
  (JNIEnv *env, jclass clazz, jbyteArray jp, jbyteArray je, jbyteArray jm) {
    if(NULL == jp || NULL == je || NULL == jm) {
        return NULL;
    }
    
    uint32_t p_len = (*env)->GetArrayLength(env, jp);
    uint32_t e_len = (*env)->GetArrayLength(env, je);
    uint32_t m_len = (*env)->GetArrayLength(env, jm);

    uint8_t pc[p_len], ec[e_len], mc[m_len];

    (*env)->GetByteArrayRegion(env, jp, 0, p_len, (jbyte *)pc);
    (*env)->GetByteArrayRegion(env, je, 0, e_len, (jbyte *)ec);
    (*env)->GetByteArrayRegion(env, jm, 0, m_len, (jbyte *)mc);

    mpz_t r, p, e, m;
    // mpz_inits(r, p, e, m);
    mpz_init(r);
    mpz_init(p);
    mpz_init(e);
    mpz_init(m);
    mpz_import(p, p_len, 1, 1, 0, 0, pc);
    mpz_import(e, e_len, 1, 1, 0, 0, ec);
    mpz_import(m, m_len, 1, 1, 0, 0, mc);
    mpz_powm(r, p, e, m);

    size_t l = m_len;
    uint8_t rc[m_len];
    mpz_export(rc, &l, 1, 1, 0, 0, r);
    jbyteArray ret = (*env)->NewByteArray(env, l);
    if(NULL != ret) {
        (*env)->SetByteArrayRegion(env, ret, 0, l, (jbyte *)rc);
    }
    //some bugs with gmp library, mpz_clears do not work
    mpz_clear(r);
    mpz_clear(p);
    mpz_clear(e);
    mpz_clear(m);
    return ret;
}


JNIEXPORT jbyteArray JNICALL Java_com_archer_math_MathLib_inv
  (JNIEnv *env, jclass clazz, jbyteArray jp, jbyteArray jm) {
    if(NULL == jp || NULL == jm) {
        return NULL;
    }
    
    uint32_t m_len = (*env)->GetArrayLength(env, jm);
    uint32_t p_len = (*env)->GetArrayLength(env, jp);

    uint8_t mc[m_len], pc[p_len];

    (*env)->GetByteArrayRegion(env, jm, 0, m_len, (jbyte *)mc);
    (*env)->GetByteArrayRegion(env, jp, 0, p_len, (jbyte *)pc);

    mpz_t r, p, m;
    // mpz_inits(r, p, e, m);
    mpz_init(r);
    mpz_init(p);
    mpz_init(m);
    mpz_import(p, p_len, 1, 1, 0, 0, pc);
    mpz_import(m, m_len, 1, 1, 0, 0, mc);

    mpz_invert(r, p, m);

    size_t l = m_len;
    uint8_t rc[m_len];
    mpz_export(rc, &l, 1, 1, 0, 0, r);
    jbyteArray ret = (*env)->NewByteArray(env, l);
    if(NULL != ret) {
        (*env)->SetByteArrayRegion(env, ret, 0, l, (jbyte *)rc);
    }
    //some bugs with gmp library, mpz_clears do not work
    mpz_clear(r);
    mpz_clear(p);
    mpz_clear(m);
    return ret;
}

JNIEXPORT jbyteArray JNICALL Java_com_archer_math_MathLib_mulm
  (JNIEnv *env, jclass clazz, jbyteArray ja, jbyteArray jb, jbyteArray jm) {
    if(NULL == ja || NULL == jb || NULL == jm) {
        return NULL;
    }
    
    uint32_t a_len = (*env)->GetArrayLength(env, ja);
    uint32_t b_len = (*env)->GetArrayLength(env, jb);
    uint32_t m_len = (*env)->GetArrayLength(env, jm);

    uint8_t ac[a_len], bc[b_len], mc[m_len];

    (*env)->GetByteArrayRegion(env, ja, 0, a_len, (jbyte *)ac);
    (*env)->GetByteArrayRegion(env, jb, 0, b_len, (jbyte *)bc);
    (*env)->GetByteArrayRegion(env, jm, 0, m_len, (jbyte *)mc);

    mpz_t r, a, b, m;
    // mpz_inits(r, p, e, m);
    mpz_init(r);
    mpz_init(a);
    mpz_init(b);
    mpz_init(m);
    mpz_import(a, a_len, 1, 1, 0, 0, ac);
    mpz_import(b, b_len, 1, 1, 0, 0, bc);
    mpz_import(m, m_len, 1, 1, 0, 0, mc);

    mpz_mul(r, a, b);
    mpz_mod(r, r, m);

    size_t l = m_len;
    uint8_t rc[m_len];
    mpz_export(rc, &l, 1, 1, 0, 0, r);
    jbyteArray ret = (*env)->NewByteArray(env, l);
    if(NULL != ret) {
        (*env)->SetByteArrayRegion(env, ret, 0, l, (jbyte *)rc);
    }
    //some bugs with gmp library, mpz_clears do not work
    mpz_clear(r);
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(m);
    return ret;
}


JNIEXPORT jbyteArray JNICALL Java_com_archer_math_MathLib_mulInvm
  (JNIEnv *env, jclass clazz, jbyteArray ja, jbyteArray jb, jbyteArray jm) {
    if(NULL == ja || NULL == jb || NULL == jm) {
        return NULL;
    }
    
    uint32_t a_len = (*env)->GetArrayLength(env, ja);
    uint32_t b_len = (*env)->GetArrayLength(env, jb);
    uint32_t m_len = (*env)->GetArrayLength(env, jm);

    uint8_t ac[a_len], bc[b_len], mc[m_len];

    (*env)->GetByteArrayRegion(env, ja, 0, a_len, (jbyte *)ac);
    (*env)->GetByteArrayRegion(env, jb, 0, b_len, (jbyte *)bc);
    (*env)->GetByteArrayRegion(env, jm, 0, m_len, (jbyte *)mc);

    mpz_t r, a, b, m;
    // mpz_inits(r, p, e, m);
    mpz_init(r);
    mpz_init(a);
    mpz_init(b);
    mpz_init(m);
    mpz_import(a, a_len, 1, 1, 0, 0, ac);
    mpz_import(b, b_len, 1, 1, 0, 0, bc);
    mpz_import(m, m_len, 1, 1, 0, 0, mc);

    mpz_invert(r, b, m);
    mpz_mul(r, a, r);
    mpz_mod(r, r, m);

    size_t l = m_len;
    uint8_t rc[m_len];
    mpz_export(rc, &l, 1, 1, 0, 0, r);
    jbyteArray ret = (*env)->NewByteArray(env, l);
    if(NULL != ret) {
        (*env)->SetByteArrayRegion(env, ret, 0, l, (jbyte *)rc);
    }
    //some bugs with gmp library, mpz_clears do not work
    mpz_clear(r);
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(m);
    return ret;
}

#ifdef __cplusplus
}
#endif
#endif
