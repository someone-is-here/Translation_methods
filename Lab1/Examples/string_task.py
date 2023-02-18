def is_palindrome():
    str = input("Enter string for checking: ")
    # Run loop from 0 to len/2
    result = "palindrome"
    for i in range(0, int(len(str) / 2)):
        if str[i] != str[len(str) - i - 1]:
            result = "not a palindrome"
            break
    print(result)
