/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef JSONOPTION_H_
#define JSONOPTION_H_

#include <config.h>
#include "ArduinoJson.h"
#include "../widgets/switch.h"
#include "../widgets/textarea.h"
#include "hardware/alloc.h"

enum OptionDataType
{
  BoolOption,
  StringOption
};

struct JsonOption
{
  JsonOption(const char* optionName) {
    strlcpy(name, optionName, MAX_OPTION_NAME_LENGTH);
  }
  virtual ~JsonOption() {}

  virtual void applyFromUI() = 0;
  virtual void save(JsonDocument& document) = 0;
  virtual void load(JsonDocument& document) = 0;

  virtual OptionDataType type() = 0;

public:
  char name[MAX_OPTION_NAME_LENGTH];
};

struct JsonBoolOption : public JsonOption
{
  JsonBoolOption(const char* optionName, bool defValue = false) : JsonOption(optionName) {
    value = defValue;
  }
  virtual ~JsonBoolOption() {
    source = nullptr;
    isControlAssigned = false;
  }
  virtual void applyFromUI() {
    if (isControlAssigned)
    {
      bool currentValue = control.value();
      value = currentValue;
      if (source != nullptr)
        *source = value;
    }
  }
  virtual void save(JsonDocument& document) {
    document[name] = value;
  }
  virtual void load(JsonDocument& document) {
    value = document[name].as<bool>();
    if (source != nullptr)
      *source = value;
    if (isControlAssigned)
      control.value(value);
  }

  /*
  * @brief Assign settings option to the variable
  */
  JsonBoolOption& assign(bool* sourceVariable) {
    source = sourceVariable;
    if (source != nullptr)
      *source = value;
    return *this;
  }
  /*
  * @brief Assign settings option to the UI widget
  */
  JsonBoolOption& assign(Switch& sourceControl) {
    isControlAssigned = true;
    control = sourceControl;
    control.value(value);
    return *this;
  }

  virtual OptionDataType type() { return OptionDataType::BoolOption; }

public:
  bool value;
  bool* source = nullptr;
  bool isControlAssigned = false;
  Switch control;
};

struct JsonStringOption : public JsonOption
{
  JsonStringOption(const char* optionName, int maxValueLength, const char* defValue = nullptr) : JsonOption(optionName)
  {
    maxLength = maxValueLength;
    value = (char*)MALLOC(maxLength);
    // todo check if malloc failed!
    if (defValue != nullptr)
      strlcpy(value, defValue, maxLength);
    else
      memset(value, 0, maxLength);
  }
  virtual ~JsonStringOption() {
    free(value);
    value = nullptr;
    source = nullptr;
    isControlAssigned = false;
  }
  virtual void applyFromUI() {
    if (isControlAssigned)
    {
      String currentValue = control.text();
      strlcpy(value, currentValue.c_str(), maxLength);
      if (source != nullptr)
        *source = value;
    }
  }
  virtual void save(JsonDocument& document) {
    document[name] = value;
  }
  virtual void load(JsonDocument& document) {
    strlcpy(value, document[name], maxLength);
    if (source != nullptr)
      *source = value;
    if (isControlAssigned)
      control.text(value);
  }

  /*
  * @brief Assign settings option to the variable
  */
  JsonStringOption& assign(String* sourceVariable) {
    source = sourceVariable;
    if (source != nullptr)
      *source = value;
    return *this;
  }
  /*
  * @brief Assign settings option to the UI widget
  */
  JsonStringOption& assign(TextArea& sourceControl) {
    isControlAssigned = true;
    control = sourceControl;
    control.text(value);
    return *this;
  }

  virtual OptionDataType type() { return OptionDataType::StringOption; }

public:
  char* value;
  int maxLength;
  String* source = nullptr;
  bool isControlAssigned = false;
  TextArea control;
};

#endif
