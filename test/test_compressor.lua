-- Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
--
-- This file is part of dromozoa-jpeg.
--
-- dromozoa-jpeg is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- dromozoa-jpeg is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with dromozoa-jpeg.  If not, see <http://www.gnu.org/licenses/>.

local jpeg = require "dromozoa.jpeg"

local verbose = os.getenv "VERBOSE" == "1"

local compressor = assert(jpeg.compressor(256))

local out = assert(io.open("test.jpg", "wb"))
assert(compressor:set_empty_output_buffer(function (data)
  if verbose then
    io.stderr:write("empty_output_buffer ", #data, "\n")
  end
  out:write(data)
end))

local width = 32
local height = 64

assert(compressor:set_image_width(width))
assert(compressor:set_image_height(height))
assert(compressor:set_input_components(3))
assert(compressor:set_in_color_space(jpeg.JCS_RGB))
assert(compressor:set_defaults())

assert(compressor:start_compress())
for y = 1, height do
  local row = {}
  for x = 1, width do
    row[x] = string.char(math.floor(x * 255 / width), math.floor(y * 255 / height), 255)
  end
  assert(compressor:write_scanlines(table.concat(row)) == 1)
end
if verbose then
  io.stderr:write "before finish_compress\n"
  io.stderr:flush()
end
assert(compressor:finish_compress())
if verbose then
  io.stderr:write "after finish_compress\n"
  io.stderr:flush()
end


out:close()
