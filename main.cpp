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
    luaX_set_field(L, -1, "JPEG_APP1", JPEG_APP0 + 1);
    luaX_set_field(L, -1, "JPEG_APP2", JPEG_APP0 + 2);
    luaX_set_field(L, -1, "JPEG_APP3", JPEG_APP0 + 3);
    luaX_set_field(L, -1, "JPEG_APP4", JPEG_APP0 + 4);
    luaX_set_field(L, -1, "JPEG_APP5", JPEG_APP0 + 5);
    luaX_set_field(L, -1, "JPEG_APP6", JPEG_APP0 + 6);
    luaX_set_field(L, -1, "JPEG_APP7", JPEG_APP0 + 7);
    luaX_set_field(L, -1, "JPEG_APP8", JPEG_APP0 + 8);
    luaX_set_field(L, -1, "JPEG_APP9", JPEG_APP0 + 9);
    luaX_set_field(L, -1, "JPEG_APP10", JPEG_APP0 + 10);
    luaX_set_field(L, -1, "JPEG_APP11", JPEG_APP0 + 11);
    luaX_set_field(L, -1, "JPEG_APP12", JPEG_APP0 + 12);
    luaX_set_field(L, -1, "JPEG_APP13", JPEG_APP0 + 13);
    luaX_set_field(L, -1, "JPEG_APP14", JPEG_APP0 + 14);
    luaX_set_field(L, -1, "JPEG_COM", JPEG_COM);
  }
}
