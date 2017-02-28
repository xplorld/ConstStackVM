# ConstStackVM

A simple, error-free stack machine that runs in C++ compile time.

Have fun changing `Insts` in `main.cpp` and run.

	clang++ main.cpp -o main -std=c++11 -O3

Result of VM instructions is just `constexpr int result;`. 
You can find the result in asm code:

	clang++ main.cpp -S -std=c++11 -O3


# features


## error free 

everything is evaluated in C++ compile time, so that C++ type system comes to help. It assures that your VM instructions are correct or refuse to compile. 

### example #1
the following code would not compile:

	PUSH 1
	JMP 10

since 10 is out of bounds.

### example #2

	PUSH 1
	JMP 0

an infinite loop! this would not gonna happen.

## fast 

Running a piece of VM code costs *zero* time!
the only bottleneck is your C++ compiler.

## seamless integration with C++

well, you can just use `result` in any part of your C++ code.

## Turing Complete

So it is a **real** VM, not a toddler's toy. (Although pretty much an undergrduate's toy.)
