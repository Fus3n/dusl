# DUSL Extension Example
(WIP) A simple demonstration of how you can make C++ bindings for DUSL, for now the interpreter only supports windows.


This simply creates an `isPrime` function and feeds it to dusl's interpreter
It is linked using shared library (`dll` on windows, the extension is named to `dusll`)

### Build
```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" .
cmake build .
```

After building, you should see a `.dusll` file inside `build/extensions/dusl_extension_example.dusll`

Just put it in the same directory as your dusl script and now you can import it:

```py
import "dusl_extension_example"

println(isPrime(12))
```