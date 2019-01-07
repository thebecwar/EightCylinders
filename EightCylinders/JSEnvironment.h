#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vcclr.h>

#include <v8.h>
#include <libplatform\libplatform.h>
#include <v8-platform.h>

#include "JSException.h"

namespace EightCylinders
{
    using namespace v8;
    static long EnvironmentCount = 0;

    class Scope
    {
    public:
        Scope(Isolate* isolate) : isolate_scope(isolate), handle_scope(isolate), context(Context::New(isolate)), context_scope(context)
        {

        }

        Isolate::Scope isolate_scope;
        HandleScope handle_scope;
        Local<Context> context;
        Context::Scope context_scope;
    };

    class PersistStore
    {
    public:
        PersistStore() {
            this->m_value.Reset();
        }
        Persistent<Value> m_value;
    };

    public ref class JSEnvironment : public System::IDisposable
    {
    private:
        static bool platformInitialized = false;
        static Platform* SharedPlatform;
        static void Init()
        {
            SharedPlatform = platform::CreateDefaultPlatform();
            V8::InitializePlatform(SharedPlatform);
            V8::InitializeExternalStartupData(".");

            V8::Initialize();
            platformInitialized = true;
        }
        static void Destroy()
        {
            V8::Dispose();
            V8::ShutdownPlatform();
            platformInitialized = false;
        }

    public:
        JSEnvironment()
        {
            if (!platformInitialized)
            {
                Init();
            }
            this->m_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
            Isolate::CreateParams params;
            params.array_buffer_allocator = this->m_allocator;
            this->m_isolate = Isolate::New(params);
            this->m_scope = new Scope(this->m_isolate);
            InterlockedIncrement(&EnvironmentCount);
        }
        ~JSEnvironment() 
        {
            delete this->m_scope;
            this->m_isolate->Dispose();
            if (InterlockedDecrement(&EnvironmentCount) <= 0)
            {
                Destroy();
            }
        }

        void PumpMessageLoop() {
            bool ran = platform::PumpMessageLoop(this->m_platform, this->m_isolate);
            if (ran)
            {

            }
        }

        System::String^ Eval(System::String^ strVal)
        {
            pin_ptr<const wchar_t> tmpVal = PtrToStringChars(strVal);
            Local<String> source = String::NewFromTwoByte(this->m_isolate, reinterpret_cast<const uint16_t*>(tmpVal), NewStringType::kNormal).ToLocalChecked();
            
            TryCatch tc(this->m_isolate);

            MaybeLocal<Script> maybeScript = Script::Compile(this->m_scope->context, source);
            Local<Script> script;
            if (!maybeScript.ToLocal(&script))
            {
                if (tc.HasCaught())
                {
                    throw gcnew JSException(tc.Message());
                }
                else
                {
                    throw gcnew System::Exception("Unknown Compiler Exception");
                }
            }
            
            MaybeLocal<Value> maybeResult = script->Run(this->m_scope->context);

            if (tc.HasCaught()) {
                throw gcnew JSException(tc.Message());
            }

            Local<Value> result = maybeResult.ToLocalChecked();
            String::Utf8Value val(this->m_isolate, result);
            return gcnew System::String(*val);
        }

        System::String^ Test()
        {

            Local<String> source = String::NewFromUtf8(this->m_isolate, "let a = 2;\nlet b = 4;\n(a+b).toString();");
            Local<Script> script = Script::Compile(this->m_scope->context, source).ToLocalChecked();
            Local<Value> result = script->Run(this->m_scope->context).ToLocalChecked();
            
            String::Utf8Value val(this->m_isolate, result);
            System::String^ str = gcnew System::String(*val);
            return str;
        }

    private:
        Platform* m_platform;
        Isolate* m_isolate;
        ArrayBuffer::Allocator* m_allocator;
        Scope* m_scope;
    };

}