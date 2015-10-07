//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.7
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------

namespace FiftyOne.Mobile.Detection.Provider.Interop.Trie {

public partial class Provider : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal Provider(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(Provider obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~Provider() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          FiftyOneDegreesTrieV3PINVOKE.delete_Provider(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public Provider(string fileName) : this(FiftyOneDegreesTrieV3PINVOKE.new_Provider__SWIG_0(fileName), true) {
    if (FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Pending) throw FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Retrieve();
  }

  public Provider(string fileName, string propertyString) : this(FiftyOneDegreesTrieV3PINVOKE.new_Provider__SWIG_1(fileName, propertyString), true) {
    if (FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Pending) throw FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Retrieve();
  }

  public Provider(string fileName, VectorString propertiesArray) : this(FiftyOneDegreesTrieV3PINVOKE.new_Provider__SWIG_2(fileName, VectorString.getCPtr(propertiesArray)), true) {
    if (FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Pending) throw FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Retrieve();
  }

  public VectorString getHttpHeaders() {
    VectorString ret = new VectorString(FiftyOneDegreesTrieV3PINVOKE.Provider_getHttpHeaders(swigCPtr), true);
    if (FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Pending) throw FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public VectorString getAvailableProperties() {
    VectorString ret = new VectorString(FiftyOneDegreesTrieV3PINVOKE.Provider_getAvailableProperties(swigCPtr), true);
    if (FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Pending) throw FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public string getDataSetName() {
    string ret = FiftyOneDegreesTrieV3PINVOKE.Provider_getDataSetName(swigCPtr);
    if (FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Pending) throw FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public string getDataSetFormat() {
    string ret = FiftyOneDegreesTrieV3PINVOKE.Provider_getDataSetFormat(swigCPtr);
    if (FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Pending) throw FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public string getDataSetPublishedDate() {
    string ret = FiftyOneDegreesTrieV3PINVOKE.Provider_getDataSetPublishedDate(swigCPtr);
    if (FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Pending) throw FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public string getDataSetNextUpdateDate() {
    string ret = FiftyOneDegreesTrieV3PINVOKE.Provider_getDataSetNextUpdateDate(swigCPtr);
    if (FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Pending) throw FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int getDataSetSignatureCount() {
    int ret = FiftyOneDegreesTrieV3PINVOKE.Provider_getDataSetSignatureCount(swigCPtr);
    if (FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Pending) throw FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public int getDataSetDeviceCombinations() {
    int ret = FiftyOneDegreesTrieV3PINVOKE.Provider_getDataSetDeviceCombinations(swigCPtr);
    if (FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Pending) throw FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Match getMatch(string userAgent) {
    global::System.IntPtr cPtr = FiftyOneDegreesTrieV3PINVOKE.Provider_getMatch__SWIG_0(swigCPtr, userAgent);
    Match ret = (cPtr == global::System.IntPtr.Zero) ? null : new Match(cPtr, true);
    if (FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Pending) throw FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public Match getMatch(MapStringString headers) {
    global::System.IntPtr cPtr = FiftyOneDegreesTrieV3PINVOKE.Provider_getMatch__SWIG_1(swigCPtr, MapStringString.getCPtr(headers));
    Match ret = (cPtr == global::System.IntPtr.Zero) ? null : new Match(cPtr, true);
    if (FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Pending) throw FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public string getMatchJson(string userAgent) {
    string ret = FiftyOneDegreesTrieV3PINVOKE.Provider_getMatchJson__SWIG_0(swigCPtr, userAgent);
    if (FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Pending) throw FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public string getMatchJson(MapStringString headers) {
    string ret = FiftyOneDegreesTrieV3PINVOKE.Provider_getMatchJson__SWIG_1(swigCPtr, MapStringString.getCPtr(headers));
    if (FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Pending) throw FiftyOneDegreesTrieV3PINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

}

}
