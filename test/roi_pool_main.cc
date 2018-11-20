#include <iostream>
#include <iomanip>
#include <sys/time.h>
#include "config.h"
#include "proposal.h"
#include "roiPool.h"

int main()
{
	Config::getInst().printCfg();
	int proposal_num = Config::getInst().getProposalNum();
	RoIPool<double> inst(proposal_num);

	inst.genInputFmap();
	inst.printInputFmap(0);

	inst.genProposal();
	for(int i = 0; i < 1; i++) inst.printProposal(i);

	timeval t1, t2;
	double time_used;

	std::cout.setf(std::ios::fixed);
	std::cout<<std::setprecision(6);

	gettimeofday(&t1, nullptr);
	inst.calcProposal();
	gettimeofday(&t2, nullptr);
	time_used = (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);
	std::cout<<"calculate RoI time: " << time_used/1000000 << std::endl;

	for(int i = 0; i < 1; i++) inst.printRoI(i);

	gettimeofday(&t1, nullptr);
	inst.calcPooling();
	gettimeofday(&t2, nullptr);
	time_used = (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);
	std::cout<<"calculate pool time: " << time_used/1000000 << std::endl;

	for(int i = 0; i < 1; i++) inst.printOutputFmap(i, 0);

	return 0;
}

