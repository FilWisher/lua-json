local json = require 'json'

print(json.parse("\"hello\""))
print(json.parse("'hello'"))
print(json.parse("1234"))
print(json.parse("true"))
print(json.parse("false"))
print(json.parse("null"))

function print_json(arg)

  if type(arg) == 'table' then
  	io.write("{\n")
  	for k, v in pairs(arg) do
      io.write(string.format("\t%s: ", k))
      print_json(v)
      io.write("\n")
    end
    io.write("}\n") 
  else
    io.write(tostring(arg))
  end

end

strings = {
	"[ 'hello', 1234, true, null, false ]",
  "{ 'hello': 1234, 'cool': true }",
  "[ ['hello'], { 'wowza': 1234, 'bur': true}, [[[null]], [false] ]]"
}

for _, j in pairs(strings) do
  res = json.parse(j)
  print_json(res)
end
