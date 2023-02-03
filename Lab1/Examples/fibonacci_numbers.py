def get_n_fibonacci_numbers():
    n = int(input("Enter positive number: "))

    if n <= 0:
        print("You enter negative number or 0")
    else:
        num_1 = 0
        num_2 = 1

        print(num_2, end=' ')

        for i in range(n - 1):
            res = num_1 + num_2
            num_1 = num_2
            num_2 = res

            print(num_2, end=' ')
