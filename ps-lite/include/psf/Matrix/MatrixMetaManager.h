/**
 * The matrix meta manager.
 */

class MatrixMetaManager{


  /**
   * Matrix id to matrix meta map
   */
  private:
  
  
  std::unordered_map<int, MatrixMeta> matrixIdToMetaMap;

  /**
   * Matrix name to matrix id map
   */
   
  std::unordered_map<std::string, int> matrixNameToIdMap;

 


  /**
   * Creates a new matrix meta manager.
   */
  public:
  
  
  MatrixMetaManager() { }

  /**
   * Add matrixes.
   *
   * @param matrixMetas the matrix metas
   */
 
 void addMatrices(std::vector<MatrixMeta> matrixMetas) {

    int size = matrixMetas.size();
    for (int i = 0; i < size; i++) {
      addMatrix(matrixMetas[i]);
    }
  
  

  }

  /**
   * Add matrix.
   *
   * @param matrixMeta the matrix meta
   */
 void addMatrix(MatrixMeta matrixMeta) {
	 
    this->matrixIdToMetaMap[matrixMeta.getId()]= matrixMeta;
    this->matrixNameToIdMap[matrixMeta.getName()]= matrixMeta.getId();

  
  }

  /**
   * Remove a matrix
   *
   * @param matrixId matrix id
   */
  void removeMatrix(int matrixId) {

    MatrixMeta meta = matrixIdToMetaMap.remove(matrixId);

    if (meta != null) {
      matrixNameToIdMap.remove(meta.getName());
    }

  }


  /**
   * Gets matrix id.
   *
   * @param matrixName the matrix name
   * @return the matrix id
   */
  int getMatrixId(String matrixName) {

    if (matrixNameToIdMap.count(matrixName)) {

      return matrixNameToIdMap[matrixName];

    } else {

      return -1;

    }

  }


  /**
   * Gets matrix meta.
   *
   * @param matrixId the matrix id
   * @return the matrix meta
   */
   
  MatrixMeta getMatrixMeta(int matrixId) {

    return matrixIdToMetaMap[matrixId];

  }

  /**
   * Gets matrix meta.
   *
   * @param matrixName the matrix name
   * @return the matrix meta
   */
 MatrixMeta getMatrixMeta(String matrixName) {

    int matrixId = getMatrixId(matrixName);

    if (matrixId == -1) {
      return null;
    } else {
      return matrixIdToMetaMap.get(matrixId);
    }
  }

  /**
   * Gets matrix ids.
   *
   * @return the matrix ids
   */
  Set<Integer> getMatrixIds() {
    return matrixIdToMetaMap.keySet();
  }

  /**
   * Gets matrix names.
   *
   * @return the matrix names
   */
  Set<String> getMatrixNames() {
    return matrixNameToIdMap.keySet();
  }

  /**
   * Is a matrix exist
   *
   * @param matrixName matrix name
   * @return true means exist
   */
   bool exists(std::string matrixName) {

    return matrixNameToIdMap.count(matrixName)==1;

  }


  /**
   * Is a matrix exist
   *
   * @param matrixId matrix id
   * @return true means exist
   */
  bool exists(int matrixId) {
    return matrixIdToMetaMap.count(matrixId)==1;
  }

  /**
   * Get all matrices meta
   *
   * @return all matrices meta
   */
   std::unordered_map<int, MatrixMeta>& getMatrixMetas() {

    return matrixIdToMetaMap;

  }

  /**
   * Get the stored pss for a matrix partition
   *
   * @param matrixId matrix id
   * @param partId   partition id
   * @return the stored pss
   */
  public List<ParameterServerId> getPss(int matrixId, int partId) {
    MatrixMeta matrixMeta = matrixIdToMetaMap.get(matrixId);
    if (matrixMeta == null) {
      return null;
    }

    return matrixMeta.getPss(partId);
  }

  /**
   * Get the stored pss for a matrix
   *
   * @param matrixId matrix id
   * @return the stored pss
   */
  public List<ParameterServerId> getPss(int matrixId) {
    MatrixMeta matrixMeta = matrixIdToMetaMap.get(matrixId);
    if (matrixMeta == null) {
      return null;
    }

    return matrixMeta.getPss();
  }

  /**
   * Remove the matrix
   *
   * @param matrixName matrix name
   */
  public void removeMatrix(String matrixName) {
    if (matrixNameToIdMap.containsKey(matrixName)) {
      int matrixId = matrixNameToIdMap.remove(matrixName);
      matrixIdToMetaMap.remove(matrixId);
    }
  }

  /**
   * Remove all matrices
   */
  public void clear() {
    matrixIdToMetaMap.clear();
    matrixNameToIdMap.clear();
  }

  /**
   * Get the master stored ps for a matrix partition
   *
   * @param matrixId matrix id
   * @param partId   partition id
   * @return the master stored ps
   */
  public ParameterServerId getMasterPs(int matrixId, int partId) {
    MatrixMeta matrixMeta = matrixIdToMetaMap.get(matrixId);
    if (matrixMeta == null) {
      return null;
    }

    return matrixMeta.getMasterPs(partId);
  }

  /**
   * Remove matrices
   *
   * @param matrixIds matrix ids
   */
  public void removeMatrices(List<Integer> matrixIds) {
    int size = matrixIds.size();
    for (int i = 0; i < size; i++) {
      removeMatrix(matrixIds.get(i));
    }
  }

  /**
   * Remove the stored ps for all matrix partitions
   *
   * @param psId ps id
   */
  public void removePs(ParameterServerId psId) {
    for (MatrixMeta matrixMeta : matrixIdToMetaMap.values()) {
      matrixMeta.removePs(psId);
    }
  }

  /**
   * Set the stored pss for a matrix partition
   *
   * @param matrixId matrix id
   * @param partId   partition id
   * @param psIds    the stored pss
   */
  public void setPss(int matrixId, int partId, List<ParameterServerId> psIds) {
    MatrixMeta matrixMeta = matrixIdToMetaMap.get(matrixId);
    if (matrixMeta == null) {
      return;
    }

    matrixMeta.setPss(partId, psIds);
  }

  /**
   * Add a the stored ps for a matrix partition
   *
   * @param matrixId matrix id
   * @param partId
   * @param psId     ps id
   */
  public void addPs(int matrixId, int partId, ParameterServerId psId) {
    MatrixMeta matrixMeta = matrixIdToMetaMap.get(matrixId);
    if (matrixMeta == null) {
      return;
    }

    matrixMeta.addPs(partId, psId);
  }

  /**
   * Get estimate sparsity
   *
   * @param matrixId matrix id
   * @return estimate sparsity
   */
  public double getEstSparsity(int matrixId) {
    MatrixMeta meta = matrixIdToMetaMap.get(matrixId);
    if (meta == null) {
      return 1.0;
    } else {
      return meta.getEstSparsity();
    }
  }

};
