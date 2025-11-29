using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading;

class Program
{

    static bool IsPrime(int n)
    {
        if (n <= 1) return false;
        if (n <= 3) return true;
        if (n % 2 == 0) return false;
        int r = (int)Math.Sqrt(n);
        for (int i = 3; i <= r; i += 2)
            if (n % i == 0) return false;
        return true;
    }

    static void Measure(string label, Func<long> action)
    {
        var sw = Stopwatch.StartNew();
        long result = action();
        sw.Stop();
        Console.WriteLine($"{label}: result={result}, elapsed={sw.ElapsedMilliseconds}ms");
    }

    static void Main()
    {
        Console.WriteLine("=== IndependentWork12 - PLINQ demo ===\n");

        var sizes = new[] { 100_000, 500_000, 1_000_000 };
        foreach (var size in sizes)
        {
            Console.WriteLine($"\n-- Data size: {size} --");

            var rnd = new Random(123);
            var data = new int[size];
            for (int i = 0; i < size; i++) data[i] = rnd.Next(2, 200_000);

            Measure("LINQ (Count primes)", () =>
            {
                return data.Where(x => IsPrime(x)).LongCount();
            });

            Measure("PLINQ (Count primes)", () =>
            {
                return data.AsParallel().Where(x => IsPrime(x)).LongCount();
            });

            Measure("LINQ (transform)", () =>
            {
                double sum = data.Where(x => x % 3 == 0)
                                 .Select(x => Math.Log(x) * Math.Sqrt(x))
                                 .Sum();
                return (long)sum;
            });

            Measure("PLINQ (transform)", () =>
            {
                double sum = data.AsParallel().Where(x => x % 3 == 0)
                                 .Select(x => Math.Log(x) * Math.Sqrt(x))
                                 .Sum();
                return (long)sum;
            });
        }

        Console.WriteLine("\n-- Side-effects demo --");
        var arr = Enumerable.Range(2, 100_000).ToArray();

        long unsafeSum = 0;
        try
        {
            arr.AsParallel().ForAll(x =>
            {
                if (IsPrime(x))
                {
                    unsafeSum += x;
                }
            });
            Console.WriteLine($"Unsafe sum (PLINQ + shared var): {unsafeSum} (expected non-deterministic!)");
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Exception in unsafe demo: {ex.Message}");
        }

        long safeSum = 0;
        arr.AsParallel().ForAll(x =>
        {
            if (IsPrime(x))
            {
                System.Threading.Interlocked.Add(ref safeSum, x);
            }
        });
        Console.WriteLine($"Safe sum (PLINQ + Interlocked): {safeSum}");

        long aggSum = arr.AsParallel().Where(IsPrime).Aggregate(
            seed: 0L,
            func: (partial, item) => partial + item,
            resultSelector: total => total);
        Console.WriteLine($"Safe sum (PLINQ Aggregate): {aggSum}");

        Console.WriteLine("\n=== End ===");
    }
}