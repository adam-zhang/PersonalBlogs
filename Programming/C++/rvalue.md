# The understanding of rvalue in C++

1. definition:

   in C++, values can be lvalue and rvalue, lvalue in statement must be in left of assignment, and rvalue must be in right of assignment, like:
	```
   	int a = 5;
	```
   in statement above, variable a is a rvalue, and 5 is an rvalue;
   of course, in C++, lvalue can be rvalue, like:
   ```
	int b = 5;
	int a = b;
   ```
   but opposite sometimes doesn't work.

2. description
  
   
