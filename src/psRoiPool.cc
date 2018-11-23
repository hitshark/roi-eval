#include <random>
#include <array>
#include <map>
#include <functional>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cmath>
#include "psRoiPool.h"
#include "config.h"
#include "proposal.h"

using std::default_random_engine;
using std::normal_distribution;
using std::uniform_real_distribution;
using std::uniform_int_distribution;
using std::max;
using std::min;
using std::floor;
using std::ceil;
using std::round;

#define FLOAT_MAX (128)		// enough

template <class T>
PSRoIPool<T>::PSRoIPool(int pnum) : proposal_num(pnum) {
	image_h_ = Config::getInst().getImageHeight();
	image_w_ = Config::getInst().getImageWidth();
	spatial_scale_ = Config::getInst().getSpatialScale();
	roi_pool_h_ = Config::getInst().getRoiPoolHeight();
	roi_pool_w_ = Config::getInst().getRoiPoolWidth();
	roi_channel_ = Config::getInst().getRoiChannel();
	orig_proposal_num_ = Config::getInst().getOrigProposalNum();
	nms_iou_threshold_ = Config::getInst().getNmsIouThreshold();
	nms_score_threshold_ = Config::getInst().getNmsScoreThreshold();
	pool_type_ = Config::getInst().getPoolType();

    class_num_ = roi_channel_ / (roi_pool_h_ * roi_pool_w_);
	fmap_h_ = static_cast<int>(image_h_ * spatial_scale_);
	fmap_w_ = static_cast<int>(image_w_ * spatial_scale_);
	fmap.resize(roi_channel_ * fmap_h_ * fmap_w_);
	rlt.resize(proposal_num * roi_pool_h_ * roi_pool_w_ * class_num_);
}

template <class T>
void PSRoIPool<T>::genInputFmap() {
	default_random_engine e(time(nullptr));
	normal_distribution<double> n(0, 3.5);
	for(int k = 0; k < roi_channel_; k++) {
		for(int i = 0; i < fmap_h_; i++) {
			for(int j = 0; j < fmap_w_; j++) {
				int addr = k * fmap_h_ * fmap_w_ + i * fmap_w_ + j;
				fmap[addr] = (T)n(e);
			}
		}
	}
}


template <class T>
void PSRoIPool<T>::genProposal() {
	const int anchor_h[] = {23, 32, 46, 45, 64, 90, 91, 128, 180, 181, 256, 362, 362, 512, 724};
	const int anchor_w[] = {46, 32, 23, 90, 64, 45, 180, 128, 91, 362, 256, 181, 724, 512, 362};
	uniform_int_distribution<int> r_anchor_size(0,14);

	uniform_int_distribution<int> r_image_y(0,image_h_-1);
	uniform_int_distribution<int> r_image_x(0,image_w_-1);

	uniform_real_distribution<double> r_bb_fix(0.01, 0.5);
	uniform_real_distribution<double> r_score(0.01, 0.99);
	default_random_engine e(time(nullptr));

	for(int i = 0; i < orig_proposal_num_; i++) {
		double score;
		std::array<double, 4> pp_fixed;
		std::array<int, 4> position;
		for(int k = 0; k < 4; k++) pp_fixed[k] = r_bb_fix(e);
		int index = r_anchor_size(e);
		position[0] = r_image_x(e);
		position[1] = r_image_y(e);
		position[2] = anchor_w[index];
		position[3] = anchor_h[index];
		score = r_score(e);

		Proposal tmp(score, pp_fixed, position);
		pp_gen.push_back(tmp);
	}
}


template <class T>
void PSRoIPool<T>::calcProposal() {
	for(int i = 0; i < orig_proposal_num_; i++) {
		pp_gen[i].calcProposal();
		pp_gen[i].calcRoI(spatial_scale_, roi_pool_h_, roi_pool_w_);
	}
}


template <class T>
void PSRoIPool<T>::doNMS() {
	std::map<double, int, std::greater<double>> score_ordered;
	for(unsigned i = 0; i < pp_gen.size(); i++) {
		if(pp_gen[i].getScore() > nms_score_threshold_) {
			score_ordered.insert(std::pair<double, int>(pp_gen[i].getScore(), i));
		}
	}
	for(int i = 0; i < proposal_num; i++) {
		std::pair<double, int> top = *(score_ordered.begin());
		Proposal cadidate = pp_gen[top.second]; 
		score_ordered.erase(top.first);
		pp_vec.push_back(cadidate);

		std::vector<int> pp_id;
		for(auto iter = score_ordered.begin(); iter != score_ordered.end(); iter++) {
			pp_id.push_back(iter->second);
		}

		std::array<int, 4> cord = cadidate.getProposal();
		int cadidate_area = (cord[2] - cord[0]) * (cord[3] - cord[1]);
		for(unsigned j = 0; j < pp_id.size(); j++) {
			Proposal comp = pp_gen[pp_id[j]];
			std::array<int, 4> comp_cord = comp.getProposal();
			// calculation overlap
			int h1 = max(comp_cord[0], cord[0]);
			int w1 = max(comp_cord[1], cord[1]);
			int h2 = min(comp_cord[2], cord[2]);
			int w2 = min(comp_cord[3], cord[3]);
			int w = max(0, w2-w1+1);
			int h = max(0, h2-h1+1);
			int overlap_area = w * h;
			double iou = static_cast<double>(overlap_area) / static_cast<double>(cadidate_area);
			if(iou > nms_iou_threshold_) {
				double comp_score = comp.getScore();
				score_ordered.erase(comp_score);
			}
		}
	}
}


template <class T>
void PSRoIPool<T>::calcPooling() {
	for(int i = 0; i < proposal_num; i++) {
		Proposal tmp = pp_vec[i];
		int roi_start_h = round(tmp.getRoiStartHeight());
		int roi_start_w = round(tmp.getRoiStartWidth());
		double bin_size_h = tmp.getBinSizeHeight();
		double bin_size_w = tmp.getBinSizeWidth();

        int rlt_proposal_base = i * roi_pool_h_ * roi_pool_w_ * class_num_;
        for (int ph = 0; ph < roi_pool_h_; ++ph) {
            for (int pw = 0; pw < roi_pool_w_; ++pw) {
                int hstart = static_cast<int>(floor(static_cast<double>(ph) * bin_size_h));
                int wstart = static_cast<int>(floor(static_cast<double>(pw) * bin_size_w));
                int hend = static_cast<int>(ceil(static_cast<double>(ph + 1) * bin_size_h));
                int wend = static_cast<int>(ceil(static_cast<double>(pw + 1) * bin_size_w));

                hstart = min(max(hstart + roi_start_h, 0), fmap_h_);
                hend = min(max(hend + roi_start_h, 0), fmap_h_);
                wstart = min(max(wstart + roi_start_w, 0), fmap_w_);
                wend = min(max(wend + roi_start_w, 0), fmap_w_);

                bool is_empty = (hend <= hstart) || (wend <= wstart);
                int fmap_channel_base = (ph * roi_pool_w_ + pw) * class_num_;
                int rlt_addr_base = rlt_proposal_base + fmap_channel_base;


                for(int c = 0; c < class_num_; c++) {
                    const int pool_index =  rlt_addr_base + c;
                    int c_fmap_base = (fmap_channel_base + c) * fmap_h_ * fmap_w_;

                    if(!pool_type_.compare("max")) {
                        rlt[pool_index] = -FLOAT_MAX;
                        if (is_empty) rlt[pool_index] = 0;
                        for (int h = hstart; h < hend; ++h) {
                            for (int w = wstart; w < wend; ++w) {
                                int index = c_fmap_base + h * fmap_w_ + w;
                                if (fmap[index] > rlt[pool_index]) {
                                    rlt[pool_index] = fmap[index];
                                }
                            }
                        }
                    }
                    else if(!pool_type_.compare("avg")) {
                        int counter = 0;
                        rlt[pool_index] = 0;
                        for(int h = hstart; h < hend; ++h) {
                            for(int w = wstart; w < wend; ++w) {
                                int index = c_fmap_base + h * fmap_w_ + w;
                                rlt[pool_index] += fmap[index];
                                counter++;
                            }
                        }
                        if(counter) rlt[pool_index] /= counter;
                    }
                }
            }
        }
    }
}

template <class T>
void PSRoIPool<T>::printInputFmap(int channel) {
    std::cout<<"roi input feature map, channel "<<channel<<std::endl;
    for(int i = 0; i < fmap_h_; i++) {
        for(int j = 0; j < fmap_w_; j++) {
            if((j > 0) && !(j % 16)) std::cout<<std::endl;
            int offset = channel * fmap_h_ * fmap_w_ + i * fmap_w_ + j;
            std::cout.setf(std::ios::fixed);
            std::cout<<std::setprecision(3);
            std::cout<<std::setw(8)<<std::setiosflags(std::ios::left);
            std::cout<<fmap[offset];
        }
        std::cout<<std::endl;
    }
}

template <class T>
void PSRoIPool<T>::printProposal(int proposal_id) {
    if(proposal_id < 0 || proposal_id >= proposal_num) {
        std::cout<<"invalid proposal_id\n";
        abort();
    }
    std::array<int, 4> anchor = pp_vec[proposal_id].getAnchor();
    std::array<double, 4> fixed = pp_vec[proposal_id].getBoxFixed();
    std::cout<<"proposal "<<proposal_id<<" : ";
    std::cout<<"anchor( ";
    for(auto x : anchor) std::cout<<std::setiosflags(std::ios::left)<<std::setw(4)<<x;
    std::cout<<")  ";
    std::cout<<"box_regr( ";
    for(auto x : fixed) {
        std::cout.setf(std::ios::fixed);
        std::cout<<std::setprecision(3);
        std::cout<<std::setw(6)<<std::setiosflags(std::ios::left);
        std::cout<<x;
    }
    std::cout<<")"<<std::endl;
}

template <class T>
void PSRoIPool<T>::printRoI(int proposal_id) {
    if(proposal_id < 0 || proposal_id >= proposal_num) {
        std::cout<<"invalid proposal_id\n";
        abort();
    }
    Proposal tmp = pp_vec[proposal_id];
    int roi_start_h = round(tmp.getRoiStartHeight());
    int roi_start_w = round(tmp.getRoiStartWidth());
    int roi_end_h = round(tmp.getRoiEndHeight());
    int roi_end_w = round(tmp.getRoiEndWidth());
    std::cout<<"proposal "<<proposal_id<<" : ";
    std::cout<<roi_start_h<<" "<<roi_start_w<<" "
        <<roi_end_h<<" "<<roi_end_w
        <<std::endl;
}

template <class T>
void PSRoIPool<T>::printOutputFmap(int proposal_id, int channel) {
    if(proposal_id < 0 || proposal_id >= proposal_num) {
        std::cout<<"invalid proposal_id\n";
        abort();
    }
    int addr_base = (proposal_id * roi_pool_h_ * roi_pool_w_ * class_num_);
    std::cout<<"result of proposal "<<proposal_id<<", channel "<<channel<<std::endl;
    std::cout.setf(std::ios::fixed);
    std::cout<<std::setprecision(3);
    for(int i = 0; i < roi_pool_h_; i++) {
        for(int j = 0; j < roi_pool_w_; j++) {
            int offset = (i * roi_pool_w_ + j) * class_num_ + channel;
            std::cout<<std::setw(8)<<std::setiosflags(std::ios::left)<<rlt[addr_base+offset];
        }
        std::cout<<std::endl;
    }
}

template class PSRoIPool<double>;
template class PSRoIPool<int>;

