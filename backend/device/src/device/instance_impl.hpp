/*
 * Copyright (c) 2022-2024 Arm Limited.
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

#pragma once

#include "ioctl/kbase_pre_r21/types.hpp"
#include "kbase_version.hpp"

#include <device/constants.hpp>
#include <device/hwcnt/backend_type.hpp>
#include <device/hwcnt/block_extents.hpp>
#include <device/hwcnt/block_metadata.hpp>
#include <device/hwcnt/sampler/kinstr_prfcnt/construct_block_extents.hpp>
#include <device/hwcnt/sampler/kinstr_prfcnt/enum_info_parser.hpp>
#include <device/hwcnt/sampler/manual.hpp>
#include <device/hwcnt/sampler/vinstr/construct_block_extents.hpp>
#include <device/instance.hpp>
#include <device/ioctl/kbase/commands.hpp>
#include <device/ioctl/kbase/types.hpp>
#include <device/ioctl/kbase_pre_r21/commands.hpp>
#include <device/ioctl/kbase_pre_r21/types.hpp>
#include <device/ioctl/pointer64.hpp>
#include <device/kbase_version.hpp>
#include <device/num_exec_engines.hpp>
#include <device/product_id.hpp>

#include <array>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <system_error>
#include <vector>

namespace hwcpipe {
namespace device {

using properties = std::vector<unsigned char>;

namespace detail {

static uint64_t get_warp_width(product_id known_pid, std::error_code &ec) {
    switch (known_pid) {
    case product_id::t60x:
    case product_id::t62x:
    case product_id::t720:
    case product_id::t760:
    case product_id::t820:
        return 1;
    case product_id::g31:
    case product_id::g68:
    case product_id::g51:
    case product_id::g71:
    case product_id::g72:
        return 4;
    case product_id::g52:
    case product_id::g76:
        return 8;
    case product_id::g57:
    case product_id::g57_2:
    case product_id::g77:
    case product_id::g78:
    case product_id::g310:
    case product_id::g510:
    case product_id::g610:
    case product_id::g710:
    case product_id::g615:
    case product_id::g715:
        return 16;
    case product_id::t830:
    case product_id::t860:
    case product_id::t880:
    case product_id::g78ae:
    case product_id::g720:
    case product_id::g620:
    case product_id::g725:
    case product_id::g625:
        return 16;
    }

    ec = std::make_error_code(std::errc::not_supported);
    return 0;
}

/** Decoder for a kbase GPU props buffer. */
class prop_decoder {
  public:
    explicit prop_decoder(properties buffer) noexcept
        : reader_{std::move(buffer)} {}

    std::tuple<constants, product_id> operator()(std::error_code &ec) noexcept {
        constants dev_consts{};
        uint64_t num_core_groups{};
        std::array<uint64_t, 16> core_mask{};
        uint64_t raw_core_features{};
        uint32_t raw_thread_features{};

        while (reader_.size() > 0) {
            auto p = next(ec);
            prop_id_type id = p.first;
            uint64_t value = p.second;
            if (ec)
                return std::make_pair<constants, product_id>({}, product_id{});

            switch (id) {
            case prop_id_type::raw_gpu_id:
                dev_consts.gpu_id = value;
                break;
            case prop_id_type::l2_log2_cache_size:
                dev_consts.l2_slice_size = (1UL << value);
                break;
            case prop_id_type::l2_num_l2_slices:
                dev_consts.num_l2_slices = value;
                break;
            case prop_id_type::raw_l2_features:
                /* log2(bus width in bits) stored in top 8 bits of register. */
                dev_consts.axi_bus_width = 1UL << ((value & 0xFF000000) >> 24);
                break;
            case prop_id_type::raw_core_features:
                raw_core_features = value;
                break;
            case prop_id_type::coherency_num_core_groups:
                num_core_groups = value;
                break;
            case prop_id_type::raw_thread_features:
                raw_thread_features = static_cast<uint32_t>(value);
                break;
            case prop_id_type::coherency_group_0:
                core_mask[0] = value;
                break;
            case prop_id_type::coherency_group_1:
                core_mask[1] = value;
                break;
            case prop_id_type::coherency_group_2:
                core_mask[2] = value;
                break;
            case prop_id_type::coherency_group_3:
                core_mask[3] = value;
                break;
            case prop_id_type::coherency_group_4:
                core_mask[4] = value;
                break;
            case prop_id_type::coherency_group_5:
                core_mask[5] = value;
                break;
            case prop_id_type::coherency_group_6:
                core_mask[6] = value;
                break;
            case prop_id_type::coherency_group_7:
                core_mask[7] = value;
                break;
            case prop_id_type::coherency_group_8:
                core_mask[8] = value;
                break;
            case prop_id_type::coherency_group_9:
                core_mask[9] = value;
                break;
            case prop_id_type::coherency_group_10:
                core_mask[10] = value;
                break;
            case prop_id_type::coherency_group_11:
                core_mask[11] = value;
                break;
            case prop_id_type::coherency_group_12:
                core_mask[12] = value;
                break;
            case prop_id_type::coherency_group_13:
                core_mask[13] = value;
                break;
            case prop_id_type::coherency_group_14:
                core_mask[14] = value;
                break;
            case prop_id_type::coherency_group_15:
                core_mask[15] = value;
                break;
            case prop_id_type::minor_revision:
            case prop_id_type::major_revision:
            default:
                break;
            }
        }
        for (auto i{0U}; i < num_core_groups; ++i)
            dev_consts.shader_core_mask |= core_mask[i];
        dev_consts.num_shader_cores = static_cast<uint64_t>(__builtin_popcountll(dev_consts.shader_core_mask));

        dev_consts.tile_size = 16;

        product_id known_pid{};
        std::tie(ec, known_pid) = product_id_from_raw_gpu_id(dev_consts.gpu_id);
        if (ec)
            return std::make_pair<constants, product_id>({}, product_id{});

        /* Warp width */
        dev_consts.warp_width = get_warp_width(known_pid, ec);
        if (ec)
            return std::make_pair<constants, product_id>({}, product_id{});

        /* Number of execution engines */
        get_num_exec_engines_args args{};
        args.known_pid = known_pid;
        args.core_count = dev_consts.num_shader_cores;
        args.core_features = raw_core_features;
        args.thread_features = raw_thread_features;
        dev_consts.num_exec_engines = get_num_exec_engines(std::move(args), ec);
        if (ec)
            return std::make_pair<constants, product_id>({}, product_id{});

        return {dev_consts, known_pid};
    }

  private:
    /** Property id type. */
    using prop_id_type = ioctl::kbase::get_gpuprops::gpuprop_code;
    /** Property size type. */
    using prop_size_type = ioctl::kbase::get_gpuprops::gpuprop_size;

    static std::pair<prop_id_type, prop_size_type> to_prop_metadata(uint32_t v) noexcept {
        /* Property id/size encoding is:
         * +--------+----------+
         * | 31   2 | 1      0 |
         * +--------+----------+
         * | PropId | PropSize |
         * +--------+----------+
         */
        static constexpr unsigned prop_id_shift = 2;
        static constexpr unsigned prop_size_mask = 0x3;

        return {static_cast<prop_id_type>(v >> prop_id_shift), static_cast<prop_size_type>(v & prop_size_mask)};
    }

    std::pair<prop_id_type, uint64_t> next(std::error_code &ec) noexcept {
        /* Swallow any bad data. */
#define ERROR_IF_READER_SIZE_LT(x)                                                                                     \
    do {                                                                                                               \
        if (reader_.size() < (x)) {                                                                                    \
            ec = std::make_error_code(std::errc::protocol_error);                                                      \
            return {};                                                                                                 \
        }                                                                                                              \
    } while (0)

        ERROR_IF_READER_SIZE_LT(sizeof(uint32_t));

        auto p = to_prop_metadata(reader_.u32());
        prop_id_type id = p.first;
        prop_size_type size = p.second;

        switch (size) {
        case prop_size_type::uint8:
            ERROR_IF_READER_SIZE_LT(sizeof(uint8_t));
            return {id, reader_.u8()};
        case prop_size_type::uint16:
            ERROR_IF_READER_SIZE_LT(sizeof(uint16_t));
            return {id, reader_.u16()};
        case prop_size_type::uint32:
            ERROR_IF_READER_SIZE_LT(sizeof(uint32_t));
            return {id, reader_.u32()};
        case prop_size_type::uint64:
            ERROR_IF_READER_SIZE_LT(sizeof(uint64_t));
            return {id, reader_.u64()};
        }

#undef ERROR_IF_READER_SIZE_LT
        return {};
    }

    /* Reads values out of the KBASE_IOCTL_GET_GPUPROPS data buffer */
    class prop_reader {
      public:
        explicit prop_reader(properties buffer) noexcept
            : buffer_{std::move(buffer)}
            , data_{buffer_.data()}
            , size_{buffer_.size()} {}

        uint8_t u8() noexcept { return le_read_bytes<uint8_t>(); }
        uint16_t u16() noexcept { return le_read_bytes<uint16_t>(); }
        uint32_t u32() noexcept { return le_read_bytes<uint32_t>(); }
        uint64_t u64() noexcept { return le_read_bytes<uint64_t>(); }

        size_t size() const noexcept { return size_; }

      private:
        template <typename u_t>
        u_t le_read_bytes() noexcept {
            u_t ret{};
            for (auto b{0U}; b < sizeof(u_t); ++b)
                ret |= static_cast<u_t>(static_cast<uint64_t>(data_[b]) << (8 * b));
            data_ += sizeof(u_t);
            size_ -= sizeof(u_t);
            return ret;
        }

        properties const buffer_;
        unsigned char const *data_;
        std::size_t size_;
    } reader_;
};

template <typename version_t>
bool is_version_set(const version_t &version) {
    return version.major != 0 || version.minor != 0;
}

} // namespace detail

/** Mali device driver instance implementation. */
template <typename syscall_iface_t>
class instance_impl : public instance, private syscall_iface_t {
    using kbase_version_type = ::hwcpipe::device::kbase_version;

  public:
    instance_impl(int fd, const syscall_iface_t &iface = {})
        : syscall_iface_t(iface)
        , fd_(fd) {
        std::error_code ec = init(iface);
        valid_ = !ec;
    }

    ~instance_impl() override = default;

    constants get_constants() const override {
        /* The constants must have been initialized. */
        assert(constants_.gpu_id != 0);
        return constants_;
    }

    hwcnt::block_extents get_hwcnt_block_extents() const override {
        /* The block extents must have been initialized. */
        assert(block_extents_.num_blocks());
        return block_extents_;
    }

    hwcnt::sampler::kinstr_prfcnt::enum_info get_enum_info() const {
        /* The enum info must have been initialized. */
        assert(ei_.num_values != 0);
        return ei_;
    }

    product_id get_product_id() const { return pid_; }

    /**
     * Check if instance_impl is valid.
     *
     * @return True if valid, false otherwise.
     */
    bool valid() const { return valid_; }

    /**
     * Get file descriptor.
     *
     * @return The file descriptor for this instance_impl.
     */
    int fd() const { return fd_; }

    kbase_version_type kbase_version() const { return kbase_version_; }

    hwcnt::backend_type backend_type() const { return backend_type_; }

  private:
    /** @return Syscall iface reference. */
    syscall_iface_t &get_syscall_iface() { return *this; }

    /** Get device constants from old ioctl. */
    std::tuple<constants, product_id> props_pre_r21(int fd, std::error_code &ec) {
        constants dev_consts{};
        product_id known_pid{};

        ioctl::kbase_pre_r21::uk_gpuprops props{};
        props.header.id = ioctl::kbase_pre_r21::header_id::get_props;

        std::tie(ec, std::ignore) = get_syscall_iface().ioctl(fd, ioctl::kbase_pre_r21::command::get_gpuprops, &props);
        if (ec)
            return std::make_pair<constants, product_id>({}, product_id{});

        dev_consts.gpu_id = props.props.raw_props.gpu_id;
        std::tie(ec, known_pid) = product_id_from_raw_gpu_id(dev_consts.gpu_id);
        if (ec)
            return std::make_pair<constants, product_id>({}, product_id{});
        dev_consts.warp_width = detail::get_warp_width(known_pid, ec);
        if (ec)
            return std::make_pair<constants, product_id>({}, product_id{});
        dev_consts.l2_slice_size = 1UL << props.props.l2_props.log2_cache_size;
        dev_consts.num_l2_slices = props.props.l2_props.num_l2_slices;
        dev_consts.axi_bus_width = 1UL << ((props.props.raw_props.l2_features & 0xFF000000) >> 24);

        for (auto i{0U}; i < props.props.coherency_info.num_core_groups; i++)
            dev_consts.shader_core_mask |= props.props.coherency_info.group[i].core_mask;
        dev_consts.num_shader_cores = static_cast<uint64_t>(__builtin_popcountll(dev_consts.shader_core_mask));

        dev_consts.tile_size = 16;

        get_num_exec_engines_args args{};
        args.known_pid = known_pid;
        args.core_count = dev_consts.num_shader_cores;
        /* No core features in this interface version. */
        args.core_features = 0;
        args.thread_features = props.props.raw_props.thread_features;

        dev_consts.num_exec_engines = get_num_exec_engines(std::move(args), ec);

        return {dev_consts, known_pid};
    }

    /** Get the raw properties buffer as it's returned from the kernel. */
    properties props_post_r21(int fd, std::error_code &ec) {
        int ret = 0;

        ioctl::kbase::get_gpuprops get_props = {};
        std::tie(ec, ret) = get_syscall_iface().ioctl(fd, ioctl::kbase::command::get_gpuprops, &get_props);
        if (ec)
            return {};

        get_props.size = static_cast<uint32_t>(ret);
        properties buffer(static_cast<std::size_t>(ret));
        get_props.buffer.reset(buffer.data());
        std::tie(ec, ret) = get_syscall_iface().ioctl(fd, ioctl::kbase::command::get_gpuprops, &get_props);
        if (ec)
            return {};

        return buffer;
    }

    /** Get CSF firmware version. It returns 0 for JM GPUs. */
    uint64_t get_fw_version() {
        if (kbase_version_.type() != ioctl_iface_type::csf)
            return 0;

        int ret = 0;
        std::error_code _;
        ioctl::kbase::cs_get_glb_iface get_glb{};

        std::tie(_, ret) = get_syscall_iface().ioctl(fd_, ioctl::kbase::command::cs_get_glb_iface, &get_glb);
        if (ret < 0)
            return 0;

        return get_glb.out.glb_version;
    }

    std::error_code version_check_pre_r21() {
        ioctl::kbase_pre_r21::version_check_args version_check_args{};
        version_check_args.header.id = ioctl::kbase_pre_r21::header_id::version_check;

        get_syscall_iface().ioctl(fd_, ioctl::kbase_pre_r21::command::version_check, &version_check_args);

        if (!detail::is_version_set(version_check_args))
            return std::make_error_code(std::errc::not_supported);

        kbase_version_ =
            kbase_version_type(version_check_args.major, version_check_args.minor, ioctl_iface_type::jm_pre_r21);

        constexpr kbase_version_type legacy_min_version{10, 2, ioctl_iface_type::jm_pre_r21};
        if (kbase_version_ < legacy_min_version)
            return std::make_error_code(std::errc::not_supported);

        return {};
    }

    std::error_code version_check_post_r21(ioctl_iface_type iface_type) {
        assert(iface_type != ioctl_iface_type::jm_pre_r21);

        ioctl::kbase::version_check version_check_args = {0, 0};

        const ioctl::kbase::command::command_type command = iface_type == ioctl_iface_type::csf
                                                                ? ioctl::kbase::command::version_check_csf
                                                                : ioctl::kbase::command::version_check_jm;

        get_syscall_iface().ioctl(fd_, command, &version_check_args);

        if (!detail::is_version_set(version_check_args))
            return std::make_error_code(std::errc::not_supported);

        kbase_version_ = kbase_version_type(version_check_args.major, version_check_args.minor, iface_type);

        return {};
    }

    /** Detect kbase version and initialize kbase_version_ field. */
    std::error_code version_check() {
        if (!version_check_pre_r21())
            return {};

        if (!version_check_post_r21(ioctl_iface_type::jm_post_r21))
            return {};

        return version_check_post_r21(ioctl_iface_type::csf);
    }

    /** Detect backend interface type and initialize backend_type_ field. */
    std::error_code backend_type_probe() {
        std::error_code ec;

        product_id known_pid = get_product_id();

        auto available_types = hwcnt::backend_type_discover(kbase_version_, known_pid);
        std::tie(ec, backend_type_) = hwcnt::backend_type_select(available_types);
        return ec;
    }

    /**
     * Initialize `block_extents_` field.
     *
     * @param[in] iface Syscall iface.
     * @return Error code.
     */
    std::error_code init_block_extents(const syscall_iface_t &iface) {
        std::error_code ec;

        product_id pid = get_product_id();

        switch (backend_type_) {
        case hwcnt::backend_type::vinstr:
        case hwcnt::backend_type::vinstr_pre_r21: {
            block_extents_ = hwcnt::sampler::vinstr::construct_block_extents(pid, constants_.num_l2_slices,
                                                                             constants_.num_shader_cores);
            break;
        }
        case hwcnt::backend_type::kinstr_prfcnt:
        case hwcnt::backend_type::kinstr_prfcnt_wa:
        case hwcnt::backend_type::kinstr_prfcnt_bad:
            using namespace hwcnt::sampler::kinstr_prfcnt;
            std::tie(ec, ei_) = parse_enum_info(fd_, iface);
            if (ec)
                return ec;

            block_extents_ = construct_block_extents(ei_);
            break;
        }

        return ec;
    }

    /**
     * Update backend_type_ if needed.
     *
     * Some kbase versions (UK 1.18) that do not include fixes for
     * kinstr_prfcnt, have issues while creating sample reader. The fix is
     * to update backend_type_ to kinstr_prfcnt_wa if the reader could not be
     * created and current backend_type is kinstr_prfcnt.
     *
     * @return Error code if an unexpected error happened.
     */
    std::error_code backend_type_fixup() {
        if (backend_type_ != hwcnt::backend_type::kinstr_prfcnt)
            return {};

        /* Testing with core configuration to potentially trigger the core mapping bug. */
        const hwcnt::sampler::configuration config_core{
            hwcnt::block_type::core,
            ei_.set,
            0b1111,
        };
        hwcnt::sampler::manual sampler{*this, &config_core, 1};

        if (!sampler)
            return std::make_error_code(std::errc::invalid_argument);

        std::error_code ec;

        ec = sampler.accumulation_start();

        if (ec)
            return ec;

        /* This will trigger an implicit sample. */
        ec = sampler.accumulation_stop(0);

        if (ec)
            return ec;

        hwcnt::sample sample(sampler.get_reader(), ec);
        if (ec)
            backend_type_ = hwcnt::backend_type::kinstr_prfcnt_wa;

        return {};
    }

    /** Call set flags ioctl. */
    std::error_code set_flags() {
        static constexpr auto system_monitor_flag_submit_disabled_bit = 1;
        static constexpr auto system_monitor_flag = 1U << system_monitor_flag_submit_disabled_bit;

        std::error_code ec;

        if (kbase_version_.type() == ioctl_iface_type::jm_pre_r21) {
            ioctl::kbase_pre_r21::set_flags_args flags{};
            flags.header.id = ioctl::kbase_pre_r21::header_id::set_flags;
            flags.create_flags = system_monitor_flag;

            std::tie(ec, std::ignore) =
                get_syscall_iface().ioctl(fd_, ioctl::kbase_pre_r21::command::set_flags, &flags);

        } else {
            ioctl::kbase::set_flags flags{system_monitor_flag};
            std::tie(ec, std::ignore) = get_syscall_iface().ioctl(fd_, ioctl::kbase::command::set_flags, &flags);
        }

        static const std::error_code eperm = std::make_error_code(std::errc::operation_not_permitted);
        static const std::error_code einval = std::make_error_code(std::errc::invalid_argument);
        static const std::error_code efault = std::make_error_code(std::errc::bad_address);

        /* Set_flags may fail with eperm if context has been already initialized. */
        if (ec == eperm)
            ec = {};
        if (ec == einval)
            ec = {};
        /* And on old kernels it may return a misleading error. */
        if (ec == efault && kbase_version_.type() == ioctl_iface_type::jm_pre_r21)
            ec = {};

        return ec;
    }

    /** Initialize constants_ field. */
    std::error_code init_constants() {
        std::error_code ec;
        if (kbase_version_.type() == ioctl_iface_type::jm_pre_r21) {
            std::tie(constants_, pid_) = props_pre_r21(fd_, ec);
            return ec;
        }

        auto p = props_post_r21(fd_, ec);
        if (ec)
            return ec;

        std::tie(constants_, pid_) = detail::prop_decoder{p}(ec);
        if (ec)
            return ec;

        constants_.fw_version = get_fw_version();

        return ec;
    }

    /** Get a raw properties buffer into structured data. */
    std::error_code init(const syscall_iface_t &iface) {
        std::error_code ec = version_check();
        if (ec)
            return ec;

        ec = set_flags();
        if (ec)
            return ec;

        ec = init_constants();
        if (ec)
            return ec;

        ec = backend_type_probe();
        if (ec)
            return ec;

        ec = init_block_extents(iface);
        if (ec)
            return ec;

        ec = backend_type_fixup();
        if (ec)
            return ec;

        return {};
    }

    constants constants_{};
    hwcnt::block_extents block_extents_{};
    kbase_version_type kbase_version_{};
    hwcnt::backend_type backend_type_{};
    hwcnt::sampler::kinstr_prfcnt::enum_info ei_{};
    product_id pid_{};

    bool valid_{true};
    int fd_;
};

} // namespace device
} // namespace hwcpipe
