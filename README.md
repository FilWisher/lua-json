# lua-json
lua extension in c for parsing json

## usage

```lua

json = require 'json'

obj = json.parse("{ 'works': true, 'perfectly': false }")
for k, v in pairs(obj) do
  print(k, v)
done

arr = json.parse("[ 1234, null, true, false, \"beautiful\" ]")
for k, v in pairs(arr) do
  print(k, v)
done
```
