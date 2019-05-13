## <i class="fa fa-code"></i> Constructor

### AutoConnectAux

```cpp
AutoConnectAux(const String& uri = String(""), const String& title = String(""), const bool menu = true, const AutoConnectElementVT addons = AutoConnectElementVT())
```
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">uri</span><span class="apidesc">URI of this custom Web Page.</span></dd>
    <dd><span class="apidef">title</span><span class="apidesc">Page title of this custom Web page. It will appear on the auto connection menu and at the top of that page.</span></dd>
    <dd><span class="apidef">menu</span><span class="apidesc">Specifies whether to display this page on menu.</span></dd>
    <dd><span class="apidef">addons</span><span class="apidesc">Reference to AutoConnectElement collection.</span></dt>
</dl>

## <i class="fa fa-code"></i> Public member functions

### <i class="fa fa-caret-right"></i> operator [ ]

```cpp
AutoConnectElement& operator[](const String& name)
```
Returns a reference to the element specified by **name**. An operator `[]` is a shortcut for [getElement](apiaux.md#getelement) function with the reference casting. Unlike getElement, which returns a pointer to that element, an operator `[]` returns a reference to that element. You also need to cast the return value to the actual type, just like the getElement function.
<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">name</span><span class="apidesc">Name of the AutoConnectElements to be retrieved.</span></dd>
    <dt>**Return value**</dt><dd>A reference to AutoConnectElement. It is different from the actual element type.</dd>
</dl>

### <i class="fa fa-caret-right"></i> add

```cpp
void add(AutoConnectElement& addon)
```
```cpp
void add(AutoConnectElementVT addons)
```
Add an element to the AutoConnectAux. An added element is displayed on the custom Web page invoked from the AutoConnect menu.
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">addon</span><span class="apidesc">Reference of AutoConnectElements. Specifies one of the AutoConnectElements classes.</span></dd>
    <dd><span class="apidef">addons</span><span class="apidesc">An array list of reference of AutoConnectElements. An [list initialization](https://en.cppreference.com/w/cpp/language/list_initialization) of the [std::vector](https://en.cppreference.com/w/cpp/container/vector) can be used for the addons parameter cause the actual definition of type **AutoConnectElementVT** is `std::vector<std::reference_wrapper<AutoConnectElement>>`.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> getElement

```cpp
T& getElement<T>(const String& name)
```
```cpp
AutoConnectElement* getElement(const String& name)
```
Get a registered AutoConnectElement as specified name. If **T** is specified as an actual type of AutoConnectElements, it returns a reference to that instance.
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">T</span><span class="apidesc">Actual type name of AutoConnectElements as [AutoConnectButton](apielements.md#autoconnectbutton), [AutoConnectCheckbox](apielements.md#autoconnectcheckbox), [AutoConnectElement](apielements.md#autoconnectelement), [AutoConnectFile](apielements.md#autoconnectfile), [AutoConnectInput](apielements.md#autoconnectinput), [AutoConnectRadio](apielements.md#autoconnectradio), [AutoConnectSelect](apielements.md#autoconnectselect), [AutoConnectSubmit](apielements.md#autoconnectsubmit), [AutoConnectText](apielements.md#autoconnecttext).</span></dd>
    <dd><span class="apidef">name</span><span class="apidesc">Name of the AutoConnectElements to be retrieved.</span></dd>
    <dt>**Return value**</dt><dd>A reference of the AutoConnectElements. If a type is not specified returns a pointer.</dd>
</dl>

### <i class="fa fa-caret-right"></i> getElements

```cpp
AutoConnectElementVT& getElements(void)
```
Get vector of reference of all elements.
<dl class="apidl">
    <dt>**Return value**</dt>
    <dd>A reference to std::vector of reference to AutoConnecctElements.</dd>
</dl>

The getElements returns a reference to std::vector of reference to AutoConnecctElements. This function is provided to handle AutoConnectElemets owned by AutoConnectAux in bulk, and you can use each method of std::vector for a return value.

```cpp
// An example of getting type and name of all AutoConnectElements registered in AutoConnectAux.
AutoConnectAux aux;
// some code here...
AutoConnectElementVt& elements = aux.getElements();
for (AutoConnectElement& elm : elements) {
    Serial.printf("name<%s> as type %d\n", elm.name.c_str(), (int)elm.typeOf());
}
```

### <i class="fa fa-caret-right"></i> load

```cpp
bool load(const String& in)
```
```cpp
bool load(const __FlashStringHelper* in)
```
```cpp
bool load(Stream& in)
```
Load all AutoConnectElements elements from JSON document into AutoConnectAux as custom Web pages. The JSON document specified by the load function must be the [document structure](acjson.md#json-objects-elements-for-the-custom-web-page) of AutoConnectAux. Its JSON document can describe multiple pages as an array.
<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">in</span><span class="apidesc">Specifies the JSON document to be load. The load function can input the following objects.

- String : Read-only String
- PROGMEM : Character array contained in the flash
- Stream : An entity that inherits stream class, generally SPIFFS or SD.
    </span></dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">JSON document as the custom Web pages successfully loaded.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">JSON document loading failed.</span></dd>
</dl>

!!! hint "Load multiple custom Web pages separately"
    Multiple custom Web pages can be loaded at once with JSON as an array. But it will consume a lot of memory. By loading a JSON document by page as much as possible, you can reduce memory consumption.

### <i class="fa fa-caret-right"></i> loadElement

```cpp
bool loadElement(const String& in, const String& name = String(""))
```
```cpp
bool loadElement(const __FlashStringHelper* in, const String& name = String(""))
```
```cpp
bool loadElement(Stream& in, const String& name = String(""))
```
Load specified element from JSON document into AutoConnectAux. The JSON document specified by the loadElement function must be the [AutoConnectElement document structure](acjson.md#json-object-for-autoconnectelements). When loading from a JSON document that describes multiple elements, its description must be an array syntax.
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">in</span><span class="apidesc">Specifies the JSON document to be load. The load function can input the following objects.

- String : Read-only String
- PROGMEM : Character array contained in the flash
- Stream : An entity that inherits stream class, generally SPIFFS or SD.
    </span></dd>
    <dd><span class="apidef">name</span><span class="apidesc">Specifies the name to be load. If the name is not specified, the loadElement function will load all elements contained in the JSON document.</span></dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">Specified AutoConnectElements successfully loaded.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">JSON document loading failed.</span></dd>
</dl>

!!! caution "Maybe it is an array"
    Please note that the JSON document that is the input for loadElement is an array syntax of AutoConnectElements when there are multiple elements. For example, the following JSON document has a syntax error:

    ```json
    {
      "name": "Caption",
      "type": "ACText",
      "value": "Hello, world"
    },
    {
      "name": "Server",
      "type": "ACInput",
      "label": "Server address"
    }
    ```
    The outermost `[`, `]` is missing.

### <i class="fa fa-caret-right"></i> menu

```cpp
void menu(const bool post)
```
Set or reset the display as menu item for this AutoConnectAux. This function programmatically manipulates the menu parameter of the [AutoConenctAux constructor](apiaux.md#autoconnectaux).
<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">Show on the menu.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">Hidden on the menu.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> on

```cpp
void on(const AuxHandlerFunctionT handler, const AutoConnectExitOrder_t order = AC_EXIT_AHEAD)
```
Register the handler function of the AutoConnectAux.
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">handler</span><span class="apidesc">A function that behaves when a request to the AutoConnectAux page occurs. AuxHandlerFunctionT type is defined by the following declaration.<p class="apidesc">`String handler(AutoConnectAux&, PageArgument&)`</p></span></dd>
    <dd><span class="apidef">order</span><span class="apidesc">Specifies when the handler is called with the following enumeration value.</span>
: - **AC_EXIT_AHEAD** :
    Called before AutoConnect generates the HTML of the page. You set the value of AutoConnectElements in the handler then its value will be displayed on the page.
: - **AC_EXIT_LATER** :
    Called after AutoConnect generates the HTML of the page. You can append to HTML generated by AutoConnect.
: - **AC_EXIT_BOTH** :
    Called even before generating HTML and after generated.</dd>
</dl>

### <i class="fa fa-caret-right"></i> onUpload

```cpp
void onUpload<T&>(T handler)
```
```cpp
void onUpload(PageBuilder::UploadFuncT uploadFunc)
```
Register the upload handler of the AutoConnectAux.
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">T</span><span class="apidesc">Specifies a class name of the custom uploader inherited from [AutoConnectUploadHandler](acupload.md#upload-handler-base-class) class. Refer to the [appendix](acupload.md#to-upload-to-a-device-other-than-flash-or-sd) for details.</span></dd>
    <dd><span class="apidef">handler</span><span class="apidesc">Specifies the custom uploader inherited from [AutoConnectUploadHandler](acupload.md#upload-handler-base-class) class. Refer to the [appendix](acupload.md#to-upload-to-a-device-other-than-flash-or-sd) for details.</span></dd>
    <dd><span class="apidef">uploadFunc</span><span class="apidesc">A function that behaves when request to upload with the AutoConnectAux page. UploadFuncT type is defined by the following declaration.<p class="apidesc">`void(const String&, const HTTPUpload&)`</p><p>A data structure of the upload file as HTTPUpload. It is defined in the ESP8266WebServer (the WebServer for ESP32) library as follows:

```cpp
typedef struct {
  HTTPUploadStatus status;
  String  filename;
  String  name;
  String  type;
  size_t  totalSize;
  size_t  currentSize;
  size_t  contentLength;
  uint8_t buf[HTTP_UPLOAD_BUFLEN];
} HTTPUpload;
```
</p>Refer to '[To upload to a device other than Flash or SD](acupload.md#to-upload-to-a-device-other-than-flash-or-sd)' in section [appendix](acupload.md) for details.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> release

```cpp
bool release(const String& name)
```
Release a specified AutoConnectElement from AutoConenctAux. The release function is provided to dynamically change the structure of the custom Web pages with the sketch. By combining the release function and the [add](apiaux.md#add) function or the [loadElement](apiaux.md#loadelement) function, the sketch can change the style of the custom Web page according to its behavior.
<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">name</span><span class="apidesc">Specifies the name of AutoConnectElements to be released.</span></dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">The AutoConnectElement successfully released.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">The AutoConnectElement can not be released.</span></dd>
</dl>

### <i class="fa fa-caret-right"></i> saveElement

```cpp
size_t saveElement(Stream& out, std::vector<String> const& names = {})
```
Write elements of AutoConnectAux to the stream. The saveElement function outputs the specified AutoConenctElements as a JSON document using the [prettyPrintTo](https://arduinojson.org/v5/api/jsonobject/prettyprintto/) function of the [ArduinoJson](https://arduinojson.org/) library.
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">out</span><span class="apidesc">Output stream to be output. SPIFFS, SD also Serial can be specified generally.</span></dd>
    <dd><span class="apidef">names</span><span class="apidesc">The array of the name of AutoConnectElements to be output. If the names parameter is not specified, all AutoConnectElements registered in AutoConnectAux are output.</span></dd>
    <dt>**Return value**</dt>
    <dd>The number of bytes written.</dd>
</dl>

!!! note "The output format is pretty"
    The saveElement function outputs a prettified JSON document.

### <i class="fa fa-caret-right"></i> setElementValue

```cpp
bool setElementValue(const String& name, const String value)
```
```cpp
bool setElementValue(const String& name, std::vector<String> const& values)
```
Sets the value of the specified AutoConnectElement. If values ​​is specified as a *std::vector* of String, the element that can set the values is the [AutoConnectRadio](apielements.md#autoconnectradio) or the [AutoConnectSelect](apielements.md#autoconnectselect). 
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">name</span><span class="apidesc">Specifies the name of the AutoConnectElements that you want to set the value.</span></dd>
    <dd><span class="apidef">value</span><span class="apidesc">Specifies the value to be set.</span></dd>
    <dd><span class="apidef">values</span><span class="apidesc">Specifies a reference of a *std::vector* of String. It contains the values of the AutoConnectRadio or the AutoConnectSelect.</span></dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">The value has been set.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">AutoConnectElements with the specified name are not registered. Or the type of the value is not consistent with the specified AutoConnectElements.</span></dd>
</dl>

!!! hint "You can directly access the value member variable."
    If you are gripping with the sketch to the AutoConnectElements of the target that sets the value, you can access the value member variable directly. The following sketch code has the same effect.
    ```cpp
    AutoConnectAux aux;
    // ... Griping the AutoConnectText here.
    aux.setElementValue("TEXT_FIELD", "New value");
    ```
    ```cpp
    AutoConnectAux aux;
    // ... Griping the AutoConnectText here.
    AutoConnectText& text = aux.getElement<AutoConnectText>("TEXT_FIELD");
    text.value = "New value";
    ```
    The difference between the setElementValue and the value access with the [getElement](apiaux.md#getelement) is the certainty of the registration state for the element. The [getElement](apiaux.md#getelement) returns an empty object if the element is not registered then a sketch assigns the value to it. May occur unexpected results and crashes. You should use the setElementValue if its registration is unsettled.

### <i class="fa fa-caret-right"></i> setTitle

```cpp
void setTitle(const String& title)
```
Set the title string of the custom Web page. This title will be displayed as the menu title of the custom Web page.
<dl class="apidl">
    <dt>**Parameter**</dt>
    <dd><span class="apidef">title</span><span class="apidesc">Title string to be display.</span></dd>
</dl>

!!! caution "Not the menu title"
    The setTitle function is not set for the AutoConnect menu title. The effect of this function is that custom Web page only. To change the AutoConnect menu title use [AutoConnectConfig::title](apiconfig.md#title).
    