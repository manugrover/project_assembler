; Kadane's Algorithm
    ldc length         ; Load the array length
    stl 0              ; Store length in local variable (offset 0)
    
    ldc array          ; Load base address of the array
    stl 1              ; Store in local variable (offset 1) as the starting address

    ldc 0              ; Initialize max_ending to 0
    stl 2              ; Store max_ending in local variable (offset 2)

    ldc 0              ; Initialize max_so_far to 0
    stl 3              ; Store max_so_far in local variable (offset 3)

    ldc 0              ; Initialize index counter to 0
    stl 4              ; Store index counter in local variable (offset 4)

loop:
    ldl 4              ; Load current index
    ldl 0              ; Load length of the array
    sub                ; Calculate length - index
    brz end            ; If index >= length, exit loop

    ; Load the current element
    ldl 4              ; Load current index
    ldc array          ; Load base address of the array
    add                ; Compute address of array[index]
    ldnl 0             ; Load value at array[index]
    stl 5              ; Store in temp variable (offset 5) as the current element

    ; Calculate max_ending = max(current element, max_ending + current element)
    ldl 2              ; Load max_ending
    ldl 5              ; Load current element
    add                ; Calculate max_ending + current element
    stl 6              ; Temporarily store max_ending + current element (offset 6)

    ldl 5              ; Reload current element
    ldl 6              ; Reload max_ending + current element
    sub                ; Compare: current element - (max_ending + current element)
    brlz update_with_sum ; If max_ending + current element >= current element, keep sum

    ; Update max_ending to current element if it is greater
    ldl 5              ; Set max_ending as current element
    stl 2
    br update_ans

update_with_sum:
    ldl 6              ; Load max_ending + current element
    stl 2              ; Update max_ending

update_ans:
    ; Update max_so_far if max_ending is greater
    ldl 2              ; Load max_ending
    ldl 3             ; Load max_so_far
    sub                ; Compare max_ending - max_so_far
    brlz next_index    ; If max_so_far >= max_ending, skip update

    ldl 2              ; Store max_ending into max_so_far
    stl 3

next_index:
    ldc 1              ; Increment index by 1
    ldl 4              ; Load current index
    add                
    stl 4              ; Store updated index
    br loop            ; Repeat loop

end:
    HALT               ; End program

length: SET 9          ; Set the length of the array to 9

array:
    data -12           ; First element of the array
    data 7            ; Second element
    data 25            ; Third element
    data 3             ; Fourth element
    data 0             ; Fifth element
    data -14           ; Sixth element
    data -20           ; Seventh element
    data 35            ; Eighth element
    data 4            ; Ninth element
