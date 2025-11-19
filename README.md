Here is a draft for a blog post (or an article for Dev.to/Medium) based on our discussion. I have structured it to highlight the journey from a simple observation to a generalized algorithm, framing it as a "branchless optimization" technique.

***

# Reinventing Legendre: A Branchless Formula for Summing Digits

As programmers, we are often taught that there is one "right way" to sum the digits of an integer. You set up a loop, you modulo by 10 to get the last digit, you divide by 10 to shift right, and you repeat until the number is 0.

It looks like this:

```c
int sum_digits(int n) {
    int sum = 0;
    while (n > 0) {
        sum += n % 10;
        n /= 10;
    }
    return sum;
}
```

It’s simple, it’s readable, and it works. But from a systems perspective, it has a flaw: **it relies on a loop.** In high-performance environments (like embedded systems or GPU shaders), branching logic (loops and if-statements) can cause pipeline stalls.

I began wondering: *Is there a pure mathematical formula to add the digits of a number in a single line, without looping?*

Most people immediately point to the **Digital Root** formula: `1 + ((n - 1) % 9)`. But that doesn't give you the *sum* of the digits; it gives you the recursive root (e.g., for 19, it gives 1, but the sum is 10).

I wanted the actual sum. So, I started experimenting with the math, and I stumbled into a 225-year-old number theory theorem by accident.

## The 2-Digit "Aha!" Moment

I started by looking at 2-digit numbers. Let's take the number **23**.
We know $2 + 3 = 5$.

I noticed that if you take the tens digit (2) and multiply it by 9, and subtract that from the original number, you get the sum.
$$ 23 - (2 \times 9) = 5 $$

Why does this work? Algebraically, a 2-digit number is $10a + b$.
If we subtract $9a$:
$$ (10a + b) - 9a = a + b $$
We are left with exactly the sum of the digits.

I wrote this as a formula using modulo to isolate the tens place:
$$ f(x) = x - \left( \frac{x - (x \pmod{10})}{10} \right) \times 9 $$

## Generalizing to $N$ Digits

Does this work for 3 digits?
Let's try **123**. The sum should be 6.
Using the 2-digit formula: $123 - 9(12) = 15$.
It failed. We are off by 9.

The issue is that the digit '1' is in the hundreds place. It contributes 100 to the value but only 1 to the sum. We need to subtract 99. My formula only subtracted 9.

To fix this, we need to strip away layers of the number. We need to subtract 9 times the "tens" **AND** 9 times the "hundreds" **AND** 9 times the "thousands."

The generalized formula for a 4-digit number becomes:

$$ f(x) = x - 9 \times \left( \left\lfloor \frac{x}{10} \right\rfloor + \left\lfloor \frac{x}{100} \right\rfloor + \left\lfloor \frac{x}{1000} \right\rfloor \right) $$

Let's trace it with **1257** (Target Sum: 15).

1.  $1257 / 10 = 125$
2.  $1257 / 100 = 12$
3.  $1257 / 1000 = 1$
4.  Sum of parts: $125 + 12 + 1 = 138$
5.  Calculation: $1257 - (138 \times 9) = 15$

It works perfectly.

## The Code: A Branchless One-Liner

If we translate this into C, ensuring we use explicit integer math, we get a branchless function that can handle any number up to 9999 (or higher if you add more terms):

```c
// Branchless digit sum for numbers up to 9999
int sum_digits_linear(int x) {
    return x - ( ( (x - x%10)/10 + 
                   (x - x%100)/100 + 
                   (x - x%1000)/1000 ) * 9 );
}
```

*Note: In standard C, `x/10` implies integer division, so `(x-x%10)/10` is syntactically redundant but mathematically rigorous—it ensures the logic holds even if you were using a calculator that processes decimals.*

## The Math: Re-discovering Legendre

After deriving this, I found out I wasn't the first to be fascinated by this relationship. In 1798, Adrien-Marie Legendre published *Essai sur la théorie des nombres*, which contains a theorem related to the sum of digits in base $p$.

In Base 10, the theorem is often stated as:
$$ S(n) = n - 9 \sum_{k=1}^{\infty} \left\lfloor \frac{n}{10^k} \right\rfloor $$

My implementation is essentially a **finite, unrolled expansion of Legendre’s Formula**. While mathematicians treat it as an infinite series, as programmers dealing with fixed-width integers (like 16-bit or 32-bit), we know exactly how many terms we need to unroll.

## Why is this useful?

You might ask: *Why write a long formula when a `while` loop works?*

1.  **Branchless Programming:**
    Modern CPUs use branch prediction to guess which way an `if` or `while` loop will go. If they guess wrong, the CPU stalls. This formula is a straight line of math. There are no branches, no jumps, and no mispredictions.

2.  **GPU Shaders (GLSL/HLSL):**
    In graphics programming, loops can be expensive or unpredictable. A fixed mathematical expression allows the GPU to execute the calculation on thousands of pixels simultaneously with constant time complexity.

3.  **SIMD Optimization:**
    Because this logic is just addition and division, it is trivial to vectorize (AVX/SSE), allowing you to calculate the digit sums of 8 or 16 numbers in a single CPU cycle.

## Conclusion

Sometimes, the "slow way" of doing math—writing it all out by hand—leads to interesting computational optimizations. By unrolling the loop and applying a bit of number theory, we can turn an iterative process into a direct calculation.

It may be 225-year-old math, but it’s a brand-new way to look at optimizing code.
