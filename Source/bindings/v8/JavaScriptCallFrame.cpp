/*
 * Copyright (c) 2010, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "JavaScriptCallFrame.h"


#include "V8Binding.h"

namespace WebCore {

JavaScriptCallFrame::JavaScriptCallFrame(v8::Handle<v8::Context> debuggerContext, v8::Handle<v8::Object> callFrame)
    : m_debuggerContext(debuggerContext)
    , m_callFrame(callFrame)
{
}

JavaScriptCallFrame::~JavaScriptCallFrame()
{
}

JavaScriptCallFrame* JavaScriptCallFrame::caller()
{
    if (!m_caller) {
        v8::HandleScope handleScope;
        v8::Context::Scope contextScope(m_debuggerContext.get());
        v8::Handle<v8::Value> callerFrame = m_callFrame.get()->Get(v8::String::NewSymbol("caller"));
        if (!callerFrame->IsObject())
            return 0;
        m_caller = JavaScriptCallFrame::create(m_debuggerContext.get(), v8::Handle<v8::Object>::Cast(callerFrame));
    }
    return m_caller.get();
}

int JavaScriptCallFrame::sourceID() const
{
    v8::HandleScope handleScope;
    v8::Context::Scope contextScope(m_debuggerContext.get());
    v8::Handle<v8::Value> result = m_callFrame.get()->Get(v8::String::NewSymbol("sourceID"));
    if (result->IsInt32())
        return result->Int32Value();
    return 0;
}

int JavaScriptCallFrame::line() const
{
    v8::HandleScope handleScope;
    v8::Context::Scope contextScope(m_debuggerContext.get());
    v8::Handle<v8::Value> result = m_callFrame.get()->Get(v8::String::NewSymbol("line"));
    if (result->IsInt32())
        return result->Int32Value();
    return 0;
}

int JavaScriptCallFrame::column() const
{
    v8::HandleScope handleScope;
    v8::Context::Scope contextScope(m_debuggerContext.get());
    v8::Handle<v8::Value> result = m_callFrame.get()->Get(v8::String::NewSymbol("column"));
    if (result->IsInt32())
        return result->Int32Value();
    return 0;
}

String JavaScriptCallFrame::functionName() const
{
    v8::HandleScope handleScope;
    v8::Context::Scope contextScope(m_debuggerContext.get());
    v8::Handle<v8::Value> result = m_callFrame.get()->Get(v8::String::NewSymbol("functionName"));
    return toWebCoreStringWithUndefinedOrNullCheck(result);
}

v8::Handle<v8::Value> JavaScriptCallFrame::scopeChain() const
{
    v8::Handle<v8::Array> scopeChain = v8::Handle<v8::Array>::Cast(m_callFrame.get()->Get(v8::String::NewSymbol("scopeChain")));
    v8::Handle<v8::Array> result = v8::Array::New(scopeChain->Length());
    for (uint32_t i = 0; i < scopeChain->Length(); i++)
        result->Set(i, scopeChain->Get(i));
    return result;
}

int JavaScriptCallFrame::scopeType(int scopeIndex) const
{
    v8::Handle<v8::Array> scopeType = v8::Handle<v8::Array>::Cast(m_callFrame.get()->Get(v8::String::NewSymbol("scopeType")));
    return scopeType->Get(scopeIndex)->Int32Value();
}

v8::Handle<v8::Value> JavaScriptCallFrame::thisObject() const
{
    return m_callFrame.get()->Get(v8::String::NewSymbol("thisObject"));
}

v8::Handle<v8::Value> JavaScriptCallFrame::evaluate(const String& expression)
{
    v8::Handle<v8::Function> evalFunction = v8::Handle<v8::Function>::Cast(m_callFrame.get()->Get(v8::String::NewSymbol("evaluate")));
    v8::Handle<v8::Value> argv[] = { v8String(expression, m_debuggerContext->GetIsolate()) };
    return evalFunction->Call(m_callFrame.get(), 1, argv);
}

v8::Handle<v8::Value> JavaScriptCallFrame::restart()
{
    v8::Handle<v8::Function> restartFunction = v8::Handle<v8::Function>::Cast(m_callFrame.get()->Get(v8::String::NewSymbol("restart")));
    v8::Debug::SetLiveEditEnabled(true);
    v8::Handle<v8::Value> result = restartFunction->Call(m_callFrame.get(), 0, 0);
    v8::Debug::SetLiveEditEnabled(false);
    return result;
}

v8::Handle<v8::Value> JavaScriptCallFrame::setVariableValue(int scopeNumber, const String& variableName, v8::Handle<v8::Value> newValue)
{
    v8::Handle<v8::Function> setVariableValueFunction = v8::Handle<v8::Function>::Cast(m_callFrame.get()->Get(v8::String::NewSymbol("setVariableValue")));
    v8::Handle<v8::Value> argv[] = {
        v8::Handle<v8::Value>(v8::Integer::New(scopeNumber)),
        v8String(variableName, m_debuggerContext->GetIsolate()),
        newValue
    };
    return setVariableValueFunction->Call(m_callFrame.get(), 3, argv);
}

} // namespace WebCore

