local json = require 'json'

print(json.parse("\"hello\""))
print(json.parse("'hello'"))
print(json.parse("1234"))
print(json.parse("true"))
print(json.parse("false"))
print(json.parse("null"))

arr = json.parse("[ 'hello', null, true, false, 1234 ]")
for k, v in pairs(arr) do
	print(k, v)
end

obj = json.parse("{ 'hello': 1234, 'wow': true }")
for k, v in pairs(obj) do
	print(k, v)
end
