#include <cmath>
#include <iostream>
#include "proposal.h"

using std::exp;
using std::fmax;

void Proposal::calcProposal() {
	double x = anchor_orig[2] * bb_fixed[0] + anchor_orig[0];
	double y = anchor_orig[3] * bb_fixed[1] + anchor_orig[1];
	double w = anchor_orig[2] * exp(bb_fixed[2]);
	double h = anchor_orig[3] * exp(bb_fixed[3]);
	anchor_fixed[0] = static_cast<int>(y - 0.5 * h);
	anchor_fixed[1] = static_cast<int>(x - 0.5 * w);
	anchor_fixed[2] = static_cast<int>(y + 0.5 * h);
	anchor_fixed[3] = static_cast<int>(x + 0.5 * w);
}

void Proposal::calcRoI(double spatial_scale, int pool_height, int pool_width) {
    roi_start_h = anchor_fixed[0] * spatial_scale;
    roi_start_w = anchor_fixed[1] * spatial_scale;
    roi_end_h = anchor_fixed[2] * spatial_scale;
    roi_end_w = anchor_fixed[3] * spatial_scale;
    roi_height = fmax(roi_end_h - roi_start_h + 1, 1.0);
    roi_width = fmax(roi_end_w - roi_start_w + 1, 1.0);
    bin_size_h = roi_height / static_cast<double>(pool_height);
    bin_size_w = roi_width / static_cast<double>(pool_width);
}

