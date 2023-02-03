def insertion_sort(array):
    for i in range(1, len(array)):
        item_for_insertion = array[i]
        j = i-1
        while j >= 0 and array[j] > item_for_insertion:
            array[j + 1] = array[j]
            j -= 1
        array[j + 1] = item_for_insertion
