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

local lines = {}
for line in io.lines "docs/libjpeg.txt" do
  lines[#lines + 1] = line
end

local sections = { { title = "USING THE IJG JPEG LIBRARY", i = 1 } }
for i = 1, #lines do
  local line = lines[i]
  if line:find "^====+$" then
    sections[#sections + 1] = {
      title = lines[i - 1];
      i = i - 1;
    }
  elseif line:find "^%-%-%-%-+$" then
    local section = sections[#sections]
    section[#section + 1] = {
      title = lines[i - 1];
      i = i - 1;
    }
  end
end

for i = 1, #sections do
  local section = sections[i]
  print(section.title)
  for j = 1, #section do
    local subsection = section[j]
    print("", subsection.title)
  end
end
