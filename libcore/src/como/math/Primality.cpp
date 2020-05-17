//=========================================================================
// Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "como/math/Primality.h"
#include "como/util/Arrays.h"

using como::util::Arrays;

namespace como {
namespace math {

const Array<Integer> Primality::sPrimes{ 2, 3, 5, 7, 11, 13, 17, 19, 23, 29,
        31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101,
        103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167,
        173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239,
        241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313,
        317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397,
        401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467,
        479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569,
        571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643,
        647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733,
        739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823,
        827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911,
        919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997, 1009,
        1013, 1019, 1021 };

static Array<IBigInteger*> CreateBIprimes(
    /* [in] */ const Array<Integer>& primes)
{
    Array<IBigInteger*> biPrimes(primes.GetLength());
    for (Integer i = 0; i < biPrimes.GetLength(); i++) {
        AutoPtr<IBigInteger> bi;
        CBigInteger::ValueOf(primes[i], bi);
        biPrimes.Set(i, bi);
    }
    return biPrimes;
}

Array<IBigInteger*> Primality::GetBIprimes()
{
    static Array<IBigInteger*> sBIprimes = CreateBIprimes(sPrimes);
    return sBIprimes;
}

ECode Primality::NextProbablePrime(
    /* [in] */ BigInteger* n,
    /* [out] */ AutoPtr<IBigInteger>& value)
{
    Integer i, j;
    Integer gapSize = 1024; // for searching of the next probable prime number
    Array<Integer> modules(sPrimes.GetLength());
    Array<Boolean> isDivisible(gapSize);
    AutoPtr<BigInt> ni = n->GetBigInt();
    // If n < "last prime of table" searches next prime in the table
    if (ni->BitLength() <= 10) {
        Integer l = (Integer)ni->LongInt();
        if (l < sPrimes[sPrimes.GetLength() - 1]) {
            for (i = 0; l >= sPrimes[i]; i++) {}
            value = GetBIprimes()[i];
            return NOERROR;
        }
    }

    AutoPtr<BigInt> startPoint = ni->Copy();
    AutoPtr<BigInt> probPrime = new BigInt();

    // Fix startPoint to "next odd number":
    startPoint->AddPositiveInteger(BigInt::RemainderByPositiveInteger(ni, 2) + 1);

    // To calculate modules: N mod p1, N mod p2, ... for first primes.
    for (i = 0; i < sPrimes.GetLength(); i++) {
        modules[i] = BigInt::RemainderByPositiveInteger(startPoint, sPrimes[i]) - gapSize;
    }
    while (true) {
        // At this point, all numbers in the gap are initialized as
        // probably primes
        Arrays::Fill(isDivisible, false);
        // To discard multiples of first primes
        for (i = 0; i < sPrimes.GetLength(); i++) {
            modules[i] = (modules[i] + gapSize) % sPrimes[i];
            j = (modules[i] == 0) ? 0 : (sPrimes[i] - modules[i]);
            for (; j < gapSize; j += sPrimes[i]) {
                isDivisible[j] = true;
            }
        }
        // To execute Miller-Rabin for non-divisible numbers by all first
        // primes
        for (j = 0; j < gapSize; j++) {
            if (!isDivisible[j]) {
                probPrime->PutCopy(startPoint);
                probPrime->AddPositiveInteger(j);
                if (probPrime->IsPrime(100)) {
                    return CBigInteger::New(probPrime, IID_IBigInteger, (IInterface**)&value);
                }
            }
        }
        startPoint->AddPositiveInteger(gapSize);
    }
}

}
}
