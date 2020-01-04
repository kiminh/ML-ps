class UpdaterFunc {
 	private:
	UpdaterParam param;
	
	
 	public:

	UpdaterFunc(UpdaterParam param) {
   		this->param = param;
 	  }
	  
 	UpdaterParam getParam() {return param;}
 	virtual void partitionUpdate(PartitionUpdaterParam partParam);
 };
 
