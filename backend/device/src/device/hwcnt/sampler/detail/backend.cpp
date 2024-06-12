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
 * Private hardware counters sampler implementation.
 */

#include <device/detail/cast_to_impl.hpp>
#include <device/hwcnt/sampler/kinstr_prfcnt/backend_wa.hpp>
#include <device/hwcnt/sampler/kinstr_prfcnt/setup.hpp>
#include <device/hwcnt/sampler/vinstr/backend.hpp>
#include <device/hwcnt/sampler/vinstr/setup.hpp>
#include <device/kbase_version.hpp>

namespace hwcpipe {
namespace device {

class handle;
class instance;

namespace hwcnt {

class reader;

namespace sampler {

struct configuration;

namespace detail {

using instance_impl_type = instance_impl<syscall::iface>;
using kinstr_backend = kinstr_prfcnt::backend<syscall::iface>;
using kinstr_backend_wa = kinstr_prfcnt::backend_wa<syscall::iface>;
using vinstr_backend = vinstr::backend<syscall::iface>;

/** Create kinstr_prfcnt backend if possible.
 *
 * @param inst    Instance implementation reference.
 * @param[in] period_ns    Period in nanoseconds between samples taken. Zero for manual context.
 * @param[in] config       Which counters to enable on per-block basis.
 * @param[in] config_len   Len of @p config array.
 * @return backend pointer, if created.
 */
static std::unique_ptr<detail::backend> kinstr_prfcnt_backend_create(const instance_impl_type &inst, uint64_t period_ns,
                                                                     const configuration *config, size_t config_len) {
    std::error_code ec;
    kinstr_backend::args_type args{};

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::tie(ec, args) = kinstr_prfcnt::setup(inst, period_ns, config, config + config_len);

    if (ec)
        return nullptr;

    if (inst.backend_type() == backend_type::kinstr_prfcnt_wa)
        return std::make_unique<kinstr_backend_wa>(std::move(args), syscall::iface{});

    return std::make_unique<kinstr_backend>(std::move(args), nullptr, syscall::iface{});
}

/** Create vinstr backend if possible.
 *
 * @param inst    Instance implementation reference.
 * @param[in] period_ns    Period in nanoseconds between samples taken. Zero for manual context.
 * @param[in] config       Which counters to enable on per-block basis.
 * @param[in] config_len   Len of @p config array.
 * @return backend pointer, if created.
 */
static std::unique_ptr<detail::backend> vinstr_backend_create(const instance_impl_type &inst, uint64_t period_ns,
                                                              const configuration *config, size_t config_len) {
    std::error_code ec;
    vinstr_backend::args_type args{};

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::tie(ec, args) = vinstr::setup(inst, period_ns, config, config + config_len);

    if (ec)
        return nullptr;

    return std::make_unique<vinstr_backend>(std::move(args), syscall::iface{});
}

backend::~backend() = default;

std::unique_ptr<detail::backend> backend::create(const instance &inst, uint64_t period_ns, const configuration *config,
                                                 size_t config_len) {
    const auto &inst_impl = hwcpipe::device::detail::cast_to_impl(inst);

    switch (inst_impl.backend_type()) {
    case backend_type::vinstr:
    case backend_type::vinstr_pre_r21:
        return vinstr_backend_create(inst_impl, period_ns, config, config_len);
    case backend_type::kinstr_prfcnt:
    case backend_type::kinstr_prfcnt_wa:
    case backend_type::kinstr_prfcnt_bad:
        return kinstr_prfcnt_backend_create(inst_impl, period_ns, config, config_len);
    }

    return nullptr;
}

} // namespace detail
} // namespace sampler
} // namespace hwcnt
} // namespace device
} // namespace hwcpipe
