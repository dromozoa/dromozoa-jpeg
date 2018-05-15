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
assert(decompressor:set_fill_input_buffer(function (n)
  return "\0\0"
end))
local result, message = decompressor:read_header()
if verbose then
  io.stderr:write(message, "\n")
end
assert(not result)

local warning = 0
local compressor = assert(jpeg.compressor())
assert(compressor:set_output_message(function (message)
  if verbose then
    io.stderr:write(message, "\n")
  end
  warning = warning + 1
end))
assert(compressor:set_empty_output_buffer(function () end))
assert(compressor:set_image_width(1))
assert(compressor:set_image_height(2))
assert(compressor:set_input_components(3))
assert(compressor:set_in_color_space(jpeg.JCS_RGB))
assert(compressor:set_defaults())
assert(compressor:start_compress())
assert(compressor:write_scanlines("\0\0\0"))
assert(compressor:write_scanlines("\0\0\0"))
assert(compressor:write_scanlines("\0\0\0"))
assert(compressor:finish_compress())
assert(warning == 1)
