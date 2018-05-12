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

local escape_table = { ["&"] = "&amp;", ["<"] = "&lt;", [">"] = "&gt;" }
local function escape(source)
  return (source:gsub("[&<>]", escape_table))
end

local function write_manual_html(lines, sections, section_index, subsection_index)
  local section = sections[section_index]
  local subsection = section[subsection_index]

  local filename = ("libjpeg-%02d-%02d.html"):format(section_index, subsection_index)
  local out = assert(io.open("docs/" .. filename, "w"))
  out:write(([[
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
<title>%s / %s</title>
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/github-markdown-css/2.10.0/github-markdown.min.css">
<style>
.markdown-body {
  box-sizing: border-box;
  min-width: 200px;
  max-width: 980px;
  margin: 0 auto;
  padding: 45px;
}
@media (max-width: 767px) {
  .markdown-body {
    padding: 15px;
  }
}
</style>
</head>
<body>
<div class="markdown-body">

<h1>%s</h1>

<h2>%s</h2>

<pre>]]):format(escape(section.title), escape(subsection.title), escape(section.title), escape(subsection.title)))

  for i = subsection.i + 2, subsection.j do
    local line = escape(lines[i])
    out:write(line, "\n")
  end

  out:write[[</pre>
</div>
</body>
</html>
]]
  out:close()

  return filename
end

local lines = {}
for line in io.lines "docs/libjpeg.txt" do
  lines[#lines + 1] = line
end

local sections = { {} }
for i = 1, #lines do
  local line = lines[i]
  if line:find "^====+$" then
    local section = sections[#sections]
    section.j = i - 2
    section[#section].j = i - 2
    sections[#sections + 1] = {
      title = lines[i - 1];
      i = i - 1;
    }
  elseif line:find "^%-%-%-%-+$" then
    local section = sections[#sections]
    if #section > 0 then
      section[#section].j = i - 2
    end
    section[#section + 1] = {
      title = lines[i - 1];
      i = i - 1;
    }
  end
end
local section = sections[#sections]
section.j = #lines
section[#section].j = #lines

local filenames = {}

for i = 2, #sections do
  local section = sections[i]
  for j = 1, #section do
    filenames[#filenames + 1] = write_manual_html(lines, sections, i, j)
  end
end

local section = sections[1]
local subsection = section[1]
local out = assert(io.open("docs/libjpeg.html", "w"))
out:write [[
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
<title>libjpeg</title>
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/github-markdown-css/2.10.0/github-markdown.min.css">
<style>
.markdown-body {
  box-sizing: border-box;
  min-width: 200px;
  max-width: 980px;
  margin: 0 auto;
  padding: 45px;
}
@media (max-width: 767px) {
  .markdown-body {
    padding: 15px;
  }
}
</style>
</head>
<body>
<div class="markdown-body">

<h1>libjpeg</h1>

<pre>]]

for i = 1, subsection.i - 1 do
  local line = escape(lines[i])
  out:write(line, "\n")
end

out:write(([[</pre>

<h2>%s</h2>

<pre>]]):format(escape(subsection.title)))

local j = 0

for i = subsection.i, subsection.j do
  local line = escape(lines[i])
  if line:find "^\t" then
    j = j + 1
    line = line:gsub("^(%s*)(.*)", function (wsp, title)
      return ([[%s<a href="%s">%s</a>]]):format(wsp, filenames[j], title)
    end)
  end
  out:write(line, "\n")
end

out:write[[</pre>
</div>
</body>
</html>
]]
out:close()
