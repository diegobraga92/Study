# Uses python3

def calc_fib_mod(n, m):
    if n < 2:
        return n

    period = [0,1, 1 % m]
    i = 2

    while not (period[i] == 1 and period[i-1] == 0):
        i = i+1
        period.append((period[i - 1] + period[i - 2]) % m)

    # Subtracts 1 from i to invert its initial value of 2
    modIndex = n % (i - 1)
    return period[modIndex]

if __name__ == '__main__':
    n, m = map(int, input().split())
    print(calc_fib_mod(n, m))
