import os

if __name__ == "__main__":
	#string = input(">>")
	fp1 = open("test.txt", 'r')
	string = fp1.read()
	print("INPUT")
	print(string)
	string = './sha256.out "' + string + '"'
	os.system(string)
	
	fp2 = open("sha256_result.txt", 'r')
	string = fp2.read()
	print("OUTPUT")
	print(string)