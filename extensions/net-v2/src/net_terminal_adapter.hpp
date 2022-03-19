/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2022. Seungwoo Kang
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * project home: https://github.com/perfkitpp
 ******************************************************************************/

//
// Created by ki608 on 2022-03-19.
//

#pragma once
#include "perfkit/common/functional.hxx"

namespace perfkit::msgpack::rpc {
struct session_profile;
}

namespace perfkit::net {
using std::shared_ptr;
using std::string;
using std::string_view;
using std::unique_ptr;
using std::vector;
using std::weak_ptr;
}  // namespace perfkit::net

namespace perfkit::msgpack::rpc {
class context;
}

namespace asio {
class io_context;
}

namespace perfkit::net {
class if_net_terminal_adapter
{
   public:
    virtual ~if_net_terminal_adapter() noexcept = default;

    virtual bool has_basic_access(msgpack::rpc::session_profile const&) const = 0;
    virtual bool has_admin_access(msgpack::rpc::session_profile const&) const = 0;

    //
    virtual asio::io_context*      event_proc() = 0;
    virtual msgpack::rpc::context* rpc()        = 0;

   public:
    auto fn_basic_access() const { return bind_front(&if_net_terminal_adapter::has_basic_access, this); }
    auto fn_admin_access() const { return bind_front(&if_net_terminal_adapter::has_admin_access, this); }
};
}  // namespace perfkit::net
