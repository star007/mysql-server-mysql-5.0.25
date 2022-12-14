/* misc.cpp                                
 *
 * Copyright (C) 2003 Sawtooth Consulting Ltd.
 *
 * This file is part of yaSSL.
 *
 * yaSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * There are special exceptions to the terms and conditions of the GPL as it
 * is applied to yaSSL. View the full text of the exception in the file
 * FLOSS-EXCEPTIONS in the directory of this software distribution.
 *
 * yaSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

/* based on Wei Dai's misc.cpp from CryptoPP */


#include "runtime.hpp"
#include "misc.hpp"

#if !defined(YASSL_MYSQL_COMPATIBLE)
extern "C" {

    // for libcurl configure test, these are the signatures they use
    // locking handled internally by library
    char CRYPTO_lock() { return 0;}
    char CRYPTO_add_lock() { return 0;}
}  // extern "C"
#endif

#ifdef YASSL_PURE_C

    void* operator new(size_t sz, TaoCrypt::new_t)
    {
    void* ptr = malloc(sz ? sz : 1);
    if (!ptr) abort();

    return ptr;
    }


    void operator delete(void* ptr, TaoCrypt::new_t)
    {
    if (ptr) free(ptr);
    }


    void* operator new[](size_t sz, TaoCrypt::new_t nt)
    {
    return ::operator new(sz, nt);
    }


    void operator delete[](void* ptr, TaoCrypt::new_t nt)
    {
    ::operator delete(ptr, nt);
    }


    /* uncomment to test
    // make sure not using globals anywhere by forgetting to use overloaded
    void* operator new(size_t sz);

    void operator delete(void* ptr);

    void* operator new[](size_t sz);

    void operator delete[](void* ptr);
    */


    namespace TaoCrypt {

        new_t tc;   // for library new

    }

#if defined(__ICC) || defined(__INTEL_COMPILER)

extern "C" {

    int __cxa_pure_virtual() {
      assert("Pure virtual method called." == "Aborted");
      return 0;
    }

}  // extern "C"

#endif

#endif // YASSL_PURE_C


namespace TaoCrypt {


inline void XorWords(word* r, const word* a, unsigned int n)
{
    for (unsigned int i=0; i<n; i++)
        r[i] ^= a[i];
}


void xorbuf(byte* buf, const byte* mask, unsigned int count)
{
    if (((size_t)buf | (size_t)mask | count) % WORD_SIZE == 0)
        XorWords((word *)buf, (const word *)mask, count/WORD_SIZE);
    else
    {
        for (unsigned int i=0; i<count; i++)
            buf[i] ^= mask[i];
    }
}


unsigned int BytePrecision(unsigned long value)
{
    unsigned int i;
    for (i=sizeof(value); i; --i)
        if (value >> (i-1)*8)
            break;

    return i;
}


unsigned int BitPrecision(unsigned long value)
{
    if (!value)
        return 0;

    unsigned int l = 0,
                 h = 8 * sizeof(value);

    while (h-l > 1)
    {
        unsigned int t = (l+h)/2;
        if (value >> t)
            l = t;
        else
            h = t;
    }

    return h;
}


unsigned long Crop(unsigned long value, unsigned int size)
{
    if (size < 8*sizeof(value))
        return (value & ((1L << size) - 1));
    else
        return value;
}


}  // namespace

