#include "psf/Matrix/MatrixMetaManager.h"
#include "psf/psf/base/PartitionKey.h"
#include <unordered_map>


class PSAgentMatrixMetaManager {

  private:

  /**
   * Matrix meta manager
   */
  MatrixMetaManager*  matrixMetaManager;



  /**
   * Matrix id -> the partitions of the matrix map
   */
   std::unordered_map<Integer, std::vector<PartitionKey>> matrixIdToPartsMap;

  /**
   * Matrix id -> (row index -> contain this row partitions map) map
   */
   std::unordered_map<int, std::unordered_map<int, std::vector<PartitionKey> > > rowIndexToPartsMap;

 


  /**
   * Create PSAgentMatrixMetaManager
   *
   * 
   */
  public:
  
  PSAgentMatrixMetaManager() {

    this->matrixMetaManager = new MatrixMetaManager();

  }

  /**
   * Add matrices meta
   *
   * @param matrixMetas matrices meta
   */
  void addMatrices(std::vector<MatrixMeta> matrixMetas) {

    int size = matrixMetas.size();

    for (int i = 0; i < size; i++) {
      addMatrix(matrixMetas[i]);
    }

  }
  


  /**
   * Add matrix meta
   *
   * @param matrixMeta matrix meta
   */
  void addMatrix(MatrixMeta matrixMeta) {

    matrixMetaManager->addMatrix(matrixMeta);

    std::vector<PartitionKey> partitions = getPartitions(matrixMeta.getId());

  }


  /**
   * Remove matrix meta
   *
   * @param matrixId matrix id
   */
  void removeMatrix(int matrixId) {

    matrixMetaManager->removeMatrix(matrixId);
 
  }



  /**
   * Get the server that hold the partition.
   *
   * @param partitionKey matrix partition key
   * @param sync         true means get from master, false means get from local cache
   * @return ParameterServerId server id
   */

  std::vector<ParameterServerId> getPss(PartitionKey partitionKey) {


    if (!sync) {
      return getPss(partitionKey);
    } else {
      List<ParameterServerId> psIds = PSAgentContext.get().getMasterClient()
        .getStoredPss(partitionKey.getMatrixId(), partitionKey.getPartitionId());
      matrixMetaManager.setPss(partitionKey.getMatrixId(), partitionKey.getPartitionId(), psIds);
      return psIds;
    }

  }



  /**
   * Get partition location: includes stored pss and the location of the pss
   *
   * @param partitionKey partition information
   * @param sync         true means get from master; false means get from cache
   * @return partition location
   * @throws ServiceException
   */

/*
 PartitionLocation getPartLocation(PartitionKey partitionKey, boolean sync)
    throws ServiceException {
    if (!sync) {
      return getPartLocation(partitionKey);
    } else {
      return PSAgentContext.get().getMasterClient()
        .getPartLocation(partitionKey.getMatrixId(), partitionKey.getPartitionId());
    }
  }
*/

  /**
   * Get partition location: includes stored pss and the location of the pss
   *
   * @param partitionKey partition information
   * @return partition location
   * @throws ServiceException
   */
/*
  PartitionLocation getPartLocation(PartitionKey partitionKey) {
	  
    std::vector<ParameterServerId> psIds = getPss(partitionKey);
	
    if (psIds == null) {
      return new PartitionLocation(new ArrayList<>());
    }

    int size = psIds.size();
    List<PSLocation> psLocs = new ArrayList<>(size);
    for (int i = 0; i < size; i++) {
      psLocs.add(new PSLocation(psIds.get(i),
        PSAgentContext.get().getLocationManager().getPsLocation(psIds.get(i))));
    }
	
    return new PartitionLocation(psLocs);
  
  }
*/





  /**
   * Get the server that hold the partition.
   *
   * @param partitionKey matrix partition key
   * @return ParameterServerId server id
   */

  std::vector<ParameterServerId> getPss(PartitionKey partitionKey) {
	  
    return matrixMetaManager->getPss(partitionKey.getMatrixId(), partitionKey.getPartitionId());
	
  }



  /**
   * Get the server that hold the partition.
   *
   * @return ParameterServerId server id
   */
    
  std::vector<ParameterServerId> getPss(int matrixId) {
    return matrixMetaManager->getPss(matrixId);
  }


  /**
   * Get list of partitionKeys for matrixId and rowId.
   *
   * @param matrixId, matrixId
   * @param rowIndex, rowId
   * @return list of partitionKeys
   */
   
// TODO   
  std::vector<PartitionKey> getPartitions(int matrixId, int rowIndex) {
	  
    std::unordered_map<int, std::vector<PartitionKey>> rowPartKeysCache = rowIndexToPartsMap.get(matrixId);
	
    
	if (rowPartKeysCache == null) {
      rowPartKeysCache = new ConcurrentHashMap<>();
      rowIndexToPartsMap.put(matrixId, rowPartKeysCache);
    }
	
    List<PartitionKey> rowParts = rowPartKeysCache.get(rowIndex);
    if (rowParts == null) {
      rowParts = getPartitionsFromMeta(matrixId, rowIndex);

      rowPartKeysCache.put(rowIndex, rowParts);
    }

    return rowParts;
  }


  private:
  

 // TODO 
  std::vector<PartitionKey> getPartitionsFromMeta(int matrixId, int rowIndex) {
	  
    std::vector<PartitionKey> partitionKeys;
	
    
	Iterator<PartitionMeta> iter =
      matrixMetaManager.getMatrixMeta(matrixId).getPartitionMetas().values().iterator();
    
	//sort
	while (iter.hasNext()) {
      PartitionKey partitionKey = iter.next().getPartitionKey();
      if (partitionKey.getMatrixId() == matrixId && partitionKey.getStartRow() <= rowIndex
        && partitionKey.getEndRow() > rowIndex)
        partitionKeys.add(partitionKey);
    }

    // Sort the partitions by start column index
    partitionKeys.sort(new Comparator<PartitionKey>() {
      @Override public int compare(PartitionKey p1, PartitionKey p2) {
        if (p1.getStartCol() < p2.getStartCol()) {
          return -1;
        } else if (p1.getStartCol() > p2.getStartCol()) {
          return 1;
        } else {
          return 0;
        }
      }
    });
	
    return partitionKeys;
  }




  /**
   * Get list of partitionkeys belong to matrixId.
   *
   * @param matrixId, identity number of matrix requested
   * @return list of partitionKeys
   */
  

  std::vector<PartitionKey> getPartitions(int matrixId) {
	  
    std::vector<PartitionKey> partitions = matrixIdToPartsMap[matrixId];

	
    if (partitions == null) {
		
      partitions = getPartitionsFromMeta(matrixId);
      matrixIdToPartsMap.put(matrixId, partitions);
	  
    }

    return partitions;
  }






  private:

//TODO  
  std::vector<PartitionKey> getPartitionsFromMeta(int matrixId) {
	  
    std::vector<PartitionKey> partitionKeys;
	
    Iterator<PartitionMeta> iter =
      matrixMetaManager.getMatrixMeta(matrixId).getPartitionMetas().values().iterator();
    
	while (iter.hasNext()) {
      partitionKeys.add(iter.next().getPartitionKey());
    }
	
	//sort
    partitionKeys.sort((PartitionKey p1, PartitionKey p2) -> {
      if (p1.getStartCol() < p2.getStartCol()) {
        return -1;
      } else if (p1.getStartCol() > p2.getStartCol()) {
        return 1;
      } else {
        return 0;
      }
    });
	
    return partitionKeys;
  }


  /**
   * Get the partitions the rows in.
   *
   * @param matrixId   matrix id
   * @param rowIndexes row indexes
   * @return Map<PartitionKey, List<Integer>> partitions to the rows contained in the partition map
   */
  public:
  
  
  Map<PartitionKey, List<Integer>> getPartitionToRowsMap(int matrixId, List<Integer> rowIndexes) {

    Map<PartitionKey, List<Integer>> partToRowsMap = new HashMap<PartitionKey, List<Integer>>();

    int rowNum = rowIndexes.size();
    int partNum;
    for (int i = 0; i < rowNum; i++) {
      List<PartitionKey> partKeys = getPartitions(matrixId, rowIndexes.get(i));
      partNum = partKeys.size();
      for (int j = 0; j < partNum; j++) {
        List<Integer> rows = partToRowsMap.get(partKeys.get(j));
        if (rows == null) {
          rows = new ArrayList<>();
          partToRowsMap.put(partKeys.get(j), rows);
        }
        rows.add(rowIndexes.get(i));
      }
    }
    return partToRowsMap;
  }



  /**
   * Get the partitions the rows in.
   *
   * @param matrixId matrix id
   * @param rowIds   row indexes
   * @return Map<PartitionKey, List<Integer>> partitions to the rows contained in the partition map
   */
  public Map<PartitionKey, List<Integer>> getPartitionToRowsMap(int matrixId, int[] rowIds) {
    Map<PartitionKey, List<Integer>> partToRowsMap = new HashMap<PartitionKey, List<Integer>>();

    int rowNum = rowIds.length;
    int partNum;
    for (int i = 0; i < rowNum; i++) {
      List<PartitionKey> partKeys = getPartitions(matrixId, rowIds[i]);
      partNum = partKeys.size();
      for (int j = 0; j < partNum; j++) {
        List<Integer> rows = partToRowsMap.get(partKeys.get(j));
        if (rows == null) {
          rows = new ArrayList<>();
          partToRowsMap.put(partKeys.get(j), rows);
        }
        rows.add(rowIds[i]);
      }
    }
    return partToRowsMap;
  }

  /**
   * Get the partitions the rows in.
   *
   * @param rowIndex    matrix id and row indexes
   * @param batchNumber the split batch size.
   * @return Map<PartitionKey, List<Integer>> partitions to the rows contained in the partition map
   */
  public Map<PartitionKey, List<RowIndex>> getPartitionToRowIndexMap(RowIndex rowIndex, int batchNumber) {

    Map<PartitionKey, List<RowIndex>> partToRowIndexMap =
      new TreeMap<PartitionKey, List<RowIndex>>();
    if (rowIndex.getRowIds() == null) {
      return partToRowIndexMap;
    }

    IntOpenHashSet rowIdSet = rowIndex.getRowIds();
    IntOpenHashSet filtedRowIdSet = rowIndex.getFiltedIdSet();
    int[] rowIds = new int[rowIdSet.size() - filtedRowIdSet.size()];
    int count = 0;
    for (int rowId : rowIdSet) {
      if (!filtedRowIdSet.contains(rowId)) {
        rowIds[count++] = rowId;
      }
    }

    Arrays.sort(rowIds);

    int partNum = 0;
    for (int i = 0; i < rowIds.length; i++) {
      List<PartitionKey> partKeys = getPartitions(rowIndex.getMatrixId(), rowIds[i]);
      partNum = partKeys.size();
      for (int j = 0; j < partNum; j++) {
        PartitionKey partitionKey = partKeys.get(j);
        List<RowIndex> indexList = partToRowIndexMap.get(partitionKey);
        if (indexList == null) {
          indexList = new ArrayList<RowIndex>();
          partToRowIndexMap.put(partitionKey, indexList);
          indexList.add(new RowIndex(rowIndex.getMatrixId(), rowIndex));
        }

        RowIndex index = indexList.get(indexList.size() - 1);
        if (index.getRowsNumber() >= batchNumber) {
          index = new RowIndex(rowIndex.getMatrixId(), rowIndex);
          indexList.add(index);
        }
        index.addRowId(rowIds[i]);
      }
    }

    return partToRowIndexMap;
  }




  /**
   * Get the number of the partitions that contains the row.
   *
   * @param matrixId matrix id
   * @param rowIndex row index
   * @return the number of the partitions that contains the row
   */
  int getRowPartitionSize(int matrixId, int rowIndex) {

    return getPartitions(matrixId, rowIndex).size();

  }


  /**
   * Clear the router table.
   */
  void clear() {
    matrixIdToPartsMap.clear();
    rowIndexToPartsMap.clear();
    matrixMetaManager.clear();
  }


  /**
   * Get matrix id
   *
   * @param matrixName matrix name
   * @return matrix id
   */
  int getMatrixId(std::string matrixName) {
	  
    if (matrixMetaManager.exists(matrixName)) {
      return matrixMetaManager.getMatrixId(matrixName);
    } else {
		//add lock
      synchronized (this) {
        if (!matrixMetaManager.exists(matrixName)) {
          try {
            PSAgentContext.get().getPsAgent().refreshMatrixInfo();
          } catch (Throwable e) {
            LOG.error("get matrix information from master failed. ", e);
          }
        }
        if (matrixMetaManager.exists(matrixName)) {
          return matrixMetaManager.getMatrixId(matrixName);
        } else {
          return -1;
        }
      }
	  
    }
  }

  /**
   * Get matrix meta
   *
   * @param matrixId matrix id
   * @return matrix meta
   */
  MatrixMeta getMatrixMeta(int matrixId) {
    return matrixMetaManager.getMatrixMeta(matrixId);
  }

  /**
   * Get matrix meta
   *
   * @param matrixName matrix name
   * @return matrix meta
   */
  MatrixMeta getMatrixMeta(std::string matrixName) {
    return matrixMetaManager.getMatrixMeta(matrixName);
  }

  /**
   * Get the master ps of a partition
   *
   * @param partKey partition key
   * @return the master ps of a partition
   */
  public ParameterServerId getMasterPS(PartitionKey partKey) {
    return matrixMetaManager.getMasterPs(partKey.getMatrixId(), partKey.getPartitionId());
  }

  /**
   * Get all matrices meta
   *
   * @return all matrices meta
   */
  std::unordered_map<int, MatrixMeta> getMatrixMetas() {
    return matrixMetaManager.getMatrixMetas();
  }

  /**
   * Is a matrix exist
   *
   * @param matrixId matrix id
   * @return true means exist
   */
   
   bool exist(int matrixId) {
    
	if (!matrixMetaManager.exists(matrixId)) {
		
	  // add lock
      synchronized (this) {
        if (!matrixMetaManager.exists(matrixId)) {
          try {
            PSAgentContext.get().getPsAgent().refreshMatrixInfo();
          } catch (Throwable e) {
            LOG.error("get matrix information from master failed. ", e);
          }
        }
      }
	  
    }
	
    return matrixMetaManager.exists(matrixId);
  }
  
};

