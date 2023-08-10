/*
 * Copyright (c) 2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#include "hwcpipe/counter_metadata.hpp"

namespace hwcpipe {
namespace database {

constexpr std::array<counter_metadata, 315> all_counter_metadata{
    counter_metadata{"GPU active cycles", "cycles"},
    counter_metadata{"GPU interrupt pending cycles", "cycles"},
    counter_metadata{"Fragment tasks", "tasks"},
    counter_metadata{"Fragment queue active cycles", "cycles"},
    counter_metadata{"Non-fragment queue active cycles", "cycles"},
    counter_metadata{"Output external write beats", "beats"},
    counter_metadata{"Output external read beats", "beats"},
    counter_metadata{"Output external read stall cycles", "cycles"},
    counter_metadata{"Output external write stall cycles", "cycles"},
    counter_metadata{"Fragment active cycles", "cycles"},
    counter_metadata{"Fragment threads", "threads"},
    counter_metadata{"Fragment helper threads", "threads"},
    counter_metadata{"Rasterized fine quads", "quads"},
    counter_metadata{"Early ZS tested quads", "quads"},
    counter_metadata{"Early ZS killed quads", "quads"},
    counter_metadata{"Late ZS tested threads", "threads"},
    counter_metadata{"Late ZS tested threads", "threads"},
    counter_metadata{"Tiles", "tiles"},
    counter_metadata{"Unchanged tiles killed", "tiles"},
    counter_metadata{"Non-fragment active cycles", "cycles"},
    counter_metadata{"Non-fragment threads", "threads"},
    counter_metadata{"Execution core active cycles", "cycles"},
    counter_metadata{"Executed instructions", "instructions"},
    counter_metadata{"Load/store total issues", "cycles"},
    counter_metadata{"Texture instructions", "instructions"},
    counter_metadata{"Texture filtering cycles", "cycles"},
    counter_metadata{"Load/store read hits", "cycles"},
    counter_metadata{"Load/store write hits", "cycles"},
    counter_metadata{"Triangle primitives", "primitives"},
    counter_metadata{"Point primitives", "primitives"},
    counter_metadata{"Line primitives", "primitives"},
    counter_metadata{"Visible front-facing primitives", "primitives"},
    counter_metadata{"Visible back-facing primitives", "primitives"},
    counter_metadata{"Visible primitives", "primitives"},
    counter_metadata{"Facing and XY plane test culled primitives", "primitives"},
    counter_metadata{"Z plane culled primitives", "primitives"},
    counter_metadata{"Tiler active cycles", "cycles"},
    counter_metadata{"Interrupt pending utilization", "percent"},
    counter_metadata{"Fragment queue utilization", "percent"},
    counter_metadata{"Non-fragment queue utilization", "percent"},
    counter_metadata{"Output external read bytes", "bytes"},
    counter_metadata{"Output external write bytes", "bytes"},
    counter_metadata{"Output external read stall rate", "percent"},
    counter_metadata{"Output external write stall rate", "percent"},
    counter_metadata{"Tiler utilization", "percent"},
    counter_metadata{"Total input primitives", "primitives"},
    counter_metadata{"Visible primitives rate", "percent"},
    counter_metadata{"Culled primitives", "primitives"},
    counter_metadata{"Facing or XY plane test cull rate", "percent"},
    counter_metadata{"Z plane test cull rate", "percent"},
    counter_metadata{"Non-fragment utilization", "percent"},
    counter_metadata{"Non-fragment cycles per thread", "cycles"},
    counter_metadata{"Fragment utilization", "percent"},
    counter_metadata{"Fragment cycles per thread", "cycles"},
    counter_metadata{"Helper thread rate", "percent"},
    counter_metadata{"Early ZS tested quad percentage", "percent"},
    counter_metadata{"Early ZS killed quad percentage", "percent"},
    counter_metadata{"Late ZS tested thread percentage", "percent"},
    counter_metadata{"Late ZS killed thread percentage", "percent"},
    counter_metadata{"Fragments per pixel", "threads"},
    counter_metadata{"Unchanged tile kill rate", "percent"},
    counter_metadata{"Execution core utilization", "percent"},
    counter_metadata{"Arithmetic unit utilization", "percent"},
    counter_metadata{"Texture samples", "issues"},
    counter_metadata{"Texture filtering cycles per instruction", "cycles"},
    counter_metadata{"Texture unit utilization", "percent"},
    counter_metadata{"Texture unit filtering utilization", "percent"},
    counter_metadata{"Load/store unit utilization", "percent"},
    counter_metadata{"Pixels", "pixels"},
    counter_metadata{"Cycles per pixel", "cycles"},
    counter_metadata{"MMU lookup requests", "requests"},
    counter_metadata{"Any lookup requests", "requests"},
    counter_metadata{"Read lookup requests", "requests"},
    counter_metadata{"Write lookup requests", "requests"},
    counter_metadata{"Forward pixel kill buffer active cycles", "cycles"},
    counter_metadata{"Load/store read issues", "cycles"},
    counter_metadata{"Load/store write issues", "cycles"},
    counter_metadata{"Load/store atomic issues", "cycles"},
    counter_metadata{"Position cache hit requests", "requests"},
    counter_metadata{"Position cache miss requests", "requests"},
    counter_metadata{"Fragment FPK buffer utilization", "percent"},
    counter_metadata{"Sample test culled primitives", "primitives"},
    counter_metadata{"Output internal read beats", "beats"},
    counter_metadata{"Internal write beats", "beats"},
    counter_metadata{"Tiler position shading requests", "requests"},
    counter_metadata{"Tiler position shading stall cycles", "cycles"},
    counter_metadata{"Tiler position FIFO full cycles", "cycles"},
    counter_metadata{"Varying cache hits", "requests"},
    counter_metadata{"Varying cache misses", "requests"},
    counter_metadata{"Tiler varying shading requests", "requests"},
    counter_metadata{"Tiler varying shading stall cycles", "cycles"},
    counter_metadata{"Rasterized primitives", "primitives"},
    counter_metadata{"Fragment warps", "warps"},
    counter_metadata{"Partial fragment warps", "warps"},
    counter_metadata{"Early ZS updated quads", "quads"},
    counter_metadata{"Late ZS tested quads", "quads"},
    counter_metadata{"Late ZS killed quads", "quads"},
    counter_metadata{"FPK occluder quads", "quads"},
    counter_metadata{"Non-fragment warps", "warps"},
    counter_metadata{"Diverged instructions", "instructions"},
    counter_metadata{"Execution engine starvation cycles", "cycles"},
    counter_metadata{"Texture quads", "quads"},
    counter_metadata{"Texture quad issues", "issues"},
    counter_metadata{"Texture quad descriptor misses", "requests"},
    counter_metadata{"Mipmapped texture quad issues", "issues"},
    counter_metadata{"Trilinear filtered texture quad issues", "issues"},
    counter_metadata{"Texture line fetch requests", "issues"},
    counter_metadata{"Compressed texture line fetch requests", "issues"},
    counter_metadata{"Texture cache lookup requests", "requests"},
    counter_metadata{"Load/store full read issues", "cycles"},
    counter_metadata{"Load/store partial read issues", "cycles"},
    counter_metadata{"Load/store full write issues", "cycles"},
    counter_metadata{"Load/store partial write issues", "cycles"},
    counter_metadata{"Varying instructions", "instructions"},
    counter_metadata{"32-bit interpolation slots", "issues"},
    counter_metadata{"16-bit interpolation slots", "issues"},
    counter_metadata{"Attribute instructions", "instructions"},
    counter_metadata{"Fragment front-end read beats from L2 cache", "beats"},
    counter_metadata{"Fragment front-end read beats from external memory", "beats"},
    counter_metadata{"Load/store read beats from L2 cache", "beats"},
    counter_metadata{"Load/store read beats from external memory", "beats"},
    counter_metadata{"Texture read beats from L2 cache", "beats"},
    counter_metadata{"Texture read beats from external memory", "beats"},
    counter_metadata{"Miscellaneous read beats from L2 cache", "beats"},
    counter_metadata{"Internal load/store write-back write beats", "beats"},
    counter_metadata{"Tile buffer write beats to L2 memory system", "beats"},
    counter_metadata{"Internal load/store other write beats", "beats"},
    counter_metadata{"Input internal read requests", "requests"},
    counter_metadata{"Input internal read stall cycles", "cycles"},
    counter_metadata{"Input internal write requests", "requests"},
    counter_metadata{"Input internal write stall cycles", "cycles"},
    counter_metadata{"Input internal snoop requests", "requests"},
    counter_metadata{"Input internal snoop stall cycles", "cycles"},
    counter_metadata{"Output internal read requests", "requests"},
    counter_metadata{"Output internal read stall cycles", "cycles"},
    counter_metadata{"Output internal write requests", "requests"},
    counter_metadata{"Input external snoop lookup requests", "requests"},
    counter_metadata{"Output external read transactions", "transactions"},
    counter_metadata{"Output external ReadNoSnoop transactions", "transactions"},
    counter_metadata{"Output external ReadUnique transactions", "transactions"},
    counter_metadata{"Output external outstanding reads 0-25%", "transactions"},
    counter_metadata{"Output external outstanding reads 25-50%", "transactions"},
    counter_metadata{"Output external outstanding reads 50-75%", "transactions"},
    counter_metadata{"Output external read latency 0-127 cycles", "beats"},
    counter_metadata{"Output external read latency 128-191 cycles", "beats"},
    counter_metadata{"Output external read latency 192-255 cycles", "beats"},
    counter_metadata{"Output external read latency 256-319 cycles", "beats"},
    counter_metadata{"Output external read latency 320-383 cycles", "beats"},
    counter_metadata{"Output external write transactions", "transactions"},
    counter_metadata{"Output external WriteNoSnoopFull transactions", "transactions"},
    counter_metadata{"Output external WriteNoSnoopPartial transactions", "transactions"},
    counter_metadata{"Output external WriteSnoopFull transactions", "transactions"},
    counter_metadata{"Output external WriteSnoopPartial transactions", "transactions"},
    counter_metadata{"Output external outstanding writes 0-25%", "transactions"},
    counter_metadata{"Output external outstanding writes 25-50%", "transactions"},
    counter_metadata{"Output external outstanding writes 50-75%", "transactions"},
    counter_metadata{"Input external snoop transactions", "transactions"},
    counter_metadata{"Input external snoop stall cycles", "cycles"},
    counter_metadata{"L2 cache read miss rate", "percent"},
    counter_metadata{"L2 cache write miss rate", "percent"},
    counter_metadata{"Output external read latency 384+ cycles", "beats"},
    counter_metadata{"Output external outstanding reads 75-100%", "transactions"},
    counter_metadata{"Output external outstanding writes 75-100%", "transactions"},
    counter_metadata{"Sample test cull rate", "percent"},
    counter_metadata{"Position shader thread invocations", "threads"},
    counter_metadata{"Position threads per input primitive", "threads"},
    counter_metadata{"Position cache hit rate", "percent"},
    counter_metadata{"Varying shader thread invocations", "threads"},
    counter_metadata{"Varying threads per input primitive", "threads"},
    counter_metadata{"Varying cache hit rate", "percent"},
    counter_metadata{"FPK occluder quad percentage", "percent"},
    counter_metadata{"FPK non-occluder quads", "quads"},
    counter_metadata{"Fragment quads shaded", "quads"},
    counter_metadata{"Partial coverage rate", "percent"},
    counter_metadata{"Early ZS updated quad percentage", "percent"},
    counter_metadata{"FPK killed quads", "quads"},
    counter_metadata{"FPK killed quad percentage", "percent"},
    counter_metadata{"Diverged instruction issue rate", "percent"},
    counter_metadata{"Texture unit cache utilization", "percent"},
    counter_metadata{"Texture accesses using mipmapping percentage", "percent"},
    counter_metadata{"Texture data fetches from compressed lines", "percent"},
    counter_metadata{"Texture accesses using trilinear filter percentage", "percent"},
    counter_metadata{"32-bit interpolation cycles", "cycles"},
    counter_metadata{"16-bit interpolation cycles", "cycles"},
    counter_metadata{"Varying cycles", "cycles"},
    counter_metadata{"Varying unit utilization", "percent"},
    counter_metadata{"Front-end read bytes from L2 cache", "bytes"},
    counter_metadata{"Front-end read bytes from external memory", "bytes"},
    counter_metadata{"Load/store read bytes from L2 cache", "bytes"},
    counter_metadata{"Load/store bytes read from L2 per access cycle", "bytes"},
    counter_metadata{"Load/store read bytes from external memory", "bytes"},
    counter_metadata{"Load/store bytes read from external memory per access cycle", "bytes"},
    counter_metadata{"Texture read bytes from L2 cache", "bytes"},
    counter_metadata{"Texture bytes read from L2 per texture cycle", "bytes"},
    counter_metadata{"Texture read bytes from external memory", "bytes"},
    counter_metadata{"Texture bytes read from external memory per texture cycle", "bytes"},
    counter_metadata{"Load/store write beats to L2 memory system", "beats"},
    counter_metadata{"Load/store write bytes", "bytes"},
    counter_metadata{"Load/store bytes written to L2 per access cycle", "bytes"},
    counter_metadata{"Tile buffer write bytes", "bytes"},
    counter_metadata{"Warps using more than 32 registers", "warps"},
    counter_metadata{"Full quad warps", "warps"},
    counter_metadata{"All registers warp rate", "percent"},
    counter_metadata{"Full quad warp rate", "percent"},
    counter_metadata{"Mipmapped texture instructions", "instructions"},
    counter_metadata{"Compressed texture instructions", "instructions"},
    counter_metadata{"3D texture instructions", "instructions"},
    counter_metadata{"Trilinear filtered texture instructions", "instructions"},
    counter_metadata{"Texture filtering coordinate stall cycles", "cycles"},
    counter_metadata{"Texture line fill stall cycles", "cycles"},
    counter_metadata{"Texture filtering partial data stall cycles", "cycles"},
    counter_metadata{"Miscellaneous write beats to L2 memory system", "beats"},
    counter_metadata{"Texture accesses using compressed texture percentage", "percent"},
    counter_metadata{"Texture accesses using 3D texture percentage", "percent"},
    counter_metadata{"Partial rasterized fine quads", "quads"},
    counter_metadata{"Arithmetic FMA instructions", "instructions"},
    counter_metadata{"Arithmetic CVT instructions", "instructions"},
    counter_metadata{"Arithmetic SFU instructions", "instructions"},
    counter_metadata{"Instruction cache misses", "requests"},
    counter_metadata{"Blend shader instructions", "instructions"},
    counter_metadata{"Texture message read beats", "beats"},
    counter_metadata{"Texture descriptor stalls", "cycles"},
    counter_metadata{"Texture fetch stalls", "cycles"},
    counter_metadata{"Texture filtering stalls", "cycles"},
    counter_metadata{"Texture filtering cycles using full bilinear", "cycles"},
    counter_metadata{"Texture filtering cycles using full trilinear", "cycles"},
    counter_metadata{"Texture messages", "issues"},
    counter_metadata{"Texture message write beats", "beats"},
    counter_metadata{"Partial coverage rate", "percent"},
    counter_metadata{"FMA pipe utilization", "percent"},
    counter_metadata{"CVT pipe utilization", "percent"},
    counter_metadata{"SFU pipe utilization", "percent"},
    counter_metadata{"Arithmetic instruction issues", "instructions"},
    counter_metadata{"Shader blend path percentage", "percent"},
    counter_metadata{"Texture input bus utilization", "percent"},
    counter_metadata{"Texture output bus utilization", "percent"},
    counter_metadata{"Texture full speed filtering percentage", "percent"},
    counter_metadata{"Any active cycles", "cycles"},
    counter_metadata{"Shader core usage", "percent"},
    counter_metadata{"MCU active cycles", "cycles"},
    counter_metadata{"Any iterator active cycles", "cycles"},
    counter_metadata{"Vertex iterator queued", "cycles"},
    counter_metadata{"Total vertex active cycles", "cycles"},
    counter_metadata{"Vertex endpoint stall cycles", "cycles"},
    counter_metadata{"Compute iterator queued", "cycles"},
    counter_metadata{"Total compute active cycles", "cycles"},
    counter_metadata{"Compute endpoint stall cycles", "cycles"},
    counter_metadata{"Fragment iterator queued", "cycles"},
    counter_metadata{"Total fragment active cycles", "cycles"},
    counter_metadata{"Fragment endpoint stall cycles", "cycles"},
    counter_metadata{"Command execution unit active", "cycles"},
    counter_metadata{"Command load/store unit active", "cycles"},
    counter_metadata{"Command stream 0 active", "cycles"},
    counter_metadata{"Command stream 1 active", "cycles"},
    counter_metadata{"Command stream 2 active", "cycles"},
    counter_metadata{"Command stream 3 active", "cycles"},
    counter_metadata{"Input internal evict requests", "requests"},
    counter_metadata{"Input internal clean unique requests", "requests"},
    counter_metadata{"Fragment iterator issue cycles", "cycles"},
    counter_metadata{"Fragment iterator utilization", "percent"},
    counter_metadata{"Vertex iterator issue cycles", "cycles"},
    counter_metadata{"Vertex iterator utilization", "percent"},
    counter_metadata{"Compute iterator issue cycles", "cycles"},
    counter_metadata{"Compute iterator utilization", "percent"},
    counter_metadata{"Microcontroller utilization", "percent"},
    counter_metadata{"Command load/store unit utilization", "percent"},
    counter_metadata{"Command execution unit utilization", "percent"},
    counter_metadata{"Narrow arithmetic instructions", "instructions"},
    counter_metadata{"Rasterized coarse quads", "quads"},
    counter_metadata{"Ray tracing triangle batches tested", "batches"},
    counter_metadata{"Ray tracing box nodes tested", "boxes"},
    counter_metadata{"Ray tracing triangle batches tested with 1-4 rays", "batches"},
    counter_metadata{"Ray tracing triangle batches tested with 5-8 rays", "batches"},
    counter_metadata{"Ray tracing triangle batches tested with 9-12 rays", "batches"},
    counter_metadata{"Ray tracing triangle batches tested with 13-16 rays", "batches"},
    counter_metadata{"Ray tracing box nodes tested with 1-4 rays", "nodes"},
    counter_metadata{"Ray tracing box nodes tested with 5-8 rays", "nodes"},
    counter_metadata{"Ray tracing box nodes tested with 9-12 rays", "nodes"},
    counter_metadata{"Ray tracing box nodes tested with 13-16 rays", "nodes"},
    counter_metadata{"Ray tracing opaque triangle hits", "tests"},
    counter_metadata{"Ray tracing non-opaque triangle hits", "tests"},
    counter_metadata{"Ray tracing first hit terminations", "rays"},
    counter_metadata{"Ray tracing miss", "rays"},
    counter_metadata{"Ray tracing rays started", "rays"},
    counter_metadata{"Ray tracing box tester issue cycles", "issues"},
    counter_metadata{"Ray tracing triangle tester issue cycles", "issues"},
    counter_metadata{"Facing test culled primitives", "primitives"},
    counter_metadata{"Frustum test culled primitives", "primitives"},
    counter_metadata{"Facing plane test cull rate", "percent"},
    counter_metadata{"Frustum test cull rate", "percent"},
    counter_metadata{"Fragment shading rate", "percent"},
    counter_metadata{"Fragment warp occupancy", "percent"},
    counter_metadata{"Narrow arithmetic percentage", "percent"},
    counter_metadata{"Ray tracing unit utilization", "percent"},
    counter_metadata{"Binning phase iterator queued", "cycles"},
    counter_metadata{"Binning phase endpoint stall cycles", "cycles"},
    counter_metadata{"Main phase iterator queued", "cycles"},
    counter_metadata{"Main phase endpoint stall cycles", "cycles"},
    counter_metadata{"L1 texture cache load cycles", "cycles"},
    counter_metadata{"Simple texture load cycles", "cycles"},
    counter_metadata{"L1 texture cache output cycles", "cycles"},
    counter_metadata{"L1 texture cache lookup cycles", "cycles"},
    counter_metadata{"Texture messages with single quad", "issues"},
    counter_metadata{"Texture cache lookup cycles", "cycles"},
    counter_metadata{"Complex texture load cycles", "cycles"},
    counter_metadata{"Texture index calculation cycles", "cycles"},
    counter_metadata{"Texture causing starvation cycles", "cycles"},
    counter_metadata{"Texture active cycles", "cycles"},
    counter_metadata{"Scissor test culled primitives", "primitives"},
    counter_metadata{"Visible primitives using DVS", "primitives"},
    counter_metadata{"Main phase iterator issue cycles", "cycles"},
    counter_metadata{"Main phase iterator utilization", "percent"},
    counter_metadata{"Binning phase iterator issue cycles", "cycles"},
    counter_metadata{"Binning phase iterator utilization", "percent"},
    counter_metadata{"Scissor test cull rate", "percent"}};

} // namespace database
} // namespace hwcpipe