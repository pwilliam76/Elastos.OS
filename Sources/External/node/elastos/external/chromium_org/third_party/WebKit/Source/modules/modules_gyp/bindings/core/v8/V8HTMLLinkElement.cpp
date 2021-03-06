// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file has been auto-generated by code_generator_v8.py. DO NOT MODIFY!

#include "config.h"
#include "V8HTMLLinkElement.h"

#include "HTMLNames.h"
#include "bindings/core/v8/V8DOMSettableTokenList.h"
#include "bindings/core/v8/V8Document.h"
#include "bindings/core/v8/V8StyleSheet.h"
#include "bindings/v8/ExceptionState.h"
#include "bindings/v8/V8DOMActivityLogger.h"
#include "bindings/v8/V8DOMConfiguration.h"
#include "bindings/v8/V8HiddenValue.h"
#include "bindings/v8/V8ObjectConstructor.h"
#include "core/dom/ContextFeatures.h"
#include "core/dom/Document.h"
#include "core/dom/custom/CustomElementCallbackDispatcher.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/TraceEvent.h"
#include "wtf/GetPtr.h"
#include "wtf/RefPtr.h"

namespace WebCore {

static void initializeScriptWrappableForInterface(HTMLLinkElement* object)
{
    if (ScriptWrappable::wrapperCanBeStoredInObject(object))
        ScriptWrappable::fromObject(object)->setTypeInfo(&V8HTMLLinkElement::wrapperTypeInfo);
    else
        ASSERT_NOT_REACHED();
}

} // namespace WebCore

void webCoreInitializeScriptWrappableForInterface(WebCore::HTMLLinkElement* object)
{
    WebCore::initializeScriptWrappableForInterface(object);
}

namespace WebCore {
const WrapperTypeInfo V8HTMLLinkElement::wrapperTypeInfo = { gin::kEmbedderBlink, V8HTMLLinkElement::domTemplate, V8HTMLLinkElement::derefObject, 0, V8HTMLLinkElement::toEventTarget, 0, V8HTMLLinkElement::installPerContextEnabledMethods, &V8HTMLElement::wrapperTypeInfo, WrapperTypeObjectPrototype, WillBeGarbageCollectedObject };

namespace HTMLLinkElementV8Internal {

template <typename T> void V8_USE(T) { }

static void disabledAttributeGetter(const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    HTMLLinkElement* impl = V8HTMLLinkElement::toNative(holder);
    v8SetReturnValueBool(info, impl->fastHasAttribute(HTMLNames::disabledAttr));
}

static void disabledAttributeGetterCallback(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMGetter");
    HTMLLinkElementV8Internal::disabledAttributeGetter(info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void disabledAttributeSetter(v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    HTMLLinkElement* impl = V8HTMLLinkElement::toNative(holder);
    TONATIVE_VOID(bool, cppValue, v8Value->BooleanValue());
    CustomElementCallbackDispatcher::CallbackDeliveryScope deliveryScope;
    impl->setBooleanAttribute(HTMLNames::disabledAttr, cppValue);
}

static void disabledAttributeSetterCallback(v8::Local<v8::String>, v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMSetter");
    CustomElementCallbackDispatcher::CallbackDeliveryScope deliveryScope;
    HTMLLinkElementV8Internal::disabledAttributeSetter(v8Value, info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void charsetAttributeGetter(const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    v8SetReturnValueString(info, impl->fastGetAttribute(HTMLNames::charsetAttr), info.GetIsolate());
}

static void charsetAttributeGetterCallback(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMGetter");
    HTMLLinkElementV8Internal::charsetAttributeGetter(info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void charsetAttributeSetter(v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    TOSTRING_VOID(V8StringResource<>, cppValue, v8Value);
    impl->setAttribute(HTMLNames::charsetAttr, cppValue);
}

static void charsetAttributeSetterCallback(v8::Local<v8::String>, v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMSetter");
    CustomElementCallbackDispatcher::CallbackDeliveryScope deliveryScope;
    HTMLLinkElementV8Internal::charsetAttributeSetter(v8Value, info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void hrefAttributeGetter(const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    HTMLLinkElement* impl = V8HTMLLinkElement::toNative(holder);
    v8SetReturnValueString(info, impl->getURLAttribute(HTMLNames::hrefAttr), info.GetIsolate());
}

static void hrefAttributeGetterCallback(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMGetter");
    HTMLLinkElementV8Internal::hrefAttributeGetter(info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void hrefAttributeSetter(v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    TOSTRING_VOID(V8StringResource<>, cppValue, v8Value);
    impl->setAttribute(HTMLNames::hrefAttr, cppValue);
}

static void hrefAttributeSetterCallback(v8::Local<v8::String>, v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMSetter");
    ScriptState* scriptState = ScriptState::from(info.GetIsolate()->GetCurrentContext());
    V8PerContextData* contextData = scriptState->perContextData();
    if (scriptState->world().isIsolatedWorld() && contextData && contextData->activityLogger()) {
        contextData->activityLogger()->logSetter("HTMLLinkElement.href", v8Value);
    }
    CustomElementCallbackDispatcher::CallbackDeliveryScope deliveryScope;
    HTMLLinkElementV8Internal::hrefAttributeSetter(v8Value, info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void crossOriginAttributeGetter(const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    String v8Value = impl->fastGetAttribute(HTMLNames::crossoriginAttr);
    if (v8Value.isNull()) {
        ;
    } else if (v8Value.isEmpty()) {
        v8Value = "anonymous";
    } else if (equalIgnoringCase(v8Value, "anonymous")) {
        v8Value = "anonymous";
    } else if (equalIgnoringCase(v8Value, "use-credentials")) {
        v8Value = "use-credentials";
    } else {
        v8Value = "anonymous";
    }
    v8SetReturnValueString(info, v8Value, info.GetIsolate());
}

static void crossOriginAttributeGetterCallback(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMGetter");
    HTMLLinkElementV8Internal::crossOriginAttributeGetter(info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void crossOriginAttributeSetter(v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    TOSTRING_VOID(V8StringResource<>, cppValue, v8Value);
    impl->setAttribute(HTMLNames::crossoriginAttr, cppValue);
}

static void crossOriginAttributeSetterCallback(v8::Local<v8::String>, v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMSetter");
    CustomElementCallbackDispatcher::CallbackDeliveryScope deliveryScope;
    HTMLLinkElementV8Internal::crossOriginAttributeSetter(v8Value, info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void hreflangAttributeGetter(const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    v8SetReturnValueString(info, impl->fastGetAttribute(HTMLNames::hreflangAttr), info.GetIsolate());
}

static void hreflangAttributeGetterCallback(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMGetter");
    HTMLLinkElementV8Internal::hreflangAttributeGetter(info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void hreflangAttributeSetter(v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    TOSTRING_VOID(V8StringResource<>, cppValue, v8Value);
    impl->setAttribute(HTMLNames::hreflangAttr, cppValue);
}

static void hreflangAttributeSetterCallback(v8::Local<v8::String>, v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMSetter");
    CustomElementCallbackDispatcher::CallbackDeliveryScope deliveryScope;
    HTMLLinkElementV8Internal::hreflangAttributeSetter(v8Value, info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void mediaAttributeGetter(const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    v8SetReturnValueString(info, impl->fastGetAttribute(HTMLNames::mediaAttr), info.GetIsolate());
}

static void mediaAttributeGetterCallback(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMGetter");
    HTMLLinkElementV8Internal::mediaAttributeGetter(info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void mediaAttributeSetter(v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    TOSTRING_VOID(V8StringResource<>, cppValue, v8Value);
    impl->setAttribute(HTMLNames::mediaAttr, cppValue);
}

static void mediaAttributeSetterCallback(v8::Local<v8::String>, v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMSetter");
    CustomElementCallbackDispatcher::CallbackDeliveryScope deliveryScope;
    HTMLLinkElementV8Internal::mediaAttributeSetter(v8Value, info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void relAttributeGetter(const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    v8SetReturnValueString(info, impl->fastGetAttribute(HTMLNames::relAttr), info.GetIsolate());
}

static void relAttributeGetterCallback(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMGetter");
    HTMLLinkElementV8Internal::relAttributeGetter(info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void relAttributeSetter(v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    TOSTRING_VOID(V8StringResource<>, cppValue, v8Value);
    impl->setAttribute(HTMLNames::relAttr, cppValue);
}

static void relAttributeSetterCallback(v8::Local<v8::String>, v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMSetter");
    CustomElementCallbackDispatcher::CallbackDeliveryScope deliveryScope;
    HTMLLinkElementV8Internal::relAttributeSetter(v8Value, info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void revAttributeGetter(const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    v8SetReturnValueString(info, impl->fastGetAttribute(HTMLNames::revAttr), info.GetIsolate());
}

static void revAttributeGetterCallback(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMGetter");
    HTMLLinkElementV8Internal::revAttributeGetter(info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void revAttributeSetter(v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    TOSTRING_VOID(V8StringResource<>, cppValue, v8Value);
    impl->setAttribute(HTMLNames::revAttr, cppValue);
}

static void revAttributeSetterCallback(v8::Local<v8::String>, v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMSetter");
    CustomElementCallbackDispatcher::CallbackDeliveryScope deliveryScope;
    HTMLLinkElementV8Internal::revAttributeSetter(v8Value, info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void sizesAttributeGetter(const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    HTMLLinkElement* impl = V8HTMLLinkElement::toNative(holder);
    v8SetReturnValueFast(info, WTF::getPtr(impl->sizes()), impl);
}

static void sizesAttributeGetterCallback(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMGetter");
    HTMLLinkElementV8Internal::sizesAttributeGetter(info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void sizesAttributeSetter(v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    HTMLLinkElement* proxyImpl = V8HTMLLinkElement::toNative(holder);
    RefPtrWillBeRawPtr<DOMSettableTokenList> impl = WTF::getPtr(proxyImpl->sizes());
    if (!impl)
        return;
    TOSTRING_VOID(V8StringResource<>, cppValue, v8Value);
    impl->setValue(cppValue);
}

static void sizesAttributeSetterCallback(v8::Local<v8::String>, v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMSetter");
    HTMLLinkElementV8Internal::sizesAttributeSetter(v8Value, info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void targetAttributeGetter(const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    v8SetReturnValueString(info, impl->fastGetAttribute(HTMLNames::targetAttr), info.GetIsolate());
}

static void targetAttributeGetterCallback(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMGetter");
    HTMLLinkElementV8Internal::targetAttributeGetter(info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void targetAttributeSetter(v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    TOSTRING_VOID(V8StringResource<>, cppValue, v8Value);
    impl->setAttribute(HTMLNames::targetAttr, cppValue);
}

static void targetAttributeSetterCallback(v8::Local<v8::String>, v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMSetter");
    CustomElementCallbackDispatcher::CallbackDeliveryScope deliveryScope;
    HTMLLinkElementV8Internal::targetAttributeSetter(v8Value, info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void typeAttributeGetter(const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    v8SetReturnValueString(info, impl->fastGetAttribute(HTMLNames::typeAttr), info.GetIsolate());
}

static void typeAttributeGetterCallback(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMGetter");
    HTMLLinkElementV8Internal::typeAttributeGetter(info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void typeAttributeSetter(v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    TOSTRING_VOID(V8StringResource<>, cppValue, v8Value);
    impl->setAttribute(HTMLNames::typeAttr, cppValue);
}

static void typeAttributeSetterCallback(v8::Local<v8::String>, v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMSetter");
    CustomElementCallbackDispatcher::CallbackDeliveryScope deliveryScope;
    HTMLLinkElementV8Internal::typeAttributeSetter(v8Value, info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void sheetAttributeGetter(const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    HTMLLinkElement* impl = V8HTMLLinkElement::toNative(holder);
    v8SetReturnValueFast(info, WTF::getPtr(impl->sheet()), impl);
}

static void sheetAttributeGetterCallback(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMGetter");
    HTMLLinkElementV8Internal::sheetAttributeGetter(info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void importAttributeGetter(const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    HTMLLinkElement* impl = V8HTMLLinkElement::toNative(holder);
    v8SetReturnValueFast(info, WTF::getPtr(impl->import()), impl);
}

static void importAttributeGetterCallback(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMGetter");
    HTMLLinkElementV8Internal::importAttributeGetter(info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void integrityAttributeGetter(const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    v8SetReturnValueString(info, impl->fastGetAttribute(HTMLNames::integrityAttr), info.GetIsolate());
}

static void integrityAttributeGetterCallback(v8::Local<v8::String>, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMGetter");
    HTMLLinkElementV8Internal::integrityAttributeGetter(info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

static void integrityAttributeSetter(v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Handle<v8::Object> holder = info.Holder();
    Element* impl = V8Element::toNative(holder);
    TOSTRING_VOID(V8StringResource<>, cppValue, v8Value);
    impl->setAttribute(HTMLNames::integrityAttr, cppValue);
}

static void integrityAttributeSetterCallback(v8::Local<v8::String>, v8::Local<v8::Value> v8Value, const v8::PropertyCallbackInfo<void>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("Blink", "DOMSetter");
    CustomElementCallbackDispatcher::CallbackDeliveryScope deliveryScope;
    HTMLLinkElementV8Internal::integrityAttributeSetter(v8Value, info);
    TRACE_EVENT_SET_SAMPLING_STATE("V8", "V8Execution");
}

} // namespace HTMLLinkElementV8Internal

static const V8DOMConfiguration::AttributeConfiguration V8HTMLLinkElementAttributes[] = {
    {"disabled", HTMLLinkElementV8Internal::disabledAttributeGetterCallback, HTMLLinkElementV8Internal::disabledAttributeSetterCallback, 0, 0, 0, static_cast<v8::AccessControl>(v8::DEFAULT), static_cast<v8::PropertyAttribute>(v8::None), 0 /* on instance */},
    {"charset", HTMLLinkElementV8Internal::charsetAttributeGetterCallback, HTMLLinkElementV8Internal::charsetAttributeSetterCallback, 0, 0, 0, static_cast<v8::AccessControl>(v8::DEFAULT), static_cast<v8::PropertyAttribute>(v8::None), 0 /* on instance */},
    {"href", HTMLLinkElementV8Internal::hrefAttributeGetterCallback, HTMLLinkElementV8Internal::hrefAttributeSetterCallback, 0, 0, 0, static_cast<v8::AccessControl>(v8::DEFAULT), static_cast<v8::PropertyAttribute>(v8::None), 0 /* on instance */},
    {"crossOrigin", HTMLLinkElementV8Internal::crossOriginAttributeGetterCallback, HTMLLinkElementV8Internal::crossOriginAttributeSetterCallback, 0, 0, 0, static_cast<v8::AccessControl>(v8::DEFAULT), static_cast<v8::PropertyAttribute>(v8::None), 0 /* on instance */},
    {"hreflang", HTMLLinkElementV8Internal::hreflangAttributeGetterCallback, HTMLLinkElementV8Internal::hreflangAttributeSetterCallback, 0, 0, 0, static_cast<v8::AccessControl>(v8::DEFAULT), static_cast<v8::PropertyAttribute>(v8::None), 0 /* on instance */},
    {"media", HTMLLinkElementV8Internal::mediaAttributeGetterCallback, HTMLLinkElementV8Internal::mediaAttributeSetterCallback, 0, 0, 0, static_cast<v8::AccessControl>(v8::DEFAULT), static_cast<v8::PropertyAttribute>(v8::None), 0 /* on instance */},
    {"rel", HTMLLinkElementV8Internal::relAttributeGetterCallback, HTMLLinkElementV8Internal::relAttributeSetterCallback, 0, 0, 0, static_cast<v8::AccessControl>(v8::DEFAULT), static_cast<v8::PropertyAttribute>(v8::None), 0 /* on instance */},
    {"rev", HTMLLinkElementV8Internal::revAttributeGetterCallback, HTMLLinkElementV8Internal::revAttributeSetterCallback, 0, 0, 0, static_cast<v8::AccessControl>(v8::DEFAULT), static_cast<v8::PropertyAttribute>(v8::None), 0 /* on instance */},
    {"sizes", HTMLLinkElementV8Internal::sizesAttributeGetterCallback, HTMLLinkElementV8Internal::sizesAttributeSetterCallback, 0, 0, 0, static_cast<v8::AccessControl>(v8::DEFAULT), static_cast<v8::PropertyAttribute>(v8::None), 0 /* on instance */},
    {"target", HTMLLinkElementV8Internal::targetAttributeGetterCallback, HTMLLinkElementV8Internal::targetAttributeSetterCallback, 0, 0, 0, static_cast<v8::AccessControl>(v8::DEFAULT), static_cast<v8::PropertyAttribute>(v8::None), 0 /* on instance */},
    {"type", HTMLLinkElementV8Internal::typeAttributeGetterCallback, HTMLLinkElementV8Internal::typeAttributeSetterCallback, 0, 0, 0, static_cast<v8::AccessControl>(v8::DEFAULT), static_cast<v8::PropertyAttribute>(v8::None), 0 /* on instance */},
    {"sheet", HTMLLinkElementV8Internal::sheetAttributeGetterCallback, 0, 0, 0, 0, static_cast<v8::AccessControl>(v8::DEFAULT), static_cast<v8::PropertyAttribute>(v8::None), 0 /* on instance */},
};

static void configureV8HTMLLinkElementTemplate(v8::Handle<v8::FunctionTemplate> functionTemplate, v8::Isolate* isolate)
{
    functionTemplate->ReadOnlyPrototype();

    v8::Local<v8::Signature> defaultSignature;
    defaultSignature = V8DOMConfiguration::installDOMClassTemplate(functionTemplate, "HTMLLinkElement", V8HTMLElement::domTemplate(isolate), V8HTMLLinkElement::internalFieldCount,
        V8HTMLLinkElementAttributes, WTF_ARRAY_LENGTH(V8HTMLLinkElementAttributes),
        0, 0,
        0, 0,
        isolate);
    v8::Local<v8::ObjectTemplate> instanceTemplate ALLOW_UNUSED = functionTemplate->InstanceTemplate();
    v8::Local<v8::ObjectTemplate> prototypeTemplate ALLOW_UNUSED = functionTemplate->PrototypeTemplate();
    if (RuntimeEnabledFeatures::htmlImportsEnabled()) {
        static const V8DOMConfiguration::AttributeConfiguration attributeConfiguration =\
        {"import", HTMLLinkElementV8Internal::importAttributeGetterCallback, 0, 0, 0, 0, static_cast<v8::AccessControl>(v8::DEFAULT), static_cast<v8::PropertyAttribute>(v8::None), 0 /* on instance */};
        V8DOMConfiguration::installAttribute(instanceTemplate, prototypeTemplate, attributeConfiguration, isolate);
    }
    if (RuntimeEnabledFeatures::subresourceIntegrityEnabled()) {
        static const V8DOMConfiguration::AttributeConfiguration attributeConfiguration =\
        {"integrity", HTMLLinkElementV8Internal::integrityAttributeGetterCallback, HTMLLinkElementV8Internal::integrityAttributeSetterCallback, 0, 0, 0, static_cast<v8::AccessControl>(v8::DEFAULT), static_cast<v8::PropertyAttribute>(v8::None), 0 /* on instance */};
        V8DOMConfiguration::installAttribute(instanceTemplate, prototypeTemplate, attributeConfiguration, isolate);
    }

    // Custom toString template
    functionTemplate->Set(v8AtomicString(isolate, "toString"), V8PerIsolateData::from(isolate)->toStringTemplate());
}

v8::Handle<v8::FunctionTemplate> V8HTMLLinkElement::domTemplate(v8::Isolate* isolate)
{
    return V8DOMConfiguration::domClassTemplate(isolate, const_cast<WrapperTypeInfo*>(&wrapperTypeInfo), configureV8HTMLLinkElementTemplate);
}

bool V8HTMLLinkElement::hasInstance(v8::Handle<v8::Value> v8Value, v8::Isolate* isolate)
{
    return V8PerIsolateData::from(isolate)->hasInstance(&wrapperTypeInfo, v8Value);
}

v8::Handle<v8::Object> V8HTMLLinkElement::findInstanceInPrototypeChain(v8::Handle<v8::Value> v8Value, v8::Isolate* isolate)
{
    return V8PerIsolateData::from(isolate)->findInstanceInPrototypeChain(&wrapperTypeInfo, v8Value);
}

HTMLLinkElement* V8HTMLLinkElement::toNativeWithTypeCheck(v8::Isolate* isolate, v8::Handle<v8::Value> value)
{
    return hasInstance(value, isolate) ? fromInternalPointer(v8::Handle<v8::Object>::Cast(value)->GetAlignedPointerFromInternalField(v8DOMWrapperObjectIndex)) : 0;
}

EventTarget* V8HTMLLinkElement::toEventTarget(v8::Handle<v8::Object> object)
{
    return toNative(object);
}

v8::Handle<v8::Object> wrap(HTMLLinkElement* impl, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    ASSERT(impl);
    ASSERT(!DOMDataStore::containsWrapper<V8HTMLLinkElement>(impl, isolate));
    return V8HTMLLinkElement::createWrapper(impl, creationContext, isolate);
}

v8::Handle<v8::Object> V8HTMLLinkElement::createWrapper(PassRefPtrWillBeRawPtr<HTMLLinkElement> impl, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    ASSERT(impl);
    ASSERT(!DOMDataStore::containsWrapper<V8HTMLLinkElement>(impl.get(), isolate));
    if (ScriptWrappable::wrapperCanBeStoredInObject(impl.get())) {
        const WrapperTypeInfo* actualInfo = ScriptWrappable::fromObject(impl.get())->typeInfo();
        // Might be a XXXConstructor::wrapperTypeInfo instead of an XXX::wrapperTypeInfo. These will both have
        // the same object de-ref functions, though, so use that as the basis of the check.
        RELEASE_ASSERT_WITH_SECURITY_IMPLICATION(actualInfo->derefObjectFunction == wrapperTypeInfo.derefObjectFunction);
    }

    v8::Handle<v8::Object> wrapper = V8DOMWrapper::createWrapper(creationContext, &wrapperTypeInfo, toInternalPointer(impl.get()), isolate);
    if (UNLIKELY(wrapper.IsEmpty()))
        return wrapper;

    installPerContextEnabledProperties(wrapper, impl.get(), isolate);
    V8DOMWrapper::associateObjectWithWrapper<V8HTMLLinkElement>(impl, &wrapperTypeInfo, wrapper, isolate, WrapperConfiguration::Dependent);
    return wrapper;
}

void V8HTMLLinkElement::derefObject(void* object)
{
#if !ENABLE(OILPAN)
    fromInternalPointer(object)->deref();
#endif // !ENABLE(OILPAN)
}

template<>
v8::Handle<v8::Value> toV8NoInline(HTMLLinkElement* impl, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return toV8(impl, creationContext, isolate);
}

} // namespace WebCore
