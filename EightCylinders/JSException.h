#pragma once

#include <v8.h>
#include <string>
#include <sstream>

namespace EightCylinders
{
    public ref class JSException : public System::Exception
    {
    private:
        static System::String^ LocalToString(v8::Isolate* isolate, v8::Local<v8::String>& strVal)
        {
            v8::String::Utf8Value val(isolate, strVal);
            return gcnew System::String(*val);
        }
        static System::String^ LocalToString(v8::Isolate* isolate, v8::MaybeLocal<v8::String>& maybeLocal)
        {
            v8::Local<v8::String> strVal;
            if (maybeLocal.ToLocal(&strVal))
            {
                v8::String::Utf8Value val(isolate, strVal);
                return gcnew System::String(*val);
            }
            else
            {
                return System::String::Empty;
            }
        }
        static int MaybeToValue(v8::Maybe<int32_t>& val)
        {
            return val.FromMaybe(-1);
        }
        
    public:
        ref class StackFrame
        {
        private:
            int m_lineNumber;
            int m_column;
            int m_scriptId;
            System::String^ m_scriptName;
            System::String^ m_scriptNameOrSourceUrl;
            System::String^ m_functionName;
            bool m_isEval;
            bool m_isConstructor;
            bool m_isWasm;

        public:
            StackFrame(v8::Isolate* isolate, v8::Local<v8::StackFrame>& frame)
            {
                this->m_lineNumber = frame->GetLineNumber();
                this->m_column = frame->GetLineNumber();
                this->m_scriptId = frame->GetScriptId();
                this->m_scriptName = LocalToString(isolate, frame->GetScriptName());
                this->m_scriptNameOrSourceUrl = LocalToString(isolate, frame->GetScriptNameOrSourceURL());
                this->m_functionName = LocalToString(isolate, frame->GetFunctionName());
                this->m_isEval = frame->IsEval();
                this->m_isConstructor = frame->IsConstructor();
                this->m_isWasm = frame->IsWasm();
            }

            property int LineNumber { int get() { return this->m_lineNumber; } }
            property int Column { int get() { return this->m_column; } }
            property int ScriptId { int get() { return this->m_scriptId; } }
            property System::String^ ScriptName { System::String^ get() { return this->m_scriptName; } }
            property System::String^ ScriptNameOrSourceUrl { System::String^ get() { return this->m_scriptNameOrSourceUrl; } }
            property System::String^ FunctionName { System::String^ get() { return this->m_functionName; } }
            property bool IsEval { bool get() { return this->m_isEval; } }
            property bool IsConstructor { bool get() { return this->m_isConstructor; } }
            property bool IsWasm { bool get() { return this->m_isWasm; } }

        };
        ref class StackTrace
        {
        private:
            array<StackFrame^>^ m_frames;

        public:
            StackTrace(v8::Isolate* isolate, v8::Local<v8::StackTrace>& trace)
            {
                if (trace.IsEmpty())
                {
                    this->m_frames = gcnew array<StackFrame^>(0);
                    return;
                }

                int frameCount = trace->GetFrameCount();
                this->m_frames = gcnew array<StackFrame^>(frameCount);

                for (int i = 0; i < frameCount; i++)
                {
                    this->m_frames[i] = gcnew StackFrame(isolate, trace->GetFrame(isolate, i));
                }
            }

            property array<StackFrame^>^ Frames
            {
                array<StackFrame^>^ get()
                {
                    return this->m_frames;
                }
            }
        };

    private:
        System::String^ m_message;
        System::String^ m_sourceLine;
        // ScriptOrigin
        JSException::StackTrace^ m_stackTrace;
        int m_lineNumber;
        int m_startPosition;
        int m_endPosition;
        int m_startColumn;
        int m_endColumn;
        bool m_isSharedCrossOrigin;

    public:
        JSException(v8::Local<v8::Message>& message)
        {
            v8::Isolate* isolate = message->GetIsolate();
            this->m_message = LocalToString(isolate, message->Get());
            this->m_sourceLine = LocalToString(isolate, message->GetSourceLine(isolate->GetCurrentContext()));
            this->m_stackTrace = gcnew StackTrace(isolate, message->GetStackTrace());
            this->m_lineNumber = MaybeToValue(message->GetLineNumber(isolate->GetCurrentContext()));
            this->m_startPosition = message->GetStartPosition();
            this->m_endPosition = message->GetEndPosition();
            this->m_startColumn = message->GetStartColumn();
            this->m_endColumn = message->GetEndColumn();
            this->m_isSharedCrossOrigin = message->IsSharedCrossOrigin();
        }

        property System::String^ Message
        {
            System::String^ get() override
            {
                return this->m_message;
            }
        }
        property System::String^ SourceLine
        {
            System::String^ get()
            {
                return this->m_sourceLine;
            }
        }
        property JSException::StackTrace^ ExceptionStack
        {
            JSException::StackTrace^ get()
            {
                return this->m_stackTrace;
            }
        }
        property int LineNumber
        {
            int get()
            {
                return this->m_lineNumber;
            }
        }
        property int StartPosition
        {
            int get()
            {
                return this->m_startPosition;
            }
        }
        property int EndPosition
        {
            int get()
            {
                return this->m_endPosition;
            }
        }
        property int StartColumn
        {
            int get()
            {
                return this->m_startColumn;
            }
        }
        property int EndColumn
        {
            int get()
            {
                return this->m_endColumn;
            }
        }
        property bool IsSharedCrossOrigin
        {
            bool get()
            {
                return this->m_isSharedCrossOrigin;
            }
        }
    };
}