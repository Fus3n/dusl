<h1 style="text-align: center">Language Roadmap</h1>

### Bugs
- Fix `not` operator not working with member access
- Fix `not` operator not working with comparision operators
- ~~Fix `li[index] = li2[index]` just putting the whole list in the first list, parsing order error.~~
- Add a `get_attr` kind of function to all object that allows retrieving of specific property without using member access operator (.) 

### <p style="text-align: center">Things To Add (language features)</p>
- ~~list support~~
- ~~indexing support~~
- ~~add "and" & "or" support~~
- ~~add commenting~~
- ~~Complete member access operators functionality (partial complete)~~
- ~~for loops~~
- ~~function recursive calls~~
- ~~add hashing for objects~~
- ~~add dictionary support~~
- ~~allow calling functions in-line expression~~
- ~~add anonymous functions~~
- ~~make builtin type's functions into c++ static functions~~
- ~~while loops~~
- ~~add breaking out of loops~~
- optional arguments in functions (WIP)
- unary operations (WIP)
- ~~struct functions~~
- ~~struct defining functionality~~
- struct access functionality
- ~~Make every object return an error object instead of exiting the program~~
- Be able to take point to a struct function out and save in a variable
- ErrorObject that tracks and prints error's
- Present each file as a module
- Separate the global context
- ~~Module System (importing) WIP~~
- FFI?


### Things To Add (C++ side)
- ~~All object methods return a result object~~
- ~~Move all builtin-functions/variables into separate file and functions to load it~~
- Standard library (io, math, simple networking)
- More streamlined process of adding functions
- Add constructors for conversion from C++ primitives to dusl primitive