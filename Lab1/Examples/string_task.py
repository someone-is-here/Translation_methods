def string_task():
    s = input('Enter string: ')

    for i in '&^%~#@':
        s = s.replace(i, '')

    print(s)
