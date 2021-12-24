# SHA-256 Hash Generator  
This is a fun little project in which I wrote a C program to generate SHA-256 hash of a given character string. This program is executed by a Python "wrapper script". This script passes the string to the C program as a command line argument. By tinkering the wrapper function, the string can either be entered by the user, or be read from a file. There is a **DO NOT TOUCH** zone in this script. Just do not tinker with it unless you know what you are doing.  
Say that you want to read input from the command line. Then you will just need to comment `string = input(">>")` line and comment the following lines:  
	`fp1 = open("test.txt", 'r')`
	`string = fp1.read()`  
In the case you want to find the hash of the contents of a text file, you have to do the opposite of the above procedure.  
The resulting digest is stored in a text file named `sha256_result.txt`.  
Note that this project was done because I wanted to understand this hash. So by no means it is "gold-plated". Also, for strings having more than 2000 characters, the result of the hash are wrong. The problem is probably in the method used to pass the string, instead of the algorithm itself. I would be really happy if someone who stumbles by this code gives me a solution to this problem.