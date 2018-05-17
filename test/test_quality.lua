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

local width = 600
local height = 800
local color_space = jpeg.JCS_RGB
local color_components = 3

local function compress(rows, quality)
  local compressor = assert(jpeg.compressor())
  local size = 0
  assert(compressor:set_empty_output_buffer(function (data)
    size = size + #data
  end))
  assert(compressor:set_image_width(width))
  assert(compressor:set_image_height(height))
  assert(compressor:set_input_components(color_components))
  assert(compressor:set_in_color_space(color_space))
  assert(compressor:set_defaults())
  assert(compressor:set_quality(quality))
  assert(compressor:start_compress())
  for y = 1, height do
    assert(compressor:write_scanlines(rows[y]))
  end
  assert(compressor:finish_compress())
  return size
end

local decompressor = assert(jpeg.decompressor())
local handle = assert(io.open("docs/test.jpg", "rb"))
assert(decompressor:set_fill_input_buffer(function (n)
  return handle:read(n)
end))
assert(decompressor:read_header() == jpeg.JPEG_HEADER_OK)
assert(decompressor:start_decompress())
assert(decompressor:get_output_width() == width)
assert(decompressor:get_output_height() == height)
assert(decompressor:get_out_color_space() == color_space)
assert(decompressor:get_out_color_components() == color_components)
local rows = {}
for y = 1, decompressor:get_output_height() do
  rows[y] = assert(decompressor:read_scanlines())
end
assert(decompressor:finish_decompress())

if verbose then
  io.stderr:write(width * height * color_components, "\n")
end

local prev = 0
for quality = 10, 90, 10 do
  local size = compress(rows, quality)
  if verbose then
    io.stderr:write(("%d\t%d\n"):format(quality, size))
  end
  assert(prev < size)
  prev = size
end
for quality = 91, 100 do
  local size = compress(rows, quality)
  if verbose then
    io.stderr:write(("%d\t%d\n"):format(quality, size))
  end
  assert(prev < size)
  prev = size
end
