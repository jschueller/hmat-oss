/*
  HMat-OSS (HMatrix library, open source software)

  Copyright (C) 2014-2015 Airbus Group SAS

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  http://github.com/jeromerobert/hmat-oss
*/

#include "hmat/hmat.h"
#include "hmat_cpp_interface.hpp"
#include "default_engine.hpp"
#include "admissibility.hpp"
#include "c_wrapping.hpp"
#include "common/my_assert.h"
hmat_cluster_tree_t * hmat_create_cluster_tree(DofCoordinate* dls, int n) {
    return (hmat_cluster_tree_t*) createClusterTree(dls, n);
}

void hmat_delete_cluster_tree(hmat_cluster_tree_t * tree) {
    delete ((ClusterTree*)tree);
}

hmat_cluster_tree_t * hmat_copy_cluster_tree(hmat_cluster_tree_t * tree) {
    return (hmat_cluster_tree_t*)((ClusterTree*)tree)->copy();
}

int hmat_tree_nodes_count(hmat_cluster_tree_t * tree)
{
    return ((ClusterTree*)tree)->nodesCount();
}

hmat_admissibility_t* hmat_create_admissibility_standard(double eta)
{
    return static_cast<hmat_admissibility_t*>((void*) new StandardAdmissibilityCondition(eta));
}

hmat_admissibility_t* hmat_create_admissibility_influence_radius(int length, double* radii)
{
    return static_cast<hmat_admissibility_t*>((void*) new InfluenceRadiusCondition(length, radii));
}

void hmat_delete_admissibility(hmat_admissibility_t * cond) {
    delete static_cast<AdmissibilityCondition*>((void*)cond);
}

void hmat_init_default_interface(hmat_interface_t * i, hmat_value_t type)
{
    switch (type) {
    case HMAT_SIMPLE_PRECISION: createCInterface<S_t, DefaultEngine>(i); break;
    case HMAT_DOUBLE_PRECISION: createCInterface<D_t, DefaultEngine>(i); break;
    case HMAT_SIMPLE_COMPLEX: createCInterface<C_t, DefaultEngine>(i); break;
    case HMAT_DOUBLE_COMPLEX: createCInterface<Z_t, DefaultEngine>(i); break;
    default: strongAssert(false);
    }
}

void hmat_get_parameters(hmat_settings_t* settings)
{
    HMatSettings& settingsCxx = HMatSettings::getInstance();
    settings->assemblyEpsilon = settingsCxx.assemblyEpsilon;
    settings->recompressionEpsilon = settingsCxx.recompressionEpsilon;
    switch (settingsCxx.compressionMethod) {
    case Svd:
      settings->compressionMethod = hmat_compress_svd;
      break;
    case AcaFull:
      settings->compressionMethod = hmat_compress_aca_full;
      break;
    case AcaPartial:
      settings->compressionMethod = hmat_compress_aca_partial;
      break;
    case AcaPlus:
      settings->compressionMethod = hmat_compress_aca_plus;
      break;
    case RkNull:
      settings->compressionMethod = hmat_compress_rk_null;
      break;
    default:
      std::cerr << "Internal error: invalid value for compression method: \"" << settingsCxx.compressionMethod << "\"." << std::endl;
      std::cerr << "Internal error: using SVD" << std::endl;
      settings->compressionMethod = hmat_compress_svd;
      break;
    }
    settings->admissibilityCondition = static_cast<hmat_admissibility_t*>((void*)settingsCxx.admissibilityCondition);
    settings->admissibilityFactor = 0.0;
    switch (settingsCxx.clustering) {
    case kGeometric:
      settings->clustering = hmat_cluster_geometric;
      break;
    case kMedian:
      settings->clustering = hmat_cluster_median;
      break;
    case kHybrid:
      settings->clustering = hmat_cluster_hybrid;
      break;
    default:
      std::cerr << "Internal error: invalid value for clustering method: \"" << settingsCxx.clustering << "\"." << std::endl;
      std::cerr << "Internal error: using median" << std::endl;
      settings->clustering = hmat_cluster_median;
      break;
    }
    settings->compressionMinLeafSize = settingsCxx.compressionMinLeafSize;
    settings->maxLeafSize = settingsCxx.maxLeafSize;
    settings->maxParallelLeaves = settingsCxx.maxParallelLeaves;
    settings->elementsPerBlock = settingsCxx.elementsPerBlock;
    settings->useLu = settingsCxx.useLu;
    settings->useLdlt = settingsCxx.useLdlt;
    settings->coarsening = settingsCxx.coarsening;
    settings->recompress = settingsCxx.recompress;
    settings->validateCompression = settingsCxx.validateCompression;
    settings->validationErrorThreshold = settingsCxx.validationErrorThreshold;
    settings->validationReRun = settingsCxx.validationReRun;
    settings->validationDump = settingsCxx.validationDump;
}

int hmat_set_parameters(hmat_settings_t* settings)
{
    strongAssert(settings != NULL);
    int rc = 0;
    HMatSettings& settingsCxx = HMatSettings::getInstance();
    settingsCxx.assemblyEpsilon = settings->assemblyEpsilon;
    settingsCxx.recompressionEpsilon = settings->recompressionEpsilon;
    switch (settings->compressionMethod) {
    case hmat_compress_svd:
      settingsCxx.compressionMethod = Svd;
      break;
    case hmat_compress_aca_full:
      settingsCxx.compressionMethod = AcaFull;
      break;
    case hmat_compress_aca_partial:
      settingsCxx.compressionMethod = AcaPartial;
      break;
    case hmat_compress_aca_plus:
      settingsCxx.compressionMethod = AcaPlus;
      break;
    case hmat_compress_rk_null:
      settingsCxx.compressionMethod = RkNull;
      break;
    default:
      std::cerr << "Invalid value for compression method: \"" << settings->compressionMethod << "\"." << std::endl;
      rc = 1;
      break;
    }
    settingsCxx.compressionMinLeafSize = settings->compressionMinLeafSize;
    if (settings->admissibilityFactor != 0.0)
      settingsCxx.admissibilityCondition = new StandardAdmissibilityCondition(settings->admissibilityFactor);
    else
      settingsCxx.admissibilityCondition = static_cast<AdmissibilityCondition*>((void*)settings->admissibilityCondition);
    switch (settings->clustering) {
    case hmat_cluster_geometric:
      settingsCxx.clustering = kGeometric;
      break;
    case hmat_cluster_median:
      settingsCxx.clustering = kMedian;
      break;
    case hmat_cluster_hybrid:
      settingsCxx.clustering = kHybrid;
      break;
    default:
      std::cerr << "Invalid value for clustering method: \"" << settings->clustering << "\"." << std::endl;
      rc = 1;
      break;
    }
    settingsCxx.maxLeafSize = settings->maxLeafSize;
    settingsCxx.maxParallelLeaves = settings->maxParallelLeaves;
    settingsCxx.elementsPerBlock = settings->elementsPerBlock;
    settingsCxx.useLu = settings->useLu;
    settingsCxx.useLdlt = settings->useLdlt;
    settingsCxx.coarsening = settings->coarsening;
    settingsCxx.recompress = settings->recompress;
    settingsCxx.validateCompression = settings->validateCompression;
    settingsCxx.validationErrorThreshold = settings->validationErrorThreshold;
    settingsCxx.validationReRun = settings->validationReRun;
    settingsCxx.validationDump = settings->validationDump;
    settingsCxx.setParameters();
    settingsCxx.printSettings();
    return rc;
}

const char * hmat_get_version()
{
    return HMAT_VERSION;
}

const char * hmat_get_build_date()
{
  return "Built on " __DATE__ " at " __TIME__ "" ;
}
