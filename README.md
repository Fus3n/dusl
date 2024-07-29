# DUSL
DUSL = **D**ynamic **U**tility And **S**cripting **L**anguage

**(WIP)** A simple scripting interpreted language written in C++, meant for utility scripts and other similar scripting needs, not meant for performance critical tasks.

### Printing
```python
println("Hello World")
print("Hello World\n")
```
### Input
```python
print("Enter your name: ")
name = readLine()
```

### Data types and variable definition
```python
a = "Hello, world" #string
b = 123 #int64
c = 1342.292 #float64
d = [1, 2, 3, "A", "B"] # list
e = {"Key1": "value1", "key2": "value2", 35: 3+5, "sub": {"sub": 1}} # dictionary
```

### Functions
```py
fn add(a, b) {
    return a + b
}

# Annonymous function
sub = fn(a, b) {
    return a - b
}
# Note: All functions return the last element by default same for annonymous functions
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

### Examples
```python
# bubble sort
fn bubbleSort(list) {
    n = list.size
    swapped = true
    while swapped {
        swapped = false
        i = 0
        while i < n - 1 {
            if (list[i] > list[i + 1]) {
                temp = list[i]
                list[i] = list[i + 1]
                list[i + 1] =  temp
                swapped = true
            }
            i = i + 1
        }
    }
}

# binary search implementation
# expects a sorted list
fn binarySearch(list, target) {
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
```

_**More on examples directory**_

## Build
```bash
mkdir build
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
cmake --build build --config Release
```

