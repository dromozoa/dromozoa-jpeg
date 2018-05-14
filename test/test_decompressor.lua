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

local decompressor = assert(jpeg.decompressor())

local handle = assert(io.open("docs/l_hires.jpg", "rb"))
assert(decompressor:set_fill_input_buffer(function ()
  local data = handle:read(4096)
  if verbose then
    io.stderr:write("fill_input_buffer ", #data, "\n")
  end
  return data
end))

assert(decompressor:read_header() == jpeg.JPEG_HEADER_OK)
assert(decompressor:start_decompress())
assert(decompressor:get_output_width() == 1084)
assert(decompressor:get_output_height() == 2318)
assert(decompressor:get_out_color_space() == jpeg.JCS_RGB)
assert(decompressor:get_out_color_components() == 3)
assert(decompressor:get_output_components() == 3)




