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

local compressor = assert(jpeg.compressor())
local decompressor = assert(jpeg.decompressor())

local handle = assert(io.open("docs/l_hires.jpg", "rb"))
assert(decompressor:set_fill_input_buffer(function (n)
  return handle:read(n)
end))

assert(decompressor:read_header() == jpeg.JPEG_HEADER_OK)
assert(decompressor:set_out_color_space(jpeg.JCS_GRAYSCALE))
assert(decompressor:start_decompress())

local height = assert(decompressor:get_output_height())
while decompressor:get_output_scanline() <= height do
  assert(decompressor:read_scanlines() > 0)
end
assert(decompressor:finish_decompress())
handle:close()

local rows = assert(decompressor:get_rows())
assert(#rows == 2318)
for i = 1, #rows do
  assert(#rows[i] == 1084)
  if verbose then
    io.stderr:write(rows[i])
  end
end

local out = assert(io.open("test.jpg", "w"))
assert(compressor:set_empty_output_buffer(function (data)
  out:write(data)
end))

assert(compressor:set_image_width(decompressor:get_output_width()))
assert(compressor:set_image_height(height))
assert(compressor:set_input_components(decompressor:get_out_color_components()))
assert(compressor:set_in_color_space(decompressor:get_out_color_space()))
assert(compressor:set_defaults())
assert(compressor:set_rows(rows))
assert(compressor:start_compress())
while compressor:get_next_scanline() <= height do
  assert(compressor:write_scanlines() > 0)
end

assert(compressor:finish_compress())
out:close()
