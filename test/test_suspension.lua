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

local decompressor = assert(jpeg.decompressor(256))

local handle = assert(io.open("docs/l_hires.jpg", "rb"))
local suspend = true
assert(decompressor:set_fill_input_buffer(function (n)
  if not suspend then
    return handle:read(n)
  end
end))

assert(decompressor:read_header() == jpeg.JPEG_SUSPENDED)
suspend = false
assert(decompressor:read_header() == jpeg.JPEG_HEADER_OK)

suspend = true
assert(decompressor:start_decompress())
assert(not decompressor:read_scanlines())
