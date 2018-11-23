#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

class Config {
	public:
		int getImageHeight() const {return image_h;}
		int getImageWidth() const {return image_w;}
		double getSpatialScale() const {return spatial_scale;}
		int getRoiPoolHeight() const {return roi_pool_h;}
		int getRoiPoolWidth() const {return roi_pool_w;}
		int getRoiChannel() const {return roi_channel;}
		int getProposalNum() const {return proposal_num;}
		string getPoolType() const {return pool_type;}
		int getBisampleNum() const {return bisample_num;}
		int getOrigProposalNum() const {return orig_proposal_num;}
		double getNmsScoreThreshold() const {return nms_score_threshold;}
		double getNmsIouThreshold() const {return nms_iou_threshold;}

		static Config & getInst() {
			static Config inst;
			return inst;
		}

		void printCfg() const;

	private:
		int image_h;
		int image_w;
		double spatial_scale;
		int roi_pool_h;
		int roi_pool_w;
		int roi_channel;
		int proposal_num;
		int orig_proposal_num;
		double nms_score_threshold;
		double nms_iou_threshold;
		string pool_type;
		int bisample_num;
		
		void parse(string file);
		string trim(const string &line);
		vector<string> split(const string &line, const string &delim = " ,:;\t");
		Config(string conf_path = "config/config.txt");
};

#endif

