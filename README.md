The program should be run by running the TestMain.cpp file. It prompts you to enter the file name (along with the extension) that you want to run. Make sure the file is in the same directory as the code. If the file is accessible the it starts to parse the code. If Not it terminates. If the program is parsed without any errors then it prints the contents of the memory and registers after parsing. After Parsing it processes the binary code generated from the text code and outputs the final contents of memory and registers.

Note:

The basic methodology of the design of the system, classes used and their functions is described in Methodology.txt.

The type of Instructions and their encoding from text to binary is in Instructions.txt.

A BubbleSort code is already written and updated in the above files this code sorts an array of 20 numbers this code.

The explanation of the code is given below

Explanation:

.data
arr:    #array to be sorted
	.word 10, 8, 3, 4, 1, 7, 2, 9, 6, 5, 13, 20, 17, 18, 12, 19, 16, 11, 15, 14
size:   #size of the array
	.word 20
.text
.globl main
main:
	li $t0, 4
	li $t3, 1
	la $s0, arr		#load the address of the first element of the array
	add $s1, $s0, $t0	#load the address of the second element of the array( which is 4+address of first element )
	li $v0, 0
	li $v1, 1
	lw $t1, size
	lw $t2, size

OuterLoop:
	beq $v0, $t1, exit		#if the loopcounter is equal to 0 ( from the size ) exit
	add $s2, $s0, $zero		#load the address of first element
	add $s3, $s1, $zero		#load the address of second element
	InnerLoop:
		beq $v1, $t2, break	#if the loopcounter equals the size break the inner loop
		lw $k0, 0($s2)		#load the element 1
		lw $k1, 0($s3)		#load the element 2
		blt $k0, $k1, continue	#if element 1 is the less than element 2 skip the swap
		sw $k1, 0($s2)		#else swap the elements
		sw $k0, 0($s3)
		continue:
			add $s2, $s2, $t0	#increment the address so that the element 1 becomes element 2 and element 2 becomes element 3
			add $s3, $s3, $t0	
			add $v1, $v1, $t3	#increment loop counter
			j InnerLoop		#repeat the loop
	break:				#by this time the last will the greatest element
		sub $t2, $t2, $t3	#decrement the loop counters
		sub $t1, $t1, $t3
		li $v1, 1		#restore v1 back to 1
		j OuterLoop
exit:
