# 📖 **get_next_line - Reading from a file, line by line!**

## 🚀 **Project Overview**  
The project involves creating a function that can **read from a file descriptor, and return one line at a time**.  
It is exceptionally useful for parsing files. It is more memory efficient when dealing with a large file, because
you store one line at a time, instead of the whole file.

The function is declared like this:
```bash
char	*get_next_line(int fd);
```

Let's say we have a file (test.txt) that contains this text:
```bash
Hello, there!
How are you today?
```

We call `get_next_line()` one time:
```bash
int	main(void)
{
	int		fd;
	char	*output;

	fd = open("test.txt", O_RDONLY);
	output = get_next_line(fd);
	printf("%s", output);
	free(output);
}
```

And we get this output:
```bash
Hello, there!
```

If we were to call `get_next_line()` for the second time, the output would be this:
```bash
How are you today?
```

If we called it for the third time, `get_next_line()` would return `NULL`, because
the end of file was reached.  

---

## 🔍So how does get_next_line() work?

In order to understand how `get_next_line()` works, we need to wrap our
heads around `static variables`. But before we get there, we need some introduction!
In this next part I will:
1. Take you through the basics of stack and heap memory.
2. Show you the main logic of `get_next_line()`.
3. Explain how a `static variable` works, and why it is important.

### Stack and heap memory
Allocating memory refers to the process of reserving a specific amount of memory for use in a program.
When you allocate memory, you are essentially telling the system that you need a certain amount of memory 
for your program to use. This memory can then be used to store data, such as variables or data structures.
There are two main ways of allocating memory, **stack and heap**.

**Stack allocation:**
- Used for local variables within functions (they only exist within the function).
- The memory is automatically allocated (when the variable is declared) and deallocated (when the function exits).
- It has a fixed size.
```bash
void	example_stack(void)
{
	int	x = 42;
}
```

**Heap allocation:**
- Used for dynamic allocation with `malloc()`.
- The memory is manually allocated, and must be freed using the `free()` function.
- The memory can be accessed outside the function, as long as the address is stored in a pointer.
- It is dynamic in size, and its size can be based on variables instead of fixed values.
```bash
void	example_heap(void)
{
	int	*x;
	x = malloc(sizeof(int));
	*x = 42;
	free(x);
}
```

### How is get_next_line() structured?

The main logic is structured in the following steps:
1. `get_next_line()` calls the `read()` function to read from the file. It reads
`BUFFER_SIZE` bytes at a time and stores them in `buffer`.
```bash
	bytesread = read(fd, buffer, BUFFER_SIZE);
```
2. The `read()` function is called within a while loop to continue reading until 
it encounters a newline character (`\n`), signaling the end of a line:
```bash
	while (bytesread > 0)
	{
		bytesread = read(fd, buffer, BUFFER_SIZE);
		if (bytesread == -1)
			return (ft_free(buffer));
		buffer[bytesread] = '\0';
		readnow = ft_strjoin(readnow, buffer);
		if (!readnow)
			return (NULL);
		if (ft_strchr_index(readnow, '\n') > -1)
			break ;
	}
```
3. Since `BUFFER_SIZE` varies, and line lengths differ, the `read()` function 
may sometimes read past the newline character. For example:
```bash
	What the file contains:
	Hello, there!
	How are you?
```
```bash
	Example of `BUFFER_SIZE` and a `read()` call:
	BUFFER_SIZE = 16;
	bytesread = read(fd, buffer, BUFFER_SIZE);
```
Since 16 bytes was read, and "Hello, there!\n" is 14 bytes -- 2 additional bytes were added to `buffer`. So after the first `get_next_line()` call, this is stored in the `readnow` variable:  
	**"Hello, there!\nHo"**
- The newline character has been read, but the retrieved data is longer than the expected line: "Hello, there!\nHo".
- `get_next_line()` should return "Hello, there!\n" as the first line.
- Since `read()` cannot "unread" data, we need to store the **remainder** somewhere.
- The remainder here is "Ho", which belongs to the next line.  

4. **In the next function call:**
- It retrieves "Ho" from the previous call.
- It reads again using `read()`.
- "w are you?\n" is appended to "Ho".
- The function returns "How are you?\n".
```bash
	What the file contains:
	Hello, there!
	How are you?
```
```bash
	Example of `BUFFER_SIZE` and a `read()` call:
	BUFFER_SIZE = 16;
	bytesread = read(fd, buffer, BUFFER_SIZE);
```
Content stored in the `readnow` variable after the second `get_next_line()` call:  
	**"How are you?\n"**

### **Now how does the static variable come into place?**

As previously mentioned, **stack memory** gets deallocated once the function exits. 

Let's do an example:
```bash
void	example_stack(void)
{
	int	x = 0;
	x += 10;
	printf("%i\n", x);
}

int	main()
{
	example_stack();
	example_stack();
	example_stack();
}
```
In this example, the output would be:
```bash
	10
	10
	10
```

This happens because **every time the function exits, the memory is deallocated.**
`example_stack()` simply can't remember what the value of `x` used to be on the last call,
because its memory is erased.
Now, if we would instead declare a **static variable**, let's take a look at what would happen:
```bash
void	example_static(void)
{
	static int	x = 0;
	x += 10;
	printf("%i\n", x);
}

int	main()
{
	example_static();
	example_static();
	example_static();
}
```
In this example, the output would be:
```bash
	10
	20
	30
```
Since the variable is now **static**, it remains allocated until
**the end of the program**. Now, this becomes very useful for our `get_next_line()`, where
we want to store the **remainder** from our `read()` function, doesn't it? 😃  

###**In conclusion**

So we know that our remainder needs to be static in order to keep its value between function calls.
In my `get_next_line()` function, I also dynamically allocate memory using `malloc` for the remainder.
This means that **the remainder is both static and dynamically allocated**.
- It is static because it needs to keep its value between function calls.
- It is dynamic because the amount of bytes stored can vary from function call to function call.  

From reviewing the code in retrospect (on a cold February evening in 2025, a year later), I have discovered that the remainder is not properly getting
freed each time it gets updated with a new allocation. I have not seen any memory leaks happening, and I am curious about how this actually
works. I am also curious about if it would be possible to solve this without a static variable, and how this could be done.
But this remains a mystery for now. 

---

## 🚧**Limitations**
`get_next_line()` is by no means perfect.  

One big weakness with the function is the fact that **it returns `NULL` both upon memory allocation failure, 
AND if `read()` has reached the end of file**. This can be highly confusing.  
Although the limitations of the subject made it impossible (as far as I am aware) to handle it in a different way, these could
have been possible solutions:
- Solution 1: Return a struct instead of a pointer, containing information on what potentially went wrong.
- Solution 2: Set an errno value to indicate error.  

Another weakness is **if you want to read only ONE line from a file, without caring about the rest**. This
would give `still reachable memory` if not handled, because of the remainder.  
Even though `still reachables` are not technically considered a memory leak, it makes your `valgrind` report 
ugly (in my opinion at least).  
Here are some workarounds:
- Solution 1: Call the function like this `get_next_line(-1)`, indicating an error with the file descriptor, forcing
the remainder to be freed.
- Solution 2: Calling `get_next_line()` in a loop until return value is `NULL`, indicating end of file.

## 🧰 **Included Files**  
- `get_next_line.c` and `get_next_line_utils.c`
- A header file.

---

## 🧑‍💻 **Cloning the Repository**

To get started, first clone the repository.
This command will clone the repo, and move you to the right directory:
```bash
git clone https://github.com/busedame/get_next_line/ get_next_line && cd get_next_line
```
---

## 🖥️ **How to Compile and Run**  

1. Either use the existing main function in `get_next_line.c` (needs to be commented out), or add a `main.c` file with a new main function.
2. Create a `test.txt` file, and write some lines of text.
3. Compile using this command:
	```bash
	cc *.c -o get_next_line
	```
4. Run using this command:
	```bash
	./get_next_line
 	```

 ---

## 🔗**Including get_next_line in your project**

Simply `#include "get_next_line.h"` in your project, alongside the source files
from this repo, and you are good to go!
