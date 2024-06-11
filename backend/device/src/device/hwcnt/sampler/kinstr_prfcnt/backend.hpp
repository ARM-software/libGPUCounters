/*
 * Copyright (c) 2022-2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
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
 */

/**
 * @file
 *
 * Hardware counters reader implementation.
 */

#pragma once

#include "backend_args.hpp"
#include "block_index_remap.hpp"
#include "convert.hpp"
#include "metadata_parser.hpp"

#include <device/hwcnt/sampler/base/backend.hpp>
#include <device/hwcnt/sampler/poll.hpp>
#include <device/ioctl/kinstr_prfcnt/commands.hpp>
#include <device/ioctl/kinstr_prfcnt/types.hpp>
#include <device/ioctl/strided_array_iterator.hpp>

namespace hwcpipe {
namespace device {
namespace hwcnt {
namespace sampler {
namespace kinstr_prfcnt {

/** HWC backend using the kinstr_prfcnt kernel ioctls. */
template <typename syscall_iface_t>
class backend : public base::backend<syscall_iface_t> {
  public:
    using args_type = backend_args<syscall_iface_t>;

    explicit backend(args_type &&args, block_index_remap *remap = nullptr, const syscall_iface_t &syscall_iface = {})
        : base_type(std::move(args.base_args), syscall_iface)
        , metadata_item_size_(args.metadata_item_size)
        , remap_(remap) {}

    std::error_code start(uint64_t user_data) override { return issue_command(cmd_code_type::start, user_data); }

    std::error_code stop(uint64_t user_data) override { return issue_command(cmd_code_type::stop, user_data); }

    std::error_code request_sample(uint64_t user_data) override {
        return issue_command(ioctl::kinstr_prfcnt::control_cmd::control_cmd_code::sample_sync, user_data);
    }

    std::error_code get_sample(sample_metadata &sm, sample_handle &sample_hndl_raw) override {
        /* Wait until sample is ready. */
        std::error_code ec = wait_for_sample(fd_, get_syscall_iface());
        if (ec)
            return ec;

        auto &sample_hndl = sample_hndl_raw.get<sample_handle_type>();

        /* A sample is available. Obtain it and return it. */
        std::tie(ec, std::ignore) =
            get_syscall_iface().ioctl(fd_, ioctl::kinstr_prfcnt::command::get_sample, &sample_hndl);

        if (ec)
            return ec;

        const auto metadata_ptr{sample_hndl.sample_metadata_ptr.get(memory_.data())};
        ioctl::strided_array_iterator<const metadata_item_type> metadata_it{metadata_ptr, metadata_item_size_};

        metadata_parser parser{sm, block_extents_, remap_};
        auto metadata_it_end = parser.end(metadata_it);
        ec = parse_all(metadata_it, metadata_it_end, parser);

        /* Put sample back if its metadata is invalid. */
        if (ec)
            put_sample(sample_hndl_raw);

        return ec;
    };

    bool next(sample_handle sample_hndl_raw, block_metadata &bm, block_handle &block_hndl_raw) const override {
        auto &sample_hndl = sample_hndl_raw.get<sample_handle_type>();
        auto &block_hndl = block_hndl_raw.get<block_handle_type>();

        /* Go through all the metadata entries until block entry is found. */
        const auto metadata_ptr{sample_hndl.sample_metadata_ptr.get(memory_.data())};

        if (block_hndl == nullptr)
            block_hndl = metadata_ptr;

        ioctl::strided_array_iterator<const metadata_item_type> it{block_hndl, metadata_item_size_};
        bool done{false};

        const auto mapping = static_cast<const uint8_t *>(memory_.data());

        done = parse_block_item(bm, it, mapping, remap_);

        block_hndl = &*it;

        return done;
    }

    std::error_code put_sample(sample_handle sample_hndl_raw) override {
        std::error_code ec;

        std::tie(ec, std::ignore) =
            get_syscall_iface().ioctl(fd_, ioctl::kinstr_prfcnt::command::put_sample, &sample_hndl_raw);

        return ec;
    }

    std::error_code discard() override { return issue_command(cmd_code_type::discard); }

  protected:
    using base_type = base::backend<syscall_iface_t>;
    using base_type::block_extents_;
    using base_type::fd_;
    using base_type::get_syscall_iface;
    using base_type::memory_;

  private:
    using metadata_item_type = ioctl::kinstr_prfcnt::metadata_item;
    using sample_handle_type = ioctl::kinstr_prfcnt::sample_access;
    using block_handle_type = const metadata_item_type *;
    using cmd_code_type = ioctl::kinstr_prfcnt::control_cmd::control_cmd_code;

    std::error_code issue_command(cmd_code_type cmd, uint64_t user_data = 0) {
        std::error_code ec;

        ioctl::kinstr_prfcnt::control_cmd command{};
        command.cmd = cmd;
        command.user_data = user_data;

        std::tie(ec, std::ignore) =
            get_syscall_iface().ioctl(fd_, ioctl::kinstr_prfcnt::command::issue_command, &command);

        return ec;
    }

    /** Size of metadata item: includes sample metadata, clock metadata and block metadata. */
    const size_t metadata_item_size_{};
    /** Block index remap instance, if any. */
    const block_index_remap *remap_;
};

} // namespace kinstr_prfcnt
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
