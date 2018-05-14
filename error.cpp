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
  namespace {
    class failure : public luaX_failure<> {
    public:
      explicit failure(const char* what) : what_(what) {}

      virtual ~failure() throw() {}

      virtual const char* what() const throw() {
        return what_.c_str();
      }

    private:
      std::string what_;
    };
  }

  void error_exit(const char* what) {
    throw failure(what);
  }

  void error_exit(j_common_ptr cinfo) {
    char what[JMSG_LENGTH_MAX] = { 0 };
    (*cinfo->err->format_message)(cinfo, what);
    throw failure(what);
  }
}
