from sorting import insertion_sort
from fibonacci_numbers import get_n_fibonacci_numbers
from numbers_task import numbers_task
from string_task import string_task


if __name__ == '__main__':
    get_n_fibonacci_numbers()
    numbers_task()
    string_task()

    arr = [9, 1, 15, 28, 6]
    insertion_sort(arr)

    print(arr)

