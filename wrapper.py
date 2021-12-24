import os

if __name__ == "__main__":
	#string = input(">>")
	fp = open("test.txt", 'r')
	string = fp.read()
	print(len(string))
	print(hex(len(string * 8)))
	string = './sha256.out "' + string + '"'
	os.system(string)