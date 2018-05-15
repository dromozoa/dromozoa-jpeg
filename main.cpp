// Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
//
// This file is part of dromozoa-jpeg.
//
// dromozoa-jpeg is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dromozoa-jpeg is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dromozoa-jpeg.  If not, see <http://www.gnu.org/licenses/>.

#include "common.hpp"

namespace dromozoa {
  void initialize_main(lua_State* L) {
    luaX_set_field<int>(L, -1, "JCS_UNKNOWN", JCS_UNKNOWN);
    luaX_set_field<int>(L, -1, "JCS_GRAYSCALE", JCS_GRAYSCALE);
    luaX_set_field<int>(L, -1, "JCS_RGB", JCS_RGB);
    luaX_set_field<int>(L, -1, "JCS_YCbCr", JCS_YCbCr);
    luaX_set_field<int>(L, -1, "JCS_CMYK", JCS_CMYK);
    luaX_set_field<int>(L, -1, "JCS_YCCK", JCS_YCCK);

    luaX_set_field(L, -1, "JPEG_SUSPENDED", JPEG_SUSPENDED);
    luaX_set_field(L, -1, "JPEG_HEADER_OK", JPEG_HEADER_OK);
    luaX_set_field(L, -1, "JPEG_HEADER_TABLES_ONLY", JPEG_HEADER_TABLES_ONLY);

    luaX_set_field(L, -1, "JPEG_RST0", JPEG_RST0);
    luaX_set_field(L, -1, "JPEG_EOI", JPEG_EOI);
    luaX_set_field(L, -1, "JPEG_APP0", JPEG_APP0);
    luaX_set_field(L, -1, "JPEG_COM", JPEG_COM);
  }
}
