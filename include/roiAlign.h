#ifndef _ROIPOOL_H_
#define _ROIPOOL_H_

#include <vector>
#include <string>
#include "proposal.h"

using std::vector;
using std::string;

template<class T>
class RoIAlign {
	public:
		explicit RoIAlign(int pnum);

		// roi pooling steps
		void genInputFmap();
		void genProposal();
		void calcProposal();
		void doNMS();
		void calcPooling();
		void biSample();
		void pool();

		// for logging
		void printInputFmap(int channel);
		void printProposal(int proposal_id);
		void printRoI(int proposal_id);
		void printOutputFmap(int proposal_id, int channel);

	private:
		int proposal_num;
		vector<Proposal> pp_gen;
		vector<Proposal> pp_vec;
		vector<T> fmap;
		vector<T> rlt;

		// config info
		int image_h_;
		int image_w_;
		double spatial_scale_;
		int roi_pool_h_;
		int roi_pool_w_;
		int roi_channel_;
		int orig_proposal_num_;
		double nms_iou_threshold_;
		double nms_score_threshold_;
		string pool_type_;
		int bisample_num_;

		// internal variable
		int fmap_h_;
		int fmap_w_;
		static const double samples_[8];
		vector<T> rlt_sampled_;
};

#endif

