## DUSL Everything
Weirdly formatted documentation that throws absolutely everything at you.

All the syntax highlighting is from python so might look weird.

```python
# single line comment
\ multiline comment \

# builtin binary operators
+, -, *, /
# logical operators
and, or, not # dusl does not support || or && or ! for these operators, only these keywords exists
# comparission
==, >, <, >=, <=
```

### Builtin functions
limited will be added in the future but this is all it has now
```py
# function(parameters) -> Return value, explanation
println("hello world") # -> none, prints given arguments with a new line
print("Hello world") # -> none, prints given arguments without a new line
readLine() # -> string, takes no argument, blocks program in terminal and takes user input untill enter is pressed and returns it
type(object) # -> type of object as a string
fillList(list, count) # -> none, really fast way to fill the list with 0's inplace
getTime() # -> int, current time in ms
getElapsedTimeMS(start ms, end ms) # -> int, time in ms in integer,
hash(object) # -> integer, get hash for supported types
system(string) # -> integer, execute the command line command, its like C's "system" function
rnd(max) # -> integer, returns a random integer from 0 to max
round(value) -> integer,  rounds the value
floor(value) -> integer, floors the value
error(string) -> throw a error with the message and stop the program
exit() # -> exit the program
```

### Data types and variable definition
```python
a = "Hello, world" #string
b = 123 #int64
c = 1342.292 #float64
d = [1, 2, 3, "A", "B"] # list
e = {"Key1": "value1", "key2": "value2", 35: 3+5, "sub": {"sub": 1}} # dictionary
```

### Properties and functions of object (very limited)

#### Int and float
```python
value = 3
value.toStr() # ->  returns string of the value
# float
value = 3.14
value.toStr() # -> returns string of the value
# for now int and float only has this one method
```

#### string
All the string functions and properties
```python
string = "Hello world"
string.size # -> int, total character count/size of string
string.split(delimiter) #  -> list of strings
string.get(int) # -> string, string at index
string.toInt() # -> int, convert to int
string.toFloat() # -> float, convert to float
string.toLower() # -> str, to lowercase
string.toUpper() # -> str, to uppercase
string.isDigit() # -> bool, check if digit or not
string.isAlpha() # -> bool, check if alpha or not
string.getCodeAt(index) -> int, returns ascii code at index
string.join(list of string) -> string, joins the elements with string value as delimeter
string.replace(from_str, to_str) # replaces a string with to_str and returns new string
string.startsWith(str) # checks if the string starts with given str, returns boolean
string.endsWith(str) # checks if the string ends with given str, returns boolean
string[index] # string, get string value at index
# these are all the string functions and props available for now
```

#### list 
All the string functions and properties
```python
list = ["hello", 42, "world", 32]
list.size # -> int, returns total size of list
list.set(int index, val) # -> set the value at index
list.push(val) # -> push value at the end of list
list.get(int index) # -> get value at index
list.pop() # -> return and remove the last element
list.forEach(callback) # -> takes a function callback and calls that with the current iteration of item
#  other ways of setting and getting in list:
list[index] = val # sets the value at index
list[index] # gets value at index
```

#### dictionary
```python
# assuming 'e' is a dictionary variable
e.size # -> int, returns total size of dictionary
e.get(key) # -> get value at key, returns none if not found, error is not raised
e.keys() # -> list, returns a list of all the keys
e.values() # -> list, returns a list of all the values
#  other ways of setting and getting in dictionary:
e[key] = val # -> set value with key
e[key] # -> get value with key
```

#### Null/None?
> there is currently a null type, but it's only meant for internal use and very few functions might return it
the plan is to handle error like rust where you have result or error and handle null with a Value or None struct.
> So for now some functions might just crash your program and print out the error, there's no error-handling or anything related to null values 
> other than it returning false values
> 
### Functions
```py
# You can make a funcitons too by doing
fn add(a, b) {
    return a + b
}

# or make closures like
sub = fn(a, b) {
    return a - b
}
# Note: All functions return the last element by default same for closures
```

###  Using for-loop
```py
# we have range its syntax is 0..100 this means 0 to 100
# we can use range for for-loop, like:

for item from 0..10 {
    println(item)
}

# or use an iterable object like string and list
value = "Hello world"
for c from value {
# loops over each charcer
    println("Character", c)
}

#list
val_list = [3, 4, 5, 6]
result = 0
for item from list {
    result = result + item
}

println("result", result)
# note "break" also exist for for-loop and while loop
```

### Using dictionary and list operations
```py
mod = {
    "on": 1,
    "shout": "yes",
    "say": fn(a) { println(a) }, # closure
    "add": fn(a, b) { a + b },
    "test": {"John": "Doe", "Liza": "Frankenstein"},
    "results": [10, 30, 50]
}
mod['test']['John'] = "Tyrant"
println(mod['test']) # after: {"Liza": "Frankenstein", "John": "Tyrant", }

mod["results"][0] = 50

println(mod["results"][0])
mod['say']("Hello, world") # Hello, world
```

### File handling
File handling is not that mature yet in DUSL, it's very limited, here are the few features

```py
# open a file
file = File("./test.txt")
println(file.size()) # returns file size in bytes
println(file.exists()) # returns if file exists or not with boolean
contents = file.read() # returns file contents as string
```

### Libraries/importing/modules
```py
# And finally for importing, theres no standard libary for now but the current "stanard lib" can be imported like:
import "std:string" # to import the string module
# to import specific functions
import [ joinStr ] from "std:string"

# same for normal imports, for example if math.flin is a file that is created in relative path and has "add" and "sub" method deifned, we can do:
import [add, sub] from "./math" # or you can do with the extension too like "./math.flin"

# ONLY std for now is std:string which only has ONE function and its called joinStr and can be used like:
import "std:string"

value = "Hello world".split(" ")
result = joinStr(value, ",") # signature: joinStr(list_of_str, delim)

println(result) # Hello,World
```

### FizzBuzz
````py
fn fizzBuzz(i) {
    if (i % 15 == 0) { return "FizzBuzz" }
    elseif (i % 3 == 0) { return "Fizz" }
    elseif (i % 5 == 0) { return "Buzz" }
    else { return i }
}
````

### Some random program
```py
# fizzbuzz
totalCount = 1000
v = []
fillList(v, totalCount)
count = 0
start = getTime()
while count < 1000 {
    v.push(fizzBuzz(count))
    count = count + 1
}
end = getTime()
elapsed = getElapsedTimeMS(start, end)
println(v)
println(elapsed)

# in-place bubblesort
fn bubbleSort(list) {
    n = list.size
    swapped = true
    while swapped {
        swapped = false
        i = 0
        while i < n - 1 {
            if (list[i] > list[i + 1]) {
                temp = list[i]
                list[i] = (list[i + 1]) # right side needs to be wrapped in brackets cuz of a bug currently
                list[i + 1] =  temp
                swapped = true
            }
            i = i + 1
        }
    }
}
# binary search (expects a sorted list)
fn binarySearch(list, target) {
    # Sort the list first
    bubbleSort(list)
    
    low = 0
    high = list.size - 1
    while low <= high {
        mid = floor((low + high) / 2)
        if (list[mid] == target) {
            return mid
        }
        elseif (list[mid] < target) {
            low = mid + 1
        }
        else {
            high = mid - 1
        }
    }
    return -1
}
# Caesar cipher
fn caesarCipher(str, shift) {
    result = []
    for char from str {
        if char.isAlpha() {
            base = char.toLower().getCodeAt(0) - 97
            newBase = (base + shift) % 26
            newChar = newBase + 97
            if char.isUpper() {
                newChar = newChar + 'A'.getCodeAt(0) - 'a'.getCodeAt(0)
            }
            result.push(newChar)
        } else {
            result.push(char)
        }
    }

    result_str = ""
    for char from result {
        if (type(char) == "int") {
            result_str = result_str + fromCharCode(char)
        } else {
            result_str = result_str + char
        }
    }

    return result_str
}
```