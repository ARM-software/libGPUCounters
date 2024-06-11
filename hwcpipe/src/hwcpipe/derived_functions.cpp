/*
 * Copyright (c) 2023-2024 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#include "hwcpipe/derived_functions.hpp"

#include <algorithm>

namespace hwcpipe {
namespace database {

using hwcpipe_double = detail::hwcpipe_double;

double MaliGPUIRQUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGPUIRQActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliGPUIRQUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGPUIRQActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUQueueActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliFragQueueUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragQueueActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliFragQueueUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragQueuedCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragQueueAssignStallCy);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double result = ((v0 - v1) / v2) * 100;
    return result;
}
double MaliFragQueueUtil_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragQueuedCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragQueueAssignStallCy);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGPUQueueActiveCy);
    hwcpipe_double result = ((v0 - v1) / v2) * 100;
    return result;
}
double MaliNonFragQueueUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliNonFragQueueActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliExtBusRdBy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliExtBusRdBt);
    hwcpipe_double result = v0 * ctx.get_mali_config_ext_bus_byte_size();
    return result;
}
double MaliExtBusWrBy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliExtBusWrBt);
    hwcpipe_double result = v0 * ctx.get_mali_config_ext_bus_byte_size();
    return result;
}
double MaliExtBusRdStallRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliExtBusRdStallCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double result = (v0 / ctx.get_mali_config_l2_cache_count() / v1) * 100;
    return result;
}
double MaliExtBusRdStallRate_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliExtBusRdStallCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUQueueActiveCy);
    hwcpipe_double result = (v0 / ctx.get_mali_config_l2_cache_count() / v1) * 100;
    return result;
}
double MaliExtBusWrStallRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliExtBusWrStallCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double result = (v0 / ctx.get_mali_config_l2_cache_count() / v1) * 100;
    return result;
}
double MaliExtBusWrStallRate_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliExtBusWrStallCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUQueueActiveCy);
    hwcpipe_double result = (v0 / ctx.get_mali_config_l2_cache_count() / v1) * 100;
    return result;
}
double MaliTilerUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTilerActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliTilerUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTilerActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUQueueActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliGeomTotalPrim_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomFaceXYPlaneCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomZPlaneCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = v0 + v1 + v2;
    return result;
}
double MaliGeomTotalPrim_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomFaceXYPlaneCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomZPlaneCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = v0 + v1 + v2 + v3;
    return result;
}
double MaliGeomTotalPrim_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomFaceCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomPlaneCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = v0 + v1 + v2 + v3;
    return result;
}
double MaliGeomTotalPrim_v3(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomFaceCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomPlaneCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomScissorCullPrim);
    hwcpipe_double v4 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = v0 + v1 + v2 + v3 + v4;
    return result;
}
double MaliGeomVisibleRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomFaceXYPlaneCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomZPlaneCullPrim);
    hwcpipe_double result = (v0 / (v1 + v2 + v0)) * 100;
    return result;
}
double MaliGeomVisibleRate_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomFaceXYPlaneCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomZPlaneCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double result = (v0 / (v1 + v2 + v3 + v0)) * 100;
    return result;
}
double MaliGeomVisibleRate_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomFaceCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomPlaneCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double result = (v0 / (v1 + v2 + v3 + v0)) * 100;
    return result;
}
double MaliGeomVisibleRate_v3(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomFaceCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomPlaneCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v4 = ctx.get_counter_value(MaliGeomScissorCullPrim);
    hwcpipe_double result = (v0 / (v1 + v2 + v3 + v4 + v0)) * 100;
    return result;
}
double MaliGeomTotalCullPrim_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomFaceXYPlaneCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomZPlaneCullPrim);
    hwcpipe_double result = v0 + v1;
    return result;
}
double MaliGeomTotalCullPrim_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomFaceXYPlaneCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomZPlaneCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double result = v0 + v1 + v2;
    return result;
}
double MaliGeomTotalCullPrim_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomFaceCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomPlaneCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double result = v0 + v1 + v2;
    return result;
}
double MaliGeomTotalCullPrim_v3(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomFaceCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomPlaneCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomScissorCullPrim);
    hwcpipe_double result = v0 + v1 + v2 + v3;
    return result;
}
double MaliGeomFaceXYPlaneCullRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomFaceXYPlaneCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomZPlaneCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = (v0 / (v0 + v1 + v2)) * 100;
    return result;
}
double MaliGeomFaceXYPlaneCullRate_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomFaceXYPlaneCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomZPlaneCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = (v0 / (v0 + v1 + v2 + v3)) * 100;
    return result;
}
double MaliGeomZPlaneCullRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomZPlaneCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomFaceXYPlaneCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = (v0 / ((v1 + v0 + v2) - v1)) * 100;
    return result;
}
double MaliGeomZPlaneCullRate_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomZPlaneCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomFaceXYPlaneCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = (v0 / ((v1 + v0 + v2 + v3) - v1)) * 100;
    return result;
}
double MaliNonFragUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliNonFragActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double result = (v0 / ctx.get_mali_config_shader_core_count() / v1) * 100;
    return result;
}
double MaliNonFragUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliNonFragActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliAnyActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliNonFragThroughputCy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliNonFragActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliNonFragThread);
    hwcpipe_double result = v0 / v1;
    return result;
}
double MaliNonFragThroughputCy_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliNonFragActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliNonFragWarp);
    hwcpipe_double result = v0 / (v1 * 4);
    return result;
}
double MaliNonFragThroughputCy_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliNonFragActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliNonFragWarp);
    hwcpipe_double result = v0 / (v1 * 8);
    return result;
}
double MaliNonFragThroughputCy_v3(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliNonFragActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliNonFragWarp);
    hwcpipe_double result = v0 / (v1 * 16);
    return result;
}
double MaliNonFragThroughputCy_v4(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliCompOrBinningActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliNonFragWarp);
    hwcpipe_double result = v0 / (v1 * 16);
    return result;
}
double MaliFragUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double result = (v0 / ctx.get_mali_config_shader_core_count() / v1) * 100;
    return result;
}
double MaliFragUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliAnyActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliFragThroughputCy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragThread);
    hwcpipe_double result = v0 / v1;
    return result;
}
double MaliFragThroughputCy_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = v0 / (v1 * 4);
    return result;
}
double MaliFragThroughputCy_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = v0 / (v1 * 8);
    return result;
}
double MaliFragThroughputCy_v3(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = v0 / (v1 * 16);
    return result;
}
double MaliFragThroughputCy_v4(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliMainActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragThread);
    hwcpipe_double result = v0 / v1;
    return result;
}
double MaliFragThroughputCy_v5(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliMainActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double v2 = ctx.get_counter_value(MaliFragPrepassWarp);
    hwcpipe_double result = v0 / ((v1 - v2) * 16);
    return result;
}
double MaliFragHelpTdRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragThread);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragHelpThread);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliFragEZSTestRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragEZSTestQd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragRastQd);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliFragEZSKillRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragEZSKillQd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragRastQd);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliFragLZSTestRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragLZSTestTd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragRastQd);
    hwcpipe_double result = (v0 / (v1 * 4)) * 100;
    return result;
}
double MaliFragLZSTestRate_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragLZSTestQd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragRastQd);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliFragLZSTestRate_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragLZSTestQd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = (v0 / (4 * v1)) * 100;
    return result;
}
double MaliFragLZSKillRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragLZSKillTd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragRastQd);
    hwcpipe_double result = (v0 / (v1 * 4)) * 100;
    return result;
}
double MaliFragLZSKillRate_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragLZSKillQd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragRastQd);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliFragLZSKillRate_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragLZSKillQd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = (v0 / (4 * v1)) * 100;
    return result;
}
double MaliFragOverdraw_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragThread);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragQueueTask);
    hwcpipe_double result = v0 / (v1 * 16 * 16);
    return result;
}
double MaliFragOverdraw_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragQueueTask);
    hwcpipe_double result = (v0 * 4) / (v1 * 32 * 32);
    return result;
}
double MaliFragOverdraw_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragQueueTask);
    hwcpipe_double result = (v0 * 8) / (v1 * 32 * 32);
    return result;
}
double MaliFragOverdraw_v3(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragQueueTask);
    hwcpipe_double result = (v0 * 16) / (v1 * 32 * 32);
    return result;
}
double MaliFragOverdraw_v4(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragThread);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragQueueTask);
    hwcpipe_double result = v0 / (v1 * 32 * 32);
    return result;
}
double MaliFragOverdraw_v5(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragThread);
    hwcpipe_double v1 = ctx.get_counter_value(MaliMainQueueTask);
    hwcpipe_double result = v0 / (v1 * 64 * 64);
    return result;
}
double MaliFragTileKillRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragTileKill);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragTile);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliFragTileKillRate_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragTileKill);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragTile);
    hwcpipe_double result = (v0 / (4 * v1)) * 100;
    return result;
}
double MaliCoreUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double result = (v0 / ctx.get_mali_config_shader_core_count() / v1) * 100;
    return result;
}
double MaliCoreUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliAnyActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliALUUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliALUUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngFMAInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliEngCVTInstr);
    hwcpipe_double v2 = ctx.get_counter_value(MaliEngSFUInstr);
    hwcpipe_double v3 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (std::max<hwcpipe_double>({v0, v1, v2 * 4}) / v3) * 100;
    return result;
}
double MaliALUUtil_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngFMAInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliEngCVTInstr);
    hwcpipe_double v2 = ctx.get_counter_value(MaliEngSFUInstr);
    hwcpipe_double v3 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (std::max<hwcpipe_double>({v0 + v1 + v2, v2 * 4}) / v3) * 100;
    return result;
}
double MaliALUUtil_v3(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngCVTInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliEngSFUInstr);
    hwcpipe_double v2 = ctx.get_counter_value(MaliEngFMAInstr);
    hwcpipe_double v3 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result =
        (std::max<hwcpipe_double>({v0 + v1 + ((v2 - std::min<hwcpipe_double>({v2, v0 + v1})) / 2), v1 * 4}) / v3) * 100;
    return result;
}
double MaliALUUtil_v4(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngFMAInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliEngCVTInstr);
    hwcpipe_double v2 = ctx.get_counter_value(MaliEngSFUInstr);
    hwcpipe_double v3 = ctx.get_counter_value(MaliEngSlot1IssueCy);
    hwcpipe_double v4 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (std::max<hwcpipe_double>({(v0 + v1 + v2) - v3, v3, v2 * 4}) / v4) * 100;
    return result;
}
double MaliTexSample_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexInstr);
    hwcpipe_double result = v0;
    return result;
}
double MaliTexSample_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexQuads);
    hwcpipe_double result = v0 * 4;
    return result;
}
double MaliTexSample_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexOutMsg);
    hwcpipe_double result = v0 * 4;
    return result;
}
double MaliTexSample_v3(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexOutMsg);
    hwcpipe_double result = v0 * 2 * 4;
    return result;
}
double MaliTexSample_v4(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexOutMsg);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexOutSingleMsg);
    hwcpipe_double result = ((v0 * 2) - v1) * 4;
    return result;
}
double MaliTexCPI_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexFiltIssueCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexInstr);
    hwcpipe_double result = v0 / v1;
    return result;
}
double MaliTexCPI_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexFiltIssueCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexQuads);
    hwcpipe_double result = v0 / (v1 * 4);
    return result;
}
double MaliTexCPI_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexFiltIssueCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexOutMsg);
    hwcpipe_double result = v0 / (v1 * 4);
    return result;
}
double MaliTexCPI_v3(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexFiltIssueCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexOutMsg);
    hwcpipe_double result = v0 / (v1 * 2 * 4);
    return result;
}
double MaliTexCPI_v4(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexFiltIssueCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexCacheLookupCy);
    hwcpipe_double v2 = ctx.get_counter_value(MaliTexCacheSimpleLoadCy);
    hwcpipe_double v3 = ctx.get_counter_value(MaliTexCacheComplexLoadCy);
    hwcpipe_double v4 = ctx.get_counter_value(MaliTexInBt);
    hwcpipe_double v5 = ctx.get_counter_value(MaliTexOutBt);
    hwcpipe_double v6 = ctx.get_counter_value(MaliTexL1CacheOutputCy);
    hwcpipe_double v7 = ctx.get_counter_value(MaliTexL1CacheLookupCy);
    hwcpipe_double v8 = ctx.get_counter_value(MaliTexIndexCy);
    hwcpipe_double v9 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double v10 = ctx.get_counter_value(MaliTexOutMsg);
    hwcpipe_double v11 = ctx.get_counter_value(MaliTexOutSingleMsg);
    hwcpipe_double result =
        ((std::max<hwcpipe_double>({v0, v1, v2, v3, v4, v5, v6, v7, v8}) / v9) * 100) / (((v10 * 2) - v11) * 4);
    return result;
}
double MaliTexUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexFiltIssueCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliTexUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexFiltIssueCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexCacheLookupCy);
    hwcpipe_double v2 = ctx.get_counter_value(MaliTexCacheSimpleLoadCy);
    hwcpipe_double v3 = ctx.get_counter_value(MaliTexCacheComplexLoadCy);
    hwcpipe_double v4 = ctx.get_counter_value(MaliTexInBt);
    hwcpipe_double v5 = ctx.get_counter_value(MaliTexOutBt);
    hwcpipe_double v6 = ctx.get_counter_value(MaliTexL1CacheOutputCy);
    hwcpipe_double v7 = ctx.get_counter_value(MaliTexL1CacheLookupCy);
    hwcpipe_double v8 = ctx.get_counter_value(MaliTexIndexCy);
    hwcpipe_double v9 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (((std::max<hwcpipe_double>({v0, v1, v2, v3, v4, v5, v6, v7, v8}) / v9) * 100) / v9) * 100;
    return result;
}
double MaliTexIssueCy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexFiltIssueCy);
    hwcpipe_double result = v0;
    return result;
}
double MaliTexIssueCy_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexFiltIssueCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexCacheLookupCy);
    hwcpipe_double v2 = ctx.get_counter_value(MaliTexCacheSimpleLoadCy);
    hwcpipe_double v3 = ctx.get_counter_value(MaliTexCacheComplexLoadCy);
    hwcpipe_double v4 = ctx.get_counter_value(MaliTexInBt);
    hwcpipe_double v5 = ctx.get_counter_value(MaliTexOutBt);
    hwcpipe_double v6 = ctx.get_counter_value(MaliTexL1CacheOutputCy);
    hwcpipe_double v7 = ctx.get_counter_value(MaliTexL1CacheLookupCy);
    hwcpipe_double v8 = ctx.get_counter_value(MaliTexIndexCy);
    hwcpipe_double v9 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (std::max<hwcpipe_double>({v0, v1, v2, v3, v4, v5, v6, v7, v8}) / v9) * 100;
    return result;
}
double MaliLSUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliLSIssueCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliLSUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliLSFullRd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliLSPartRd);
    hwcpipe_double v2 = ctx.get_counter_value(MaliLSFullWr);
    hwcpipe_double v3 = ctx.get_counter_value(MaliLSPartWr);
    hwcpipe_double v4 = ctx.get_counter_value(MaliLSAtomic);
    hwcpipe_double v5 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = ((v0 + v1 + v2 + v3 + v4) / v5) * 100;
    return result;
}
double MaliGPUPix_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragQueueTask);
    hwcpipe_double result = v0 * 16 * 16;
    return result;
}
double MaliGPUPix_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragQueueTask);
    hwcpipe_double result = v0 * 32 * 32;
    return result;
}
double MaliGPUPix_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliMainQueueTask);
    hwcpipe_double result = v0 * 64 * 64;
    return result;
}
double MaliGPUCyPerPix_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragQueueTask);
    hwcpipe_double result = v0 / (v1 * 16 * 16);
    return result;
}
double MaliGPUCyPerPix_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragQueueTask);
    hwcpipe_double result = v0 / (v1 * 32 * 32);
    return result;
}
double MaliGPUCyPerPix_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGPUQueueActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragQueueTask);
    hwcpipe_double result = v0 / (v1 * 32 * 32);
    return result;
}
double MaliGPUCyPerPix_v3(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliMainQueueTask);
    hwcpipe_double result = v0 / (v1 * 64 * 64);
    return result;
}
double MaliFragFPKBUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragFPKActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliFragFPKBUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragFPKActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliMainActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliL2CacheRdMissRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliExtBusRd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliL2CacheRdLookup);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliL2CacheWrMissRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliExtBusWr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliL2CacheWrLookup);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliExtBusRdLat384_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliExtBusRdBt);
    hwcpipe_double v1 = ctx.get_counter_value(MaliExtBusRdLat0);
    hwcpipe_double v2 = ctx.get_counter_value(MaliExtBusRdLat128);
    hwcpipe_double v3 = ctx.get_counter_value(MaliExtBusRdLat192);
    hwcpipe_double v4 = ctx.get_counter_value(MaliExtBusRdLat256);
    hwcpipe_double v5 = ctx.get_counter_value(MaliExtBusRdLat320);
    hwcpipe_double result = v0 - v1 - v2 - v3 - v4 - v5;
    return result;
}
double MaliExtBusRdOTQ4_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliExtBusRd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliExtBusRdOTQ1);
    hwcpipe_double v2 = ctx.get_counter_value(MaliExtBusRdOTQ2);
    hwcpipe_double v3 = ctx.get_counter_value(MaliExtBusRdOTQ3);
    hwcpipe_double result = v0 - v1 - v2 - v3;
    return result;
}
double MaliExtBusWrOTQ4_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliExtBusWr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliExtBusWrOTQ1);
    hwcpipe_double v2 = ctx.get_counter_value(MaliExtBusWrOTQ2);
    hwcpipe_double v3 = ctx.get_counter_value(MaliExtBusWrOTQ3);
    hwcpipe_double result = v0 - v1 - v2 - v3;
    return result;
}
double MaliGeomSampleCullRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomFaceXYPlaneCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomZPlaneCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = (v0 / ((v1 + v2 + v0 + v3) - v1 - v2)) * 100;
    return result;
}
double MaliGeomSampleCullRate_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomFaceCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomPlaneCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = (v0 / ((v1 + v2 + v0 + v3) - v1 - v2)) * 100;
    return result;
}
double MaliGeomSampleCullRate_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomFaceCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomPlaneCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomScissorCullPrim);
    hwcpipe_double v4 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = (v0 / ((v1 + v2 + v0 + v3 + v4) - v2 - v3 - v1)) * 100;
    return result;
}
double MaliGeomPosShadThread_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomPosShadTask);
    hwcpipe_double result = v0 * 4;
    return result;
}
double MaliGeomPosShadThread_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomPosShadTask);
    hwcpipe_double result = v0 * 16;
    return result;
}
double MaliGeomPosShadThreadPerPrim_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomPosShadTask);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomFaceXYPlaneCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomZPlaneCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v4 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = (v0 * 4) / (v1 + v2 + v3 + v4);
    return result;
}
double MaliGeomPosShadThreadPerPrim_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomPosShadTask);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomFaceCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomPlaneCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v4 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = (v0 * 4) / (v1 + v2 + v3 + v4);
    return result;
}
double MaliGeomPosShadThreadPerPrim_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomPosShadTask);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomFaceCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomPlaneCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v4 = ctx.get_counter_value(MaliGeomScissorCullPrim);
    hwcpipe_double v5 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = (v0 * 4) / (v1 + v2 + v3 + v4 + v5);
    return result;
}
double MaliGeomPosShadThreadPerPrim_v3(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomPosShadTask);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomFaceCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomPlaneCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v4 = ctx.get_counter_value(MaliGeomScissorCullPrim);
    hwcpipe_double v5 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = (v0 * 16) / (v1 + v2 + v3 + v4 + v5);
    return result;
}
double MaliTilerPosCacheHitRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTilerPosCacheHit);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTilerPosCacheMiss);
    hwcpipe_double result = (v0 / (v0 + v1)) * 100;
    return result;
}
double MaliGeomVarShadThread_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomVarShadTask);
    hwcpipe_double result = v0 * 4;
    return result;
}
double MaliGeomVarShadThread_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomVarShadTask);
    hwcpipe_double result = v0 * 16;
    return result;
}
double MaliGeomVarShadThreadPerPrim_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomVarShadTask);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = (v0 * 4) / v1;
    return result;
}
double MaliTilerVarCacheHitRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTilerVarCacheHit);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTilerVarCacheMiss);
    hwcpipe_double result = (v0 / (v0 + v1)) * 100;
    return result;
}
double MaliFragOpaqueQdRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragOpaqueQd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragRastQd);
    hwcpipe_double v2 = ctx.get_counter_value(MaliFragEZSKillQd);
    hwcpipe_double result = (v0 / (v1 - v2)) * 100;
    return result;
}
double MaliFragTransparentQd_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragRastQd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragEZSKillQd);
    hwcpipe_double v2 = ctx.get_counter_value(MaliFragOpaqueQd);
    hwcpipe_double result = v0 - v1 - v2;
    return result;
}
double MaliFragShadedQd_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = v0;
    return result;
}
double MaliFragShadedQd_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = (v0 * 8) / 4;
    return result;
}
double MaliFragShadedQd_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = (v0 * 16) / 4;
    return result;
}
double MaliFragPartWarpRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragPartWarp);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliFragEZSUpdateRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragEZSUpdateQd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragRastQd);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliFragFPKKillQd_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragRastQd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragEZSKillQd);
    hwcpipe_double v2 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = v0 - v1 - v2;
    return result;
}
double MaliFragFPKKillQd_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragRastQd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragEZSKillQd);
    hwcpipe_double v2 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = v0 - v1 - ((v2 * 8) / 4);
    return result;
}
double MaliFragFPKKillQd_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragRastQd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragEZSKillQd);
    hwcpipe_double v2 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = v0 - v1 - ((v2 * 16) / 4);
    return result;
}
double MaliFragFPKKillRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragRastQd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragEZSKillQd);
    hwcpipe_double v2 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = ((v0 - v1 - v2) / v0) * 100;
    return result;
}
double MaliFragFPKKillRate_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragRastQd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragEZSKillQd);
    hwcpipe_double v2 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = ((v0 - v1 - ((v2 * 8) / 4)) / v0) * 100;
    return result;
}
double MaliFragFPKKillRate_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragRastQd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragEZSKillQd);
    hwcpipe_double v2 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = ((v0 - v1 - ((v2 * 16) / 4)) / v0) * 100;
    return result;
}
double MaliNonFragThread_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliNonFragWarp);
    hwcpipe_double result = v0 * 4;
    return result;
}
double MaliNonFragThread_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliNonFragWarp);
    hwcpipe_double result = v0 * 8;
    return result;
}
double MaliNonFragThread_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliNonFragWarp);
    hwcpipe_double result = v0 * 16;
    return result;
}
double MaliFragThread_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = v0 * 4;
    return result;
}
double MaliFragThread_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = v0 * 8;
    return result;
}
double MaliFragThread_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = v0 * 16;
    return result;
}
double MaliEngDivergedInstrRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngDivergedInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliEngInstr);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliEngDivergedInstrRate_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngDivergedInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliEngFMAInstr);
    hwcpipe_double v2 = ctx.get_counter_value(MaliEngCVTInstr);
    hwcpipe_double v3 = ctx.get_counter_value(MaliEngSFUInstr);
    hwcpipe_double result = (v0 / (v1 + v2 + v3)) * 100;
    return result;
}
double MaliTexCacheUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexCacheLookup);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliTexMipInstrRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexQuadPassMip);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexQuadPass);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliTexMipInstrRate_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexMipInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexInstr);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliTexCacheCompressFetchRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexCacheCompressFetch);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexCacheFetch);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliTexTriInstrRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexQuadPassTri);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexQuads);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliTexTriInstrRate_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexTriInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexInstr);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliLSRdCy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliLSFullRd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliLSPartRd);
    hwcpipe_double result = v0 + v1;
    return result;
}
double MaliLSWrCy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliLSFullWr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliLSPartWr);
    hwcpipe_double result = v0 + v1;
    return result;
}
double MaliLSIssueCy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliLSFullRd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliLSPartRd);
    hwcpipe_double v2 = ctx.get_counter_value(MaliLSFullWr);
    hwcpipe_double v3 = ctx.get_counter_value(MaliLSPartWr);
    hwcpipe_double v4 = ctx.get_counter_value(MaliLSAtomic);
    hwcpipe_double result = v0 + v1 + v2 + v3 + v4;
    return result;
}
double MaliVar32IssueCy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliVar32IssueSlot);
    hwcpipe_double result = v0;
    return result;
}
double MaliVar32IssueCy_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliVar32IssueSlot);
    hwcpipe_double result = v0 / 2;
    return result;
}
double MaliVar32IssueCy_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliVar32IssueSlot);
    hwcpipe_double result = v0 / 4;
    return result;
}
double MaliVar16IssueCy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliVar16IssueSlot);
    hwcpipe_double result = v0;
    return result;
}
double MaliVar16IssueCy_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliVar16IssueSlot);
    hwcpipe_double result = v0 / 2;
    return result;
}
double MaliVar16IssueCy_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliVar16IssueSlot);
    hwcpipe_double result = v0 / 4;
    return result;
}
double MaliVarIssueCy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliVar32IssueSlot);
    hwcpipe_double v1 = ctx.get_counter_value(MaliVar16IssueSlot);
    hwcpipe_double result = v0 + v1;
    return result;
}
double MaliVarIssueCy_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliVar32IssueSlot);
    hwcpipe_double v1 = ctx.get_counter_value(MaliVar16IssueSlot);
    hwcpipe_double result = (v0 / 2) + (v1 / 2);
    return result;
}
double MaliVarIssueCy_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliVar32IssueSlot);
    hwcpipe_double v1 = ctx.get_counter_value(MaliVar16IssueSlot);
    hwcpipe_double result = (v0 / 4) + (v1 / 4);
    return result;
}
double MaliVarUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliVar32IssueSlot);
    hwcpipe_double v1 = ctx.get_counter_value(MaliVar16IssueSlot);
    hwcpipe_double v2 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = ((v0 + v1) / v2) * 100;
    return result;
}
double MaliVarUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliVar32IssueSlot);
    hwcpipe_double v1 = ctx.get_counter_value(MaliVar16IssueSlot);
    hwcpipe_double v2 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (((v0 / 2) + (v1 / 2)) / v2) * 100;
    return result;
}
double MaliVarUtil_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliVar32IssueSlot);
    hwcpipe_double v1 = ctx.get_counter_value(MaliVar16IssueSlot);
    hwcpipe_double v2 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (((v0 / 4) + (v1 / 4)) / v2) * 100;
    return result;
}
double MaliSCBusFFEL2RdBy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliSCBusFFEL2RdBt);
    hwcpipe_double result = v0 * 16;
    return result;
}
double MaliSCBusFFEExtRdBy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliSCBusFFEExtRdBt);
    hwcpipe_double result = v0 * 16;
    return result;
}
double MaliSCBusLSL2RdBy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliSCBusLSL2RdBt);
    hwcpipe_double result = v0 * 16;
    return result;
}
double MaliSCBusLSL2RdByPerRd_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliSCBusLSL2RdBt);
    hwcpipe_double v1 = ctx.get_counter_value(MaliLSFullRd);
    hwcpipe_double v2 = ctx.get_counter_value(MaliLSPartRd);
    hwcpipe_double result = (v0 * 16) / (v1 + v2);
    return result;
}
double MaliSCBusLSExtRdBy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliSCBusLSExtRdBt);
    hwcpipe_double result = v0 * 16;
    return result;
}
double MaliSCBusLSExtRdByPerRd_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliSCBusLSExtRdBt);
    hwcpipe_double v1 = ctx.get_counter_value(MaliLSFullRd);
    hwcpipe_double v2 = ctx.get_counter_value(MaliLSPartRd);
    hwcpipe_double result = (v0 * 16) / (v1 + v2);
    return result;
}
double MaliSCBusTexL2RdBy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliSCBusTexL2RdBt);
    hwcpipe_double result = v0 * 16;
    return result;
}
double MaliSCBusTexL2RdByPerRd_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliSCBusTexL2RdBt);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexFiltIssueCy);
    hwcpipe_double result = (v0 * 16) / v1;
    return result;
}
double MaliSCBusTexExtRdBy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliSCBusTexExtRdBt);
    hwcpipe_double result = v0 * 16;
    return result;
}
double MaliSCBusTexExtRdByPerRd_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliSCBusTexExtRdBt);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexFiltIssueCy);
    hwcpipe_double result = (v0 * 16) / v1;
    return result;
}
double MaliSCBusLSWrBt_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliSCBusLSWBWrBt);
    hwcpipe_double v1 = ctx.get_counter_value(MaliSCBusLSOtherWrBt);
    hwcpipe_double result = v0 + v1;
    return result;
}
double MaliSCBusLSWrBy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliSCBusLSWBWrBt);
    hwcpipe_double v1 = ctx.get_counter_value(MaliSCBusLSOtherWrBt);
    hwcpipe_double result = (v0 + v1) * 16;
    return result;
}
double MaliSCBusLSWrBy_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliSCBusLSWrBt);
    hwcpipe_double result = v0 * 16;
    return result;
}
double MaliSCBusLSWrByPerWr_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliSCBusLSWBWrBt);
    hwcpipe_double v1 = ctx.get_counter_value(MaliSCBusLSOtherWrBt);
    hwcpipe_double v2 = ctx.get_counter_value(MaliLSFullWr);
    hwcpipe_double v3 = ctx.get_counter_value(MaliLSPartWr);
    hwcpipe_double result = ((v0 + v1) * 16) / (v2 + v3);
    return result;
}
double MaliSCBusLSWrByPerWr_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliSCBusLSWrBt);
    hwcpipe_double v1 = ctx.get_counter_value(MaliLSFullWr);
    hwcpipe_double v2 = ctx.get_counter_value(MaliLSPartWr);
    hwcpipe_double result = (v0 * 16) / (v1 + v2);
    return result;
}
double MaliSCBusTileWrBy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliSCBusTileWrBt);
    hwcpipe_double result = v0 * 16;
    return result;
}
double MaliCoreAllRegsWarpRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliCoreAllRegsWarp);
    hwcpipe_double v1 = ctx.get_counter_value(MaliNonFragWarp);
    hwcpipe_double v2 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = (v0 / (v1 + v2)) * 100;
    return result;
}
double MaliCoreFullWarpRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliCoreFullWarp);
    hwcpipe_double v1 = ctx.get_counter_value(MaliNonFragWarp);
    hwcpipe_double v2 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = (v0 / (v1 + v2)) * 100;
    return result;
}
double MaliTexCompressInstrRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexCompressInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexInstr);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliTex3DInstrRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTex3DInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexInstr);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliSCBusOtherWrBy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliSCBusOtherWrBt);
    hwcpipe_double result = v0 * 16;
    return result;
}
double MaliFragRastPartQdRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragRastPartQd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragRastQd);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliEngFMAPipeUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngFMAInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliEngFMAPipeUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngFMAInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (v0 / (2 * v1)) * 100;
    return result;
}
double MaliEngCVTPipeUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngCVTInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliEngCVTPipeUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngCVTInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (v0 / (2 * v1)) * 100;
    return result;
}
double MaliEngSFUPipeUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngSFUInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = ((v0 * 4) / v1) * 100;
    return result;
}
double MaliEngArithInstr_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngFMAInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliEngCVTInstr);
    hwcpipe_double v2 = ctx.get_counter_value(MaliEngSFUInstr);
    hwcpipe_double result = v0 + v1 + v2;
    return result;
}
double MaliEngSWBlendRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngSWBlendInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = ((v0 * 2) / v1) * 100;
    return result;
}
double MaliEngSWBlendRate_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngSWBlendInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = ((v0 * 4) / v1) * 100;
    return result;
}
double MaliTexQuads_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexOutMsg);
    hwcpipe_double result = v0;
    return result;
}
double MaliTexQuads_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexOutMsg);
    hwcpipe_double result = v0 * 2;
    return result;
}
double MaliTexQuads_v2(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexOutMsg);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexOutSingleMsg);
    hwcpipe_double result = (v0 * 2) - v1;
    return result;
}
double MaliTexInBusUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexInBt);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliTexOutBusUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexOutBt);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliTexFiltFullRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliTexFullBiFiltCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliTexFullTriFiltCy);
    hwcpipe_double v2 = ctx.get_counter_value(MaliTexFiltIssueCy);
    hwcpipe_double result = ((v0 + v1) / v2) * 100;
    return result;
}
double MaliAnyUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliAnyActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double result = (v0 / ctx.get_mali_config_shader_core_count() / v1) * 100;
    return result;
}
double MaliAnyUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliAnyActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUQueueActiveCy);
    hwcpipe_double result = (v0 / ctx.get_mali_config_shader_core_count() / v1) * 100;
    return result;
}
double MaliFragQueueActiveCy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragQueuedCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragQueueAssignStallCy);
    hwcpipe_double result = v0 - v1;
    return result;
}
double MaliVertQueueActiveCy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliVertQueuedCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliVertQueueAssignStallCy);
    hwcpipe_double result = v0 - v1;
    return result;
}
double MaliVertQueueUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliVertQueuedCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliVertQueueAssignStallCy);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double result = ((v0 - v1) / v2) * 100;
    return result;
}
double MaliVertQueueUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliVertQueuedCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliVertQueueAssignStallCy);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGPUQueueActiveCy);
    hwcpipe_double result = ((v0 - v1) / v2) * 100;
    return result;
}
double MaliCompQueueActiveCy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliCompQueuedCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCompQueueAssignStallCy);
    hwcpipe_double result = v0 - v1;
    return result;
}
double MaliCompQueueUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliCompQueuedCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCompQueueAssignStallCy);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double result = ((v0 - v1) / v2) * 100;
    return result;
}
double MaliCompQueueUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliCompQueuedCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCompQueueAssignStallCy);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGPUQueueActiveCy);
    hwcpipe_double result = ((v0 - v1) / v2) * 100;
    return result;
}
double MaliCSFMCUUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliCSFMCUActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliCSFMCUUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliCSFMCUActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUQueueActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliCSFLSUUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliCSFLSUActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliCSFLSUUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliCSFLSUActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUQueueActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliCSFCEUUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliCSFCEUActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliCSFCEUUtil_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliCSFCEUActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGPUQueueActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliGPUActiveCy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGPUQueueActiveCy);
    hwcpipe_double result = v0;
    return result;
}
double MaliGeomFaceCullRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomFaceCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomPlaneCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = (v0 / (v0 + v1 + v2 + v3)) * 100;
    return result;
}
double MaliGeomFaceCullRate_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomFaceCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomPlaneCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomScissorCullPrim);
    hwcpipe_double v4 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = (v0 / ((v0 + v1 + v2 + v3 + v4) - v1 - v3)) * 100;
    return result;
}
double MaliGeomPlaneCullRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomPlaneCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomFaceCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = (v0 / ((v1 + v0 + v2 + v3) - v1)) * 100;
    return result;
}
double MaliGeomPlaneCullRate_v1(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomPlaneCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomFaceCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomScissorCullPrim);
    hwcpipe_double v4 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = (v0 / (v1 + v0 + v2 + v3 + v4)) * 100;
    return result;
}
double MaliFragShadRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragRastCoarseQd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragRastQd);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliCoreFragWarpOcc_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragThread);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = (v0 / (v1 * 16)) * 100;
    return result;
}
double MaliEngNarrowInstrRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngNarrowInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliEngFMAInstr);
    hwcpipe_double v2 = ctx.get_counter_value(MaliEngCVTInstr);
    hwcpipe_double v3 = ctx.get_counter_value(MaliEngSFUInstr);
    hwcpipe_double result = (v0 / (v1 + v2 + v3)) * 100;
    return result;
}
double MaliRTUUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliRTUBoxIssueCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliRTUTriIssueCy);
    hwcpipe_double v2 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (std::max<hwcpipe_double>({v0, v1}) / v2) * 100;
    return result;
}
double MaliMainQueueActiveCy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliMainQueuedCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliMainQueueAssignStallCy);
    hwcpipe_double result = v0 - v1;
    return result;
}
double MaliMainQueueUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliMainQueuedCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliMainQueueAssignStallCy);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double result = ((v0 - v1) / v2) * 100;
    return result;
}
double MaliBinningQueueActiveCy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliBinningQueuedCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliBinningQueueAssignStallCy);
    hwcpipe_double result = v0 - v1;
    return result;
}
double MaliBinningQueueUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliBinningQueuedCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliBinningQueueAssignStallCy);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGPUActiveCy);
    hwcpipe_double result = ((v0 - v1) / v2) * 100;
    return result;
}
double MaliGeomScissorCullRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliGeomScissorCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliGeomFaceCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliGeomPlaneCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliGeomSampleCullPrim);
    hwcpipe_double v4 = ctx.get_counter_value(MaliGeomVisiblePrim);
    hwcpipe_double result = (v0 / ((v1 + v2 + v3 + v0 + v4) - v2)) * 100;
    return result;
}
double MaliCompOrBinningUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliCompOrBinningActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliAnyActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliMainUtil_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliMainActiveCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliAnyActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliFragPrepassKillRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragPrepassKillQd);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragPrepassTestQd);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliFragMainPassStallRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragMainPassStallCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliMainActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliFragInputPrim_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragPrepassCullPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliFragPrepassPrim);
    hwcpipe_double result = (v0 + v1) - v2;
    return result;
}
double MaliFragPrepassPrimRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragPrepassPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliFragPrepassCullPrim);
    hwcpipe_double result = (v0 / ((v1 + v2) - v0)) * 100;
    return result;
}
double MaliFragPrepassCullPrimRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragPrepassCullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliFragPrepassPrim);
    hwcpipe_double result = (v0 / ((v1 + v0) - v2)) * 100;
    return result;
}
double MaliFragPrepassUncullPrimRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragPrepassUncullPrim);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragPrim);
    hwcpipe_double v2 = ctx.get_counter_value(MaliFragPrepassCullPrim);
    hwcpipe_double v3 = ctx.get_counter_value(MaliFragPrepassPrim);
    hwcpipe_double result = (v0 / ((v1 + v2) - v3)) * 100;
    return result;
}
double MaliFragPrepassWarpRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragPrepassWarp);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double result = (v0 / (v1 - v0)) * 100;
    return result;
}
double MaliFragPrepassThread_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragPrepassWarp);
    hwcpipe_double result = v0 * 16;
    return result;
}
double MaliFragMainThread_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliFragWarp);
    hwcpipe_double v1 = ctx.get_counter_value(MaliFragPrepassWarp);
    hwcpipe_double result = (v0 - v1) * 16;
    return result;
}
double MaliEngSlot0IssueCy_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngFMAInstr);
    hwcpipe_double v1 = ctx.get_counter_value(MaliEngCVTInstr);
    hwcpipe_double v2 = ctx.get_counter_value(MaliEngSFUInstr);
    hwcpipe_double v3 = ctx.get_counter_value(MaliEngSlot1IssueCy);
    hwcpipe_double result = (v0 + v1 + v2) - v3;
    return result;
}
double MaliEngAttrBackpressureRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngAttrBackpressureCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliEngBlendBackpressureRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngBlendBackpressureCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliEngLSBackpressureRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngLSBackpressureCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliEngTexBackpressureRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngTexBackpressureCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliEngVarBackpressureRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngVarBackpressureCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}
double MaliEngZSBackpressureRate_v0(const context &ctx) {
    hwcpipe_double v0 = ctx.get_counter_value(MaliEngZSBackpressureCy);
    hwcpipe_double v1 = ctx.get_counter_value(MaliCoreActiveCy);
    hwcpipe_double result = (v0 / v1) * 100;
    return result;
}

} // namespace database

} // namespace hwcpipe