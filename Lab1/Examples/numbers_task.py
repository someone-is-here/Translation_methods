def numbers_task():
    prev = -1
    curr_rep_len = 0
    max_rep_len = 0

    element = int(input("Enter sequence element: "))

    while element != 0:
        if prev == element:
            curr_rep_len += 1
        else:
            prev = element
            max_rep_len = max(max_rep_len, curr_rep_len)
            curr_rep_len = 1

        element = int(input("Enter sequence element: "))

    max_rep_len = max(max_rep_len, curr_rep_len)
    print(max_rep_len)
