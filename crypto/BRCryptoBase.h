//
//  BRCryptoBase.h
//  BRCore
//
//  Created by Ed Gamble on 3/19/19.
//  Copyright © 2019 breadwallet. All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.

#ifndef BRCryptoBase_h
#define BRCryptoBase_h

#include <inttypes.h>
#include <stdatomic.h>
#include "support/BRInt.h"
#include "support/BRSyncMode.h"
#include "ethereum/base/BREthereumHash.h"
// temporary
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct BRCryptoWalletRecord *BRCryptoWallet;

    typedef struct BRCryptoWalletManagerRecord *BRCryptoWalletManager;

    // Cookies are used as markers to match up an asynchronous operation
    // request with its corresponding event.
    typedef void *BRCryptoCookie;

    typedef enum {
        CRYPTO_FALSE = 0,
        CRYPTO_TRUE  = 1
    } BRCryptoBoolean;

#define AS_CRYPTO_BOOLEAN(zeroIfFalse)   ((zeroIfFalse) ? CRYPTO_TRUE : CRYPTO_FALSE)

    typedef uint64_t BRCryptoBlockChainHeight;

    // Private-ish
    typedef enum {
        BLOCK_CHAIN_TYPE_BTC,
        BLOCK_CHAIN_TYPE_ETH,
        BLOCK_CHAIN_TYPE_GEN
    } BRCryptoBlockChainType;


    /// MARK: Reference Counting

    typedef struct {
        _Atomic(unsigned int) count;
        void (*free) (void *);
    } BRCryptoRef;

#if !defined (CRYPTO_REF_DEBUG)
#define CRYPTO_REF_DEBUG 0
#endif

#define DECLARE_CRYPTO_GIVE_TAKE(type, preface) \
  extern type preface##Take (type obj);  \
  extern void preface##Give (type obj)

#define IMPLEMENT_CRYPTO_GIVE_TAKE(type, preface) \
  extern type              \
  preface##Take (type obj) {   \
    atomic_fetch_add (&obj->ref.count, 1); \
    return obj;            \
  }                        \
  extern void              \
  preface##Give (type obj) {  \
    unsigned int __count = atomic_fetch_sub (&obj->ref.count, 1); \
    assert (0 != __count); \
    if (1 == __count) {    \
        if (0 != CRYPTO_REF_DEBUG) { printf ("CRY: Release: %s\n", #type); } \
        obj->ref.free (obj);  \
    }                      \
  }

#define CRYPTO_AS_FREE(release)     ((void (*) (void *)) release)

#define CRYPTO_REF_ASSIGN(release)  (BRCryptoRef) { 1, CRYPTO_AS_FREE (release) }

#if !defined (private_extern)
#  define private_extern          extern
#endif

#ifdef __cplusplus
}
#endif

#endif /* BRCryptoBase_h */
