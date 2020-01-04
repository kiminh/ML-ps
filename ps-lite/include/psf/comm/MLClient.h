
#include "psf/psf/base/get/GetResult.h"
#include "psf/psf/base/get/GetFunc.h"
#include "psf/comm/UserRequestAdapter.h"
#include "psf/Matrix/PSMatrixMetaManager.h"
#include "ps/kv_app.h"
#include "ps/simple_app.h"


//MLClient.cc
class MLClient:public SimpleApp{

 private:

  UserRequestAdapter* adapter;

 // PSAgentMatrixMetaManager* matrix_meta_manager;

   
  public:

   /** avoid too many this-> */
  using SimpleApp::obj_;
  /**
   * \brief callback function for \ref Push and \ref Pull
   *
   * It is called by the data receiving thread of this instance when the push or
   * pull is actually finished. Namely the kv pairs have already written into
   * servers' data structure or the kv pairs have already pulled back.
   */
  using Callback = std::function<void()>;



 explicit MLClient(int app_id, int customer_id) : SimpleApp() {

    using namespace std::placeholders;
    
    slicer_ = std::bind(&MLClient::DefaultSlicer, this, _1, _2, _3);

    obj_ = new Customer(app_id, customer_id, std::bind(&MLClient::Process, this, _1));
    
    // add for psf    
    adapter = new UserRequestAdapter();
   // matrix_meta_manager = new PSAgentMatrixMetaManager();

  }

  /** \brief deconstructor */
  virtual ~MLClient() {

    delete obj_; obj_ = nullptr;
    if(adapter)delete adapter;
   // if(matrix_meta_manager) delete matrix_meta_manager;

    }


  GetResult get(GetFunc func) {
  
//    checkNotNull(func, "func");

 //   try {
      
     return adapter->get(func);
    
//	} catch (Throwable x) {
//      throw new AngelException(x);
//    }
  
}




  /**
   * \brief internal pull, C/D can be either SArray or std::vector
   */
  template <typename C, typename D>
  int AddPullCB(const SArray<Key>& keys, C* vals, D* lens,
            int cmd, const Callback& cb);

  /**
   * \brief add a callback for a request. threadsafe.
   * @param cb callback
   * @param timestamp the timestamp of the request
   */

  void AddCallback(int timestamp, const Callback& cb) {

    if (!cb) return;

    std::lock_guard<std::mutex> lk(mu_);

    callbacks_[timestamp] = cb;

  }


  /**
   * \brief run and delete the callback
   * \param timestamp the timestamp of the callback
   */
  void RunCallback(int timestamp);
  /**
   * \brief send the kv list to all servers
   * @param timestamp the timestamp of the request
   * @param push whether or not it is a push request
   * @param push whether or not it is a pull request
   * @param cmd command
   */
  void Send(int timestamp, bool push, bool pull, int cmd, const KVPairs<Val>& kvs);



  using SlicedKVs = std::vector<std::pair<bool, KVPairs<Val>>>;


  /**
   * \brief Waits until a push or pull has been finished
   *
   * Sample usage:
   * \code
   *   int ts = w.Pull(keys, &vals);
   *   Wait(ts);
   *   // now vals is ready for use
   * \endcode
   *
   * \param timestamp the timestamp returned by the push or pull
   */
  void Wait(int timestamp) { obj_->WaitRequest(timestamp); }


  void Process(const Message& msg);

  /** \brief default kv slicer */
  void DefaultSlicer(const KVPairs<Val>& send,
                     const std::vector<Range>& ranges,
                     SlicedKVs* sliced);


  /** \brief data buffer for received kvs for each timestamp */
  std::unordered_map<int, std::vector<KVPairs<Val>>> recv_kvs_;

  /** \brief callbacks for each timestamp */
  std::unordered_map<int, Callback> callbacks_;

  /** \brief lock */
  std::mutex mu_;
  /** \brief kv list slicer */
  Slicer slicer_;

  
  
};





  
