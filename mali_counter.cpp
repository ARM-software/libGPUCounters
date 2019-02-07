/*
 * Copyright (c) 2017-2019 ARM Limited.
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
#include "mali_counter.h"

namespace vkb
{
namespace
{
	struct MaliHWInfo
	{
		unsigned mp_count;
		unsigned gpu_id;
		unsigned r_value;
		unsigned p_value;
		unsigned core_mask;
		unsigned l2_slices;
	};

	MaliHWInfo get_mali_hw_info(const char *path)
	{
		int fd = open(path, O_RDWR);        // NOLINT

		if (fd < 0)
		{
			throw std::runtime_error("Failed to get HW info.");
		}

		{
			mali_userspace::kbase_uk_hwcnt_reader_version_check_args version_check_args;
			version_check_args.header.id = mali_userspace::UKP_FUNC_ID_CHECK_VERSION;        // NOLINT
			version_check_args.major     = 10;
			version_check_args.minor     = 2;

			if (mali_userspace::mali_ioctl(fd, version_check_args) != 0)
			{
				mali_userspace::kbase_ioctl_version_check _version_check_args = { 0, 0 };
				if (ioctl(fd, KBASE_IOCTL_VERSION_CHECK, &_version_check_args) < 0)
				{
					close(fd);
					throw std::runtime_error("Failed to check version.");
				}
			}
		}

		{
			mali_userspace::kbase_uk_hwcnt_reader_set_flags flags;        // NOLINT
			memset(&flags, 0, sizeof(flags));
			flags.header.id    = mali_userspace::KBASE_FUNC_SET_FLAGS;        // NOLINT
			flags.create_flags = mali_userspace::BASE_CONTEXT_CREATE_KERNEL_FLAGS;

			if (mali_userspace::mali_ioctl(fd, flags) != 0)
			{
				mali_userspace::kbase_ioctl_set_flags _flags = {1u << 1};
				if (ioctl(fd, KBASE_IOCTL_SET_FLAGS, &_flags) < 0)
				{
					close(fd);
					throw std::runtime_error("Failed settings flags ioctl.");
				}
			}
		}

		{
			MaliHWInfo hw_info;        // NOLINT
			memset(&hw_info, 0, sizeof(hw_info));
			mali_userspace::kbase_uk_gpuprops props = {};
			props.header.id                         = mali_userspace::KBASE_FUNC_GPU_PROPS_REG_DUMP;
			if (mali_ioctl(fd, props) == 0)
			{
				hw_info.gpu_id  = props.props.core_props.product_id;
				hw_info.r_value = props.props.core_props.major_revision;
				hw_info.p_value = props.props.core_props.minor_revision;
				for (uint32_t i = 0; i < props.props.coherency_info.num_core_groups; i++)
					hw_info.core_mask |= props.props.coherency_info.group[i].core_mask;
				hw_info.mp_count  = __builtin_popcountll(hw_info.core_mask);
				hw_info.l2_slices = props.props.l2_props.num_l2_slices;

				close(fd);
			}
			else
			{
				mali_userspace::kbase_ioctl_get_gpuprops get_props = {};
				int                                      ret;
				if ((ret = ioctl(fd, KBASE_IOCTL_GET_GPUPROPS, &get_props)) < 0)
				{
					throw std::runtime_error("Failed getting GPU properties.");
					close(fd);
				}

				get_props.size = ret;
				std::vector<uint8_t> buffer(ret);
				get_props.buffer.value = buffer.data();
				ret                    = ioctl(fd, KBASE_IOCTL_GET_GPUPROPS, &get_props);
				if (ret < 0)
				{
					throw std::runtime_error("Failed getting GPU properties.");
					close(fd);
				}

#define READ_U8(p) ((p)[0])
#define READ_U16(p) (READ_U8((p)) | (uint16_t(READ_U8((p) + 1)) << 8))
#define READ_U32(p) (READ_U16((p)) | (uint32_t(READ_U16((p) + 2)) << 16))
#define READ_U64(p) (READ_U32((p)) | (uint64_t(READ_U32((p) + 4)) << 32))

				mali_userspace::gpu_props props = {};

				const auto *ptr  = buffer.data();
				int         size = ret;
				while (size > 0)
				{
					uint32_t type       = READ_U32(ptr);
					uint32_t value_type = type & 3;
					uint64_t value;

					ptr += 4;
					size -= 4;

					switch (value_type)
					{
						case KBASE_GPUPROP_VALUE_SIZE_U8:
							value = READ_U8(ptr);
							ptr++;
							size--;
							break;
						case KBASE_GPUPROP_VALUE_SIZE_U16:
							value = READ_U16(ptr);
							ptr += 2;
							size -= 2;
							break;
						case KBASE_GPUPROP_VALUE_SIZE_U32:
							value = READ_U32(ptr);
							ptr += 4;
							size -= 4;
							break;
						case KBASE_GPUPROP_VALUE_SIZE_U64:
							value = READ_U64(ptr);
							ptr += 8;
							size -= 8;
							break;
					}

					for (unsigned i = 0; mali_userspace::gpu_property_mapping[i].type; i++)
					{
						if (mali_userspace::gpu_property_mapping[i].type == (type >> 2))
						{
							auto  offset = mali_userspace::gpu_property_mapping[i].offset;
							void *p      = reinterpret_cast<uint8_t *>(&props) + offset;
							switch (mali_userspace::gpu_property_mapping[i].size)
							{
								case 1:
									*reinterpret_cast<uint8_t *>(p) = value;
									break;
								case 2:
									*reinterpret_cast<uint16_t *>(p) = value;
									break;
								case 4:
									*reinterpret_cast<uint32_t *>(p) = value;
									break;
								case 8:
									*reinterpret_cast<uint64_t *>(p) = value;
									break;
								default:
									throw std::runtime_error("Invalid property size.");
									close(fd);
							}
							break;
						}
					}
				}

				hw_info.gpu_id  = props.product_id;
				hw_info.r_value = props.major_revision;
				hw_info.p_value = props.minor_revision;
				for (uint32_t i = 0; i < props.num_core_groups; i++)
					hw_info.core_mask |= props.core_mask[i];
				hw_info.mp_count = __builtin_popcountll(hw_info.core_mask);
				//hw_info.l2_slices = props.l2_slices;

				close(fd);
			}

			return hw_info;
		}
	}
}        // namespace

MaliCounter::MaliCounter()
{
	_counters =
	    {
	        {"GPU_ACTIVE", Measurement(0, "cycles")},
	        {"JS0_JOBS", Measurement(0, "jobs")},
	        {"JS1_JOBS", Measurement(0, "jobs")},
	        {"L2_READ_LOOKUP", Measurement(0, "cache lookups")},
	        {"L2_EXT_READ", Measurement(0, "transactions")},
	        {"L2_EXT_AR_STALL", Measurement(0, "stall cycles")},
	        {"L2_WRITE_LOOKUP", Measurement(0, "cache lookups")},
	        {"L2_EXT_WRITE", Measurement(0, "transactions")},
	        {"L2_EXT_W_STALL", Measurement(0, "stall cycles")},
	    };

	init();
}

MaliCounter::~MaliCounter()
{
	term();
}

void MaliCounter::init()
{
	term();

	MaliHWInfo hw_info = get_mali_hw_info(_device);

	_num_cores = hw_info.mp_count;

	_fd = open(_device, O_RDWR | O_CLOEXEC | O_NONBLOCK);        // NOLINT

	if (_fd < 0)
	{
		throw std::runtime_error("Failed to open /dev/mali0.");
	}

	{
		mali_userspace::kbase_uk_hwcnt_reader_version_check_args check;        // NOLINT
		memset(&check, 0, sizeof(check));

		if (mali_userspace::mali_ioctl(_fd, check) != 0)
		{
			mali_userspace::kbase_ioctl_version_check _check = { 0, 0 };
			if (ioctl(_fd, KBASE_IOCTL_VERSION_CHECK, &_check) < 0)
			{
				throw std::runtime_error("Failed to get ABI version.");
			}
		}
		else if (check.major < 10)
		{
			throw std::runtime_error("Unsupported ABI version 10.");
		}
	}

	{
		mali_userspace::kbase_uk_hwcnt_reader_set_flags flags;        // NOLINT
		memset(&flags, 0, sizeof(flags));
		flags.header.id    = mali_userspace::KBASE_FUNC_SET_FLAGS;        // NOLINT
		flags.create_flags = mali_userspace::BASE_CONTEXT_CREATE_KERNEL_FLAGS;

		if (mali_userspace::mali_ioctl(_fd, flags) != 0)
		{
			mali_userspace::kbase_ioctl_set_flags _flags = {1u << 1};
			if (ioctl(_fd, KBASE_IOCTL_SET_FLAGS, &_flags) < 0)
			{
				throw std::runtime_error("Failed settings flags ioctl.");
			}
		}
	}

	{
		mali_userspace::kbase_uk_hwcnt_reader_setup setup;        // NOLINT
		memset(&setup, 0, sizeof(setup));
		setup.header.id    = mali_userspace::KBASE_FUNC_HWCNT_READER_SETUP;        // NOLINT
		setup.buffer_count = _buffer_count;
		setup.jm_bm        = -1;
		setup.shader_bm    = -1;
		setup.tiler_bm     = -1;
		setup.mmu_l2_bm    = -1;
		setup.fd           = -1;

		if (mali_userspace::mali_ioctl(_fd, setup) != 0)
		{
			mali_userspace::kbase_ioctl_hwcnt_reader_setup _setup = {};
			_setup.buffer_count = _buffer_count;
			_setup.jm_bm = -1;
			_setup.shader_bm = -1;
			_setup.tiler_bm = -1;
			_setup.mmu_l2_bm = -1;

			int ret;
			if ((ret = ioctl(_fd, KBASE_IOCTL_HWCNT_READER_SETUP, &_setup)) < 0)
			{
				throw std::runtime_error("Failed setting hwcnt reader ioctl.");
			}
			_hwc_fd = ret;
		}
		else
		{
			_hwc_fd = setup.fd;
		}
	}

	{
		uint32_t api_version = ~mali_userspace::HWCNT_READER_API;

		if (ioctl(_hwc_fd, mali_userspace::KBASE_HWCNT_READER_GET_API_VERSION, &api_version) != 0)        // NOLINT
		{
			throw std::runtime_error("Could not determine hwcnt reader API.");
		}
		else if (api_version != mali_userspace::HWCNT_READER_API)
		{
			throw std::runtime_error("Invalid API version.");
		}
	}

	if (ioctl(_hwc_fd, static_cast<int>(mali_userspace::KBASE_HWCNT_READER_GET_BUFFER_SIZE), &_buffer_size) != 0)        // NOLINT
	{
		throw std::runtime_error("Failed to get buffer size.");
	}

	if (ioctl(_hwc_fd, static_cast<int>(mali_userspace::KBASE_HWCNT_READER_GET_HWVER), &_hw_ver) != 0)        // NOLINT
	{
		throw std::runtime_error("Could not determine HW version.");
	}

	if (_hw_ver < 5)
	{
		throw std::runtime_error("Unsupported HW version.");
	}

	_sample_data = static_cast<uint8_t *>(mmap(nullptr, _buffer_count * _buffer_size, PROT_READ, MAP_PRIVATE, _hwc_fd, 0));

	if (_sample_data == MAP_FAILED)        // NOLINT
	{
		throw std::runtime_error("Failed to map sample data.");
	}

	auto product = std::find_if(std::begin(mali_userspace::products), std::end(mali_userspace::products), [&](const mali_userspace::CounterMapping &cm) {
		return (cm.product_mask & hw_info.gpu_id) == cm.product_id;
	});

	if (product != std::end(mali_userspace::products))
	{
		_names_lut = product->names_lut;
	}
	else
	{
		throw std::runtime_error("Could not identify GPU.");
	}

	_raw_counter_buffer.resize(_buffer_size / sizeof(uint32_t));

	// Build core remap table.
	_core_index_remap.clear();
	_core_index_remap.reserve(hw_info.mp_count);

	unsigned int mask = hw_info.core_mask;

	while (mask != 0)
	{
		unsigned int bit = __builtin_ctz(mask);
		_core_index_remap.push_back(bit);
		mask &= ~(1u << bit);
	}
}

void MaliCounter::term()
{
	if (_sample_data != nullptr)
	{
		munmap(_sample_data, _buffer_count * _buffer_size);
		_sample_data = nullptr;
	}

	if (_hwc_fd >= 0)
	{
		close(_hwc_fd);
		_hwc_fd = -1;
	}

	if (_fd >= 0)
	{
		close(_fd);
		_fd = -1;
	}
}

void MaliCounter::sample_counters()
{
	if (ioctl(_hwc_fd, mali_userspace::KBASE_HWCNT_READER_DUMP, 0) != 0)
	{
		throw std::runtime_error("Could not sample hardware counters.");
	}
}

void MaliCounter::wait_next_event()
{
	pollfd poll_fd;        // NOLINT
	poll_fd.fd     = _hwc_fd;
	poll_fd.events = POLLIN;

	const int count = poll(&poll_fd, 1, -1);

	if (count < 0)
	{
		throw std::runtime_error("poll() failed.");
	}

	if ((poll_fd.revents & POLLIN) != 0)
	{
		mali_userspace::kbase_hwcnt_reader_metadata meta;        // NOLINT

		if (ioctl(_hwc_fd, static_cast<int>(mali_userspace::KBASE_HWCNT_READER_GET_BUFFER), &meta) != 0)        // NOLINT
		{
			throw std::runtime_error("Failed READER_GET_BUFFER.");
		}

		memcpy(_raw_counter_buffer.data(), _sample_data + _buffer_size * meta.buffer_idx, _buffer_size);
		_timestamp = meta.timestamp;

		if (ioctl(_hwc_fd, mali_userspace::KBASE_HWCNT_READER_PUT_BUFFER, &meta) != 0)        // NOLINT
		{
			throw std::runtime_error("Failed READER_PUT_BUFFER.");
		}
	}
	else if ((poll_fd.revents & POLLHUP) != 0)
	{
		throw std::runtime_error("HWC hung up.");
	}
}

const uint32_t *MaliCounter::get_counters() const
{
	return _raw_counter_buffer.data();
}

const uint32_t *MaliCounter::get_counters(mali_userspace::MaliCounterBlockName block, int core) const
{
	switch (block)
	{
		case mali_userspace::MALI_NAME_BLOCK_JM:
			return _raw_counter_buffer.data() + mali_userspace::MALI_NAME_BLOCK_SIZE * 0;
		case mali_userspace::MALI_NAME_BLOCK_MMU:
			return _raw_counter_buffer.data() + mali_userspace::MALI_NAME_BLOCK_SIZE * 2;
		case mali_userspace::MALI_NAME_BLOCK_TILER:
			return _raw_counter_buffer.data() + mali_userspace::MALI_NAME_BLOCK_SIZE * 1;
		default:
			if (core < 0)
			{
				throw std::runtime_error("Invalid core number.");
			}

			return _raw_counter_buffer.data() + mali_userspace::MALI_NAME_BLOCK_SIZE * (3 + _core_index_remap[core]);
	}
}

int MaliCounter::find_counter_index_by_name(mali_userspace::MaliCounterBlockName block, const char *name)
{
	const char *const *names = &_names_lut[mali_userspace::MALI_NAME_BLOCK_SIZE * block];

	for (int i = 0; i < mali_userspace::MALI_NAME_BLOCK_SIZE; ++i)
	{
		if (strstr(names[i], name) != nullptr)
		{
			return i;
		}
	}

	return -1;
}

void MaliCounter::start()
{
	sample_counters();
	wait_next_event();
	_start_time = _timestamp;
}

void MaliCounter::stop()
{
	sample_counters();
	wait_next_event();

	const uint32_t *jm_counter = get_counters(mali_userspace::MALI_NAME_BLOCK_JM);
	_counters.at("GPU_ACTIVE") = Measurement(jm_counter[find_counter_index_by_name(mali_userspace::MALI_NAME_BLOCK_JM, "GPU_ACTIVE")], _counters.at("GPU_ACTIVE").unit());
	_counters.at("JS0_JOBS")   = Measurement(jm_counter[find_counter_index_by_name(mali_userspace::MALI_NAME_BLOCK_JM, "JS0_JOBS")], _counters.at("JS0_JOBS").unit());
	_counters.at("JS1_JOBS")   = Measurement(jm_counter[find_counter_index_by_name(mali_userspace::MALI_NAME_BLOCK_JM, "JS1_JOBS")], _counters.at("JS1_JOBS").unit());

	const uint32_t *mmu_counter     = get_counters(mali_userspace::MALI_NAME_BLOCK_MMU);
	_counters.at("L2_READ_LOOKUP")  = Measurement(mmu_counter[find_counter_index_by_name(mali_userspace::MALI_NAME_BLOCK_MMU, "L2_READ_LOOKUP")], _counters.at("L2_READ_LOOKUP").unit());
	_counters.at("L2_EXT_READ")     = Measurement(mmu_counter[find_counter_index_by_name(mali_userspace::MALI_NAME_BLOCK_MMU, "L2_EXT_READ")], _counters.at("L2_EXT_READ").unit());
	_counters.at("L2_EXT_AR_STALL") = Measurement(mmu_counter[find_counter_index_by_name(mali_userspace::MALI_NAME_BLOCK_MMU, "L2_EXT_AR_STALL")], _counters.at("L2_EXT_AR_STALL").unit());
	_counters.at("L2_WRITE_LOOKUP") = Measurement(mmu_counter[find_counter_index_by_name(mali_userspace::MALI_NAME_BLOCK_MMU, "L2_WRITE_LOOKUP")], _counters.at("L2_WRITE_LOOKUP").unit());
	_counters.at("L2_EXT_WRITE")    = Measurement(mmu_counter[find_counter_index_by_name(mali_userspace::MALI_NAME_BLOCK_MMU, "L2_EXT_WRITE")], _counters.at("L2_EXT_WRITE").unit());
	_counters.at("L2_EXT_W_STALL")  = Measurement(mmu_counter[find_counter_index_by_name(mali_userspace::MALI_NAME_BLOCK_MMU, "L2_EXT_W_STALL")], _counters.at("L2_EXT_W_STALL").unit());

	_stop_time = _timestamp;
}

std::string MaliCounter::id() const
{
	return "Mali Counter";
}

Instrument::MeasurementsMap MaliCounter::measurements() const
{
	MeasurementsMap measurements{
	    {"Timespan", Measurement(_stop_time - _start_time, "ns")},
	    {"GPU cycles", _counters.at("GPU_ACTIVE")},
	    {"Fragment jobs", _counters.at("JS0_JOBS")},
	    {"Vertex/compute jobs", _counters.at("JS1_JOBS")},
	    {"L2 cache read lookups", _counters.at("L2_READ_LOOKUP")},
	    {"L2 cache external reads", _counters.at("L2_EXT_READ")},
	    {"L2 cache external read stalls", _counters.at("L2_EXT_AR_STALL")},
	    {"L2 cache write lookups", _counters.at("L2_WRITE_LOOKUP")},
	    {"L2 cache external writes", _counters.at("L2_EXT_WRITE")},
	    {"L2 cache external write stalls", _counters.at("L2_EXT_W_STALL")},
	};

	return measurements;
}
}        // namespace vkb
