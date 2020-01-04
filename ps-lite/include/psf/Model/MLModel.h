class MLModel(conf: Configuration, _ctx: TaskContext = null) {
	
  implicit def ctx: TaskContext = _ctx


  private ：

   std::unordered_map<std::string, PSModel> psModels;

  /**
    * Get all PSModels
    *
    * @return a name to PSModel map
    */
  std::unordered_map<std::string,PSModel> getPSModels {
    return psModels
  }

  /**
    * Get a PSModel use name. With this method, user can refer to one PSModel simply with mlModel.psModelName
    *
    * @param name PSModel name
    * @return
    */
  PSModel getPSModel(std::string name){
    return psModels[name];
  }

  /**
    * Add a new PSModel
    *
    * @param name    PSModel name
    * @param psModel PSModel
    */
  void addPSModel(std::string name, PSModel PsModel){
    psModels[name]=psModel;
  }

  /**
    * Add a new PSModel
    *
    * @param psModel PSModel
    */
  void addPSModel(PSModel psModel){
	  
    psModels[psModel.modelName]=psModel;
  }

  /**
    * Predict use the PSModels and predict data
    *
    * @param storage predict data
    * @return predict result
    */
  def predict(storage: DataBlock[LabeledData]): DataBlock[PredictResult]

  /**
    * Set save path for the PSModels
    *
    * @param conf Application configuration
    */
  void setSavePath(conf: Configuration){
	  
    val path = conf.get(AngelConf.ANGEL_SAVE_MODEL_PATH)
	
	for(auto &e：psModels){
		
		if(e.second.needSave)e.second.setSavePath(path)
		
		
	}
	
	
	
/*     if (path != null)
      psModels.values().foreach { case model: PSModel =>
        if (model.needSave) model.setSavePath(path)
      } */
   
}

  /**
    * Set PSModels load path
    *
    * @param conf Application configuration
    */
  void setLoadPath(conf: Configuration){
    
	val path = conf.get(AngelConf.ANGEL_LOAD_MODEL_PATH)
	
	for(auto &e：psModels){
		
		if(e.second.needSave)e.second.setLoadPath(path)
		
		
	}
	
	
/*     if (path != null)
      psModels.values().foreach { case model: PSModel =>
        if (model.needSave) model.setLoadPath(path)
      } */

  }
  
  

};
