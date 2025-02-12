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
heads around `static variables`. In this next part I will:
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
```bash
	Content stored in the `readnow` variable after the first `get_next_line()` call:
	"Hello, there!\nHo"
```
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
```bash
	Content stored in the `readnow` variable after the second `get_next_line()` call:
	"How are you?\n"
```

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
Since the variable is now **static**, the memory address and value remains allocated until
**the end of the program**. Now, this becomes very useful for our `get_next_line()`, where
we want to store the **remainder** from our `read()` function, doesn't it? 😃  

Even though it would technically be possible to create a `get_next_line()` with a remainder
allocated on the heap, this would require a different structure that would go beyond what is
allowed by the subject.

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
- A `Makefile`.
- A header file.

---

## 🧑‍💻 **Cloning the Repository**

To get started, first clone the repository.
This command will clone the repo, and move you to the right directory:
```bash
git clone https://github.com/busedame/ft_printf/ ft_printf && cd ft_printf
```
---

## 🖥️ **How to Compile and Run**  

1. **Compile the library**:  
	Run the following command to compile the library:  
	```bash
	make
	```
	This simply compiles the library, but does **not** make an executable.
2. **Test the function**:
	- Either use the existing main function in `ft_printf.c` (needs to be commented out), or add a `main.c` file with a new main function.
	- Compile using this command:
	```bash
	cc *.c -o ft_printf
	```
 	- Run using this command:
	```bash
	./ft_printf
 	```

 ---

## 🔗**Including ft_printf in your project**

Ensure that you have both your main project and the `ft_printf` library in the same directory or organized in separate directories. You will need the path to your `ft_printf` directory to link it in the `Makefile`.

- **In the header file**: Include `ft_printf.h`

- **In the Makefile**:

  1. **Path to the ft_printf project**:
     ```makefile
     FT_PRINTF_PATH = <path_to_ft_printf_directory>
     ```

  2. **Link the libftprintf.a library**:
     ```makefile
     FT_PRINTF = $(FT_PRINTF_PATH)/libftprintf.a
     LIBS = -L$(FT_PRINTF_PATH) -lft
     ```

  3. **Compile the project and link with ft_printf**:
     ```makefile
     $(NAME): $(OBJ) $(FT_PRINTF)
         $(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LIBS)
     ```

  4. **Ensure that ft_printf is compiled during the build process**:
     ```makefile
     $(FT_PRINTF):
         $(MAKE) -C $(FT_PRINTF_PATH)
     ```

  5. **Clean up object files**:
     ```makefile
     clean:
         rm -f $(OBJ)
         $(MAKE) -C $(FT_PRINTF_PATH) clean
     ```

  6. **Clean up everything (object files, executables, and libftprintf.a)**:
     ```makefile
     fclean: clean
         rm -f $(NAME)
         $(MAKE) -C $(FT_PRINTF_PATH) fclean
     ```

**Example of Makefile**:
```bash
CC = cc
CFLAGS = -Wall -Wextra -Werror
OPTIONS = -c

# Path to the ft_printf project
FT_PRINTF_PATH = ./ft_printf

# Source files for your project
SRC = main.c

# Object files for your project
OBJ = $(SRC:.c=.o)

# Final executable name
NAME = my_project

# Link the libftprintf.a library
FT_PRINTF = $(FT_PRINTF_PATH)/libftprintf.a
LIBS = -L$(FT_PRINTF_PATH) -lftprintf

# All: Compile both ft_printf and your project
all: $(NAME)

$(NAME): $(OBJ) $(FT_PRINTF)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LIBS)

# Compile .c files to .o object files
%.o: %.c
	$(CC) $(CFLAGS) $(OPTIONS) $< -o $@

# Ensure that ft_printf is compiled during the build process
$(FT_PRINTF):
	$(MAKE) -C $(FT_PRINTF_PATH)

# Clean up object files
clean:
	rm -f $(OBJ)
	$(MAKE) -C $(FT_PRINTF_PATH) clean

# Clean up everything (object files, executables, and libftprintf.a)
fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(FT_PRINTF_PATH) fclean

# Rebuild everything (clean and then compile both ft_printf and your project)
re: fclean all

.PHONY: all clean fclean re
```

