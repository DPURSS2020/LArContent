/**
 *  @file   larpandoracontent/LArTwoDReco/LArClusterMopUp/SlidingConeClusterMopUpAlgorithm.h
 * 
 *  @brief  Header file for the sliding cone cluster mop up algorithm class.
 * 
 *  $Log: $
 */
#ifndef LAR_SLIDING_CONE_CLUSTER_MOP_UP_ALGORITHM_H
#define LAR_SLIDING_CONE_CLUSTER_MOP_UP_ALGORITHM_H 1

#include "larpandoracontent/LArThreeDReco/LArPfoMopUp/PfoMopUpBaseAlgorithm.h"

#include <unordered_map>

namespace lar_content
{

/**
 *  @brief  SlidingConeClusterMopUpAlgorithm class
 */
class SlidingConeClusterMopUpAlgorithm : public PfoMopUpBaseAlgorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

    /**
     *  @brief  Default constructor
     */
    SlidingConeClusterMopUpAlgorithm();

private:
    /**
     *  @brief  ClusterMerge class
     */
    class ClusterMerge
    {
    public:
        /**
         *  @brief  Constructor
         * 
         *  @param  pParentCluster the address of the candidate parent (shower) cluster
         *  @param  boundedFraction1 the bounded fraction for algorithm-specified cone angle 1
         *  @param  boundedFraction2 the bounded fraction for algorithm-specified cone angle 2
         */
        ClusterMerge(const pandora::Cluster *const pParentCluster, const float boundedFraction1, const float boundedFraction2);

        /**
         *  @brief  Get the address of the candidate parent (shower) cluster
         *
         *  @return the address of the candidate parent (shower) cluster
         */
        const pandora::Cluster *GetParentCluster() const;

        /**
         *  @brief  Get the bounded fraction for algorithm-specified cone angle 1
         *
         *  @return the bounded fraction for algorithm-specified cone angle 1
         */
        float GetBoundedFraction1() const;

        /**
         *  @brief  Get the bounded fraction for algorithm-specified cone angle 2
         *
         *  @return the bounded fraction for algorithm-specified cone angle 2
         */
        float GetBoundedFraction2() const;

        /**
         *  @brief  operator <
         * 
         *  @param  rhs object for comparison
         * 
         *  @return boolean
         */
        bool operator<(const ClusterMerge &rhs) const;

    private:
        const pandora::Cluster *m_pParentCluster;       ///< The address of the candidate parent (shower) cluster
        float                   m_boundedFraction1;     ///< The bounded fraction for algorithm-specified cone angle 1
        float                   m_boundedFraction2;     ///< The bounded fraction for algorithm-specified cone angle 2
    };

    typedef std::vector<ClusterMerge> ClusterMergeList;

    pandora::StatusCode Run();

    /**
     *  @brief  Get the neutrino interaction vertex if it is available and if the algorithm is configured to do so
     * 
     *  @param  pVertex to receive the neutrino interaction vertex
     */
    void GetInteractionVertex(const pandora::Vertex *&pVertex) const;

    typedef std::unordered_map<const pandora::Cluster*, const pandora::ParticleFlowObject*> ClusterToPfoMap;

    /**
     *  @brief  Get all 3d clusters contained in the input pfo lists and a mapping from clusters to pfos
     * 
     *  @param  clusters3D to receive the sorted list of 3d clusters
     *  @param  clusterToPfoMap to receive the mapping from 3d cluster to pfo
     */
    void GetThreeDClusters(pandora::ClusterVector &clusters3D, ClusterToPfoMap &clusterToPfoMap) const;

    typedef std::unordered_map<const pandora::Cluster*, ClusterMergeList> ClusterMergeMap;

    /**
     *  @brief  Get the cluster merge map describing all potential 3d cluster merges
     * 
     *  @param  pVertex the neutrino interaction vertex, if available
     *  @param  clusters3D the sorted list of 3d clusters
     *  @param  clusterToPfoMap the mapping from 3d cluster to pfo
     *  @param  clusterMergeMap to receive the populated cluster merge map
     */
    void GetClusterMergeMap(const pandora::Vertex *const pVertex, const pandora::ClusterVector &clusters3D, const ClusterToPfoMap &clusterToPfoMap,
        ClusterMergeMap &clusterMergeMap) const;

    /**
     *  @brief  Make pfo merges based on the provided cluster merge map
     * 
     *  @param  clusterToPfoMap the mapping from 3d cluster to pfo
     *  @param  clusterMergeMap the populated cluster merge map
     * 
     *  @return whether a pfo merge has been made
     */
    bool MakePfoMerges(const ClusterToPfoMap &clusterToPfoMap, const ClusterMergeMap &clusterMergeMap) const;

    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    typedef std::unordered_map<const pandora::Cluster*, const pandora::Cluster*> ClusterReplacementMap;

    pandora::StringVector   m_inputPfoListNames;        ///< The input pfo list names
    bool                    m_useVertex;                ///< Whether to use the interaction vertex to select useful cone directions
    unsigned int            m_maxIterations;            ///< The maximum allowed number of algorithm iterations
    unsigned int            m_maxHitsToConsider3DTrack; ///< The maximum number of hits in a 3d track cluster to warrant inclusion in algorithm
    unsigned int            m_minHitsToConsider3DShower;///< The minimum number of hits in a 3d shower cluster to attempt cone fits
    unsigned int            m_halfWindowLayers;         ///< The number of layers to use for half-window of sliding fit
    unsigned int            m_nConeFitLayers;           ///< The number of layers over which to sum fitted direction to obtain cone fit
    unsigned int            m_nConeFits;                ///< The number of cone fits to perform, spread roughly uniformly along the shower length
    float                   m_coneLengthMultiplier;     ///< The cone length multiplier to use when calculating bounded cluster fractions
    float                   m_maxConeLength;            ///< The maximum allowed cone length to use when calculating bounded cluster fractions
    float                   m_coneTanHalfAngle1;        ///< The cone tan half angle to use when calculating bounded cluster fractions 1
    float                   m_coneBoundedFraction1;     ///< The minimum cluster bounded fraction for association 1
    float                   m_coneTanHalfAngle2;        ///< The cone tan half angle to use when calculating bounded cluster fractions 2
    float                   m_coneBoundedFraction2;     ///< The minimum cluster bounded fraction for association 2
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *SlidingConeClusterMopUpAlgorithm::Factory::CreateAlgorithm() const
{
    return new SlidingConeClusterMopUpAlgorithm();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline SlidingConeClusterMopUpAlgorithm::ClusterMerge::ClusterMerge(const pandora::Cluster *const pParentCluster, const float boundedFraction1, const float boundedFraction2) :
    m_pParentCluster(pParentCluster),
    m_boundedFraction1(boundedFraction1),
    m_boundedFraction2(boundedFraction2)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const pandora::Cluster *SlidingConeClusterMopUpAlgorithm::ClusterMerge::GetParentCluster() const
{
    return m_pParentCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float SlidingConeClusterMopUpAlgorithm::ClusterMerge::GetBoundedFraction1() const
{
    return m_boundedFraction1;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float SlidingConeClusterMopUpAlgorithm::ClusterMerge::GetBoundedFraction2() const
{
    return m_boundedFraction2;
}

} // namespace lar_content

#endif // #ifndef LAR_SLIDING_CONE_CLUSTER_MOP_UP_ALGORITHM_H
