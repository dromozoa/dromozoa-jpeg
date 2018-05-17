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

local function read_uint8(data, i)
  return data:byte(i)
end

local function read_uint16(data, i)
  local a, b = data:byte(i, i + 1)
  return a * 0x100 + b
end

local function read_int16(data, i)
  local v = read_uint16(data, i)
  if v < 0x8000 then
    return v
  else
    return v - 0x10000
  end
end

local function read_uint32(data, i)
  local a, b, c, d = data:byte(i, i + 3)
  return a * 0x1000000 + b * 0x10000 + c * 0x100 + d
end

local function read_int32(data, i)
  local v = read_uint32(data, i)
  if v < 0x80000000 then
    return v
  else
    return v - 0x100000000
  end
end

local function read_urational(data, i)
  local f = read_uint32(data, i)
  local d = read_uint32(data, i + 4)
  return f .. "/" .. d
end

local function read_rational(data, i)
  local f = read_int32(data, i)
  local d = read_int32(data, i + 4)
  return f .. "/" .. d
end

local function read_values(data, item, read, size)
  local i = item.offset + 7
  local j = i + item.count * size - 1
  local result = {}
  for i = i, j, size do
    result[#result + 1] = read(data, i)
  end
  return table.concat(result, ", ")
end

local function read_value(data, item)
  local i = item.offset + 7
  if item.type == 1 then
    if item.count == 1 then
      return item.offset
    else
      return read_values(data, item, read_uint8, 1)
    end
  elseif item.type == 2 then
    if item.count <= 4 then
      local a = item.offset
      local d = a % 256
      a = (a - d) / 256
      local c = a % 256
      a = (a - c) / 256
      local b = a % 256
      a = (a - b) / 256
      if item.count == 1 then
        return ""
      elseif item.count == 2 then
        return string.char(a)
      elseif item.count == 3 then
        return string.char(a, b)
      elseif item.count == 4 then
        return string.char(a, b, c)
      end
    else
      return data:sub(i, i + item.count - 2)
    end
  elseif item.type == 3 then
    if item.count == 1 then
      return item.offset
    else
      return read_values(data, item, read_uint16, 2)
    end
  elseif item.type == 4 then
    if item.count == 1 then
      return item.offset
    else
      return read_values(data, item, read_uint32, 4)
    end
  elseif item.type == 5 then
    if item.count == 1 then
      return read_urational(data, i)
    else
      return read_values(data, item, read_urational, 8)
    end
  elseif item.type == 7 then
    return "(undefined)"
  elseif item.type == 10 then
    if item.count == 1 then
      return read_rational(data, i)
    else
      return read_values(data, item, read_srational, 8)
    end
  else
    return "(not implmented)"
  end
end

local function read_ifd(data, i)
  local n = read_uint16(data, i)
  if verbose then
    io.stderr:write("n ", n, "\n")
  end
  local items = {}
  local i = i + 2
  local j = i + n * 12 - 1
  for i = i, j, 12 do
    local item = {
      tag = read_uint16(data, i);
      type = read_uint16(data, i + 2);
      count = read_uint32(data, i + 4);
      offset = read_uint32(data, i + 8);
    }
    item.value = read_value(data, item)
    if verbose then
      io.stderr:write(("0x%04X | %2d | %3d | 0x%08x | %s\n"):format(item.tag, item.type, item.count, item.offset, item.value))
    end
    items[#items + 1] = item
  end
  return items
end

local decompressor = assert(jpeg.decompressor())
local handle = assert(io.open("docs/test.jpg", "rb"))
assert(decompressor:set_fill_input_buffer(function (n)
  return handle:read(n)
end))

assert(decompressor:save_markers(jpeg.JPEG_APP1))
assert(decompressor:read_header() == jpeg.JPEG_HEADER_OK)

local markers = assert(decompressor:get_marker_list())
assert(#markers == 2)

for i = 1, #markers do
  local marker = markers[i]
  assert(marker.marker == jpeg.JPEG_APP1)
  local data = marker.data

  local out = assert(io.open(("test%d.dat"):format(i), "wb"))
  out:write(data)
  out:close()

  if i == 1 then
    assert(data:find "^Exif%z%zMM")

    -- "II\42\0" little-endian
    -- "MM\0\42" big-endian
    assert(data:sub(7, 10) == "MM\0\42")

    -- Offset of IFD
    assert(read_uint32(data, 11) == 8)

    local ifd0 = read_ifd(data, 15)
    local offset_exif
    local offset_gps

    for i = 1, #ifd0 do
      local item = ifd0[i]
      if item.tag == 0x8769 then
        offset_exif = item.value
      elseif item.tag == 0x8825 then
        offset_gps = item.value
      end
    end

    assert(offset_exif)
    assert(offset_gps)

    local ifd_exif = read_ifd(data, offset_exif + 7)
    local exif = {}
    for i = 1, #ifd_exif do
      local item = ifd_exif[i]
      exif[item.tag] = item.value
    end

    assert(exif[0xA434] == "iPhone 6s Plus back camera 4.15mm f/2.2")

    local ifd_gps = read_ifd(data, offset_gps + 7)
    local gps = {}
    for i = 1, #ifd_gps do
      local item = ifd_gps[i]
      gps[item.tag] = item.value
    end

    assert(gps[0x0001] == "N")
    assert(gps[0x0002] == "35/1, 41/1, 4183/100")
    assert(gps[0x0003] == "E")
    assert(gps[0x0004] == "139/1, 42/1, 1976/100")
  elseif i == 2 then
    assert(data:find "^http://")
  end
end

