/**
 * Declaration of AutoConnectAux basic class.
 * @file AutoConnectAuxBasis.h
 * @author hieromon@gmail.com
 * @version  0.9.7
 * @date 2018-12-17
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTAUX_H_
#define _AUTOCONNECTAUX_H_

#include "AutoConnectDefs.h"
#include <vector>
#include <memory>
#include <functional>
#include <type_traits>
#ifdef AUTOCONNECT_USE_JSON
#include <Stream.h>
#endif // !AUTOCONNECT_USE_JSON
#include <PageBuilder.h>
#include "AutoConnectElement.h"

class AutoConnect;  // Reference to avoid circular
class AutoConnectAux;  // Reference to avoid circular

// Manage placed AutoConnectElement with a vector
typedef std::vector<std::reference_wrapper<AutoConnectElement>> AutoConnectElementVT;

// A type of callback function when  AutoConnectAux page requested.
typedef std::function<String(AutoConnectAux&, PageArgument&)>  AuxHandlerFunctionT;

// A type for the order in which callback functions are called.
typedef enum {
  AC_EXIT_AHEAD = 1,    /**< Callback before building HTML */
  AC_EXIT_LATER = 2,    /**< Callback after building HTML */
  AC_EXIT_BOTH = 3      /**< Callback twice before and after building HTML */
} AutoConnectExitOrder_t;

/**
 * A class that handles an auxiliary page with AutoConnectElement
 * that placed on it by binding it to the AutoConnect menu.
 * @param  uri     An uri string of this page.
 * @param  title   A title string of this page.
 * @param  addons  A set of AutoConnectElement vector.
 * @param  menu    A switch for item displaying in AutoConnect menu.
 */
class AutoConnectAux : public PageBuilder {
 public:
  explicit AutoConnectAux(const String& uri = String(""), const String& title = String(""), const bool menu = true, const AutoConnectElementVT addons = AutoConnectElementVT()) :
    _title(title), _menu(menu), _uriStr(String(uri)), _addonElm(addons), _handler(nullptr), _order(AC_EXIT_AHEAD), _uploadHandler(nullptr) { _uri = _uriStr.c_str(); _next.release(); _ac.release(); }
  ~AutoConnectAux();
  AutoConnectElement& operator[](const String& name) { return *getElement(name); }
  void  add(AutoConnectElement& addon);                                 /**< Add an element to the auxiliary page */
  void  add(AutoConnectElementVT addons);                               /**< Add the element set to the auxiliary page */
  template<typename T>
  T&    getElement(const String& name);
  AutoConnectElement*   getElement(const String& name);                 /**< Get registered AutoConnectElement as specified name */
  AutoConnectElementVT& getElements(void) { return _addonElm; }         /**< Get vector of all elements */
  void  menu(const bool post) { _menu = post; }                         /**< Set or reset the display as menu item for this aux */
  bool  release(const String& name);                                    /**< Release an AutoConnectElement */
  bool  setElementValue(const String& name, const String value);        /**< Set value to specified element */
  bool  setElementValue(const String& name, std::vector<String> const& values);  /**< Set values collection to specified element */
  void  setTitle(const String& title) { _title = title; }               /**< Set a title of the auxiliary page */
  void  on(const AuxHandlerFunctionT handler, const AutoConnectExitOrder_t order = AC_EXIT_AHEAD) { _handler = handler; _order = order; }   /**< Set user handler */
  void  onUpload(PageBuilder::UploadFuncT uploadFunc) override { _uploadHandler = uploadFunc; }
  template<typename T>
  void  onUpload(T& uploadClass) {
    static_assert(std::is_base_of<AutoConnectUploadHandler, T>::value, "onUpload type must be inherited AutoConnectUploadHandler");
    _uploadHandler = std::bind(&T::upload, &uploadClass, std::placeholders::_1, std::placeholders::_2);
  }

#ifdef AUTOCONNECT_USE_JSON
  bool  load(const String& in);                                         /**< Load whole elements to AutoConnectAux Page */
  bool  load(const __FlashStringHelper* in);                            /**< Load whole elements to AutoConnectAux Page */
  bool  load(Stream& in);                                               /**< Load whole elements to AutoConnectAux Page */
  bool  loadElement(const String& in, const String& name = String("")); /**< Load specified element */
  bool  loadElement(const __FlashStringHelper* in, const String& name = String("")); /**< Load specified element */
  bool  loadElement(Stream& in, const String& name = String(""));       /**< Load specified element */
  size_t  saveElement(Stream& out, std::vector<String> const& names = {});    /**< Write elements of AutoConnectAux to the stream */
#endif // !AUTOCONNECT_USE_JSON

 protected:
  void  upload(const String& requestUri, const HTTPUpload& upload);     /**< Uploader wrapper */
  void  _concat(AutoConnectAux& aux);                                   /**< Make up chain of AutoConnectAux */
  void  _join(AutoConnect& ac);                                         /**< Make a link to AutoConnect */
  PageElement*  _setupPage(const String& uri);                          /**< AutoConnectAux page builder */
  const String  _insertElement(PageArgument& args);                     /**< Insert a generated HTML to the page built by PageBuilder */
  const String  _injectTitle(PageArgument& args) const { (void)(args); return _title; } /**< Returns title of this page to PageBuilder */
  const String  _injectMenu(PageArgument& args);                        /**< Inject menu title of this page to PageBuilder */
  const String  _indicateUri(PageArgument& args);                       /**< Inject the uri that caused the request */
  const String  _indicateEncType(PageArgument& args);                   /**< Inject the ENCTYPE attribute */
  void  _storeElements(WebServerClass* webServer);                      /**< Store element values from contained in request arguments */
  static AutoConnectElement&  _nullElement(void);                       /**< A static returning value as invalid */

#ifdef AUTOCONNECT_USE_JSON
  template<typename T>
  bool  _parseJson(T in);
  bool  _load(JsonObject& in);                                          /**< Load all elements from JSON object */
  bool  _loadElement(JsonVariant& in, const String& name);              /**< Load an element as specified name from JSON object */
  template<typename T>
  bool  _parseElement(T in, const String& name);
  AutoConnectElement& _loadElement(JsonObject& in, const String& name); /**< Load an element as specified name from JSON object */
  AutoConnectElement* _createElement(const JsonObject& json);           /**< Create an AutoConnectElement instance from JSON object */
  static ACElement_t  _asElementType(const String& type);               /**< Convert a string of element type to the enumeration value */
#endif // !AUTOCONNECT_USE_JSON

  String  _title;                             /**< A title of the page */
  bool    _menu;                              /**< Switch for menu displaying */
  String  _uriStr;                            /**< uri as String */
  AutoConnectElementVT  _addonElm;            /**< A vector set of AutoConnectElements placed on this auxiliary page */
  std::unique_ptr<AutoConnectAux> _next;      /**< Auxiliary pages chain list */
  std::unique_ptr<AutoConnect>    _ac;        /**< Hosted AutoConnect instance */
  AuxHandlerFunctionT   _handler;             /**< User sketch callback function when AutoConnectAux page requested. */
  AutoConnectExitOrder_t  _order;             /**< The order in which callback functions are called. */
  PageBuilder::UploadFuncT    _uploadHandler; /**< The AutoConnectFile corresponding to current upload */
  AutoConnectFile*      _currentUpload;       /**< AutoConnectFile handling the current upload */
  static const char _PAGE_AUX[] PROGMEM;      /**< Auxiliary page template */

  // Protected members can be used from AutoConnect which handles AutoConnectAux pages.
  friend class AutoConnect;
};

#endif // _AUTOCONNECTAUX_H_
