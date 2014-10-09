/**
 * Author: Dmitriy Morozov
 * Department of Computer Science, Duke University, 2007
 */

#ifndef __AVIDA_POPULATION_DETAIL_H__
#define __AVIDA_POPULATION_DETAIL_H__

#include <string>
#include <vector>

/**
 * Stores organism details stored in a single line of a population detail file in data/ 
 * directory of Avida's output.
 */
class AvidaOrganismDetail
{
	public:
		typedef			int							IDType;
		/// Distance between two genomes
		typedef			unsigned int				DistanceType;
		typedef			unsigned int				CountType;

						AvidaOrganismDetail(std::string line);

		DistanceType 	genome_distance(const AvidaOrganismDetail& other) const;
		
		IDType			id() const									{ return id_; }
		float			fitness() const								{ return fitness_; }
		CountType		length() const								{ return genome_length_; }
		std::string		genome() const								{ return genome_; }

	private:
		IDType			id_, parent_id_;
		int				parent_distance_;
		CountType		num_organisms_alive_, num_organisms_ever_;
		CountType		genome_length_;
		float 			merit_, gestation_time_;
		float 			fitness_;
		int 			update_born_, update_deactivated_, depth_phylogenetic_tree_;
		std::string 	genome_;
};

/**
 * Stores entire population details (all organisms in a given time step), i.e., stores
 * an entire population detail file in data/ directory of Avida's ouptut.
 */
class AvidaPopulationDetail
{
	public:
		typedef			std::vector<AvidaOrganismDetail>				OrganismVector;
        typedef         OrganismVector::const_iterator                  OrganismIndex;

						AvidaPopulationDetail(std::string filename);

		const OrganismVector& 
                        get_organisms() const						    { return organisms_; }

        /// \name Rips
        /// @{
        typedef         int                                             IndexType;
        typedef         double                                          DistanceType;

        DistanceType    operator()(IndexType a, IndexType b) const      { return organisms_[a].genome_distance(organisms_[b]); }

        size_t          size() const                                    { return organisms_.size(); }
        IndexType       begin() const                                   { return 0; }
        IndexType       end() const                                     { return size(); }
        /// @}

	private:
		OrganismVector	organisms_;
};

std::ostream& operator<<(std::ostream& out, AvidaPopulationDetail::OrganismIndex i)
{ return (out << (i->id())); }


#include "avida-population-detail.hpp"

#endif //__AVIDA_POPULATION_DETAIL_H__
