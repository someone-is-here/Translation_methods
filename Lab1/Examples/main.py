from factorial import factorial_calc
from fibonacci_numbers import get_n_fibonacci_numbers
from numbers_task import numbers_task
from string_task import is_palindrome


if __name__ == '__main__':
    # get_n_fibonacci_numbers()
    # numbers_task()
    number = int(input("Enter a number: "))
    factorial = 1

    if number < 0:
        print("Sorry, factorial does not exist for negative numbers")
    elif number == 0:
        print("The factorial of 0 is 1")
    else:
        for i in range(1, number + 1):
            factorial = factorial * i
        print(factorial)

    # is_palindrome()

    # factorial_calc()
