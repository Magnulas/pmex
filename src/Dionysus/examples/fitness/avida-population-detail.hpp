#include <fstream>
#include <sstream>
#include "utilities/log.h"

/** AvidaOrganismDetail */

AvidaOrganismDetail::
AvidaOrganismDetail(std::string line)
{
	std::stringstream linestream(line);
	linestream >> id_ >> parent_id_ >> parent_distance_;
	linestream >> num_organisms_alive_ >> num_organisms_ever_;
	linestream >> genome_length_;
	linestream >> merit_ >> gestation_time_;
	linestream >> fitness_;
	linestream >> update_born_ >> update_deactivated_ >> depth_phylogenetic_tree_;
	linestream >> genome_;

	AssertMsg(genome_length_ == genome_.size(), "Genome must be of given length");
}

AvidaOrganismDetail::DistanceType
AvidaOrganismDetail::
genome_distance(const AvidaOrganismDetail& other) const
{
	AssertMsg(genome_.size() == other.genome_.size(), "Currently genome sizes must be the same for distance computation");
	AssertMsg(genome_length_ == genome_.size(), "Genome length value must match the length of the genome string");
	AssertMsg(other.genome_length_ == other.genome_.size(), "Genome length value must match the length of the genome string");

	CountType count = 0;
	for (CountType i = 0; i < genome_.size(); ++i)
		if (genome_[i] != other.genome_[i])
			++count;

	return count;
}


/** AvidaPopulationDetail */
AvidaPopulationDetail::
AvidaPopulationDetail(std::string filename)
{
	std::ifstream infile(filename.c_str());
	while(infile)
	{
		std::string line;
		std::getline(infile, line);
		
		// Skip comments and empty lines
		char c = '#';
		std::istringstream linestream(line);
		linestream >> c;
		if (c == '#') continue;

		organisms_.push_back(AvidaOrganismDetail(line));
	}
}
