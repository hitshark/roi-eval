#ifndef _PROPOSAL_H_
#define _PROPOSAL_H_

#include <array>

class Proposal {
	public:
		Proposal(double cls_score, const std::array<double, 4> &regr_val, const std::array<int, 4> &anchor) :
			score(cls_score),
			bb_fixed(regr_val),
			anchor_orig(anchor) {}

		// member access function
		double getRoiStartHeight() const {return roi_start_h;}
		double getRoiStartWidth() const {return roi_start_w;}
		double getRoiEndHeight() const {return roi_end_h;}
		double getRoiEndWidth() const {return roi_end_w;}
		double getRoiHeight() const {return roi_height;}
		double getRoiWidth() const {return roi_width;}
		double getBinSizeHeight() const {return bin_size_h;}
		double getBinSizeWidth() const {return bin_size_w;}

		double getScore() const {return score;}
		std::array<int, 4> getAnchor() const {return anchor_orig;}
		std::array<double, 4> getBoxFixed() const {return bb_fixed;}
		std::array<int, 4> getProposal() const {return anchor_fixed;}

		// roi calculation steps
		void calcProposal();
		void calcRoI(double spatial_scale, int pool_height, int pool_width);

	private:
		double roi_start_h;
		double roi_start_w;
		double roi_end_h;
		double roi_end_w;
		double roi_height;
		double roi_width;
		double bin_size_h;
		double bin_size_w;

		double score;
		std::array<double, 4> bb_fixed;	// delta x, delta y, scale w, scale h
		std::array<int, 4> anchor_orig;	// original anchor cordinate (x, y, w, h)
		std::array<int, 4> anchor_fixed; // fixed anchor position (start_h, start_w, end_h, end_w)
};

#endif

