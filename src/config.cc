#include <fstream>
#include <iostream>
#include <iomanip>
#include "config.h"

Config::Config(string conf_path) {
	// check path validation
	parse(conf_path);
}

string Config::trim(const string &line) {
	string str = line;
	string::iterator iter;
	for(iter = str.begin(); iter < str.end(); iter++) {
		if(!isspace(*iter)) break;
	}
	str.erase(0, iter-str.begin());

	string::reverse_iterator riter;
	for(riter = str.rbegin(); riter < str.rend(); riter++) {
		if(!isspace(*riter)) break;
	}
	str.erase(str.rend()-riter, riter-str.rbegin());

	return str;
}

vector<string> Config::split(const string &line, const string &delim) {
	vector<string> rlt;
	string::size_type pos = 0;
	string::size_type start = 0;
	do {
		pos = line.find_first_of(delim, start);
		int len = (pos == string::npos) ? (line.size()-start) : (pos-start);
		if(len) {
			string substr = trim(line.substr(start, len));
			if(!substr.empty()) rlt.emplace_back(substr);
		}
		start = pos + 1;
	} while(pos != string::npos);
	
	return rlt;
}
   	
void Config::parse(string file) {
	std::ifstream ifs(file);
	if(ifs.fail()) {
		std::cout<<"can't open config file: "<<file<<std::endl;
		abort();
	}
	while(!ifs.eof()) {
		string line;
		getline(ifs, line);
		line = trim(line);
		if((line.size() == 0) || (line[0] == '#')) continue;
		vector<string> vec = split(line, ":");
		if(vec.size() == 1) continue;
		else if(vec.size() < 1 || vec.size() > 2) {
			std::cout<<"invalid config item"<<std::endl;
			abort();
		}
		else {
			string item = vec[0];
			if(!item.compare("image_h")) image_h = std::stoul(vec[1]); 
			else if(!item.compare("image_w")) image_w = std::stoul(vec[1]);
			else if(!item.compare("spatial_scale")) spatial_scale = std::stod(vec[1]);
			else if(!item.compare("roi_pool_h")) roi_pool_h = std::stoul(vec[1]);
			else if(!item.compare("roi_pool_w")) roi_pool_w = std::stoul(vec[1]);
			else if(!item.compare("roi_channel")) roi_channel = std::stoul(vec[1]);
			else if(!item.compare("proposal_num")) proposal_num = std::stoul(vec[1]);
			else if(!item.compare("pool_type")) pool_type = vec[1];
			else if(!item.compare("bisample_num")) bisample_num = std::stoul(vec[1]);
		}
	}
}

void Config::printCfg() const {
	using std::cout;
	using std::endl;
	using std::setw;
	using std::setiosflags;
	cout<<setiosflags(std::ios::left);
	cout<<setw(15)<<"image_h" << ": " << image_h << endl
		<<setw(15)<<"image_w" << ": " << image_w << endl
		<<setw(15)<<"spatial_scale" << ": " << spatial_scale << endl
		<<setw(15)<<"roi_pool_h" << ": " << roi_pool_h << endl
		<<setw(15)<<"roi_pool_w" << ": " << roi_pool_w << endl
		<<setw(15)<<"roi_channel" << ": " << roi_channel << endl
		<<setw(15)<<"proposal_num" << ": " << proposal_num << endl
		<<setw(15)<<"pool_type" << ": " << pool_type << endl
		<<setw(15)<<"bisample_num" << ": " << bisample_num << endl;
}



