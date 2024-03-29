#include <jni.h>
#include "jssl.h"
#include "keyagreement.h"
#include "com_canonical_openssl_keyagreement_OpenSSLKeyAgreement.h"
#include "evp_utils.h"
#include "jni_utils.h"

extern OSSL_LIB_CTX *global_libctx;


int get_key_type(key_agreement_algorithm algo) {
    switch(algo) {
        case DIFFIE_HELLMAN: return EVP_PKEY_DH;
        case ELLIPTIC_CURVE: return EVP_PKEY_EC;
        default: return -1;
    }
}

/*
 * Class:     OpenSSLKeyAgreementSpi
 * Method:    engineInit0
 * Signature: (I[B)J
 */
JNIEXPORT long JNICALL Java_com_canonical_openssl_keyagreement_OpenSSLKeyAgreement_engineInit0
  (JNIEnv *env, jobject this, jint algo, jbyteArray keyBytes) {
    key_agreement_algorithm type = algo;
    key_agreement *agreement = init_key_agreement(type, global_libctx);
    byte* key_bytes = jbyteArray_to_byte_array(env, keyBytes);
    size_t key_length = array_length(env, keyBytes);
    key_pair private_key = { create_private_key(get_key_type(type), key_bytes, key_length) };
    set_private_key(agreement, &private_key);
    return (long)agreement; 
}

/*
 * Class:     OpenSSLKeyAgreementSpi
 * Method:    engineDoPhase0
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_com_canonical_openssl_keyagreement_OpenSSLKeyAgreement_engineDoPhase0
  (JNIEnv *env, jobject this, jbyteArray keyBytes) {
    key_agreement *agreement = (key_agreement *)get_long_field(env, this, "nativeHandle");
    byte* key_bytes = jbyteArray_to_byte_array(env, keyBytes);
    size_t key_length = array_length(env, keyBytes);
    key_pair public_key = { create_public_key(key_bytes, key_length) };
    set_peer_key(agreement, &public_key);
}

/*
 * Class:     OpenSSLKeyAgreementSpi
 * Method:    engineGenerateSecret0
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_canonical_openssl_keyagreement_OpenSSLKeyAgreement_engineGenerateSecret0
  (JNIEnv * env, jobject this) {
    key_agreement *agreement = (key_agreement *)get_long_field(env, this, "nativeHandle");
    shared_secret *secret = generate_shared_secret(agreement);
    return byte_array_to_jbyteArray(env, secret->bytes, secret->length);
}
