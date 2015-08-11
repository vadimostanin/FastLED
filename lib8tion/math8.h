#ifndef __INC_LIB8TION_MATH_H
#define __INC_LIB8TION_MATH_H

///@ingroup lib8tion

///@defgroup Math Basic math operations
///@{
/// add one byte to another, saturating at 0xFF
/// @param i - first byte to add
/// @param j - second byte to add
/// @returns the sum of i & j, capped at 0xFF
LIB8STATIC uint8_t qadd8( uint8_t i, uint8_t j)
{
#if QADD8_C == 1
    unsigned int t = i + j;
    if( t > 255) t = 255;
    return t;
#elif QADD8_AVRASM == 1
    asm volatile(
         /* First, add j to i, conditioning the C flag */
         "add %0, %1    \n\t"

         /* Now test the C flag.
           If C is clear, we branch around a load of 0xFF into i.
           If C is set, we go ahead and load 0xFF into i.
         */
         "brcc L_%=     \n\t"
         "ldi %0, 0xFF  \n\t"
         "L_%=: "
         : "+a" (i)
         : "a"  (j) );
    return i;
#elif QADD8_ARM_DSP_ASM == 1
    asm volatile( "uqadd8 %0, %0, %1" : "+r" (i) : "r" (j));
    return i;
#else
#error "No implementation for qadd8 available."
#endif
}

/// Add one byte to another, saturating at 0x7F
/// @param i - first byte to add
/// @param j - second byte to add
/// @returns the sum of i & j, capped at 0xFF
LIB8STATIC int8_t qadd7( int8_t i, int8_t j)
{
#if QADD7_C == 1
    int16_t t = i + j;
    if( t > 127) t = 127;
    return t;
#elif QADD7_AVRASM == 1
    asm volatile(
         /* First, add j to i, conditioning the V flag */
         "add %0, %1    \n\t"

         /* Now test the V flag.
          If V is clear, we branch around a load of 0x7F into i.
          If V is set, we go ahead and load 0x7F into i.
          */
         "brvc L_%=     \n\t"
         "ldi %0, 0x7F  \n\t"
         "L_%=: "
         : "+a" (i)
         : "a"  (j) );

    return i;
#elif QADD7_ARM_DSP_ASM == 1
    asm volatile( "qadd8 %0, %0, %1" : "+r" (i) : "r" (j));
    return i;
#else
#error "No implementation for qadd7 available."
#endif
}

/// subtract one byte from another, saturating at 0x00
/// @returns i - j with a floor of 0
LIB8STATIC uint8_t qsub8( uint8_t i, uint8_t j)
{
#if QSUB8_C == 1
    int t = i - j;
    if( t < 0) t = 0;
    return t;
#elif QSUB8_AVRASM == 1

    asm volatile(
         /* First, subtract j from i, conditioning the C flag */
         "sub %0, %1    \n\t"

         /* Now test the C flag.
          If C is clear, we branch around a load of 0x00 into i.
          If C is set, we go ahead and load 0x00 into i.
          */
         "brcc L_%=     \n\t"
         "ldi %0, 0x00  \n\t"
         "L_%=: "
         : "+a" (i)
         : "a"  (j) );

    return i;
#else
#error "No implementation for qsub8 available."
#endif
}

/// add one byte to another, with one byte result
LIB8STATIC uint8_t add8( uint8_t i, uint8_t j)
{
#if ADD8_C == 1
    int t = i + j;
    return t;
#elif ADD8_AVRASM == 1
    // Add j to i, period.
    asm volatile( "add %0, %1" : "+a" (i) : "a" (j));
    return i;
#else
#error "No implementation for add8 available."
#endif
}


/// subtract one byte from another, 8-bit result
LIB8STATIC uint8_t sub8( uint8_t i, uint8_t j)
{
#if SUB8_C == 1
    int t = i - j;
    return t;
#elif SUB8_AVRASM == 1
    // Subtract j from i, period.
    asm volatile( "sub %0, %1" : "+a" (i) : "a" (j));
    return i;
#else
#error "No implementation for sub8 available."
#endif
}

/// Calculate an integer average of two unsigned
///       8-bit integer values (uint8_t).
///       Fractional results are rounded down, e.g. avg8(20,41) = 30
LIB8STATIC uint8_t avg8( uint8_t i, uint8_t j)
{
#if AVG8_C == 1
    return (i + j) >> 1;
#elif AVG8_AVRASM == 1
    asm volatile(
         /* First, add j to i, 9th bit overflows into C flag */
         "add %0, %1    \n\t"
         /* Divide by two, moving C flag into high 8th bit */
         "ror %0        \n\t"
         : "+a" (i)
         : "a"  (j) );
    return i;
#else
#error "No implementation for avg8 available."
#endif
}


/// Calculate an integer average of two signed 7-bit
///       integers (int8_t)
///       If the first argument is even, result is rounded down.
///       If the first argument is odd, result is result up.
LIB8STATIC int8_t avg7( int8_t i, int8_t j)
{
#if AVG7_C == 1
    return ((i + j) >> 1) + (i & 0x1);
#elif AVG7_AVRASM == 1
    asm volatile(
                 "asr %1        \n\t"
                 "asr %0        \n\t"
                 "adc %0, %1    \n\t"
                 : "+a" (i)
                 : "a"  (j) );
    return i;
#else
#error "No implementation for avg7 available."
#endif
}

///       Calculate the remainder of one unsigned 8-bit
///       value divided by anoter, aka A % M.
///       Implemented by repeated subtraction, which is
///       very compact, and very fast if A is 'probably'
///       less than M.  If A is a large multiple of M,
///       the loop has to execute multiple times.  However,
///       even in that case, the loop is only two
///       instructions long on AVR, i.e., quick.
LIB8STATIC uint8_t mod8( uint8_t a, uint8_t m)
{
#if defined(__AVR__)
    asm volatile (
                  "L_%=:  sub %[a],%[m]    \n\t"
                  "       brcc L_%=        \n\t"
                  "       add %[a],%[m]    \n\t"
                  : [a] "+r" (a)
                  : [m] "r"  (m)
                  );
#else
    while( a >= m) a -= m;
#endif
    return a;
}

///          Add two numbers, and calculate the modulo
///          of the sum and a third number, M.
///          In other words, it returns (A+B) % M.
///          It is designed as a compact mechanism for
///          incrementing a 'mode' switch and wrapping
///          around back to 'mode 0' when the switch
///          goes past the end of the available range.
///          e.g. if you have seven modes, this switches
///          to the next one and wraps around if needed:
///            mode = addmod8( mode, 1, 7);
///          See 'mod8' for notes on performance.
LIB8STATIC uint8_t addmod8( uint8_t a, uint8_t b, uint8_t m)
{
#if defined(__AVR__)
    asm volatile (
                  "       add %[a],%[b]    \n\t"
                  "L_%=:  sub %[a],%[m]    \n\t"
                  "       brcc L_%=        \n\t"
                  "       add %[a],%[m]    \n\t"
                  : [a] "+r" (a)
                  : [b] "r"  (b), [m] "r" (m)
                  );
#else
    a += b;
    while( a >= m) a -= m;
#endif
    return a;
}

/// 8x8 bit multiplication, with 8 bit result
LIB8STATIC uint8_t mul8( uint8_t i, uint8_t j)
{
#if MUL8_C == 1
    return ((int)i * (int)(j) ) & 0xFF;
#elif MUL8_AVRASM == 1
    asm volatile(
         /* Multiply 8-bit i * 8-bit j, giving 16-bit r1,r0 */
         "mul %0, %1          \n\t"
         /* Extract the LOW 8-bits (r0) */
         "mov %0, r0          \n\t"
         /* Restore r1 to "0"; it's expected to always be that */
         "clr __zero_reg__    \n\t"
         : "+a" (i)
         : "a"  (j)
         : "r0", "r1");

    return i;
#else
#error "No implementation for mul8 available."
#endif
}


/// saturating 8x8 bit multiplication, with 8 bit result
/// @returns the product of i * j, capping at 0xFF
LIB8STATIC uint8_t qmul8( uint8_t i, uint8_t j)
{
#if QMUL8_C == 1
    int p = ((int)i * (int)(j) );
    if( p > 255) p = 255;
    return p;
#elif QMUL8_AVRASM == 1
    asm volatile(
                 /* Multiply 8-bit i * 8-bit j, giving 16-bit r1,r0 */
                 "  mul %0, %1          \n\t"
                 /* If high byte of result is zero, all is well. */
                 "  tst r1              \n\t"
                 "  breq Lnospill_%=    \n\t"
                 /* If high byte of result > 0, saturate low byte to 0xFF */
                 "  ldi %0,0xFF         \n\t"
                 "  rjmp Ldone_%=       \n\t"
                 "Lnospill_%=:          \n\t"
                 /* Extract the LOW 8-bits (r0) */
                 "  mov %0, r0          \n\t"
                 "Ldone_%=:             \n\t"
                 /* Restore r1 to "0"; it's expected to always be that */
                 "  clr __zero_reg__    \n\t"
                 : "+a" (i)
                 : "a"  (j)
                 : "r0", "r1");

    return i;
#else
#error "No implementation for qmul8 available."
#endif
}


/// take abs() of a signed 8-bit uint8_t
LIB8STATIC int8_t abs8( int8_t i)
{
#if ABS8_C == 1
    if( i < 0) i = -i;
    return i;
#elif ABS8_AVRASM == 1


    asm volatile(
         /* First, check the high bit, and prepare to skip if it's clear */
         "sbrc %0, 7 \n"

         /* Negate the value */
         "neg %0     \n"

         : "+r" (i) : "r" (i) );
    return i;
#else
#error "No implementation for abs8 available."
#endif
}

///         square root for 16-bit integers
///         About three times faster and five times smaller
///         than Arduino's general sqrt on AVR.
LIB8STATIC uint8_t sqrt16(uint16_t x)
{
    if( x <= 1) {
        return x;
    }

    uint8_t low = 1; // lower bound
    uint8_t hi, mid;

    if( x > 7904) {
        hi = 255;
    } else {
        hi = (x >> 5) + 8; // initial estimate for upper bound
    }

    do {
        mid = (low + hi) >> 1;
        if ((uint16_t)(mid * mid) > x) {
            hi = mid - 1;
        } else {
            if( mid == 255) {
                return 255;
            }
            low = mid + 1;
        }
    } while (hi >= low);

    return low - 1;
}

///@}
#endif
