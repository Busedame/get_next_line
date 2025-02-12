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

If we were to call `get_next_line()` again, the output would be "How are you today?",
because this is the next line of the file.  
If we called it for the third time, `get_next_line()` would return `NULL`, because
the end of file was reached.  

---

`static variables` was a new (and crucial) concept to me while working with this project. To understand the concept of  
static variables, I will give a short reminder of how stack and heap memory allocation works.

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

### Static variables

In `get_next_line()`, we want to read from a file, one line at a time.

The function is structured in the following steps:
1. `get_next_line()` uses the `read()` function to read from the file. It reads
`BUFFER_SIZE` bytes at a time (the size is defined in `get_next_line.h`), and
stores the bytes in `buffer`.
```bash
	bytesread = read(fd, buffer, BUFFER_SIZE);
```
2. The `read()` function is called in a `while`-loop, because it wants to read
the file until it encounters a `\n` character, indicating that the line has
ended.
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
3. Since the `BUFFER_SIZE` is a varying size, and the lines may vary in length, the
`read()` function may sometimes read past the `\n` character. For example:
```bash
	What the file contains:
	Hello, there!
	How are you?
```
```bash
	An example of buffer size and a `read()` call:
	BUFFER_SIZE = 16;
	bytesread = read(fd, buffer, BUFFER_SIZE);
```
```bash
	What is getting stored in the `readnow` variable on first get_next_line() call:
	"Hello, there!\nHo"
```
4. In this example, we have encountered a newline character. But our line is a bit longer
than expected ("Hello, there!\nHo"). We want the `get_next_line()` function to return "Hello, there!\n",
because that is the first line.  
Since the `read()` function can't "unread" what has already been read, we need to store the
**remainder** somewhere. The remainder in this case is "Ho", because this
belongs to the next line.
5. The `get_next_line()` function will return "Hello, there!\n" on the first call,
and then store "Ho" for the next function call. The next time `get_next_line()`
is called, it already has "Ho" from last function call -> it reads again using `read()` -> 
appends "w are you?\n" to "Ho", and it returns the line "How are you?".
```bash
	What the file contains:
	Hello, there!
	How are you?
```
```bash
	An example of buffer size and a `read()` call:
	BUFFER_SIZE = 16;
	bytesread = read(fd, buffer, BUFFER_SIZE);
```
```bash
	What is getting stored in the `readnow` variable on second get_next_line() call:
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
because its memory is wiped out.
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
**the end of them program**. Now, this becomes very useful for our `get_next_line()`, where
we want to store the **remainder** from our `read()` function, doesn't it?

An essential part of my implementation of `ft_printf()` is the `va_start()`, `va_arg()` and `va_end()` macros
from the `stdarg.h` library. They are used for handling variable argument lists in functions.
- `va_start()` initializes a `va_list` variable to be used with `va_arg` and `va_end`. It must be called before accessing the variable arguments.
- `va_arg()` retrieves the next argument in the list. The type of the expected argument needs to be specified.
- `va_end()` cleans up the va_list when you are done with it. It should be called before the function returns.

The `ft_printf()` function is declared like this:
```bash
int ft_printf(const char *, ...);
```

#### Arguments of `ft_printf()`  
- `const char *` -- A string containing characters and/or type identifiers.  
- `...` -- An undefined amount of arguments, with undefined types.  

#### Return value of `ft_printf()`  
The return value for `ft_printf()` is an `int`, indicating **how many characters were printed**.  
It returns `-1` upon error.  

`ft_printf()` writes to the standard output, and is limited to handling the following conversions:
- `%c` Prints a single character.
- `%s` Prints a string (as defined by the common C convention).
- `%p` The void * pointer argument has to be printed in hexadecimal format.
- `%d` Prints a decimal (base 10) number.
- `%i` Prints an integer in base 10.
- `%u` Prints an unsigned decimal (base 10) number.
- `%x` Prints a number in hexadecimal (base 16) lowercase format.
- `%X` Prints a number in hexadecimal (base 16) uppercase format.
- `%%` Prints a percent sign.

#### Understanding `va_start()` in `ft_printf()`
The `const char *` is a string that can contain both text and type identifiers, like "%s", "%p", etc.  
By using the function `va_start()`, the **type identifiers** in `const char *` gets connected to the **data** `...`, which is stored in a `va_list`.  
- For example: `ft_printf("%i", 42);`  
- In this example, `const char *` is `%i` and `...` is `42`.
- What will be printed on the standard output: `42`.  

#### Understanding `va_arg()` in `ft_printf()`
The function `va_arg()` is used to iterate through the type identifiers and their respective data.  
In `ft_printf()` -> whenever a type identifier is found in `const char *`, it gets connected to the next argument in `...`.  
- For example: `ft_printf("Hello, world! My name is %s, and I am %i years old.\n", "Bob", 40);`  
- In this example, `const char *` is `"Hello, world! My name is %s, and I am %i years old.\n"`, and `...` is `"Bob"` and `40`.
- This essentially means that `%s` expands to `"Bob"`, and `%i` expands to `40`. 
- What will be printed on the standard output: `"Hello, world! My name is Bob, and I am 40 years old."`.  

#### Understanding `va_end()` in `ft_printf()`
When `const char *` has reached the `'\0'`, meaning it has reached its end, `va_end()` is called to clean up the `va_list`.  

---

## 🧰 **Included Files**  
- The `ft_printf.c` file containing the `ft_printf()` function, and the remaining `*.c` files containing all the printing functions for the different types.
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

